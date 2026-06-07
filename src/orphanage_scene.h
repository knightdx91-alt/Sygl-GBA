#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "text_box.h"
#include "choice_box.h"
#include "game_state.h"

enum class OrphanageResult { STAY, LEAVE_FOR_SCHOOL };

class OrphanageScene
{
public:
    explicit OrphanageScene(GameState& state);
    OrphanageResult update();

private:
    void try_move(int dx, int dy);
    bool tile_walkable(int tx, int ty) const;
    void screen_pos(int tx, int ty, int& sx, int& sy) const;
    void check_npc_proximity();
    void update_dialogue();
    void open_npc_dialogue(int npc_id);

    GameState& _state;
    bn::sprite_text_generator _gen;
    bn::regular_bg_ptr _bg;
    int _floor_tile_index = 0;

    bn::optional<bn::sprite_ptr> _player_sprite;

    // NPC 0 = Mrs. Brown  NPC 1 = orphan kid
    static constexpr int NPC_COUNT = 2;
    static constexpr int NPC_TILE_X[NPC_COUNT] = { 22, 10 };
    static constexpr int NPC_TILE_Y[NPC_COUNT] = { 10, 20 };
    bn::optional<bn::sprite_ptr> _npc_sprite[NPC_COUNT];

    bn::vector<bn::sprite_ptr, 16> _prompt_sprites;
    int _near_npc = -1;

    TextBox   _text_box;
    ChoiceBox _choice_box;
    int  _active_npc     = -1;
    int  _choice_result  = -1;
    bool _brown_complete = false;
    int  _leave_timer    = 0;

    int _move_timer = 0;
    int _move_dx    = 0;
    int _move_dy    = 0;
};
