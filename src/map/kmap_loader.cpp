/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.org/
*/

#include "otpch.h"

#include "map/kmap_loader.hpp"

#include <fstream>
#include <filesystem>

bool KmapLoader::load(Map &map, const std::string &fileName)
{
	int64_t start = OTSYS_TIME();
	if (loadFile(fileName) == false)
	{
		return false;
	}

	auto kmap = Kmap::GetMap(buffer.data());
	loadHeader(map, kmap->header(), fileName);
	loadData(map, kmap->data());

	SPDLOG_INFO("Map loading time: {} seconds", (OTSYS_TIME() - start) / (1000.));
	return true;
}

bool KmapLoader::loadFile(const std::string &fileName)
{
	std::fstream fileStream(fileName, std::ios:: in | std::ios::binary);
	if (!fileStream.is_open())
	{
		SPDLOG_ERROR("Unable to load {}, could not open file", fileName);
		return false;
	}

	if (!fileStream.good())
	{
		SPDLOG_ERROR("Unable to load {}, error for read file", fileName);
		return false;
	}

	std::vector<uint8_t> fileBuffer((std::istreambuf_iterator<char> (fileStream)), (std::istreambuf_iterator<char> ()));
	buffer = fileBuffer;

	fileStream.close();
	return true;
}

void KmapLoader::loadHeader(
	Map &map, const Kmap::MapHeader *header, const std::string &fileName
)
{
	map.width = header->width();
	map.height = header->height();
	SPDLOG_INFO("Map size: {}x{}", map.width, map.height);

	map.monsterfile = readResourceFile(fileName, *header->monster_spawn_file());
	map.npcfile = readResourceFile(fileName, *header->npc_spawn_file());
	map.housefile = readResourceFile(fileName, *header->house_file());
}

void KmapLoader::loadData(Map &map, const Kmap::MapData *mapData)
{
	for (auto area: *mapData->areas())
	{
		for (auto tile: *area->tiles())
		{
			loadTile(map, tile, area->position());
		}
	}

	for (auto town: *mapData->towns())
	{
		loadTown(map, town);
	}

	if (auto waypoints = mapData->waypoints();
	waypoints)
	{
		for (auto waypoint: *waypoints)
		{
			loadWaypoint(map, waypoint);
		}
	}
}

void KmapLoader::loadTile(Map& map, const Kmap::Tile* kTile, const Kmap::Position* areaPosition)
{
	const Position tilePosition(
		areaPosition->x() + kTile->x(),
		areaPosition->y() + kTile->y(),
		areaPosition->z()
	);

	Item* groundItem = nullptr;
	Tile *tile = loadHouses(map, tilePosition, kTile->house_id());

	if (groundItem = loadItem(kTile->ground(), tile); groundItem)
	{
		if (tile == nullptr)
		{
			tile = createTile(tilePosition, groundItem);
		}
		groundItem->setLoadedFromMap(true);
	}

	auto kItems = kTile->items();
	if (kItems != nullptr) {
		for (auto kItem : *kItems)
		{
			Item *item = loadItem(kItem, tile);

			if (item == nullptr)
			{
				continue;
			}

			if (tile) {
				tile->internalAddThing(item);
				item->startDecaying();
				item->setLoadedFromMap(true);
			}

			if (tile == nullptr)
			{
				if (item->isGroundTile())
				{
					delete groundItem;
					groundItem = item;
					continue;
				}

				tile = createTile(tilePosition, groundItem, item);
			}

			tile->internalAddThing(item);
			item->startDecaying();
			item->setLoadedFromMap(true);
		}
	}

	if (tile == nullptr)
	{
		tile = createTile(tilePosition, groundItem);
	}

	tile->setFlag(static_cast<TileFlags_t>(readFlags(kTile->flags())));
	map.setTile(tilePosition, tile);
}

Tile* KmapLoader::loadHouses(Map &map, const Position &tilePosition, const uint32_t houseId)
{
	if (houseId == 0)
	{
		return nullptr;
	}

	if (House* house = map.houses.addHouse(houseId))
	{
		Tile* tile = new HouseTile(tilePosition.x, tilePosition.y, tilePosition.z, house);
		house->addTile(static_cast<HouseTile*>(tile));
		return tile;
	}

	SPDLOG_ERROR("{} - Could not create house id: {}, on position: {}", __FUNCTION__, houseId, tilePosition.toString());
	return nullptr;
}

