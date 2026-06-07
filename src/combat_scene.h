#pragma once
#include "bn_vector.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_optional.h"
#include "game_state.h"

enum class CombatResult { ONGOING, VICTORY, GAME_OVER };

class CombatScene
{
public:
    CombatScene(GameState& state, EnemyType enemy_type);

    CombatResult update();

private:
    enum class Phase { PLAYER_TURN, ENEMY_TURN, VICTORY, GAME_OVER };

    void render_all();
    void render_status(const char* msg);
    int roll_damage(int atk, int def);

    GameState& _state;
    bn::sprite_text_generator _gen;

    EnemyType _enemy_type;
    int _enemy_hp;
    int _enemy_hp_max;
    int _cursor = 0;      // 0=Attack, 1=Flee

    Phase _phase = Phase::PLAYER_TURN;
    int _delay = 0;       // frames to wait before next action

    bn::vector<bn::sprite_ptr, 128> _sprites;

    bn::sprite_ptr _player_sprite;
    bn::sprite_ptr _enemy_sprite;
};
