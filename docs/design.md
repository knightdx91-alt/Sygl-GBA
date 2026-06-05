# Sygl GBA — Design Document

## Overview
A GBA JRPG based on the Sygl book series. Final Fantasy I/IV style with tile-based movement, turn-based battles, and sygl-class selection. 30-hour target runtime with 5 distinct endings.

## Platform
- **Hardware**: Game Boy Advance (240×160, 15-bit color)
- **Framework**: Butano (C++17 over devkitARM)
- **Distribution**: GitHub Pages via EmulatorJS

## Protagonist
**Balthazaar (Bal)** — orphan at Saint Andre's orphanage. Missing left hand since birth → no sigil → no sygl affinity. Still capable of magic through discipline. Wants to join the Dawn Faction but can't due to the no-sigil rule.

## The Five Sygls (player classes)
At game start the player chooses a sygl for their character (not Bal — Bal has none). The chosen sygl defines stats, magic, and story affinity.

| Sygl    | Originator | HP | MP | ATK | DEF | SPD | Magic affinity     |
|---------|------------|----|----|-----|-----|-----|--------------------|  
| Duality | Aren       | 42 | 38 |  7  |  5  |  7  | Dual-type spells   |
| Waves   | Kenmei     | 45 | 45 |  5  |  6  |  6  | Healing, flow      |
| Terra   | Naisura    | 58 | 25 |  8  | 10  |  4  | Earth, endurance   |
| Tempus  | Raizen     | 38 | 35 |  6  |  4  | 10  | Speed, time        |
| Blood   | Amelia     | 40 | 50 |  6  |  4  |  6  | Power, life-force  |

## Screen Layout (240×160)
- **BG0**: Town/dungeon tilemap (scrolling)
- **BG1**: HUD overlay (HP/MP bars, non-scrolling)
- **OBJ**: Player + NPC + enemy sprites

## Zone Roadmap
1. **Saint Andre's Orphanage** — tutorial zone, opening scene
2. **New Spring** — first town: inn, market, Dawn Hall guild
3. **The Heir's School** — north of New Spring (canonical name)
4. **Wilderness** — overworld travel, random encounters

## Combat System
- Turn order by SPD stat
- Commands: Attack / Magic / Item / Run
- Magic costs MP; damage scales with sygl affinity
- XP → level up → stat growth per sygl template

## Faction System
- **Dawn Faction**: rising sun badge; requires a sigil to join
- **Other factions**: TBD based on lore
- Allegiance choices affect endings

## Endings (5 total)
1. **Ascension** — hardest to achieve; unlocks NG+ (Hard mode only)
2-5. TBD based on faction choices and story flags

## NG+ Rules
- Unlocked ONLY by Ascension ending on Hard mode
- Not available from Normal/Easy playthroughs

## Dorm characters (The Heir's School)
- **Thessaly** — Duality sygl
- **Sienne** — Waves sygl
- **Aldric** — Terra sygl
- **Viridia** — Tempus sygl
- **Bal** — Blood sygl (party member, no playable sygl of his own)

## Lore reference
See `assets/lore/sygl_complete.txt`
