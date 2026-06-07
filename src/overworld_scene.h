#pragma once
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "text_box.h"

class OverworldScene
{
public:
    OverworldScene();

    void update();

private:
    void update_player();
    void update_npc_interaction();
    void render_hud();

    static constexpr int STEP = 8;
    static constexpr int LEFT_BOUND  = -96;
    static constexpr int RIGHT_BOUND =  96;
    static constexpr int TOP_BOUND   = -56;
    static constexpr int BOT_BOUND   =  40; // leave bottom row for text box

    static constexpr int NPC_X = 40;
    static constexpr int NPC_Y = 0;
    static constexpr int INTERACT_DIST = 20;

    bn::sprite_text_generator _gen;

    // Player character sprite(s)
    bn::vector<bn::sprite_ptr, 8> _player_sprites;
    bn::fixed _px, _py;

    // NPC sprite(s)
    bn::vector<bn::sprite_ptr, 8> _npc_sprites;

    // Prompt "Press A"
    bn::vector<bn::sprite_ptr, 16> _prompt_sprites;
    bool _prompt_visible = false;

    // Pause overlay
    bn::vector<bn::sprite_ptr, 32> _pause_sprites;
    bool _paused = false;

    TextBox _text_box;
};
