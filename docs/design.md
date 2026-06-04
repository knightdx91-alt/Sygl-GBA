# Sygl GBA — Design Document

## Overview
A GBA top-down RPG based on the Sygl book series.
Final Fantasy I/IV style: overworld exploration, tile-based movement,
turn-based battles, and sygl-driven class selection.

## Screen Layout (240x160)
- BG0: Town/dungeon tilemap (scrolling)
- BG1: HUD overlay (HP/MP bars, non-scrolling)
- OBJ: Player + NPC + enemy sprites

## Sygl Classes
| Sygl  | HP | MP | ATK | DEF | SPD | Specialty        |
|-------|----|----|-----|-----|-----|------------------|
| Fire  | 40 | 30 |  8  |  4  |  7  | Physical damage  |
| Water | 45 | 40 |  6  |  6  |  6  | Balanced         |
| Earth | 55 | 25 |  7  |  9  |  4  | Tank/endurance   |
| Air   | 38 | 35 |  6  |  4  |  9  | Speed/evasion    |
| Void  | 35 | 50 |  5  |  3  |  7  | Spell power      |

## Zone Roadmap
1. **New Spring** (starting town) — inn, market, Dawn Hall guild
2. **Sygldry Academy** — north of New Spring
3. **Wilderness** — random encounters, overworld travel

## Combat
- Turn order determined by SPD stat
- Commands: Attack / Magic / Item / Run
- Magic costs MP, damage scales with sygl affinity
- XP → level up → stat growth

## Lore Asset
See `assets/lore/sygl_complete.txt` — the full source novel used as
narrative reference for characters, locations, and lore.
