"""Generate pixel-art 4bpp BMP assets for Sygl GBA using canonical palette."""
import struct, os, json

OUT = os.path.join(os.path.dirname(__file__), '..', 'assets', 'graphics')
os.makedirs(OUT, exist_ok=True)

# ── Palette (from "Games Pallets - Sygl Project") ────────────────────────────
TRANSPARENT   = (255,   0, 255)   # magenta = GBA transparent for sprites
VOID_DEEP     = (  8,   8,  16)   # #080810
TEMPLE_SHADOW = ( 32,  24,  32)   # #201820
ORPHAN_SLATE  = (112, 112, 120)   # #707078
MEM_FRAGMENT  = (184, 200, 216)   # #B8C8D8
ASCEND_WHITE  = (248, 248, 248)   # #F8F8F8
BLOOD_RED     = (168,   8,  16)   # #A80810
BRAZIER_GLOW  = (248, 176,  56)   # #F8B038
WAVES_BLUE    = ( 64, 128, 248)   # #4080F8
WAVES_LIGHT   = (168, 208, 255)   # lighter waves highlight
TERRA_EARTH   = (136,  96,  48)   # #886030
TERRA_DARK    = ( 80,  56,  24)   # darker terra shadow
TEMPUS_BOLT   = (232, 240,  24)   # #E8F018
DUAL_LIGHT    = (248, 248, 200)   # #F8F8C8
SKIN_MID      = (220, 170, 120)   # skin tone
SKIN_DARK     = (160, 110,  70)   # skin shadow
WOLF_FUR      = (148, 120,  80)   # wolf body
WOLF_DARK     = ( 80,  60,  35)   # wolf shadow
BANDIT_RED    = (192,  40,  40)   # bandit outfit
BANDIT_DARK   = (110,  20,  20)   # bandit shadow
NPC_GREEN     = ( 60, 160,  80)   # NPC 1 shirt
NPC_GOLD      = (200, 160,  40)   # NPC 2 shirt
NPC_PURPLE    = (160,  80, 200)   # NPC 3 shirt
STONE_FLOOR   = (100,  96,  88)   # room floor
STONE_GROUT   = ( 64,  60,  56)   # floor grout lines
WALL_MID      = (100,  72,  44)   # wall face (terra range)
WALL_DARK     = ( 60,  44,  24)   # wall mortar
WALL_LIGHT    = (140, 104,  64)   # wall highlight

# ── BMP writer ────────────────────────────────────────────────────────────────

def pack_row_4bpp(row_pixels, width):
    row = bytearray()
    for i in range(0, width, 2):
        hi = row_pixels[i] & 0xF
        lo = (row_pixels[i + 1] & 0xF) if i + 1 < width else 0
        row.append((hi << 4) | lo)
    while len(row) % 4:
        row.append(0)
    return bytes(row)

def write_4bpp_bmp(path, width, height, palette_rgb, pixels_toptobottom):
    assert len(palette_rgb) <= 16
    palette_rgb = list(palette_rgb) + [(0, 0, 0)] * (16 - len(palette_rgb))
    pixel_data = b''
    for row in reversed(pixels_toptobottom):
        pixel_data += pack_row_4bpp(row, width)
    ct = b''.join(struct.pack('BBBB', b, g, r, 0) for r, g, b in palette_rgb)
    pixel_offset = 14 + 40 + len(ct)
    file_size = pixel_offset + len(pixel_data)
    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, pixel_offset)
    dib = struct.pack('<IiiHHIIiiII',
        40, width, height, 1, 4, 0,
        len(pixel_data), 2835, 2835, 16, 16)
    with open(path, 'wb') as f:
        f.write(header + dib + ct + pixel_data)

def write_json(path, data):
    with open(path, 'w') as f:
        json.dump(data, f, indent=4)

# ── Sprite pixel art (16x16 top-down SD humanoid) ────────────────────────────
# Palette layout per sprite:
#  0=transparent  1=skin_mid  2=skin_dark  3=hair/accent
#  4=shirt        5=shirt_dark 6=pants     7=pants_dark
#  8=outline/void 9=white/light 10=detail   11=boot

