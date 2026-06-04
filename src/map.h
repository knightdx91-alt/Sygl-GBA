#pragma once
#include <gba_types.h>

#define MAP_COLS 32
#define MAP_ROWS 32

#define TILE_GRASS   0
#define TILE_STONE   1
#define TILE_WATER   2
#define TILE_WALL    3
#define TILE_DOOR    4
#define TILE_TREE    5

void map_init(void);
void map_draw(void);
int  map_is_solid(s16 x, s16 y);
