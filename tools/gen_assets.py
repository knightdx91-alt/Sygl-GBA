"""Generate placeholder 4bpp BMP assets for Sygl GBA."""
import struct, os, json

OUT = os.path.join(os.path.dirname(__file__), '..', 'assets', 'graphics')
os.makedirs(OUT, exist_ok=True)

TRANSPARENT = (255, 0, 255)  # magenta = GBA transparent for sprites

def pack_row_4bpp(row_pixels, width):
    """Pack a list of palette indices into 4bpp bytes. High nibble = left pixel."""
    row = bytearray()
    for i in range(0, width, 2):
        hi = row_pixels[i] & 0xF
        lo = (row_pixels[i + 1] & 0xF) if i + 1 < width else 0
        row.append((hi << 4) | lo)
    # Pad to 4-byte boundary
    while len(row) % 4:
        row.append(0)
    return bytes(row)

def write_4bpp_bmp(path, width, height, palette_rgb, pixels_toptobottom):
    """
    palette_rgb: list of (r,g,b), exactly 16 entries
    pixels_toptobottom: list of rows (top first), each row a list of indices
    """
    assert len(palette_rgb) <= 16
    palette_rgb = list(palette_rgb) + [(0,0,0)] * (16 - len(palette_rgb))

    pixel_data = b''
    for row in reversed(pixels_toptobottom):  # BMP = bottom-to-top
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

# ── Sprites ──────────────────────────────────────────────────────────────────

def sprite_pixels(body_idx, outline_idx, accent_idx):
    """16x16 simple humanoid placeholder. 0=transparent."""
    T, B, O, A = 0, body_idx, outline_idx, accent_idx
    return [
        [T,T,T,T,T,T,O,O,O,O,T,T,T,T,T,T],  # 0 head top
        [T,T,T,T,T,O,B,B,B,B,O,T,T,T,T,T],  # 1
        [T,T,T,T,T,O,B,A,A,B,O,T,T,T,T,T],  # 2 eyes
        [T,T,T,T,T,O,B,B,B,B,O,T,T,T,T,T],  # 3 head bot
        [T,T,T,O,O,O,B,B,B,B,O,O,O,T,T,T],  # 4 shoulders
        [T,T,O,B,B,B,B,B,B,B,B,B,B,O,T,T],  # 5 body
        [T,T,O,B,B,B,B,B,B,B,B,B,B,O,T,T],  # 6
        [T,T,O,B,B,B,B,B,B,B,B,B,B,O,T,T],  # 7
        [T,T,O,B,B,B,B,B,B,B,B,B,B,O,T,T],  # 8
        [T,T,T,O,B,B,O,T,T,O,B,B,O,T,T,T],  # 9 hips
        [T,T,T,O,B,B,O,T,T,O,B,B,O,T,T,T],  # 10 legs
        [T,T,T,O,B,B,O,T,T,O,B,B,O,T,T,T],  # 11
        [T,T,T,O,B,B,O,T,T,O,B,B,O,T,T,T],  # 12
        [T,T,T,O,B,B,O,T,T,O,B,B,O,T,T,T],  # 13
        [T,T,T,O,B,B,B,O,O,B,B,B,O,T,T,T],  # 14 feet
        [T,T,T,T,O,O,O,T,T,T,O,O,O,T,T,T],  # 15
    ]

def make_sprite(filename, body_rgb, outline_rgb, accent_rgb):
    pal = [TRANSPARENT, body_rgb, outline_rgb, accent_rgb]
    pix = sprite_pixels(1, 2, 3)
    write_4bpp_bmp(os.path.join(OUT, filename + '.bmp'), 16, 16, pal, pix)
    write_json(os.path.join(OUT, filename + '.json'),
               {'type': 'sprite', 'height': 16})

make_sprite('player',
            (50, 100, 200), (20, 55, 130), (180, 220, 255))
make_sprite('npc',
            (40, 160, 60), (20, 90, 30), (180, 255, 190))
make_sprite('npc2',
            (180, 140, 40), (110, 80, 20), (255, 220, 120))
make_sprite('npc3',
            (150, 60, 200), (80, 30, 120), (220, 160, 255))
make_sprite('enemy_bandit',
            (200, 40, 40), (120, 20, 20), (255, 160, 160))
make_sprite('enemy_wolf',
            (140, 100, 60), (80, 55, 30), (220, 190, 150))

# ── Room tilemap ──────────────────────────────────────────────────────────────
# 256x256 pixels = 32x32 tiles of 8x8 each
# Palette[0]: bg fill (near-black)
# Palette[1]: floor (dark navy)
# Palette[2]: wall  (dark stone/brown)
# Palette[3]: wall accent

MAP_W_TILES = 32
MAP_H_TILES = 32
TILE_PX = 8
FLOOR = 1
WALL = 2
WALL_ACC = 3

def tile_8x8(index):
    """Return a solid 8x8 block of palette index."""
    return [[index]*8 for _ in range(8)]

def wall_tile_8x8():
    """Wall tile with a slight accent border."""
    row = [[WALL]*8 for _ in range(8)]
    for i in range(8):
        row[0][i] = WALL_ACC
        row[i][0] = WALL_ACC
    return row

# Build the full 256x256 pixel map
pixels = [[0]*256 for _ in range(256)]

for tile_row in range(MAP_H_TILES):
    for tile_col in range(MAP_W_TILES):
        is_wall = (tile_row == 0 or tile_row == MAP_H_TILES - 1 or
                   tile_col == 0 or tile_col == MAP_W_TILES - 1)
        tile = wall_tile_8x8() if is_wall else tile_8x8(FLOOR)
        for r in range(TILE_PX):
            for c in range(TILE_PX):
                pixels[tile_row * TILE_PX + r][tile_col * TILE_PX + c] = tile[r][c]

room_pal = [
    (20, 16, 12),     # 0 backdrop (dark)
    (160, 140, 100),  # 1 floor (warm stone/tan)
    (80, 60, 45),     # 2 wall (dark brown)
    (110, 88, 66),    # 3 wall accent (lighter brown)
]
write_4bpp_bmp(os.path.join(OUT, 'room.bmp'), 256, 256, room_pal, pixels)
write_json(os.path.join(OUT, 'room.json'), {'type': 'regular_bg'})

print("Assets generated in", os.path.abspath(OUT))
