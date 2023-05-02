/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.com/
 */

#ifndef SRC_IO_FUNCTIONS_IOLOGINDATALOAD_HPP_
#define SRC_IO_FUNCTIONS_IOLOGINDATALOAD_HPP_

#include "io/iologindata.h"

class IOLoginDataLoad : public IOLoginData {
	public:
		static void loadPlayerForgeHistory(std::shared_ptr<Player> player, DBResult_ptr result);
		static void loadRewardItems(std::shared_ptr<Player> player);
		static void loadPlayerBosstiary(std::shared_ptr<Player> player, DBResult_ptr result);

	private:
		static void bindRewardBag(std::shared_ptr<Player> player, ItemMap &itemMap);
		static void insertItemsIntoRewardBag(const ItemMap &itemMap);
};

#endif // SRC_IO_FUNCTIONS_IOLOGINDATALOAD_HPP_
