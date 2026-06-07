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

// Rooms within the orphanage
enum class ORoom { CORRIDOR, OFFICE, ROOFTOP };

class OrphanageScene
{
public:
    explicit OrphanageScene(GameState& state);
    OrphanageResult update();

private:
    void try_move(int dx, int dy);
    bool tile_walkable(int tx, int ty) const;
    void screen_pos(int tx, int ty, int& sx, int& sy) const;

    void switch_room(ORoom to, int px, int py);
    void check_transitions();

    void check_npc_proximity();
    void update_dialogue();
    void open_npc_dialogue(int npc_id);

    GameState& _state;
    bn::sprite_text_generator _gen;

    ORoom _room = ORoom::CORRIDOR;
    bn::optional<bn::regular_bg_ptr> _bg;
    int _floor_tile_index = 0;

    bn::optional<bn::sprite_ptr> _player_sprite;

    // Up to 2 NPCs per room; populated per-room in switch_room()
    static constexpr int MAX_NPCS = 2;
    bn::optional<bn::sprite_ptr> _npc_sprite[MAX_NPCS];
    int _npc_tile_x[MAX_NPCS] = {};
    int _npc_tile_y[MAX_NPCS] = {};
    int _npc_count = 0;

    bn::vector<bn::sprite_ptr, 16> _prompt_sprites;
    int _near_npc = -1;

    TextBox   _text_box;
    ChoiceBox _choice_box;
    int  _active_npc    = -1;
    int  _choice_result = -1;

    // Story flags
    bool _rooftop_seen   = false; // player witnessed Bal fly
    bool _brown_complete = false; // Mrs. Brown full dialogue done
    int  _leave_timer    = 0;

    // Cutscene mode (text box plays; movement disabled)
    bool _cutscene_active = false;

    int _move_timer = 0;
    int _move_dx    = 0;
    int _move_dy    = 0;
};
