#pragma once
#include <cstdint>

enum class SyglType : uint8_t { NONE, DUALITY, WAVES, TERRA, TEMPUS, BLOOD };

enum class StatusFlag : uint8_t {
    NONE     = 0,
    POISON   = 1 << 0,
    BURN     = 1 << 1,
    PARALYZE = 1 << 2,
    SLEEP    = 1 << 3,
    SILENCE  = 1 << 4,
    CHARM    = 1 << 5,
};

enum class ItemId : uint8_t {
    NONE = 0,
    HERB,           // restore 20 HP
    ETHER,          // restore 10 MP
    ANTIDOTE,       // cure POISON and BURN
    IRON_SWORD,     // +3 ATK
    SHORT_STAFF,    // +1 ATK, +2 MAG
    LEATHER_ARMOR,  // +3 DEF
    ROBE,           // +1 DEF, +3 MP_MAX
    SPEED_RING,     // +2 SPD
    POWER_RING,     // +2 ATK
    ITEM_COUNT
};

enum class SpellId : uint8_t {
    NONE = 0,
    HEAL, AQUA, CLEANSE,       // Waves
    STONE, FORTIFY,            // Terra
    SPARK, SWIFT,              // Tempus
    DRAIN, BLEED,              // Blood
    FLASH, SHADOW,             // Duality
    SPELL_COUNT
};

struct ItemDef {
    const char* name;
    bool is_equipment;
    bool combat_usable;
    int16_t hp_restore;
    int16_t mp_restore;
    uint8_t cures;
    int8_t atk_mod, def_mod, spd_mod, mag_mod, hp_max_mod, mp_max_mod;
};

struct SpellDef {
    const char* name;
    SyglType sygl;
    int8_t mp_cost;
    int8_t power;
    bool is_heal;
    bool targets_all;
    uint8_t inflicts;
};

extern const ItemDef  ITEM_TABLE[(int)ItemId::ITEM_COUNT];
extern const SpellDef SPELL_TABLE[(int)SpellId::SPELL_COUNT];

inline const ItemDef&  item_def(ItemId id)   { return ITEM_TABLE[(int)id]; }
inline const SpellDef& spell_def(SpellId id) { return SPELL_TABLE[(int)id]; }
