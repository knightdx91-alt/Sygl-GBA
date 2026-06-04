#pragma once
#include <gba_types.h>

typedef enum {
    SYGL_NONE = -1,
    SYGL_FIRE  = 0,
    SYGL_WATER = 1,
    SYGL_EARTH = 2,
    SYGL_AIR   = 3,
    SYGL_VOID  = 4,
} SyglType;

typedef struct {
    s16 map_x, map_y;
    s16 px, py;
    u8  dir;
    u8  move_timer;
    SyglType sygl;
    u16 hp, hp_max;
    u16 mp, mp_max;
    u16 atk, def, spd;
    u16 level, xp;
} Player;

extern Player player;

void player_init(void);
void player_update(void);
void player_draw(void);
void player_set_sygl(SyglType sygl);
