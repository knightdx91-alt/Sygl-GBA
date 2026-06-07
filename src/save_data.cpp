#include "save_data.h"
#include "bn_sram.h"

namespace
{
    SramData g_sram;
    bool g_loaded = false;

    uint32_t compute_checksum(const SaveSlot& slot)
    {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&slot);
        uint32_t sum = 0;
        // skip the first 8 bytes (tag + checksum fields)
        for (int i = 8; i < (int)sizeof(SaveSlot); ++i)
            sum += bytes[i];
        return sum;
    }
}

namespace save_data
{
    void init()
    {
        bn::sram::read(g_sram);
        g_loaded = true;
    }

    bool has_any_save()
    {
        if (!g_loaded) init();
        if (g_sram.global_tag != SRAM_TAG) return false;
        for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        {
            if (g_sram.slots[i].tag == SRAM_TAG &&
                g_sram.slots[i].checksum == compute_checksum(g_sram.slots[i]))
                return true;
        }
        return false;
    }

    bool load_slot(int index, SaveSlot& out)
    {
        if (!g_loaded) init();
        if (index < 0 || index >= SAVE_SLOT_COUNT) return false;
        const SaveSlot& s = g_sram.slots[index];
        if (s.tag != SRAM_TAG) return false;
        if (s.checksum != compute_checksum(s)) return false;
        out = s;
        return true;
    }

    void write_slot(int index, const SaveSlot& slot)
    {
        if (index < 0 || index >= SAVE_SLOT_COUNT) return;
        g_sram.global_tag = SRAM_TAG;
        g_sram.slots[index] = slot;
        g_sram.slots[index].tag = SRAM_TAG;
        g_sram.slots[index].checksum = compute_checksum(g_sram.slots[index]);
        bn::sram::write(g_sram);
    }
}
