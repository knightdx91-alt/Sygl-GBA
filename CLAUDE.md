# Sygl GBA — Codebase Guide

## What this game is
A GBA JRPG built with **Butano** (C++17 over devkitARM). Based on the Sygl manuscript by Dream.
Golden Sun–style: tile-based overworld, turn-based combat, visible enemies, deep RPG systems.
The player creates an original character (the "fifth Heir") who arrives at The Heir's School alongside
Bal and becomes woven into the canonical story. 30+ hour target. Five endings.

## Lore foundation
See `assets/lore/sygl_complete.txt` and `docs/design.md`. Key facts:

- **Protagonist: Balthazaar "Bal" Norheart** — orphan at Saint Andre's. Missing left hand since
  infancy (Blood sygl removed by doctors at birth — standard practice). Can still do magic but
  weaker; blockage is **mental/psychological**, not physical. Gains his hand and sygl back after
  bonding Amelia in the temple.
- **Amelia** — the Blood Sygl originator. ~3,000-year-old vampire goddess, imprisoned in the
  mausoleum behind Saint Andre's. Pale skin, midnight-black hair, blood-red eyes. Bal's familiar,
  co-commander, and love interest. She is a co-protagonist — present from Act I onward.
- **The PC** — a custom character (name + 1 of 4 sprites) who is also at Saint Andre's and arrives
  at The Heir's School as the fifth Heir. NOT Bal. The PC has a sygl from the start.
- **Viridia Vizer** — Tempus sygl, pale blue crackling hair, tall. Converts to Blood mid-story.
  Keeps her storm/lightning inheritance gift after conversion. Familiar: Hopper the Bloodwolf.
- **Lora Aegis** — Waves sygl, brown hair, glasses, freckled. Rapier prodigy, alchemy specialist.
  Converts to Blood mid-story. Keeps her Waves abilities after conversion.
- **The Heir's School** — canonical name (NEVER "Sygldry Academy"). Exists on a different magical
  plane. Five originator towers, three floors, five sygl dorms. Accessible only via portal + escort.
