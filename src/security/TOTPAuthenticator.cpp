/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#include "pch.hpp"

#include "TOTPAuthenticator.hpp"
#include "database/database.h"

TOTPAuthenticator::TOTPAuthenticator() = default;

TOTPAuthenticator::~TOTPAuthenticator() = default;

std::string TOTPAuthenticator::base32Decode(const std::string &encoded) {
	std::istringstream istr(encoded);
	std::ostringstream ostr;
	Poco::Base32Decoder decoder(istr);
	Poco::StreamCopier::copyStream(decoder, ostr);
	std::string result = ostr.str();
	return result;
}

uint64_t TOTPAuthenticator::hostToBigEndian(uint64_t value) {
	uint64_t result = 0;
	for (int i = 0; i < sizeof(value); ++i) {
		result <<= 8;
		result |= (value >> (i * 8)) & 0xFF;
	}
	return result;
}

std::string TOTPAuthenticator::verifyToken(uint32_t accountId, uint32_t tokenTime) {

	if (accountId <= 0 || tokenTime <= 0) {
		SPDLOG_WARN("verifyToken called with invalid accountId or tokenTime. accountId: {}, tokenTime: {}", accountId, tokenTime);
		return {};
	}

	std::string secret2FA = getSecret2FA(accountId);

	if (secret2FA.empty()) {
		SPDLOG_WARN("Failed to retrieve secret key for accountId: {}", accountId);
		return {};
	}

	// Decode the secret
	std::string secret;
	try {
		secret = base32Decode(secret2FA);
	} catch (const std::exception &e) {
		SPDLOG_WARN("Error decoding secret key for accountId {}: {}", accountId, e.what());
		return {};
	}

	// Calculate the time step
	uint64_t timeStep = hostToBigEndian(tokenTime);

	// Calculate HMAC
	Poco::HMACEngine<Poco::SHA1Engine> hmac(secret);
	std::string messageStr(reinterpret_cast<const char*>(&timeStep), sizeof(timeStep));
	hmac.update(messageStr);
	const Poco::DigestEngine::Digest &digest = hmac.digest();

	// Perform the TOTP calculation
	int offset = digest[digest.size() - 1] & 0xF;
	unsigned int binCode = (static_cast<unsigned int>(digest[offset] & 0x7f) << 24)
		| (static_cast<unsigned int>(digest[offset + 1] & 0xff) << 16)
		| (static_cast<unsigned int>(digest[offset + 2] & 0xff) << 8)
		| (static_cast<unsigned int>(digest[offset + 3] & 0xff));

	unsigned int token = binCode % 1000000; // 6-digit TOTP

	return std::to_string(token);
}

std::string TOTPAuthenticator::getSecret2FA(uint32_t accountId) {
	std::ostringstream query;
	query << "SELECT `secret` FROM `account_authentication` WHERE `account_id` = " << accountId;
	if (auto results = Database::getInstance().storeQuery(query.str())) {
		return results->getString("secret");
	}
	return {};
}
