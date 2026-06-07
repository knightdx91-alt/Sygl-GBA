"""
kenney_to_butano.py — Build Butano 4bpp tilemap BMPs from Kenney Roguelike Indoors sheet.

Reads specific tiles from the Kenney sheet (16×16 with 1px margin), scales them
to 8×8 (native GBA tile size), arranges them into a 256×256 pixel room BMP
(32×32 tiles), quantizes to ≤16 colors, and writes Butano-compatible BMP + JSON.

Usage:
    python3 tools/kenney_to_butano.py

Outputs: assets/graphics/orphanage_corridor.bmp/.json
                          orphanage_office.bmp/.json
                          orphanage_rooftop.bmp/.json
"""

import struct, os, sys
from PIL import Image

# ── Kenney sheet layout ────────────────────────────────────────────────────────

SHEET_PATH  = os.path.join(os.path.dirname(__file__), '..', 'assets', 'source',
                           'kenney_roguelike', 'roguelikeIndoor_transparent.png')
TILE_IN     = 16        # Kenney tile size
TILE_OUT    = 8         # GBA tile size
MARGIN      = 1
STEP        = TILE_IN + MARGIN
MAP_TILES   = 32        # 32×32 game tiles
MAP_PX      = MAP_TILES * TILE_OUT   # 256×256

OUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'assets', 'graphics')
os.makedirs(OUT_DIR, exist_ok=True)


def load_sheet():
    return Image.open(SHEET_PATH).convert('RGBA')


def get_tile(sheet, col, row):
    """Extract one 16×16 tile and scale to 8×8."""
    x, y = col * STEP, row * STEP
    tile = sheet.crop((x, y, x + TILE_IN, y + TILE_IN))
    return tile.resize((TILE_OUT, TILE_OUT), Image.LANCZOS)


# ── Tile dictionary ────────────────────────────────────────────────────────────
# Each entry: symbol → (sheet_col, sheet_row)
# All tiles chosen to share ≤16 colors total.

TILE_DEFS = {
    # Floor
    ' ': (0, 0),   # stone floor A (walkable)
    '.': (1, 0),   # stone floor B
    ',': (4, 0),   # stone floor C
    # Walls (darker stone)
    '#': (2, 3),   # wall face (solid blocker)
    '@': (3, 3),   # wall alt
    '|': (0, 3),   # wall side
    '-': (1, 3),   # wall top
    # Bookshelves (top / bottom row)
    'A': (0, 6),   # shelf top-left
    'B': (1, 6),   # shelf top-mid
    'C': (2, 6),   # shelf top-right
    'a': (0, 7),   # shelf bot-left
    'b': (1, 7),   # shelf bot-mid
    'c': (2, 7),   # shelf bot-right
    # Desk / table
    'D': (0, 8),   # desk top-left (grey)
    'E': (1, 8),   # desk top-mid
    'F': (2, 8),   # desk top-right
    'G': (3, 8),   # desk bot corner
    # Doors
    'H': (0,12),   # door top-left
    'I': (1,12),   # door top-right
    'h': (0,13),   # door bot-left
    'i': (1,13),   # door bot-right
    # Stairs
    'S': (6, 4),   # stairs left
    'T': (7, 4),   # stairs right
}


# ── Room layouts ───────────────────────────────────────────────────────────────
# 32 chars wide × 32 rows.  ' ' = walkable floor, '#' = wall blocker.

