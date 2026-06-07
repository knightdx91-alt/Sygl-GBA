# Sygl GBA — Design Document

## Overview
A GBA JRPG adapted from the Sygl manuscript by Dream. Golden Sun–style tile-based movement,
turn-based combat, visible enemies on overworld. The player creates a custom character (the
"fifth Heir") who arrives at The Heir's School alongside Bal and is woven into the canonical story.
Target: 30+ hours, 5 endings, NG+ via Ascension only.

## Platform
- **Hardware**: Game Boy Advance (240×160, 15-bit color)
- **Framework**: Butano (C++17 over devkitARM)
- **Distribution**: GitHub Pages via EmulatorJS; ROM also downloadable

---

## Characters

### Balthazaar "Bal" Norheart — story protagonist, party member
- Age ~16. Missing left hand (stump at wrist) — Blood sygl was surgically removed at birth.
  Standard practice: doctors remove the hand of any infant born with a Blood sygl.
- Can still do magic but weaker. His blockage is **mental/psychological**, not physical — he
  pushes past it through willpower. He keeps trying with his left wrist hoping a sygl appears.
- Personality: curious, calm under pressure, protective of companions, natural leader.
- **Pre-temple sprite**: stump at left wrist. **Post-temple sprite**: full left hand, Blood sygl visible.
- Weapon: rapier. Blood magic: Blood Whip, Blood Dart, Blood Dagger, Conversion ritual.
- Auto-included in all mandatory story scenes regardless of active party.
- His magic limitations arise from trauma — avoid writing him as suddenly competent (no JRPG "competence beats").

### Amelia — Blood Sygl originator, Bal's familiar, co-protagonist
- ~3,000-year-old vampire goddess. Turned at age 16; presents as late teens.
- Appearance: pale skin, midnight-black hair to mid-back, blood-red eyes.
  Temple outfit: long black dress, teardrop ruby earrings.
  School outfit: black skirt, tight black sweater, black-and-red socks above the calf.
- Imprisoned in the mausoleum behind Saint Andre's for ~3,000 years by the other originators.
- Powers: teleportation, shapeshifting (bats), near-indestructibility, memory absorption via blood
  offering, Bloodwolf creation, familiar bonding from inside her own temple.
- Personality: mischievous, haughty, privately lonely and emotionally raw, ultimately warm and
  playful. Calls Bal "Master" (partly teasing). Cries blood tears.
- Joins permanently after the temple bonding. Present from Act I.

### The Player Character (PC)
- Also an orphan at Saint Andre's. Custom name. 4 sprite options (2 male, 2 female).
- Has a sygl from birth — determined by hidden quiz in early dialogue.
- Arrives at The Heir's School as the "fifth Heir" alongside Bal.
- Party leader in gameplay; Bal is the story protagonist.

### Viridia Vizer
- Tempus sygl → converts to Blood in Act III.
- Appearance: pale blue hair that crackles with electricity, pale blue eyes, tall, tan skin.
- From a village where sygls are rare; escaped an arranged marriage when her sygl appeared.
- Inheritance gift: rare storm/lightning control (crackling hair is the visible marker).
  **She keeps all storm powers after conversion** — converts are more powerful because of this.
- Familiar: Hopper, a juvenile Bloodwolf (light silver and blood-red fur; pelt repels longswords).
  Amelia retrieves Hopper from a mountain cave as a gift. Collar has a size-adjustment rune.
- Weapon: long spear.

### Lora Aegis
- Waves sygl → converts to Blood in Act III.
- Appearance: long brown hair, glasses that slide down her nose, freckled nose.
- Rapier prodigy — trained by her father, won her village tournament at 13 defeating all the boys.
- Alchemy specialist. Every family member before her was Waves sygl.
- **Keeps Waves healing abilities after conversion.**
- Familiar: a raven (stays at school during quests).
- Key contribution in the story: brews the night-stalker poison; saves Viridia with a purification rune.

### Recruitable party members (5 total, one per sygl)
Found via each sygl's priest at the school. Require faction rep ≥ 50 + personal rep threshold.