def humanoid(hair, shirt, shirt_d, pants, pants_d, boot, detail):
    T=0; S=1; Sd=2; H=3; C=4; Cd=5; P=6; Pd=7; O=8; W=9; D=10; B=11
    # Top-down SD sprite: head top, body, legs visible from above+front
    return [
        [T, T, T, T, T, O, O, O, O, O, O, T, T, T, T, T],  # 0
        [T, T, T, O, O, S, S, S, S, S, S, O, O, T, T, T],  # 1
        [T, T, O, S, S, Sd,H, H, H, H,Sd, S, S, O, T, T],  # 2 hair
        [T, T, O, S, H, H, S, S, S, S, H, H, S, O, T, T],  # 3 hair sides
        [T, T, O, S, S, S, S, S, S, S, S, S, S, O, T, T],  # 4 face
        [T, T, O, S,Sd, D, S, S, S, S, D,Sd, S, O, T, T],  # 5 eyes
        [T, T, O, S, S, S, S, S, S, S, S, S, S, O, T, T],  # 6 face lower
        [T, O, O, O, C, C, C, C, C, C, C, C, O, O, O, T],  # 7 shoulders
        [T, O, C, C, C,Cd, C, C, C, C,Cd, C, C, C, O, T],  # 8 chest
        [T, O, C, C,Cd, C, C, C, C, C, C,Cd, C, C, O, T],  # 9 waist
        [T, O, P, P,Pd, P, O, T, T, O, P,Pd, P, P, O, T],  # 10 hips/legs
        [T, T, O, P, P,Pd, O, T, T, O,Pd, P, P, O, T, T],  # 11 upper leg
        [T, T, O, P, P, P, O, T, T, O, P, P, P, O, T, T],  # 12 lower leg
        [T, T, O,Pd, P, P, O, T, T, O, P, P,Pd, O, T, T],  # 13
        [T, T, O, B, B, B, O, T, T, O, B, B, B, O, T, T],  # 14 boots
        [T, T, T, O, O, O, T, T, T, T, O, O, O, T, T, T],  # 15
    ]

def make_humanoid_sprite(filename, hair_rgb, shirt_rgb, shirt_d_rgb,
                          pants_rgb, pants_d_rgb, boot_rgb, detail_rgb, json_extra=None):
    pal = [
        TRANSPARENT,    # 0
        SKIN_MID,       # 1
        SKIN_DARK,      # 2
        hair_rgb,       # 3
        shirt_rgb,      # 4
        shirt_d_rgb,    # 5
        pants_rgb,      # 6
        pants_d_rgb,    # 7
        VOID_DEEP,      # 8 outline
        ASCEND_WHITE,   # 9
        detail_rgb,     # 10
        boot_rgb,       # 11
    ]
    pix = humanoid(hair_rgb, shirt_rgb, shirt_d_rgb, pants_rgb, pants_d_rgb, boot_rgb, detail_rgb)
    write_4bpp_bmp(os.path.join(OUT, filename + '.bmp'), 16, 16, pal, pix)
    meta = {'type': 'sprite', 'height': 16}
    if json_extra:
        meta.update(json_extra)
    write_json(os.path.join(OUT, filename + '.json'), meta)

# Player — Waves sygl: blue shirt, dark hair, slate pants
make_humanoid_sprite('player',
    hair_rgb   = TEMPLE_SHADOW,
    shirt_rgb  = WAVES_BLUE,
    shirt_d_rgb= (32, 80, 180),
    pants_rgb  = ORPHAN_SLATE,
    pants_d_rgb= (72, 72, 80),
    boot_rgb   = TERRA_DARK,
    detail_rgb = WAVES_LIGHT)

# NPC 1 — green shirt, brown hair
make_humanoid_sprite('npc',
    hair_rgb   = TERRA_EARTH,
    shirt_rgb  = NPC_GREEN,
    shirt_d_rgb= (30, 110, 50),
    pants_rgb  = (80, 60, 40),
    pants_d_rgb= (55, 40, 25),
    boot_rgb   = VOID_DEEP,
    detail_rgb = BRAZIER_GLOW)

# NPC 2 — gold/merchant look
make_humanoid_sprite('npc2',
    hair_rgb   = ORPHAN_SLATE,
    shirt_rgb  = NPC_GOLD,
    shirt_d_rgb= (140, 110, 25),
    pants_rgb  = (90, 70, 30),
    pants_d_rgb= (65, 50, 20),
    boot_rgb   = TERRA_DARK,
    detail_rgb = ASCEND_WHITE)

# NPC 3 — purple/magical look
make_humanoid_sprite('npc3',
    hair_rgb   = VOID_DEEP,
    shirt_rgb  = NPC_PURPLE,
    shirt_d_rgb= (100, 45, 140),
    pants_rgb  = (60, 40, 80),
    pants_d_rgb= (40, 25, 60),
    boot_rgb   = (30, 20, 50),
    detail_rgb = DUAL_LIGHT)

