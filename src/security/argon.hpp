/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#ifndef SRC_SECURITY_ARGON_HPP_
#define SRC_SECURITY_ARGON_HPP_

class Argon2 {
	public:
		Argon2() = default;
		~Argon2() = default;

		// Singleton - ensures we don't accidentally copy it
		Argon2(const Argon2 &) = delete;
		void operator=(const Argon2 &) = delete;

		bool argon(const char* password_attempt, const std::string &email);

	private:
		bool verifyPassword(const char* password, const std::string &phash);
		std::vector<uint8_t> base64_decode(const std::string &input);
};

#endif // SRC_SECURITY_ARGON_HPP_