- **The Dawn Faction** — guild in New Spring. Does NOT discriminate against Blood mages (Soren
  processes Bal's team "Sygls Best" registration without issue). Wooden member cards, monthly dues,
  job board. Staff: Soren (right) and Kim (left, behind counter).
- **Saint Andre's Orphanage** — 3-story building, 1700s, grey slate roof. Remote lane. Mausoleum
  (Amelia's temple) directly behind it. BOTH Bal and the PC are orphans here.

## The Five Sygls (playable classes — PC chooses one)
| Sygl    | Originator     | Focus                           |
|---------|----------------|---------------------------------|
| Duality | Aren (+ Nera)  | Balance, light/shadow, time-of-day mechanic |
| Waves   | Kenmei         | Healing, water/ice, status cleanse |
| Terra   | Naisura        | Earth defense, physical buffs, slow tank |
| Tempus  | Raizen         | Lightning, speed, glass cannon |
| Blood   | Amelia         | Blood whip, conversion, life drain |

The quiz is **10 questions hidden inside early dialogue** — no quiz UI ever appears.
At the claiming ceremony the highest-weighted originator steps forward. Manual override offered.

## Party roster
| Member    | Sygl             | Joins        | Notes |
|-----------|------------------|--------------|-------|
| PC        | Player's choice  | Start        | Custom name + sprite |
| Bal       | None → Blood     | Temple scene | Missing hand until temple; auto-included in story scenes |
| Amelia    | Blood (Originator)| Temple scene | Near-indestructible; story-locked in some scenes |
| Lora      | Waves → Blood    | Act II school| Keeps Waves abilities post-conversion |
| Viridia   | Tempus → Blood   | Act II school| Keeps storm powers post-conversion |
| + 5 recruitable NPCs (one per sygl, gated by faction rep + personal rep) |

## File structure
```
Makefile              — Butano build config
.github/workflows/    — CI: builds sygl.gba, deploys to GitHub Pages
src/
  main.cpp           — entry point, scene loop
  game_state.h       — GameState, EnemyType, Party
  party.h / party.cpp       — Party, Inventory
  character.h / character.cpp — Character, stats, equipment, spells
  item_data.h / item_data.cpp — ItemDef, SpellDef, tables
  save_data.h / save_data.cpp — SRAM save/load (5 slots)
  overworld_scene.h/cpp      — overworld, NPC interaction, pause menu
  combat_scene.h/cpp         — turn-based combat
  title_scene.h/cpp          — title screen, new game / continue
  text_box.h/cpp             — typewriter dialogue box
  choice_box.h/cpp           — dialogue choice prompts
docs/
  design.md          — full design doc and systems spec
  DECISIONS.md       — architectural decision log
assets/
  lore/sygl_complete.txt  — source novel
  graphics/               — BMP + JSON for sprites and tilemaps
web/                  — GitHub Pages site (EmulatorJS)
tools/
  gen_assets.py      — generates placeholder BMP assets
butano/               — Butano submodule
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

## Active branch: main — ONLY branch
**All commits go directly to main. No feature branches. No PRs. Never push to any other branch.**
CI must stay green. `sygl.gba` artifact uploads on every push.

## 12-Phase Roadmap
| Phase | Deliverable | Status |
|-------|-------------|--------|
| 0  | Title screen compiles, CI passes | ✅ Done |
| 1  | Engine core: walk, dialogue, save/load | ✅ Done (gaps: animation, music) |
| 2  | Core gameplay loop: NPCs, combat, XP | ✅ Done (gaps: fade transitions) |
| 3  | RPG systems: party, equipment, spells, familiars | ✅ Done (gaps: Amelia, Bal auto-include) |
| 4  | Progression systems: allegiance meters, faction rep, quiz state machine | ⬜ Not started |
| 5  | Act I & II content: orphanage prologue + The Heir's School | ⬜ Not started |
| 6  | Act III content: New Spring full build, Dawn Guild missions, Saegen Woods dungeon | ⬜ Not started |
| 7  | Act IV content: conversion arc, war, Amelia's temple HQ | ⬜ Not started |
| 8  | Act V content: Londra infiltration, capture, mage duel | ⬜ Not started |
| 9  | All 5 endings fully implemented | ⬜ Not started |
| 10 | NG+ systems: Ascendant mode, Omni-Sygl, [Ascendant] dialogue flags | ⬜ Not started |
| 11 | Ascendant superbosses + True Ending (Cycle Broken) | ⬜ Not started |
| 12 | Polish, save integrity, localization, ROM packaging | ⬜ Not started |

## Key technical rules
- `bn::sprite_ptr` has no default constructor — always use `bn::optional<bn::sprite_ptr>`
- `constexpr` static array members need inline initializers in class body (C++17)
- Walkability: `bn::regular_bg_map_cell_info(cell).tile_index() == _floor_tile_index`
- SRAM: `bn::sram::read(T&)` / `bn::sram::write(const T&)` — trivially copyable structs only
- INCLUDES must use relative paths only (common_setup.mak prepends CURDIR)
- 32-bit LCG for random: `g_rng = g_rng * 1664525u + 1013904223u`
- Cache-bust web ROM: `'sygl.gba?v=' + Date.now()` in index.html

## Immediate next task: Phase 5 — Act I (Orphanage Prologue)
Build Saint Andre's Orphanage as the game's opening zone:
1. Orphanage tilemap (grey slate interior + rooftop)
2. PC starts in their room — quiz question 1 embedded in interaction with Mrs. Brown
3. Explore orphanage: courtyard, corridor, Mrs. Brown's office, rooftop (off-limits but accessible)
4. Observe Bal flying into the mausoleum (cutscene)
5. PC goes to Mrs. Brown — learns both they and Bal have been accepted to The Heir's School
6. Travel sequence to school (Verin's cart, dark forest, portal)
7. Claiming ceremony — quiz result determines which originator steps forward for PC
