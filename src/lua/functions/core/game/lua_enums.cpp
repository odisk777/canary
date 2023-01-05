/**
 * Canary - A free and open-source MMORPG server emulator
 * Copyright (©) 2019-2022 OpenTibiaBR <opentibiabr@outlook.com>
 * Repository: https://github.com/opentibiabr/canary
 * License: https://github.com/opentibiabr/canary/blob/main/LICENSE
 * Contributors: https://github.com/opentibiabr/canary/graphs/contributors
 * Website: https://docs.opentibiabr.org/
*/

#include "pch.hpp"

#include "config/configmanager.h"

#include "lua/functions/core/game/lua_enums.hpp"

#include "creatures/players/account/account.hpp"
#include "creatures/creature.h"
#include "lua/creature/creatureevent.h"
#include "declarations.hpp"
#include "game/functions/game_reload.hpp"
#include "game/game.h"

#define registerEnumClass(luaState, enumClassType) { \
	auto number = magic_enum::enum_integer(enumClassType); \
	auto name = magic_enum::enum_name(enumClassType).data(); \
	registerGlobalVariable(luaState, name, number); \
} void(0)

#define registerEnum(L, value) { \
	std::string enumName = #value; \
	registerGlobalVariable(L, enumName.substr(enumName.find_last_of(':') + 1), value); \
} void(0)

void LuaEnums::init(lua_State* L) {
	initOthersEnums(L);
	initAccountEnums(L);
	initDailyRewardEnums(L);
	initBugCategoryEnums(L);
	initReportTypeEnums(L);
	initCallbackParamEnums(L);
	initCombatEnums(L);
	initCombatParamEnums(L);
	initCombatFormulaEnums(L);
	initDirectionEnums(L);
	initFactionEnums(L);
	initConditionEnums(L);
	initConditionIdEnums(L);
	initConditionParamEnums(L);
	initConstMeEnums(L);
	initConstAniEnums(L);
	initConstPropEnums(L);
	initConstSlotEnums(L);
	initCreatureEventEnums(L);
	initGameStateEnums(L);
	initMessageEnums(L);
	initCreatureTypeEnums(L);
	initClientOsEnums(L);
	initFightModeEnums(L);
	initItemAttributeEnums(L);
	initItemTypeEnums(L);
	initItemIdEnums(L);
	initPlayerFlagEnums(L);
	initReportReasonEnums(L);
	initSkillEnums(L);
	initSkullEnums(L);
	initTalkTypeEnums(L);
	initBestiaryEnums(L);
	initTextColorEnums(L);
	initTileStateEnums(L);
	initSpeechBubbleEnums(L);
	initMapMarkEnums(L);
	initReturnValueEnums(L);
	initReloadTypeEnums(L);
	initCreaturesEventEnums(L);
	initForgeEnums(L);
	initWebhookEnums(L);
}

