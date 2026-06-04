#pragma once
#include <gba_types.h>

typedef struct {
    const char *name;
    u16 hp, hp_max;
    u16 atk, def, spd;
    u16 xp_reward;
} Enemy;

void battle_start(const Enemy *enemy);
void battle_update(void);
void battle_draw(void);
int  battle_is_over(void);
