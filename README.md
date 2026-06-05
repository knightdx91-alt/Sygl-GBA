# Sygl GBA

A Game Boy Advance JRPG adventure based on the Sygl book series.

## Play Online
Visit the [GitHub Pages site](https://knightdx91-alt.github.io/Sygl-GBA/) to play in your browser using EmulatorJS.

## Download ROM
Download the latest `sygl.gba` from the [Releases](https://github.com/knightdx91-alt/Sygl-GBA/releases) page or from the GitHub Pages site.

## Building Locally

### Requirements
- devkitPro (devkitARM)
- Make
- Python 3

### Build
```bash
export PYTHON=python3
make clean
make -j$(nproc)
```

The compiled ROM will be at `build/sygl.gba`.

## Documentation

- **[CLAUDE.md](CLAUDE.md)** - Codebase guide & architecture
- **[docs/design.md](docs/design.md)** - Game design document
- **[docs/DECISIONS.md](docs/DECISIONS.md)** - Architecture decisions
- **[assets/lore/sygl_complete.txt](assets/lore/sygl_complete.txt)** - Source novel/lore

## Play Locally

Use any GBA emulator:
- **Emulation Station** - MAME, Retroarch
- **mGBA** - Cross-platform
- **VBA-M** - Windows/Linux
- **Real hardware** - Flash to cartridge

## License

All code is proprietary. Sygl universe belongs to the original author.