### Canonical NPCs (require portraits + dialogue)
| Name | Role | Location |
|------|------|----------|
| Mrs. Brown (Sarah) | Orphanage administrator | Saint Andre's |
| Verin | Elderly escort mage | Dark forest cottage |
| Mr. Porter | Familiar class teacher (hostile to Bal) | The Heir's School |
| Mr. Winters | Rune Working teacher | The Heir's School |
| Zoran | Warfare teacher | The Heir's School |
| Sean | Tempus Lead Mage (antagonist student) | The Heir's School |
| Soren | Dawn Hall staff (right side) | New Spring |
| Kim | Dawn Hall staff (left side, counter) | New Spring |
| Mrs. Summers | Alchemist | New Spring |
| Thad Moren | Blood priest | Amelia's Temple |
| Xavier Giles | Blood priest | Amelia's Temple |
| Commander Fern | Blood army commander | Army camp |
| Toma | Raizen's High Priest, final boss | Londra / Pashar |
| Aleric | Raizen's most dangerous priest | Londra |
| Titus | Raizen's impulsive priest | Londra |
| Leon | Raizen's youngest priest | Londra |
| Mrs. Locklan | Winespring Inn proprietor | Londra |
| James | Brewer | Londra |
| Aren / Nera | Duality originator (male day / female night forms) | School ceremony |
| Kenmei | Waves originator | School ceremony |
| Naisura | Terra originator | School ceremony |
| Raizen | Tempus originator, antagonist | School ceremony / war |

---

## Five-Act Story Structure

### Act I — Orphanage and Awakening (~2 hours)
Both the PC and Bal are at Saint Andre's. Bal discovers the mausoleum, enchants a carpet
(`Levimotorcorpus`), flies in, meets and bonds Amelia. Amelia regrows his hand, gifts him
a Blood sygl. Mrs. Brown arranges enrollment for both Bal and the PC. Escort Verin takes
them by portal and horse-cart to the school. Bal arrives with Amelia (disguised). PC arrives
with other new students. The claiming ceremony: PC's originator is revealed by hidden quiz
result. Amelia drops her disguise, claims Bal as High Priest, warns the other originators.

**Quiz questions 1–4 embedded here** — in conversations with Mrs. Brown, other orphans,
and the school gate guard.

### Act II — The School (~5 hours)
School life begins. PC and Bal explore the school. Classes: Familiar (Mr. Porter — hostile
to Bal), Rune Working (Mr. Winters — praises Bal), Warfare (Zoran — Lora and Bal both choose
rapiers). Viridia introduces herself at lunch; Lora is met in Warfare class. First trip to
New Spring: Dawn Hall registration as team "Sygls Best" (Bal's idea, Lora's dream, Viridia
names it). Bal makes a 4th Dawn card for Amelia under the name "Amelia Norheart" — she cries
blood tears. First job: collect Aurora Felis flowers in Saegen Woods.

**Quiz questions 5–10 embedded here** — in conversations with classmates, library, first meal,
the school gate guard, and how the PC treats Bal in early scenes.

### Act III — Dawn Guild and New Spring (~8 hours)
Open hub. Dawn faction quests, side missions, crafting unlocks. Saegen Woods dungeon (ley-line
bear miniboss). Bal explains conversion to Lora and Viridia; both agree. Conversion performed:
both now Blood sygl, retain their original powers. War declared on Raizen. School confrontation
(Sean tries to physically remove Viridia; Bal uses blood whip). Anti-Blood pogroms in Raizen's
territory. Amelia negotiates with Aren to lift the school segregation rule.

### Act IV — Conversion and War (~8 hours)
Blood army at Amelia's hidden temple. Assassination attempt: assassin Iro attacks Bal at the
lake; Bal kills him with a blood whip garrote. Two scouting party attacks on the temple. Spy
network: agents with no sygls operate in Raizen's territory. Londra mission: PC, Bal, Lora,
Viridia infiltrate in disguise. Lora brews night-stalker poison, injected into a cask of
Ventish brandy. The three regular priests (Aleric, Titus, Leon) drink the cask and die. Raizen
retaliates with a lightning ambush — Lora and Viridia are captured by Toma.

