# DECISIONS — Architectural and Design Choices

## Engine: Butano (C++17)
Switched from raw libgba C. Butano provides hardware abstractions needed for a 30-hour JRPG
scope (scene management, sprite/BG layers, audio, save). Actively maintained, clean C++ API.

## School name: The Heir's School
Canonical name from the manuscript. NEVER "Sygldry Academy" — that is a lore error in older
design notes. All code, dialogue, and docs must use "The Heir's School" exclusively.

## Sygl names: from the book
Correct names: Duality, Waves, Terra, Tempus, Blood.
Originators: Aren/Nera, Kenmei, Naisura, Raizen, Amelia.
NOT Fire/Water/Earth/Air/Void.

## PC is NOT Bal
The player creates a custom character (the "fifth Heir"), also an orphan at Saint Andre's.
Bal is a separate party member who is auto-included in mandatory story scenes.
The current code (Phases 0–3) treats the PC as the sole character — this must be refactored
when building Act I content in Phase 5.

## Bal has two sprite states
- Pre-temple: missing left hand (stump at wrist). This is his most important visual identifier.
- Post-temple: full left hand with Blood sygl visible on palm.
The sprite swap happens at the end of the temple bonding sequence.

## Bal's magic limitations are mental, not physical
His blockage is entirely psychological — stemming from trauma. He can use his right hand but
keeps trying his left wrist hoping a sygl will appear. The writing must never give him sudden
unearned competence ("competence beats"). Growth is gradual and hard-fought.

## Amelia is a co-protagonist, present from Act I
She is NOT a late-game reveal. She appears in the temple and is present throughout the story.
She needs a full sprite, portrait, and dialogue system from Phase 5 forward.

## The Dawn Faction does NOT discriminate against Blood mages
In the novel, Soren processes Bal's team registration without issue. The discrimination comes
from other students and teachers (Mr. Porter, the originator-imposed segregation rule), NOT
from the Dawn faction itself. Dialogue must reflect this.

## Quiz is embedded in dialogue — no quiz UI
Ten questions woven into natural conversations during Acts I and II. The player never sees
a quiz screen. The originator with the highest weighted score steps forward at the claiming
ceremony. Manual override offered at the ceremony. See design.md for the 10 question hooks.

## 5 save slots — one per sygl path
Current implementation has 1 slot. Must be expanded to 5 when Phase 4 save system is rebuilt.
SRAM layout: 2 KB global header + 5 slots of ~5 KB + 5 KB autosave = 32 KB total.

## Converts retain their original inherited gifts
When a character converts to Blood sygl, they KEEP all abilities from their original sygl
plus gain Blood magic. This is why the other originators feared conversion campaigns —
converts become more powerful hybrids. Viridia keeps storm powers; Lora keeps Waves healing.
This must be reflected in party combat mechanics.

## NG+ unlock condition
NG+ only after Ascension ending. Not available from Normal/Easy playthroughs.
NG+ resets allegiance meters and faction/personal rep to allow different paths.
NG+ retains 50% of endgame levels and all 150 crafting recipes.
The Omni-Sygl (swap at save points) is the NG+ signature mechanic.

## Font strategy
Using `FONTS := butano/template/fonts` to pick up `variable_8x16_sprite_font.bdf` from
Butano's own template directory.

## Asset generation: programmatic placeholders
`tools/gen_assets.py` generates placeholder 4bpp BMPs using the canonical palette from the
"Games Pallets - Sygl Project" Google Drive document. These are placeholders only — final
pixel art must be created separately. Palette colors are hardcoded from that document.

## Camera: player-centered scrolling (deferred)
Current overworld moves the player sprite against a static background. The correct GBA
approach is to keep the player sprite centered and scroll `bg.set_position()` instead.
This will be implemented when the continuous Pokémon-style world map is built (Phase 6+).