# ── Enemy: Bandit (humanoid, red outfit) ────────────────────────────────────
make_humanoid_sprite('enemy_bandit',
    hair_rgb   = VOID_DEEP,
    shirt_rgb  = BANDIT_RED,
    shirt_d_rgb= BANDIT_DARK,
    pants_rgb  = (80, 30, 30),
    pants_d_rgb= (55, 18, 18),
    boot_rgb   = VOID_DEEP,
    detail_rgb = BRAZIER_GLOW)

# ── Enemy: Wolf (4-legged animal, top-down) ──────────────────────────────────
def wolf_pixels():
    T=0; F=1; Fd=2; O=3; E=4; N=5
    # Colour indices: 0=transp 1=fur 2=fur_dark 3=outline 4=eye 5=nose
    return [
        [T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T],  # 0
        [T, T, T, O, O, O, O, O, O, O, O, O, T, T, T, T],  # 1
        [T, T, O, F, F, F, F, F, F, F, F, F, O, T, T, T],  # 2 head
        [T, T, O, F,Fd, E, F, F, F, E,Fd, F, O, T, T, T],  # 3 eyes
        [T, T, O, F, F, F, F, N, F, F, F, F, O, T, T, T],  # 4 snout
        [T, O, F, F, F, F, F, F, F, F, F, F, F, O, T, T],  # 5 neck/shoulder
        [O, F, F,Fd, F, F, F, F, F, F, F, F,Fd, F, O, T],  # 6 body top
        [O, F, F, F, F, F, F, F, F, F, F, F, F, F, O, T],  # 7 body
        [O, F, F, F, F,Fd,Fd,Fd, F, F, F, F, F, F, O, T],  # 8 spine shadow
        [O, F, F,Fd, F, F, F, F, F, F, F, F,Fd, F, O, T],  # 9 body lower
        [T, O, F, F, F, F, F, F, F, F, F, F, F, O, T, T],  # 10 hips
        [T, O,Fd, F, O, T, T, T, T, T, O, F,Fd, O, T, T],  # 11 rear legs
        [T, O, F,Fd, O, T, T, T, T, T, O,Fd, F, O, T, T],  # 12
        [T, T, O, F, O, T, T, T, T, T, O, F, O, T, T, T],  # 13
        [T, T, O, O, T, T, T, T, T, T, T, O, O, T, T, T],  # 14 paws
        [T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T],  # 15
    ]

wolf_pal = [
    TRANSPARENT, WOLF_FUR, WOLF_DARK, VOID_DEEP,
    TEMPUS_BOLT,  # eye
    VOID_DEEP,    # nose
]
write_4bpp_bmp(os.path.join(OUT, 'enemy_wolf.bmp'), 16, 16, wolf_pal, wolf_pixels())
write_json(os.path.join(OUT, 'enemy_wolf.json'), {'type': 'sprite', 'height': 16})

# ── Room tilemap (256x256 = 32x32 tiles of 8x8) ──────────────────────────────
# Palette:
#  0=transparent/void  1=stone_floor  2=grout  3=wall_mid
#  4=wall_dark  5=wall_light

MAP_W = MAP_H = 32
TILE  = 8

