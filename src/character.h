#pragma once
#include "item_data.h"

struct Character {
    char      name[12] = {};
    SyglType  sygl     = SyglType::NONE;
    bool      active   = false;

    int level   = 1;
    int xp      = 0;
    int xp_next = 10;

    int hp      = 20;
    int hp_max  = 20;
    int mp      = 10;
    int mp_max  = 10;
    int atk     = 5;
    int def     = 2;
    int spd     = 8;
    int mag     = 4;

    ItemId weapon    = ItemId::NONE;
    ItemId armor     = ItemId::NONE;
    ItemId accessory = ItemId::NONE;

    static constexpr int MAX_SPELLS = 8;
    SpellId known_spells[MAX_SPELLS] = {};
    int     spell_count = 0;

    uint8_t status = 0;
    bool    familiar_ready = true;

    bool is_alive() const { return hp > 0; }
    bool has_status(StatusFlag f) const { return (status & static_cast<uint8_t>(f)) != 0; }
    void apply_status(StatusFlag f)     { status |= static_cast<uint8_t>(f); }
    void clear_status(StatusFlag f)     { status &= ~static_cast<uint8_t>(f); }
    void clear_all_status()             { status = 0; }

    int eff_atk() const;
    int eff_def() const;
    int eff_spd() const;
    int eff_mag() const;
    int eff_hp_max() const;
    int eff_mp_max() const;

    void learn_spell(SpellId id);
    bool knows_spell(SpellId id) const;
    void gain_xp(int amount);
    void equip(ItemId id);
};
