#pragma once
#include <gba_types.h>

void tiles_init(void);  /* load palettes + tile graphics into VRAM */
void oam_clear(void);   /* zero all OAM entries (hide sprites)     */
