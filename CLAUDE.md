# Sygl GBA — Codebase Guide

## What this game is
A GBA JRPG built with **Butano** (C++17 over devkitARM). Based on the Sygl book series by the user. Final Fantasy I/IV style: overworld exploration, tile-based movement, turn-based battles, sygl-driven class selection.

## Lore foundation
See `assets/lore/sygl_complete.txt` — source novel. Key facts:
- Protagonist: **Balthazaar (Bal)** — orphan at Saint Andre's, no left hand = no sigil, still does magic through effort
- **Sigils (Sygls)** appear on the hand, define magic affinity and faction eligibility
- **Dawn Faction** — rising sun badge, does not admit people without sigils. Bal has always wanted to join.
- **The Heir's School** — canonical school name (not Sygldry Academy)
- Setting: a world where sigil magic is the norm; those without sigils are marginalized

## The Five Sygls (playable classes)
| Sygl    | Originator | Focus              |
|---------|------------|--------------------|
| Duality | Aren       | Balance, dual-type magic |
| Waves   | Kenmei     | Healing, water/flow      |
| Terra   | Naisura    | Defense, earth/endurance |
| Tempus  | Raizen     | Speed, time manipulation |
| Blood   | Amelia     | Power, life-force magic  |

Note: Bal (the protagonist) has NO sygl — this is central to the story.

## Key design decisions
- Engine: **Butano** (C++17, devkitARM)
- School: **The Heir's School** (never "Sygldry Academy")
- NG+: unlocked ONLY by Ascension ending on Hard mode
- Sygl selection quiz happens at game start, after name entry

## File structure
```
Makefile              — Butano build config
.github/workflows/    — CI: builds sygl.gba, deploys to GitHub Pages
src/
  main.cpp           — entry point
docs/
  design.md          — full design doc and roadmap
  DECISIONS.md       — architectural decision log
assets/
  lore/sygl_complete.txt  — source novel
web/                  — GitHub Pages host (EmulatorJS)
butano/               — Butano submodule (GValiente/butano)
```

## Build system
```makefile
BUILD    := build
TARGET   := sygl
SOURCES  := src
INCLUDES := include
FONTS    := butano/template/fonts
```
CI: `devkitpro/devkitarm:latest` Docker container. `export PYTHON=python3` required.

## Active branch: main
CI must stay green. `sygl.gba` artifact uploads on every push.

## Roadmap phases
1. **Phase 0** ✓ — Title screen ROM compiles and CI passes
2. **Phase 1** — Scene manager + name entry + sygl quiz
3. **Phase 2** — New Spring overworld (tilemap + player movement)
4. **Phase 3** — Dialogue system + NPCs (Soren, Kim at Dawn Hall)
5. **Phase 4** — Turn-based combat
6. **Phase 5** — The Heir's School zone
7. **Phase 6** — Allegiance/faction system
8. **Phase 7** — 5 endings (including Ascension for NG+)
