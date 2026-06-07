#include "bn_core.h"
#include "save_data.h"
#include "title_scene.h"
#include "overworld_scene.h"

int main()
{
    bn::core::init();
    save_data::init();

    // Title screen
    {
        TitleScene title(save_data::has_any_save());
        while (!title.done())
        {
            title.update();
            bn::core::update();
        }

        if (!title.chose_new_game())
        {
            // TODO: show save slot select and load
        }
        else
        {
            // Initialise a fresh slot 0
            SaveSlot fresh = {};
            fresh.level = 1;
            fresh.hp_max = 20;
            fresh.mp_max = 10;
            fresh.location_id = 1;
            save_data::write_slot(0, fresh);
        }
    }

    // Overworld
    {
        OverworldScene overworld;
        while (true)
        {
            overworld.update();
            bn::core::update();
        }
    }
}
