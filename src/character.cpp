#include "character.h"

int Character::eff_atk() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).atk_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).atk_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).atk_mod;
    return atk + bonus;
}

int Character::eff_def() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).def_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).def_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).def_mod;
    return def + bonus;
}

int Character::eff_spd() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).spd_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).spd_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).spd_mod;
    return spd + bonus;
}

int Character::eff_mag() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).mag_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).mag_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).mag_mod;
    return mag + bonus;
}

int Character::eff_hp_max() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).hp_max_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).hp_max_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).hp_max_mod;
    return hp_max + bonus;
}

int Character::eff_mp_max() const
{
    int bonus = 0;
    if (weapon    != ItemId::NONE) bonus += item_def(weapon).mp_max_mod;
    if (armor     != ItemId::NONE) bonus += item_def(armor).mp_max_mod;
    if (accessory != ItemId::NONE) bonus += item_def(accessory).mp_max_mod;
    return mp_max + bonus;
}

void Character::learn_spell(SpellId id)
{
    if (spell_count >= MAX_SPELLS) return;
    if (knows_spell(id)) return;
    known_spells[spell_count++] = id;
}

bool Character::knows_spell(SpellId id) const
{
    for (int i = 0; i < spell_count; ++i)
        if (known_spells[i] == id) return true;
    return false;
}

void Character::gain_xp(int amount)
{
    xp += amount;
    while (xp >= xp_next)
    {
        xp -= xp_next;
        level++;
        xp_next = xp_next + xp_next / 2;
        hp_max  += 5;
        mp_max  += 3;
        atk     += 1;
        def     += 1;
        hp = hp_max;  // full heal on level up
    }
}

void Character::equip(ItemId id)
{
    if (id == ItemId::NONE) return;
    const ItemDef& def_ref = item_def(id);
    if (!def_ref.is_equipment) return;

    // Simple heuristic: check primary stat bonus
    if (def_ref.spd_mod > 0)
        accessory = id;
    else if (def_ref.def_mod > 0)
        armor = id;
    else
        weapon = id;
}