void LuaEnums::initOthersEnums(lua_State* L) {
	registerEnum(L, WORLD_TYPE_NO_PVP);
	registerEnum(L, WORLD_TYPE_PVP);
	registerEnum(L, WORLD_TYPE_PVP_ENFORCED);
	registerEnum(L, AMMO_NONE);
	registerEnum(L, AMMO_BOLT);
	registerEnum(L, AMMO_ARROW);
	registerEnum(L, AMMO_SPEAR);
	registerEnum(L, AMMO_THROWINGSTAR);
	registerEnum(L, AMMO_THROWINGKNIFE);
	registerEnum(L, AMMO_STONE);
	registerEnum(L, AMMO_SNOWBALL);

	registerEnum(L, CHARM_OFFENSIVE);
	registerEnum(L, CHARM_DEFENSIVE);
	registerEnum(L, CHARM_PASSIVE);
	registerEnum(L, CHARM_GUT);
	registerEnum(L, CHARM_SCAVENGE);

	// Use with container:addItem, container:addItemEx and possibly other functions.
	registerEnum(L, FLAG_NOLIMIT);
	registerEnum(L, FLAG_IGNOREBLOCKITEM);
	registerEnum(L, FLAG_IGNOREBLOCKCREATURE);
	registerEnum(L, FLAG_CHILDISOWNER);
	registerEnum(L, FLAG_PATHFINDING);
	registerEnum(L, FLAG_IGNOREFIELDDAMAGE);
	registerEnum(L, FLAG_IGNORENOTMOVEABLE);
	registerEnum(L, FLAG_IGNOREAUTOSTACK);

	// Use with house:getAccessList, house:setAccessList
	registerEnum(L, GUEST_LIST);

	registerEnum(L, LIGHT_STATE_DAY);
	registerEnum(L, LIGHT_STATE_NIGHT);
	registerEnum(L, LIGHT_STATE_SUNSET);
	registerEnum(L, LIGHT_STATE_SUNRISE);
	registerEnum(L, STORAGEVALUE_EMOTE);

	registerEnum(L, MAX_LOOTCHANCE);

	registerEnum(L, ORIGIN_NONE);
	registerEnum(L, ORIGIN_CONDITION);
	registerEnum(L, ORIGIN_SPELL);
	registerEnum(L, ORIGIN_MELEE);
	registerEnum(L, ORIGIN_RANGED);
	
	registerEnum(L, PLAYERSEX_FEMALE);
	registerEnum(L, PLAYERSEX_MALE);

	registerEnum(L, RESPAWNPERIOD_ALL);
	registerEnum(L, RESPAWNPERIOD_DAY);
	registerEnum(L, RESPAWNPERIOD_NIGHT);

	// Use with itemType:getSlotPosition
	registerEnum(L, SLOTP_WHEREEVER);
	registerEnum(L, SLOTP_HEAD);
	registerEnum(L, SLOTP_NECKLACE);
	registerEnum(L, SLOTP_BACKPACK);
	registerEnum(L, SLOTP_ARMOR);
	registerEnum(L, SLOTP_RIGHT);
	registerEnum(L, SLOTP_LEFT);
	registerEnum(L, SLOTP_LEGS);
	registerEnum(L, SLOTP_FEET);
	registerEnum(L, SLOTP_RING);
	registerEnum(L, SLOTP_AMMO);
	registerEnum(L, SLOTP_DEPOT);
	registerEnum(L, SLOTP_TWO_HAND);

	// Spells
	registerEnum(L, SPELL_INSTANT);
	registerEnum(L, SPELL_RUNE);

	registerEnum(L, SUBOWNER_LIST);

	registerEnum(L, VOCATION_NONE);

	registerEnum(L, ZONE_PROTECTION);
	registerEnum(L, ZONE_NOPVP);
	registerEnum(L, ZONE_PVP);
	registerEnum(L, ZONE_NOLOGOUT);
	registerEnum(L, ZONE_NORMAL);

	registerEnum(L, WEAPON_NONE);
	registerEnum(L, WEAPON_SWORD);
	registerEnum(L, WEAPON_CLUB);
	registerEnum(L, WEAPON_AXE);
	registerEnum(L, WEAPON_SHIELD);
	registerEnum(L, WEAPON_DISTANCE);
	registerEnum(L, WEAPON_WAND);
	registerEnum(L, WEAPON_AMMO);
}

void LuaEnums::initAccountEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<AccountType>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initDailyRewardEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<DailyRewardStatus>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initBugCategoryEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<BugReportType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initReportTypeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<RuleViolationType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCallbackParamEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<CallBackParam_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCombatEnums(lua_State* L) {
	registerEnum(L, COMBAT_NONE);
	registerEnum(L, COMBAT_PHYSICALDAMAGE);
	registerEnum(L, COMBAT_ENERGYDAMAGE);
	registerEnum(L, COMBAT_EARTHDAMAGE);
	registerEnum(L, COMBAT_FIREDAMAGE);
	registerEnum(L, COMBAT_UNDEFINEDDAMAGE);
	registerEnum(L, COMBAT_LIFEDRAIN);
	registerEnum(L, COMBAT_MANADRAIN);
	registerEnum(L, COMBAT_HEALING);
	registerEnum(L, COMBAT_DROWNDAMAGE);
	registerEnum(L, COMBAT_ICEDAMAGE);
	registerEnum(L, COMBAT_HOLYDAMAGE);
	registerEnum(L, COMBAT_DEATHDAMAGE);
}

