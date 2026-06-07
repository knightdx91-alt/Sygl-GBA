#include "party.h"

// ── Inventory ─────────────────────────────────────────────────────────────────

bool Inventory::add(ItemId id, int qty)
{
    // Try to stack on existing slot
    for (int i = 0; i < count; ++i)
    {
        if (slots[i].id == id)
        {
            slots[i].qty = (uint8_t)(slots[i].qty + qty);
            return true;
        }
    }
    // New slot
    if (count >= CAPACITY) return false;
    slots[count].id  = id;
    slots[count].qty = (uint8_t)qty;
    ++count;
    return true;
}

bool Inventory::remove(ItemId id, int qty)
{
    for (int i = 0; i < count; ++i)
    {
        if (slots[i].id == id)
        {
            if (slots[i].qty < (uint8_t)qty) return false;
            slots[i].qty -= (uint8_t)qty;
            if (slots[i].qty == 0)
            {
                // Compact array
                for (int j = i; j < count - 1; ++j)
                    slots[j] = slots[j + 1];
                slots[count - 1] = {};
                --count;
            }
            return true;
        }
    }
    return false;
}

int Inventory::qty_of(ItemId id) const
{
    int total = 0;
    for (int i = 0; i < count; ++i)
        if (slots[i].id == id) total += slots[i].qty;
    return total;
}

// ── Party ─────────────────────────────────────────────────────────────────────

bool Party::all_defeated() const
{
    for (int i = 0; i < active_count; ++i)
        if (members[i].hp > 0) return false;
    return true;
}

void Party::reset_familiars()
{
    for (int i = 0; i < MAX_TOTAL; ++i)
        members[i].familiar_ready = true;
}

void Party::init_default()
{
    // Reset all members
    for (int i = 0; i < MAX_TOTAL; ++i)
        members[i] = Character{};

    Character& p = members[0];

    // Name
    const char* n = "Hero";
    int ni = 0;
    while (n[ni] && ni < 11) { p.name[ni] = n[ni]; ++ni; }
    p.name[ni] = '\0';

    p.sygl    = SyglType::WAVES;
    p.active  = true;
    p.level   = 1;
    p.xp      = 0;
    p.xp_next = 10;

    p.hp      = 20;
    p.hp_max  = 20;
    p.mp      = 12;
    p.mp_max  = 12;
    p.atk     = 5;
    p.def     = 2;
    p.spd     = 8;
    p.mag     = 6;

    p.learn_spell(SpellId::HEAL);
    p.learn_spell(SpellId::AQUA);

    active_count  = 1;
    reserve_count = 0;

    inventory = Inventory{};
    inventory.add(ItemId::HERB,  2);
    inventory.add(ItemId::ETHER, 1);
}
