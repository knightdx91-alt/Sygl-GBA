#pragma once
#include <cstdint>

static constexpr uint32_t SRAM_TAG = 0x5359474Cu; // "SYGL"
static constexpr int SAVE_SLOT_COUNT = 5;

struct SaveSlot
{
    uint32_t tag;
    uint32_t checksum;
    char player_name[12];
    uint8_t sygl_id;          // 0=none 1=Duality 2=Waves 3=Terra 4=Tempus 5=Blood
    uint8_t level;
    uint16_t hp_max;
    uint16_t mp_max;
    int8_t loyalty_bal;
    int8_t loyalty_originator;
    int8_t self_interest;
    uint8_t pad;
    uint16_t location_id;
    uint16_t play_time_hours;
    uint8_t endings_bitfield; // bit 0-4: Canon,Pyrrhic,Betrayal,Peacemaker,Ascension
    bool ng_plus_active;
    uint8_t reserved[20];
};

struct SramData
{
    uint32_t global_tag;
    uint8_t reserved[28];
    SaveSlot slots[SAVE_SLOT_COUNT];
};

namespace save_data
{
    void init();
    bool has_any_save();
    bool load_slot(int index, SaveSlot& out);
    void write_slot(int index, const SaveSlot& slot);
}
