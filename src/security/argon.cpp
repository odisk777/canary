/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#include "pch.hpp"

#include "database/database.h"
#include "security/argon.hpp"

bool Argon2::verifyPassword(const char* password, const std::string &phash) {
	// Parse the hash string
	std::regex re("\\$argon2id\\$v=([0-9]+)\\$m=([0-9]+),t=([0-9]+),p=([0-9]+)\\$([A-Za-z0-9+/]+)\\$([A-Za-z0-9+/]+)");
	std::smatch match;
	if (!std::regex_search(phash, match, re) || match.size() != 7) {
		SPDLOG_DEBUG("Failed to parse hash string");
		return false;
	}

	// Extract parameters
	uint32_t version = std::stoi(match[1]);
	uint32_t m_cost = std::stoi(match[2]);
	uint32_t t_cost = std::stoi(match[3]);
	uint32_t parallelism = std::stoi(match[4]);
	std::vector<uint8_t> salt = base64_decode(match[5]);
	std::vector<uint8_t> hash = base64_decode(match[6]);

	// Hash the password
	std::vector<uint8_t> computed_hash(hash.size());
	if (argon2id_hash_raw(t_cost, m_cost, parallelism, password, std::strlen(password), salt.data(), salt.size(), computed_hash.data(), computed_hash.size()) != ARGON2_OK) {
		SPDLOG_DEBUG("Error hashing password");
		return false;
	}

	// Compare
	return computed_hash == hash;
}

std::vector<uint8_t> Argon2::base64_decode(const std::string &input) {
	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::vector<uint8_t> ret;
	int i = 0;
	uint32_t val = 0;
	for (char c : input) {
		if (isspace(c) || c == '=') {
			continue;
		}

		size_t pos = base64_chars.find(c);
		if (pos == std::string::npos) {
			SPDLOG_DEBUG("Invalid character in base64 string");
		}

		val = (val << 6) + pos;
		if (++i % 4 == 0) {
			ret.push_back((val >> 16) & 0xFF);
			ret.push_back((val >> 8) & 0xFF);
			ret.push_back(val & 0xFF);
		}
	}

	switch (i % 4) {
		case 1:
			SPDLOG_DEBUG("Invalid length for base64 string");
		case 2:
			ret.push_back((val >> 4) & 0xFF);
			break;
		case 3:
			ret.push_back((val >> 10) & 0xFF);
			ret.push_back((val >> 2) & 0xFF);
			break;
	}

	return ret;
}

bool Argon2::argon(const char* password_attempt, const std::string &accountPassword) {
	const char* password = password_attempt;
	const std::string hashed_password = accountPassword;

	return verifyPassword(password, hashed_password);
}
