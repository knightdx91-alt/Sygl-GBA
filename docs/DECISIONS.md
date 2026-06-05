# DECISIONS

## Engine: Butano (C++17)
Switched from raw libgba C. Butano provides hardware abstractions needed for a 30-hour JRPG scope (scene management, sprite/BG layers, audio, save).

## School name: The Heir's School
Canonical name from the manuscript. Never "Sygldry Academy".

## Sygl names: from the book
Correct names: Duality, Waves, Terra, Tempus, Blood.
Originators: Aren, Kenmei, Naisura, Raizen, Amelia.
Not: Fire/Water/Earth/Air/Void — those were wrong.

## NG+ unlock condition
NG+ only after Ascension ending on Hard mode. Not from Normal/Easy Ascension.

## Font strategy
Using `FONTS := butano/template/fonts` to pick up `variable_8x16_sprite_font.bdf` from Butano's own template directory. If that path doesn't exist in the submodule, fall back to embedding font tile data directly in C++ as a `bn::sprite_font`.

## Protagonist: Bal has no sygl
Balthazaar is the story protagonist with no sigil (missing left hand). Player picks a sygl for their own character. Bal is a party member.