void LuaEnums::initCombatParamEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<CombatParam_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCombatFormulaEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<formulaType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initDirectionEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Direction>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initFactionEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Faction_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConditionEnums(lua_State* L) {
	registerEnum(L, CONDITION_NONE);
	registerEnum(L, CONDITION_POISON);
	registerEnum(L, CONDITION_FIRE);
	registerEnum(L, CONDITION_ENERGY);
	registerEnum(L, CONDITION_BLEEDING);
	registerEnum(L, CONDITION_HASTE);
	registerEnum(L, CONDITION_PARALYZE);
	registerEnum(L, CONDITION_OUTFIT);
	registerEnum(L, CONDITION_INVISIBLE);
	registerEnum(L, CONDITION_LIGHT);
	registerEnum(L, CONDITION_MANASHIELD);
	registerEnum(L, CONDITION_INFIGHT);
	registerEnum(L, CONDITION_DRUNK);
	registerEnum(L, CONDITION_EXHAUST);
	registerEnum(L, CONDITION_REGENERATION);
	registerEnum(L, CONDITION_SOUL);
	registerEnum(L, CONDITION_DROWN);
	registerEnum(L, CONDITION_MUTED);
	registerEnum(L, CONDITION_CHANNELMUTEDTICKS);
	registerEnum(L, CONDITION_YELLTICKS);
	registerEnum(L, CONDITION_ATTRIBUTES);
	registerEnum(L, CONDITION_FREEZING);
	registerEnum(L, CONDITION_DAZZLED);
	registerEnum(L, CONDITION_CURSED);
	registerEnum(L, CONDITION_EXHAUST_COMBAT);
	registerEnum(L, CONDITION_EXHAUST_HEAL);
	registerEnum(L, CONDITION_PACIFIED);
	registerEnum(L, CONDITION_SPELLCOOLDOWN);
	registerEnum(L, CONDITION_SPELLGROUPCOOLDOWN);
	registerEnum(L, CONDITION_ROOTED);
}

void LuaEnums::initConditionIdEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ConditionId_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConditionParamEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ConditionParam_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConstMeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<MagicEffectClasses>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConstAniEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ShootType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConstPropEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ItemProperty>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initConstSlotEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Slots_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCreatureEventEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<CreatureEventType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initGameStateEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<GameState_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initMessageEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<MessageClasses>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCreatureTypeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<CreatureType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initClientOsEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<OperatingSystem_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initFightModeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<FightMode_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initItemAttributeEnums(lua_State* L) {
	registerEnum(L, ITEM_ATTRIBUTE_NONE);
	registerEnum(L, ITEM_ATTRIBUTE_ACTIONID);
	registerEnum(L, ITEM_ATTRIBUTE_UNIQUEID);
	registerEnum(L, ITEM_ATTRIBUTE_DESCRIPTION);
	registerEnum(L, ITEM_ATTRIBUTE_TEXT);
	registerEnum(L, ITEM_ATTRIBUTE_DATE);
	registerEnum(L, ITEM_ATTRIBUTE_WRITER);
	registerEnum(L, ITEM_ATTRIBUTE_NAME);
	registerEnum(L, ITEM_ATTRIBUTE_ARTICLE);
	registerEnum(L, ITEM_ATTRIBUTE_PLURALNAME);
	registerEnum(L, ITEM_ATTRIBUTE_WEIGHT);
	registerEnum(L, ITEM_ATTRIBUTE_ATTACK);
	registerEnum(L, ITEM_ATTRIBUTE_DEFENSE);
	registerEnum(L, ITEM_ATTRIBUTE_EXTRADEFENSE);
	registerEnum(L, ITEM_ATTRIBUTE_ARMOR);
	registerEnum(L, ITEM_ATTRIBUTE_HITCHANCE);
	registerEnum(L, ITEM_ATTRIBUTE_SHOOTRANGE);
	registerEnum(L, ITEM_ATTRIBUTE_OWNER);
	registerEnum(L, ITEM_ATTRIBUTE_DURATION);
	registerEnum(L, ITEM_ATTRIBUTE_DECAYSTATE);
	registerEnum(L, ITEM_ATTRIBUTE_CORPSEOWNER);
	registerEnum(L, ITEM_ATTRIBUTE_CHARGES);
	registerEnum(L, ITEM_ATTRIBUTE_FLUIDTYPE);
	registerEnum(L, ITEM_ATTRIBUTE_DOORID);
	registerEnum(L, ITEM_ATTRIBUTE_SPECIAL);
	registerEnum(L, ITEM_ATTRIBUTE_OPENCONTAINER);
	registerEnum(L, ITEM_ATTRIBUTE_QUICKLOOTCONTAINER);
	registerEnum(L, ITEM_ATTRIBUTE_DURATION_TIMESTAMP);
}

