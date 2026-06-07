#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "text_box.h"
#include "choice_box.h"
#include "game_state.h"

enum class OverworldResult { STAY, COMBAT, QUIT };

class OverworldScene
{
public:
    explicit OverworldScene(GameState& state);

    OverworldResult update();
    EnemyType pending_enemy() const { return _pending_enemy; }

private:
    void try_move(int dx, int dy);
    bool tile_walkable(int tx, int ty) const;
    void screen_pos(int tx, int ty, int& sx, int& sy) const;

    void check_npc_proximity();
    void update_dialogue();
    void open_npc_dialogue(int npc_id);
    void open_choice_for_npc(int npc_id);

    OverworldResult check_enemy_contact();
    void render_hud();

    GameState& _state;
    bn::sprite_text_generator _gen;

    // Tilemap background
    bn::regular_bg_ptr _bg;
    int _floor_tile_index = 0;

    // Player sprite
    bn::optional<bn::sprite_ptr> _player_sprite;

    // NPCs (3 total) — sprite + tile position
    static constexpr int NPC_COUNT = 3;
    static constexpr int NPC_TILE_X[NPC_COUNT] = {  8, 16, 24 };
    static constexpr int NPC_TILE_Y[NPC_COUNT] = { 12,  8, 12 };
    bn::optional<bn::sprite_ptr> _npc_sprite[NPC_COUNT];

    // Enemies (2 types, 2 placements) — public for main.cpp lookup
    static constexpr int ENEMY_COUNT = 2;
public:
    static constexpr int ENEMY_TILE_X[ENEMY_COUNT] = { 10, 22 };
    static constexpr int ENEMY_TILE_Y[ENEMY_COUNT] = { 20, 20 };
private:
    static constexpr EnemyType ENEMY_TYPE[ENEMY_COUNT] = {
        EnemyType::BANDIT, EnemyType::WOLF };
    bn::optional<bn::sprite_ptr> _enemy_sprite[ENEMY_COUNT];

    // Interaction prompt
    bn::vector<bn::sprite_ptr, 16> _prompt_sprites;
    int _near_npc = -1;

    // Dialogue state
    TextBox _text_box;
    ChoiceBox _choice_box;
    int _active_npc = -1;
    int _choice_result = -1;

    // HUD (level / HP)
    bn::vector<bn::sprite_ptr, 32> _hud_sprites;
    int _last_hp = -1;

    // Movement repeat (held directional)
    int _move_timer  = 0;
    int _move_dx     = 0;
    int _move_dy     = 0;

    // Pause / start menu
    bool _paused     = false;
    int  _pause_page = 0;   // 0 = Stats, 1 = Items
    bn::vector<bn::sprite_ptr, 64> _pause_sprites;
    void open_pause_menu();
    void close_pause_menu();
    void render_pause_page();

    // Combat transition
    EnemyType _pending_enemy = EnemyType::BANDIT;
};