**Allegiance pivot here.** Dominant axis determines path forward.

### Act V — Londra and the Duel (~5 hours + ending-specific content)
Toma's letter: mage duel, one-on-one. Win = Lora and Viridia go free. Lose = they die.
Amelia reveals: drinking blood heals Blood mages and amplifies High Priest power. Bal drinks
from a silver chalice of Amelia's blood; sees her full memories; heals completely.
The duel in a Roman-style coliseum: Toma reveals a life-bonded storm elemental (Akora —
technically not a familiar, so legal). Akora defeats Bal. As a last act, Bal converts Akora
with his own blood — Akora becomes a blood elemental (previously thought impossible). The
life-bond involuntarily converts Toma to the Blood Sygl. War ends. Raizen is imprisoned in
Amelia's old temple for 3,000 years.

---

## The Five Endings

| Ending | Unlock condition | Outcome |
|--------|-----------------|---------|
| Canon | High Bal loyalty | Toma defeated, Raizen imprisoned, victory epilogue |
| Pyrrhic | High Bal loyalty but Lora or Viridia neglected | Same but one party member dies; somber epilogue |
| Betrayal | High Originator loyalty | PC fights alongside Toma; Bal dies; Raizen restored |
| Peacemaker | ≥+50 faction rep with 3 different sygls | War ends via negotiated reform; no one dies |
| Ascension | Specific quest chain + combat/crafting thresholds | PC becomes Sixth Originator; unlocks NG+ |

NG+ exclusive: **True Ending (Cycle Broken)** — max rep all 5 sygls + defeat all 5 Originator
Remnant superbosses → sever Originators from mortal realm entirely.

---

## Combat System

Turn-based, four active party members vs up to six enemies. Turn order by SPD.
Commands: Attack / Magic / Skill / Item / Defend / Familiar Summon / Flee.

Elemental affinities follow the five sygls. ~25% damage swing for weakness/resistance.

### Blood Magic (Bal and converted party members)
- **Blood Whip** — ranged, can grab/throw/garrote. Hard as steel, razor-sharp with spikes.
- **Blood Dart** — fast ranged damage. Small weaponized blood construct.
- **Blood Dagger** — close melee blood construct. Lethal.
- **Blood Healing** — drink blood to heal. High Priests: full heal + power amplification.
- **Conversion** — story-gated. Cut both palms, mingle blood, push to target's magical center.
  Irreversible. Target retains all inherited gifts from original sygl. Requires consent.

### Familiar Summon
- Once per combat. Cooldown resets after combat ends.
- Amelia: teleportation strike, near-impossible to resist.
- Hopper (Viridia): heavy physical attack, longsword-resistant pelt.

### Status Effects
Poison, burn, paralyze, sleep, silence, charm.

### Sygl Surge (Limit Break)
Unlocked at high relationship affinity between paired characters. Powerful joint attack.

---

## The Hidden Quiz (Sygl Selection)

Ten questions embedded in natural dialogue during Acts I and II. Player never sees a quiz UI.
The originator with the highest weighted score steps forward at the claiming ceremony.
A manual override is offered ("If a different originator calls to you, step forward now").