void LuaEnums::initItemTypeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ItemTypes_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initItemIdEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<item_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initPlayerFlagEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<PlayerFlags_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initReportReasonEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<RuleViolationReasons_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initSkillEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<skills_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initSkullEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Skulls_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initTalkTypeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<SpeakClasses>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initBestiaryEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<BestiaryType_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initTextColorEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<TextColor_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initTileStateEnums(lua_State* L) {
	registerEnum(L, TILESTATE_NONE);
	registerEnum(L, TILESTATE_PROTECTIONZONE);
	registerEnum(L, TILESTATE_NOPVPZONE);
	registerEnum(L, TILESTATE_NOLOGOUT);
	registerEnum(L, TILESTATE_PVPZONE);
	registerEnum(L, TILESTATE_FLOORCHANGE);
	registerEnum(L, TILESTATE_FLOORCHANGE_DOWN);
	registerEnum(L, TILESTATE_FLOORCHANGE_NORTH);
	registerEnum(L, TILESTATE_FLOORCHANGE_SOUTH);
	registerEnum(L, TILESTATE_FLOORCHANGE_EAST);
	registerEnum(L, TILESTATE_FLOORCHANGE_WEST);
	registerEnum(L, TILESTATE_TELEPORT);
	registerEnum(L, TILESTATE_MAGICFIELD);
	registerEnum(L, TILESTATE_MAILBOX);
	registerEnum(L, TILESTATE_TRASHHOLDER);
	registerEnum(L, TILESTATE_BED);
	registerEnum(L, TILESTATE_DEPOT);
	registerEnum(L, TILESTATE_BLOCKSOLID);
	registerEnum(L, TILESTATE_BLOCKPATH);
	registerEnum(L, TILESTATE_IMMOVABLEBLOCKSOLID);
	registerEnum(L, TILESTATE_IMMOVABLEBLOCKPATH);
	registerEnum(L, TILESTATE_IMMOVABLENOFIELDBLOCKPATH);
	registerEnum(L, TILESTATE_NOFIELDBLOCKPATH);
	registerEnum(L, TILESTATE_FLOORCHANGE_SOUTH_ALT);
	registerEnum(L, TILESTATE_FLOORCHANGE_EAST_ALT);
	registerEnum(L, TILESTATE_SUPPORTS_HANGABLE);
}

// Use with npc:setSpeechBubble
void LuaEnums::initSpeechBubbleEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<SpeechBubble_t>()) {
		registerEnumClass(L, value);
	}
}

// Use with player:addMapMark
void LuaEnums::initMapMarkEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<MapMark_t>()) {
		registerEnumClass(L, value);
	}
}

// Use with Game.getReturnMessage
void LuaEnums::initReturnValueEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ReturnValue>()) {
		registerEnumClass(L, value);
	}
}

// Reload
void LuaEnums::initReloadTypeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Reload_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initCreaturesEventEnums(lua_State* L) {
	// Monsters and npcs
		for (auto value : magic_enum::enum_values<MonstersEvent_t>()) {
		registerEnumClass(L, value);
	}
}

void LuaEnums::initForgeEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<ForgeClassifications_t>()) {
		registerEnumClass(L, value);
	}
}

// Webhook default colors
void LuaEnums::initWebhookEnums(lua_State* L) {
	for (auto value : magic_enum::enum_values<Webhook_Colors_t>()) {
		registerEnumClass(L, value);
	}
}