CORRIDOR_MAP = (
    "################################"  # row 0  — top wall
    "################################"  # row 1
    "##  AB BC BC BC BC  ####...##  #"  # row 2  — shelves top
    "##  ab bc bc bc bc  ####...##  #"  # row 3  — shelves bot
    "##  ..  ..  ..  ..  ####...##  #"  # row 4
    "##  ..  ..  ..  ..  HI .HI ##  #"  # row 5  — door gap top
    "##  ..  ..  ..  ..  hi .hi ##  #"  # row 6  — door gap bot
    "##  ..  ..  ..  ..  ####...##  #"  # row 7
    "##  ..  ..  ..  ..  ####...##  #"  # row 8
    "##  ..  ..  ..  ..  ####...##  #"  # row 9
    "#   ..  ..  ..  ..  ####...  . #"  # row 10
    "#   ..  ..  ..  ..  ####...  . #"  # row 11
    "#   ..  ..  ..  ..  ####...  . #"  # row 12 — left opening rows
    "#   ..  ..  ..  ..  ####...  . #"  # row 13
    "##  ..  ..  ..  ..  ####...##  #"  # row 14
    "##  ..  ..  ..  ..  ####...##  #"  # row 15
    "##  ..  ..  ..  ..  ####...##  #"  # row 16
    "##  ..  ..  ..  ..  ####...##  #"  # row 17
    "##  ..  ..  ..  ..  ####...##  #"  # row 18
    "##  ..  ..  ..  ..  ####...##  #"  # row 19
    "##  ..  ..  ..  ..  ####...##  #"  # row 20
    "##  ..  ..  ..  ..  ####...##  #"  # row 21
    "##  ..  ..  ..  ..  ####...##  #"  # row 22
    "##  ..  ..  ..  ..  ####...##  #"  # row 23
    "##  ..  ..  ..  ..  ####...##  #"  # row 24
    "##  ..  ..  ..  ..  ####...##  #"  # row 25
    "##  ..  ..  ..  ..  ####...##  #"  # row 26
    "##  ..  ..  ..  ..  ####...##  #"  # row 27
    "################################"  # row 28
    "################################"  # row 29
    "################   #############"  # row 30 — top exit cols 16-18
    "################################"  # row 31
)

OFFICE_MAP = (
    "################################"  # row 0
    "################################"  # row 1
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 2
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 3
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 4
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 5
    "## ABBC.. .. .. .. ...BC.## ####"  # row 6  — shelves
    "## abbc.. .. .. .. ...bc.## ####"  # row 7
    "##  ..DEEF..  ..  ..  ..  ..####"  # row 8  — desk top
    "##  ..GGGF..  ..  ..  ..  ..####"  # row 9  — desk bot
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 10
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 11
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 12
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 13
    "##  ..  ..  ..  ..  ..  ..      "  # row 14 — right exit (cols 28-31 open)
    "##  ..  ..  ..  ..  ..  ..      "  # row 15 — right exit
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 16
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 17
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 18
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 19
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 20
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 21
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 22
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 23
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 24
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 25
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 26
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 27
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 28
    "##  ..  ..  ..  ..  ..  ..  ####"  # row 29
    "################################"  # row 30
    "################################"  # row 31
)

ROOFTOP_MAP = (
    "################################"
    "################################"
    "################################"
    "################################"
    "################################"
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "##  ..  ..  ..  ..  ..  ..  ##  "
    "################################"
    "################################"
    "###############   ##############"
    "################################"
)


# ── BMP writer ─────────────────────────────────────────────────────────────────

def pack_row_4bpp(row: list, width: int) -> bytes:
    out = bytearray()
    for i in range(0, width, 2):
        lo = row[i] & 0xF
        hi = (row[i + 1] & 0xF) if i + 1 < width else 0
        out.append(lo | (hi << 4))
    while len(out) % 4:
        out.append(0)
    return bytes(out)


def write_4bpp_bmp(path, width, height, palette_rgb, pixels_topdown):
    pal = list(palette_rgb) + [(0, 0, 0)] * (16 - len(palette_rgb))
    pixel_data = b''.join(pack_row_4bpp(row, width) for row in reversed(pixels_topdown))
    ct = b''.join(struct.pack('BBBB', b, g, r, 0) for r, g, b in pal)
    pixel_offset = 14 + 40 + len(ct)
    file_size = pixel_offset + len(pixel_data)
    header = struct.pack('<2sIHHI', b'BM', file_size, 0, 0, pixel_offset)
    dib = struct.pack('<IiiHHIIiiII',
                     40, width, height, 1, 4, 0,
                     len(pixel_data), 2835, 2835, 16, 16)
    with open(path, 'wb') as f:
        f.write(header + dib + ct + pixel_data)


