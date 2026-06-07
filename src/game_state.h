#pragma once
#include <cstdint>
#include "party.h"

enum class EnemyType { BANDIT, WOLF };

struct EnemyStats {
    const char* name;
    int hp_max, atk, def, spd, xp_reward;
};

static constexpr EnemyStats ENEMY_TABLE[] = {
    { "Bandit", 12, 4, 1, 6, 5 },
    { "Wolf",    8, 5, 0, 10, 4 },
};

inline const EnemyStats& enemy_stats(EnemyType t) {
    return ENEMY_TABLE[(int)t];
}

struct GameState {
    int  tile_x = 16;
    int  tile_y = 16;
    bool enemy_alive[2] = { true, true };
    Party party;

    GameState() { party.init_default(); }
};