# Floor tile: stone blocks with grout lines
def floor_tile():
    F = 1; G = 2
    # Stone slabs: 8x8 with grout on right edge + bottom edge alternating
    return [
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [G, G, G, G, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [G, G, G, G, G, G, G, G],
    ]

# Wall tile: brick pattern (running bond)
def wall_tile(offset=0):
    W = 3; D = 4; L = 5
    # Brick rows, alternating mortar offset
    row_A = [W, W, W, D, W, W, W, W]  # mortar at col 3
    row_B = [W, D, W, W, W, W, D, W]  # offset mortar
    top   = [L, L, L, L, L, L, L, L]  # top highlight
    mort  = [D, D, D, D, D, D, D, D]  # mortar row
    if offset == 0:
        return [top, row_A, row_A, mort, row_B, row_B, row_B, mort]
    else:
        return [top, row_B, row_B, mort, row_A, row_A, row_A, mort]

# Build pixel grid
pixels = [[0]*256 for _ in range(256)]
ft = floor_tile()

for tr in range(MAP_H):
    for tc in range(MAP_W):
        is_wall = (tr == 0 or tr == MAP_H-1 or tc == 0 or tc == MAP_W-1)
        tile = wall_tile((tr + tc) % 2) if is_wall else ft
        for r in range(TILE):
            for c in range(TILE):
                pixels[tr*TILE + r][tc*TILE + c] = tile[r][c]

room_pal = [
    VOID_DEEP,    # 0
    STONE_FLOOR,  # 1 floor
    STONE_GROUT,  # 2 grout
    WALL_MID,     # 3 wall face
    WALL_DARK,    # 4 wall mortar
    WALL_LIGHT,   # 5 wall highlight
]
write_4bpp_bmp(os.path.join(OUT, 'room.bmp'), 256, 256, room_pal, pixels)
write_json(os.path.join(OUT, 'room.json'), {'type': 'regular_bg'})

# ── Orphanage interior (256x256 = 32x32 tiles of 8x8) ────────────────────────
# Palette:
#  0=void  1=slate_floor  2=slate_grout  3=grey_wall  4=wall_dark  5=wall_light
#  6=desk_wood  7=desk_dark

SLATE_GROUT      = ( 88,  88,  96)
GREY_WALL_MID    = (128, 128, 136)
GREY_WALL_DARK   = ( 80,  80,  88)
GREY_WALL_LIGHT  = (168, 168, 176)

orphan_pal = [
    VOID_DEEP,        # 0
    ORPHAN_SLATE,     # 1 floor
    SLATE_GROUT,      # 2 grout
    GREY_WALL_MID,    # 3 wall face
    GREY_WALL_DARK,   # 4 wall mortar
    GREY_WALL_LIGHT,  # 5 wall highlight
    TERRA_EARTH,      # 6 desk wood
    TERRA_DARK,       # 7 desk shadow
]

def orphan_floor_tile():
    F = 1; G = 2
    return [
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [G, G, G, G, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [F, F, F, F, F, F, F, G],
        [G, G, G, G, G, G, G, G],
    ]

def orphan_wall_tile(offset=0):
    W = 3; D = 4; L = 5
    row_A = [W, W, W, D, W, W, W, W]
    row_B = [W, D, W, W, W, W, D, W]
    top   = [L, L, L, L, L, L, L, L]
    mort  = [D, D, D, D, D, D, D, D]
    if offset == 0:
        return [top, row_A, row_A, mort, row_B, row_B, row_B, mort]
    else:
        return [top, row_B, row_B, mort, row_A, row_A, row_A, mort]

def orphan_desk_tile():
    D = 6; Dd = 7
    return [
        [D,  D, D,  D,  D, D,  D,  D],
        [D, Dd, D,  D,  D, D, Dd,  D],
        [D,  D, D,  D,  D, D,  D,  D],
        [Dd,Dd, Dd, Dd, Dd,Dd, Dd, Dd],
        [D,  D, D,  D,  D, D,  D,  D],
        [D, Dd, D,  D,  D, D, Dd,  D],
        [D,  D, D,  D,  D, D,  D,  D],
        [Dd,Dd, Dd, Dd, Dd,Dd, Dd, Dd],
    ]

# Desk block: rows 7-8, cols 23-25 (obstacle near Mrs. Brown at tile 22,10)
DESK_TR_MIN, DESK_TR_MAX = 7, 8
DESK_TC_MIN, DESK_TC_MAX = 23, 25

oph_pixels = [[0]*256 for _ in range(256)]
oft  = orphan_floor_tile()
odt  = orphan_desk_tile()

for tr in range(MAP_H):
    for tc in range(MAP_W):
        is_border = (tr == 0 or tr == MAP_H-1 or tc == 0 or tc == MAP_W-1)
        is_desk   = (DESK_TR_MIN <= tr <= DESK_TR_MAX and
                     DESK_TC_MIN <= tc <= DESK_TC_MAX)
        if is_border:
            tile = orphan_wall_tile((tr + tc) % 2)
        elif is_desk:
            tile = odt
        else:
            tile = oft
        for r in range(TILE):
            for c in range(TILE):
                oph_pixels[tr*TILE + r][tc*TILE + c] = tile[r][c]

write_4bpp_bmp(os.path.join(OUT, 'orphanage_interior.bmp'), 256, 256, orphan_pal, oph_pixels)
write_json(os.path.join(OUT, 'orphanage_interior.json'), {'type': 'regular_bg'})

print("Assets generated in", os.path.abspath(OUT))