# ── Quantizer ──────────────────────────────────────────────────────────────────

TRANSPARENT = (248, 0, 248)

def to_5bit(c):
    return (c >> 3) << 3


def quantize(img: Image.Image):
    """Convert RGBA image to 4bpp palette + pixel grid. Transparent → index 0."""
    palette = [TRANSPARENT]
    pal_map = {}
    w, h = img.size
    pix = img.load()
    pixels = [[0] * w for _ in range(h)]
    for y in range(h):
        for x in range(w):
            r, g, b, a = pix[x, y]
            if a < 128:
                pixels[y][x] = 0
                continue
            r5, g5, b5 = to_5bit(r), to_5bit(g), to_5bit(b)
            key = (r5, g5, b5)
            if key not in pal_map:
                if len(palette) >= 16:
                    best = min(range(1, len(palette)),
                               key=lambda i: sum((palette[i][c]-key[c])**2 for c in range(3)))
                    pal_map[key] = best
                else:
                    pal_map[key] = len(palette)
                    palette.append(key)
            pixels[y][x] = pal_map[key]
    while len(palette) < 16:
        palette.append((0, 0, 0))
    return palette, pixels


# ── Room builder ───────────────────────────────────────────────────────────────

def build_room(sheet, layout_str: str, name: str):
    assert len(layout_str) == MAP_TILES * MAP_TILES, \
        f"{name}: map must be exactly {MAP_TILES*MAP_TILES} chars, got {len(layout_str)}"

    # Cache scaled tiles
    tile_cache = {}
    for sym, (col, row) in TILE_DEFS.items():
        tile_cache[sym] = get_tile(sheet, col, row)

    # Default floor for unknown symbols
    floor_tile = tile_cache[' ']

    # Compose the 256×256 image
    canvas = Image.new('RGBA', (MAP_PX, MAP_PX), (0, 0, 0, 0))
    for ty in range(MAP_TILES):
        for tx in range(MAP_TILES):
            sym = layout_str[ty * MAP_TILES + tx]
            tile = tile_cache.get(sym, floor_tile)
            canvas.paste(tile, (tx * TILE_OUT, ty * TILE_OUT), tile)

    palette, pixels = quantize(canvas)
    print(f'  {name}: {len(palette)} palette entries, '
          f'{len(set(c for row in pixels for c in row))} used indices')

    bmp_path  = os.path.join(OUT_DIR, f'{name}.bmp')
    json_path = os.path.join(OUT_DIR, f'{name}.json')
    write_4bpp_bmp(bmp_path, MAP_PX, MAP_PX, palette, pixels)
    import json
    with open(json_path, 'w') as f:
        json.dump({'type': 'regular_bg'}, f)
    print(f'  Written: {bmp_path}')


# ── Main ───────────────────────────────────────────────────────────────────────

if __name__ == '__main__':
    if not os.path.exists(SHEET_PATH):
        print(f'ERROR: Kenney sheet not found at:\n  {SHEET_PATH}')
        print('Copy roguelikeIndoor_transparent.png to assets/source/kenney_roguelike/')
        sys.exit(1)

    sheet = load_sheet()
    print(f'Sheet loaded: {sheet.size}')
    print()

    print('Building orphanage_corridor ...')
    build_room(sheet, CORRIDOR_MAP, 'orphanage_corridor')

    print('Building orphanage_office ...')
    build_room(sheet, OFFICE_MAP, 'orphanage_office')

    print('Building orphanage_rooftop ...')
    build_room(sheet, ROOFTOP_MAP, 'orphanage_rooftop')

    print('\nDone.')
