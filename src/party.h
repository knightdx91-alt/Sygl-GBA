#pragma once
#include "character.h"

struct InventorySlot {
    ItemId  id  = ItemId::NONE;
    uint8_t qty = 0;
};

struct Inventory {
    static constexpr int CAPACITY = 24;
    InventorySlot slots[CAPACITY] = {};
    int count = 0;

    bool add(ItemId id, int qty = 1);
    bool remove(ItemId id, int qty = 1);
    int  qty_of(ItemId id) const;
    bool has(ItemId id) const { return qty_of(id) > 0; }
};

struct Party {
    static constexpr int MAX_ACTIVE  = 4;
    static constexpr int MAX_RESERVE = 5;
    static constexpr int MAX_TOTAL   = MAX_ACTIVE + MAX_RESERVE;

    Character members[MAX_TOTAL] = {};
    int active_count  = 0;
    int reserve_count = 0;

    Inventory inventory;

    Character&       active(int i)       { return members[i]; }
    const Character& active(int i) const { return members[i]; }
    Character&       player()            { return members[0]; }
    const Character& player()      const { return members[0]; }

    bool all_defeated() const;
    void reset_familiars();
    void init_default();   // set up starting player character
};
