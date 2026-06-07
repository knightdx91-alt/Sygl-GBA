#pragma once
#include <cstdint>

enum class EnemyType { BANDIT, WOLF };

struct EnemyStats
{
    const char* name;
    int hp_max, atk, def, spd, xp_reward;
};

static constexpr EnemyStats ENEMY_TABLE[] = {
    { "Bandit", 12, 4, 1, 6, 5  },  // BANDIT
    { "Wolf",   8,  5, 0, 10, 4 },  // WOLF
};

inline const EnemyStats& enemy_stats(EnemyType t)
{
    return ENEMY_TABLE[(int)t];
}

struct GameState
{
    // Player stats
    int level    = 1;
    int hp       = 20;
    int hp_max   = 20;
    int mp       = 10;
    int mp_max   = 10;
    int atk      = 5;
    int def      = 2;
    int spd      = 8;
    int xp       = 0;
    int xp_next  = 10;

    // Overworld position (tile coords)
    int tile_x = 16;
    int tile_y = 16;

    // Enemy alive flags (indexed by placement order in overworld)
    bool enemy_alive[4] = { true, true, true, true };

    void gain_xp(int amount)
    {
        xp += amount;
        while (xp >= xp_next)
        {
            xp -= xp_next;
            level++;
            xp_next = xp_next + xp_next / 2;
            hp_max  += 5;
            mp_max  += 3;
            atk     += 1;
            def     += 1;
            hp = hp_max;  // full heal on level up
        }
    }
};