Questions are embedded in:
1. How the PC responds when Mrs. Brown asks why they stayed quiet during a bullying incident
2. What the PC takes from their room when packing to leave for school
3. How the PC treats Bal (the kid who can't do magic "right") at the orphanage
4. What the PC says to the school gate guard
5. Whether the PC joins a crowd or sits alone at first meal
6. How the PC handles a locked door in the library
7. What the PC does when a classmate cheats on a class assignment
8. Whether the PC defends or stays silent during the first Mr. Porter incident
9. What the PC offers Bal when they first formally meet him
10. How the PC spends an unscheduled free day

---

## The Allegiance System (Hidden)

Three signed integers, range -100 to +100. Never displayed. Narrative consequences only.

- **Loyalty to Bal** — supporting his decisions, defending him, helping Sygls Best
- **Loyalty to Originator** — serving the PC's sygl priest, accepting offers, attending ceremonies
- **Self-Interest** — power-grabs, betraying any side for personal gain, Ascension path

Branch checks at: Act IV pivot, Act V infiltration, capture sequence, final duel, ending check.

---

## Faction Reputation

Five signed integers (one per sygl), -100 to +100. Visible in priest dialogue screens.

| Threshold | Effect |
|-----------|--------|
| +25 | First faction recipe unlocked |
| +50 | Recruit for that sygl becomes available |
| +50 (3 sygls) | Peacemaker ending becomes reachable |
| +75 | Advanced faction recipes + exclusive gear |
| +100 | Faction Champion title + cosmetic |
| -50 | Priest refuses quests |
| -100 | That sygl's NPCs attack on sight |

---

## Crafting System (6 Specialties, ~150 Recipes)

| Specialty | Lead character | Description |
|-----------|---------------|-------------|
| Rune Engraving | Bal | Apply runes to weapons/armor/accessories |
| Alchemy | Lora | Potions, poisons, salves, oils |
| Smithing | Viridia | Base weapons and armor |
| Enchanting | Cross-character | Fuse runes with smithed items |
| Inscription | Any | Spell scrolls, books (sellable for Dawn rep) |
| Blood Crafting | Amelia-only | Heart rings, blood elixirs (late-game) |

Star Ocean model: experimentation, random outcomes, discovery moments.
Multi-character crafting unlocks unique recipes (Bal + Lora = rune-poisoned blades, etc.).

---

## Map Areas Required

### Act I
- Saint Andre's Orphanage — exterior, front courtyard, interior corridor, Mrs. Brown's office
  (spartan: 1 photo of dead husband, 2 filing cabinets), Bal's small room, kitchen, rooftop
  (grey slate tiles — off-limits sign; boy died here years ago), back courtyard
- Amelia's Temple — Greek-style interior, open magical "sky" roof (illusion), two huge braziers,
  large statue at far end, golden double-doors (one-way lock — kept Amelia in, not others out)
- Dark forest road + Verin's cottage (firepit in front yard, ~10 miles from school)

### Act II
- The Heir's School exterior — castle approach, large iron gate, surrounding wall, inner courtyard
  with raised stone stage and 5 marble thrones (each with originator's sygl in gold)
- School ground floor — corridor with paintings, dining hall (5 long sygl-specific tables),
  Familiar class room (door to forest), training grounds (weapon racks, 6 training areas)
- School second floor — library (stairs up, right, second door on left)
- Blood Sygl dorm — stone pedestal entrance (prick palm → blood sygl opens the wall), fireplace,
  red couches, kitchen (wood stove), four-poster canopy bed, library room (configurable by Lead Mage)
- 4 other sygl dorms (Duality, Waves, Terra, Tempus)
- 5 Originator towers (exterior locked until story progress)

### Act III
- New Spring — town square, Dawn Hall (3-story wooden building, large covered porch, ornate sign,
  Soren right / Kim left behind counter, open 24/7), Mrs. Summers alchemist, enchanted weapons
  vendor, familiar items shop (Viridia buys Hopper's adjustable collar here), bookstore, food stalls
- Saegen Woods — multi-screen forest dungeon, river with brightly colored fish, aurora felis
  flower spawn, ley-line bear territory (bear miniboss enhanced by ley line)
- Whitehaven — small mid-point town, food stall (meat pies)

### Act IV
- Amelia's Hidden Temple HQ — canyon with 3 mountain sides + 1 defensible pass, massive interior
  (hundreds), High Priest quarters, 2 priest rooms, barracks, command area
- Blood army camp + battlefield maps (2 variants: defensive and offensive)

### Act V
- Londra — 3 districts, high stone walls + guarded gate (scrutinizes entry), Three Boars Inn
  (3-story stone building, stables, caters to upper class, back half reserved by priests monthly,
  ale delivered every other Wednesday from James's brewery), Winespring Inn (Mrs. Locklan, warm),
  James's brewery (large building east side, warehouse on water)
- The Coliseum — Roman-style mage duel arena; rune circle confining area for Lora and Viridia

### Plus
- World map connecting all zones
- 5 Originator tower interiors (late-game access)
- Blood Tower interior (endgame)

---

## Sprites Required

### Main cast (idle + 4-direction walk + combat poses)
| Sprite | Description |
|--------|-------------|
| PC × 4 | 2 male, 2 female variants; sygl accent color changes by quiz result |
| Bal (pre-temple) | ~16, stump at left wrist, strong build |
| Bal (post-temple) | Full left hand, Blood sygl visible on palm — swap after bonding |
| Amelia (temple outfit) | Pale, midnight-black hair, blood-red eyes, long black dress |
| Amelia (school outfit) | Black skirt, tight black sweater, black-and-red socks above calf |
| Viridia | Pale blue crackling hair, blue eyes, tall, tan, spear |
| Lora | Brown long hair, glasses, freckled, rapier |
| Hopper (small) | Juvenile Bloodwolf — light silver + blood-red fur; collar on |
| Hopper (large) | Full-size battle form — pelt repels longswords |

### Portraits (3–4 expressions each, ~25 characters)
Bal, Amelia, Viridia, Lora, Mrs. Brown, Verin, Mr. Porter (hostile expression key),
Mr. Winters, Zoran, Soren, Kim, Mrs. Summers, Thad Moren, Xavier Giles, Commander Fern,
Toma, Raizen, Aren (male), Nera (female — Aren's night form), Aleric, Titus, Leon,
Mrs. Locklan, PC (reaction expressions)

### Enemies (~40 types)
| Category | Types |
|----------|-------|
| Bandits | Scout, Fighter, Archer, Leader (longsword) |
| Forest | Wolf, Bloodwolf (silver-red fur), ley-line Bear (large), Raven |
| School | Sean (Tempus Lead Mage — story fight) |
| Army | Tempus Infantry, Archer, Storm Mage, Officer |
| Assassination | Iro (assassin, lake fight) |
| Boss tier | Bear Miniboss, Storm Elemental Akora, Blood Elemental (Akora converted), Toma |
| NG+ superbosses | Originator Remnants × 5 (Aren, Kenmei, Naisura, Raizen, Amelia) |

---

## Key Items (from novel)
| Item | Description |
|------|-------------|
| Enchanted carpet | Tutorial — Bal's `Levimotorcorpus` transport to the temple |
| Earth-manipulation ring | Bal buys for Viridia in New Spring |
| Alchemy book | Bal buys for Lora in New Spring |
| Heart-ring | Mysterious paired ring found on bandit leader — plot item |
| Signal stones | Paired light+rune communication stones — crafted equipment |
| Hopper's collar | Size-adjustment rune; allows Bloodwolf to shrink to dog-size |
| Night-stalker poison | Lora's brew: tasteless, 30-hour kill, no antidote |
| Aurora Felis flowers | Rare (bloom every 3 years), used in healing potions, 72-hour shelf life |
| Ventish brandy cask | Act IV quest item; vessel for the poison delivery |
| Blood vials | Bal's combat consumable — bandolier of his own blood |
| Silver chalice | Amelia's blood vessel; used to amplify Bal before the duel |

---

## Audio Required (~20 tracks)
Saint Andre's theme, temple discovery, travel/journey, school arrival, school day-to-day,
Dawn Hall/New Spring market, Saegen Woods, Blood army camp, Londra infiltration, normal
combat, boss combat, Amelia's theme, war declaration, mage duel, 5 ending themes, title screen

---

## Save Data
5 slots (one per sygl path). Each slot ~5 KB. SRAM is 32 KB total.
Per-slot: party data, inventory, quest flags, allegiance triple, faction rep ×5, personal rep
table, crafting state, world flags, romance state, checksum.
Global header (2 KB): endings unlocked, NG+ flag, difficulty unlocks, language preference.
