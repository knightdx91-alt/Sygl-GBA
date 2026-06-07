#include "item_data.h"

static constexpr uint8_t CURE_POISON_BURN =
    static_cast<uint8_t>(StatusFlag::POISON) |
    static_cast<uint8_t>(StatusFlag::BURN);

const ItemDef ITEM_TABLE[(int)ItemId::ITEM_COUNT] = {
    // NONE
    { "", false, false, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // HERB
    { "Herb",         false, true,  20, 0, 0,  0, 0, 0, 0, 0, 0 },
    // ETHER
    { "Ether",        false, true,  0, 10, 0,  0, 0, 0, 0, 0, 0 },
    // ANTIDOTE
    { "Antidote",     false, true,  0, 0, CURE_POISON_BURN, 0, 0, 0, 0, 0, 0 },
    // IRON_SWORD
    { "Iron Sword",   true,  false, 0, 0, 0,   3, 0, 0, 0, 0, 0 },
    // SHORT_STAFF
    { "Short Staff",  true,  false, 0, 0, 0,   1, 0, 0, 2, 0, 0 },
    // LEATHER_ARMOR
    { "Leather Armor",true,  false, 0, 0, 0,   0, 3, 0, 0, 0, 0 },
    // ROBE
    { "Robe",         true,  false, 0, 0, 0,   0, 1, 0, 0, 0, 3 },
    // SPEED_RING
    { "Speed Ring",   true,  false, 0, 0, 0,   0, 0, 2, 0, 0, 0 },
    // POWER_RING
    { "Power Ring",   true,  false, 0, 0, 0,   2, 0, 0, 0, 0, 0 },
};

const SpellDef SPELL_TABLE[(int)SpellId::SPELL_COUNT] = {
    // NONE
    { "", SyglType::NONE, 0, 0, false, false, 0 },
    // HEAL
    { "Heal",    SyglType::WAVES,  8, 25, true,  false, 0 },
    // AQUA
    { "Aqua",    SyglType::WAVES,  6, 18, false, false, 0 },
    // CLEANSE
    { "Cleanse", SyglType::WAVES,  4,  0, true,  false, 0 },
    // STONE
    { "Stone",   SyglType::TERRA,  6, 15, false, false, 0 },
    // FORTIFY
    { "Fortify", SyglType::TERRA,  8,  0, true,  false, 0 },
    // SPARK
    { "Spark",   SyglType::TEMPUS, 6, 20, false, false, 0 },
    // SWIFT
    { "Swift",   SyglType::TEMPUS, 4,  0, true,  false, 0 },
    // DRAIN
    { "Drain",   SyglType::BLOOD,  8, 14, false, false, 0 },
    // BLEED
    { "Bleed",   SyglType::BLOOD,  5, 10, false, false,
      static_cast<uint8_t>(StatusFlag::POISON) },
    // FLASH
    { "Flash",   SyglType::DUALITY, 6, 16, false, false, 0 },
    // SHADOW
    { "Shadow",  SyglType::DUALITY, 7, 14, false, false,
      static_cast<uint8_t>(StatusFlag::SILENCE) },
};
