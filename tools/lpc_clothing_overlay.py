"""
lpc_clothing_overlay.py — Generate a clothing overlay layer for an LPC 576×256 walk sheet.

Paints solid-colored shapes over the body region of every frame:
  - torso:  y=32-45 within frame (shirt/jacket area)
  - legs:   y=46-60 within frame (trousers/skirt area)

Only paints where the body layer has an opaque pixel, so the clothing
naturally follows the character silhouette.

Usage:
    python3 tools/lpc_clothing_overlay.py \\
        --body   assets/source/lpc_layers/body_walk.png \\
        --torso  <R,G,B> \\
        --legs   <R,G,B> \\
        --output assets/source/lpc_layers/<name>_clothing.png

Example (orphanage grey uniform):
    python3 tools/lpc_clothing_overlay.py \\
        --body assets/source/lpc_layers/body_walk.png \\
        --torso 120,120,130 --legs 80,80,90 \\
        --output assets/source/lpc_layers/kid_clothing.png
"""

import sys, argparse
from PIL import Image


FRAME_SIZE  = 64
COLS        = 9
ROWS        = 4
TORSO_Y     = (32, 46)   # inclusive start, exclusive end within frame
LEGS_Y      = (46, 61)


def parse_rgb(s: str):
    parts = s.split(',')
    assert len(parts) == 3, f"Expected R,G,B got '{s}'"
    return tuple(int(x.strip()) for x in parts)


def generate_overlay(body_path: str, torso_rgb, legs_rgb, output_path: str):
    body = Image.open(body_path).convert('RGBA')
    out  = Image.new('RGBA', body.size, (0, 0, 0, 0))

    bpix = body.load()
    opix = out.load()

    for row in range(ROWS):
        for col in range(COLS):
            ox = col * FRAME_SIZE
            oy = row * FRAME_SIZE
            for ly in range(FRAME_SIZE):
                for lx in range(FRAME_SIZE):
                    px_r, px_g, px_b, px_a = bpix[ox + lx, oy + ly]
                    if px_a < 128:
                        continue
                    gy = oy + ly   # global y doesn't matter; use local ly
                    if TORSO_Y[0] <= ly < TORSO_Y[1]:
                        r, g, b = torso_rgb
                    elif LEGS_Y[0] <= ly < LEGS_Y[1]:
                        r, g, b = legs_rgb
                    else:
                        continue
                    opix[ox + lx, oy + ly] = (r, g, b, 220)

    out.save(output_path)
    print(f"Written: {output_path}")


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--body',   required=True)
    ap.add_argument('--torso',  required=True, help='R,G,B for shirt/jacket')
    ap.add_argument('--legs',   required=True, help='R,G,B for trousers/skirt')
    ap.add_argument('--output', required=True)
    args = ap.parse_args()

    generate_overlay(args.body, parse_rgb(args.torso), parse_rgb(args.legs), args.output)