Item* KmapLoader::loadItem(const Kmap::Item *kItem, Tile *tile)
{
	if (kItem == nullptr)
	{
		return nullptr;
	}

	Item *item = Item::CreateItem(kItem->id(), 1);

	if (item == nullptr)
	{
		SPDLOG_WARN("{} - Failed to create item #{}.", __FUNCTION__, kItem->id());
		return nullptr;
	}

	HouseTile* houseTile = dynamic_cast<HouseTile*>(tile);
	if (houseTile && item->isMoveable())
	{
		SPDLOG_WARN("{} - Failed to create moveable item #{} inside a house.", __FUNCTION__, kItem->id());
		delete item;
		return item;
	}

	if (auto attributes = kItem->attributes(); attributes)
	{
		attributes->count();
		attributes->count();
		attributes->description();

		if (auto action = attributes->action(); action)
		{
			action->action_id();
			action->unique_id();
		}
	}

	if (auto details = kItem->details(); details)
	{
		details->depot_id();
		details->door_id();
		details->teleport();

		auto kContainerItem = details->container_items();
		if (kContainerItem == nullptr)
		{
			return item;
		}

		if (const ItemType &itemType = Item::items[item->getID()];
		!itemType.isContainer() && kContainerItem->size() > 0)
		{
			SPDLOG_ERROR("{} - Container items found, but item {} is not a container.", __FUNCTION__, kItem->id());
			return item;
		}

		for (auto containerItem: *kContainerItem)
		{
			loadItem(containerItem, tile);
		}
	}

	return item;
}

void KmapLoader::loadTown(Map &map, const Kmap::Town *kTown)
{
	auto townId = kTown->id();
	if (townId == 0)
	{
		SPDLOG_ERROR("{} - Invalid town id", __FUNCTION__);
		return;
	}

	Town *town = new Town(townId);
	if (!map.towns.addTown(townId, town))
	{
		SPDLOG_ERROR("{} - Cannot create town with id: {}, discarding town", __FUNCTION__, townId);
		delete town;
		return;
	}

	const std::string townName = kTown->name()->str();
	if (townName.empty())
	{
		SPDLOG_WARN("{} - Could not read town name for town id {}", __FUNCTION__, townId);
	}
	town->setName(townName);

	Position townPosition(
		kTown->position()->x(),
		kTown->position()->y(),
		kTown->position()->z()
	);
	// Sanity check, if there is an error in the get, we will know where the problem is
	if (townPosition.x == 0 || townPosition.y == 0 || townPosition.z == 0)
	{
		SPDLOG_ERROR("{} - Invalid town position", __FUNCTION__);
		return;
	}

	town->setTemplePos(townPosition);
}

void KmapLoader::loadWaypoint(Map &map, const Kmap::Waypoint *waypoint)
{
	const std::string waypointName = waypoint->name()->str();

	Position waypointPosition(
		waypoint->position()->x(),
		waypoint->position()->y(),
		waypoint->position()->z()
	);

	if (waypointPosition.x == 0 || waypointPosition.y == 0 || waypointPosition.z == 0 || waypointName.empty())
	{
		SPDLOG_ERROR("{} - Invalid waypoint.", __FUNCTION__);
		return;
	}

	map.waypoints[waypointName] = waypointPosition;
}

std::string KmapLoader::readResourceFile(
	const std::string &fileName, const flatbuffers::String &resourceFile
)
{
	return fileName.substr(0, fileName.rfind('/') + 1) + resourceFile.str();
}

TileFlags_t KmapLoader::readFlags(uint32_t encodedflags)
{
	std::map<uint32_t, TileFlags_t> flagConvertionMap;
	{
		{
			OTBM_TILEFLAG_PROTECTIONZONE, TILESTATE_PROTECTIONZONE;
		}
		{
			OTBM_TILEFLAG_NOPVPZONE, TILESTATE_NOPVPZONE;
		}
		{
			OTBM_TILEFLAG_PVPZONE, TILESTATE_PVPZONE;
		}
		{
			OTBM_TILEFLAG_NOLOGOUT, TILESTATE_NOLOGOUT;
		}
	};

	uint32_t tileFlags = TILESTATE_NONE;
	for (auto const &[encodedFlag, tileFlag]: flagConvertionMap)
	{
		if ((encodedflags & encodedFlag) != 0)
		{
			tileFlags |= tileFlag;
		}
	}

	return static_cast<TileFlags_t>(tileFlags);
}

Tile* KmapLoader::createTile(const Position &position, Item* ground/* = nullptr*/, Item* item/* = nullptr*/)
{
	if (!ground) {
		return new StaticTile(position.x, position.y, position.z);
	}

	Tile *tile = ((item && item->isBlocking()) || ground->isBlocking())
			? new StaticTile(position.x, position.y, position.z)
			: tile = new DynamicTile(position.x, position.y, position.z);

	tile->internalAddThing(ground);

	return tile;
}