"""
lpc_to_gba.py — Convert LPC character spritesheets to Butano-compatible 4bpp BMPs.

LPC sheet format: 576×256, 9 frames × 4 directions (down/left/right/up), 64×64 per frame.
Output: 16×16 GBA sprite with 16 frames (4 dir × 4 walk frames) as a horizontal strip.

Usage:
    python3 tools/lpc_to_gba.py <output_name> <layer1.png> [<layer2.png> ...]

Example:
    python3 tools/lpc_to_gba.py player \\
        assets/source/lpc_layers/body_walk.png \\
        assets/source/lpc_layers/head_walk.png \\
        assets/source/lpc_layers/face_walk.png
"""

import sys, os, struct, json
from PIL import Image

FRAME_SIZE  = 64          # LPC frame size (px)
OUT_SIZE    = 16          # GBA sprite size (px)
DIRECTIONS  = 4           # down, left, right, up
WALK_FRAMES = [1, 2, 3, 5]  # which LPC frame indices to use (skip idle 0)
N_FRAMES    = DIRECTIONS * len(WALK_FRAMES)

OUT_DIR = os.path.join(os.path.dirname(__file__), '..', 'assets', 'graphics')
os.makedirs(OUT_DIR, exist_ok=True)


def composite_layers(png_paths: list[str]) -> Image.Image:
    base = None
    for path in png_paths:
        layer = Image.open(path).convert('RGBA')
        if base is None:
            base = Image.new('RGBA', layer.size, (0, 0, 0, 0))
        base = Image.alpha_composite(base, layer)
    return base


def extract_frames(sheet: Image.Image) -> list[Image.Image]:
    """Extract DIRECTIONS × WALK_FRAMES crops, each 64×64."""
    frames = []
    for row in range(DIRECTIONS):
        for col in WALK_FRAMES:
            x = col * FRAME_SIZE
            y = row * FRAME_SIZE
            crop = sheet.crop((x, y, x + FRAME_SIZE, y + FRAME_SIZE))
            frames.append(crop)
    return frames


def scale_frame(frame: Image.Image) -> Image.Image:
    """Scale 64×64 → 16×16 with LANCZOS, then snap to nearest-neighbour."""
    return frame.resize((OUT_SIZE, OUT_SIZE), Image.LANCZOS)


def build_strip(frames: list[Image.Image]) -> Image.Image:
    """Arrange frames as a horizontal strip: (N_FRAMES*16) × 16 RGBA."""
    strip = Image.new('RGBA', (N_FRAMES * OUT_SIZE, OUT_SIZE), (0, 0, 0, 0))
    for i, f in enumerate(frames):
        strip.paste(f, (i * OUT_SIZE, 0))
    return strip


def quantize_to_15bit(img: Image.Image) -> tuple[list[tuple], list[list[int]]]:
    """
    Convert RGBA strip to:
      - palette_rgb: list of up to 16 (R,G,B) rounded to GBA 5-bit per channel
      - pixels:      list of rows (list of palette indices)
    Transparent pixels → index 0 (magenta = GBA transparent).
    """
    TRANSPARENT = (248, 0, 248)   # GBA magenta transparent

    def to_5bit(c):
        return (c >> 3) << 3      # round to nearest 5-bit step

    palette = [TRANSPARENT]       # index 0 = transparent
    pal_map = {}

    w, h = img.size
    pix_data = img.load()
    pixels = [[0] * w for _ in range(h)]

    for y in range(h):
        for x in range(w):
            r, g, b, a = pix_data[x, y]
            if a < 128:
                pixels[y][x] = 0
                continue
            r5, g5, b5 = to_5bit(r), to_5bit(g), to_5bit(b)
            key = (r5, g5, b5)
            if key not in pal_map:
                if len(palette) >= 16:
                    # Palette full — find nearest
                    best = min(range(1, len(palette)),
                               key=lambda i: sum((palette[i][c]-key[c])**2
                                                 for c in range(3)))
                    pal_map[key] = best
                else:
                    pal_map[key] = len(palette)
                    palette.append(key)
            pixels[y][x] = pal_map[key]

    # Pad palette to 16
    while len(palette) < 16:
        palette.append((0, 0, 0))

    return palette, pixels


def pack_row_4bpp(row: list[int], width: int) -> bytes:
    out = bytearray()
    for i in range(0, width, 2):
        hi = row[i] & 0xF
        lo = (row[i + 1] & 0xF) if i + 1 < width else 0
        out.append((hi << 4) | lo)
    while len(out) % 4:
        out.append(0)
    return bytes(out)


def write_4bpp_bmp(path: str, width: int, height: int,
                   palette_rgb: list, pixels_topdown: list):
    palette_rgb = list(palette_rgb) + [(0, 0, 0)] * (16 - len(palette_rgb))
    pixel_data = b''
    for row in reversed(pixels_topdown):
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


def write_json(path: str, data: dict):
    with open(path, 'w') as f:
        json.dump(data, f, indent=4)


def convert(name: str, layer_paths: list[str]):
    print(f"Compositing {len(layer_paths)} layer(s)...")
    sheet = composite_layers(layer_paths)

    print(f"Sheet size: {sheet.size}  (expected 576×256 for LPC walk)")
    frames_raw = extract_frames(sheet)
    frames_scaled = [scale_frame(f) for f in frames_raw]

    strip = build_strip(frames_scaled)
    print(f"Strip size: {strip.size}  ({N_FRAMES} frames × {OUT_SIZE}px)")

    palette, pixels = quantize_to_15bit(strip)
    print(f"Palette: {len(palette)} colours (incl. transparent at index 0)")

    bmp_path  = os.path.join(OUT_DIR, f'{name}.bmp')
    json_path = os.path.join(OUT_DIR, f'{name}.json')

    write_4bpp_bmp(bmp_path, N_FRAMES * OUT_SIZE, OUT_SIZE, palette, pixels)
    write_json(json_path, {
        'type': 'sprite',
        'height': OUT_SIZE,
        # Butano uses 'graphics' to know how many tiles wide the sheet is
        # Each frame is OUT_SIZE px wide; Butano auto-detects from BMP width
    })

    print(f"Written: {bmp_path}")
    print(f"Written: {json_path}")
    print()
    print("Frame layout (for C++ animation code):")
    print(f"  Frames 0–{len(WALK_FRAMES)-1}  : down  walk")
    print(f"  Frames {len(WALK_FRAMES)}–{2*len(WALK_FRAMES)-1} : left  walk")
    print(f"  Frames {2*len(WALK_FRAMES)}–{3*len(WALK_FRAMES)-1}: right walk")
    print(f"  Frames {3*len(WALK_FRAMES)}–{4*len(WALK_FRAMES)-1}: up    walk")


if __name__ == '__main__':
    if len(sys.argv) < 3:
        print(__doc__)
        sys.exit(1)
    name   = sys.argv[1]
    layers = sys.argv[2:]
    convert(name, layers)
