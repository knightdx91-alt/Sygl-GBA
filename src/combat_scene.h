#pragma once
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_vector.h"
#include "game_state.h"

enum class CombatResult { ONGOING, VICTORY, GAME_OVER };

class CombatScene {
public:
    CombatScene(GameState& state, EnemyType enemy_type);
    CombatResult update();

private:
    enum class Phase {
        PLAYER_TURN, MAGIC_SELECT, ITEM_SELECT,
        ENEMY_TURN, VICTORY, GAME_OVER
    };

    static int roll_damage(int atk, int def);
    static int roll_magic(int mag, int power);

    void render_all();
    void render_status(const char* msg);
    void render_main_menu();
    void render_magic_menu();
    void render_item_menu();
    void tick_status_effects();

    GameState&              _state;
    bn::sprite_text_generator _gen;
    EnemyType               _enemy_type;
    int                     _enemy_hp;
    int                     _enemy_hp_max;
    uint8_t                 _enemy_status = 0;

    bn::sprite_ptr          _player_sprite;
    bn::sprite_ptr          _enemy_sprite;

    Phase   _phase      = Phase::PLAYER_TURN;
    int     _cursor     = 0;
    int     _sub_cursor = 0;
    int     _delay      = 0;

    bn::vector<bn::sprite_ptr, 96> _sprites;
};
