#include "bn_core.h"
#include "bn_bg_palettes.h"
#include "bn_color.h"
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "common_variable_8x16_sprite_font.h"
#include "save_data.h"
#include "title_scene.h"
#include "overworld_scene.h"
#include "combat_scene.h"
#include "game_state.h"

static void show_game_over(bn::sprite_text_generator& gen)
{
    bn::vector<bn::sprite_ptr, 32> sprites;
    bn::bg_palettes::set_transparent_color(bn::color(0, 0, 0));
    gen.set_center_alignment();
    gen.generate(0, -20, bn::string_view("GAME OVER"), sprites);
    gen.generate(0,   4, bn::string_view("Press A to return"), sprites);

    while (!bn::keypad::a_pressed())
        bn::core::update();
}

int main()
{
    bn::core::init();
    save_data::init();

    GameState state;

    // ── Title screen ──────────────────────────────────────────────────────────
    {
        TitleScene title(save_data::has_any_save());
        while (!title.done())
        {
            title.update();
            bn::core::update();
        }

        if (title.chose_new_game())
        {
            SaveSlot fresh = {};
            fresh.level = 1;
            fresh.hp_max = 20;
            fresh.mp_max = 10;
            fresh.location_id = 1;
            save_data::write_slot(0, fresh);
        }
        // TODO: load slot select for Continue
    }

    // ── Main game loop ────────────────────────────────────────────────────────
    while (true)
    {
        // Overworld
        EnemyType combat_enemy = EnemyType::BANDIT;
        int combat_enemy_idx = -1;
        {
            OverworldScene overworld(state);
            OverworldResult result = OverworldResult::STAY;
            while (result == OverworldResult::STAY)
            {
                result = overworld.update();
                bn::core::update();
            }
            if (result == OverworldResult::COMBAT)
            {
                combat_enemy = overworld.pending_enemy();
                // find which index we're fighting
                for (int i = 0; i < 2; ++i)
                {
                    if (state.tile_x == OverworldScene::ENEMY_TILE_X[i] &&
                        state.tile_y == OverworldScene::ENEMY_TILE_Y[i])
                    {
                        combat_enemy_idx = i;
                        break;
                    }
                }
            }
        }
        // Overworld is now destroyed; all its VRAM is freed

        if (combat_enemy_idx < 0) continue; // shouldn't happen

        // Combat
        bool player_alive = true;
        {
            CombatScene combat(state, combat_enemy);
            CombatResult result = CombatResult::ONGOING;
            while (result == CombatResult::ONGOING)
            {
                result = combat.update();
                bn::core::update();
            }
            if (result == CombatResult::VICTORY)
                state.enemy_alive[combat_enemy_idx] = false;
            else
                player_alive = false;
        }

        if (!player_alive)
        {
            bn::sprite_text_generator gen(common::variable_8x16_sprite_font);
            show_game_over(gen);
            // Reset state and restart
            state = GameState{};
            {
                TitleScene title(false);
                while (!title.done())
                {
                    title.update();
                    bn::core::update();
                }
            }
        }
        // Loop back to overworld
    }
}
