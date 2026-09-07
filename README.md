# BotW Unexplored

BotW Unexplored is a Nintendo Switch homebrew map companion for *The Legend of Zelda: Breath of the Wild*. It reads the selected local save (or a previously created backup while the game is running) and shows collectible progress directly on the console.

This repository is the maintained fork of [lud99/botw-unexplored](https://github.com/lud99/botw-unexplored). The current fork release is **v2.2.3**.

[Русская версия документации](docs/README.ru.md)

## Features

- English, Russian, and Spanish user interfaces. Russian Korok hints and location names use the Switch localization data.
- Koroks, shrines, DLC shrines, locations, Hinoxes, Taluses, and Moldugas.
- Normal Mode and Master Mode, including save-backup support when BotW is running.
- A localized object information card with type, name where available, game coordinates, icon, and completion state.
- Existing Korok hints, guide images, and route rendering remain available through touch selection.
- `Missing`, `Completed`, and `All` display modes for every category.
- Per-profile and per-mode manual progress for every supported object type. It never modifies the BotW save.
- Controller cursor: D-Pad moves the cursor, `A` opens the nearest visible object, and `ZR` smoothly focuses the nearest missing visible object.
- Persisted camera, zoom, legend visibility, display mode, and language. Legacy settings are migrated safely.

## Installation

1. Download `botw-unexplored.nro` from [Releases](https://github.com/sklart/botw-unexplored/releases).
2. Copy it to `sdmc:/switch/botw-unexplored/botw-unexplored.nro`.
3. Start it from the Homebrew Menu and select the Switch profile that owns the BotW save.

For backup-based use while BotW is running, start BotW Unexplored once with the game closed first. This creates a backup of the selected save.

## Controls

| Context | Control | Action |
| --- | --- | --- |
| Map | Left stick or touch drag | Move the map |
| Map | Right-stick Y, `L` / `R` | Zoom |
| Map | `X` | Open or close the legend / close an information panel |
| Map | `Y` | Switch Normal Mode / Master Mode when available |
| Map | `-` | Choose another profile |
| Map | `+` | Exit |
| Map | D-Pad | Move the controller cursor |
| Map | `A` | Open the full Korok guide for a Korok, or ObjectInfo for another object |
| Map | `ZR` | Smoothly focus the nearest missing visible object and place the cursor on it |
| Object info | `B` | Mark a missing object as found while using a backup |
| Legend | D-Pad, `A`, or touch | Select/toggle a category or cycle display mode |
| Legend | `ZL` | Switch language |
| Korok hint | `B` | Mark the Korok as found while using a backup and close the hint |

## Saved data

The app stores its own files under `sdmc:/switch/botw-unexplored/`:

- `settings.txt` is a versioned settings file. Older settings files are accepted and rewritten in the current format.
- `language.txt` remains for compatibility with earlier releases.
- `manual_progress.dat` is a versioned manual-progress file. Entries are keyed by Switch profile, Normal/Master Mode, object type, and completion hash. A completion later confirmed by a readable game save removes the corresponding override.

No file in this directory changes the actual BotW save.

## Build

Install devkitPro/devkitA64 and the Switch packages:

```bash
pacman -S switch-mesa switch-glad switch-glm switch-freetype
```

Then build:

```bash
make
```

`make DEBUG=1` enables per-frame OpenGL diagnostics. The release build does not poll `glGetError()` every frame.

The project uses `aarch64-none-elf-pkg-config` for FreeType when available and retains a tested fallback for older devkitPro installations.

## Tests and CI

Run the host-side checks with:

```bash
bash tests/run_host_tests.sh
```

They cover settings migration and validation, UTF-8 decoding, manual-progress profile/mode/type separation, object metadata mapping, data counts, and duplicate completion hashes. GitHub Actions also builds the Switch `.nro` and uploads it as an artifact.

## Localization sources

The location-localization generator is in [`tools`](tools/README.md). It expects message archives extracted from a lawfully obtained Switch copy of the game; those game files are deliberately ignored by Git and are not distributed by this repository.

## Credits

- [lud99/botw-unexplored](https://github.com/lud99/botw-unexplored) — original application and codebase.
- [marcrobledo/savegame-editors](https://github.com/marcrobledo/savegame-editors), [MrCheeze/botw-waypoint-map](https://github.com/MrCheeze/botw-waypoint-map), [Zelda Dungeon](https://www.zeldadungeon.net/), and [d4mation/botw-unexplored-viewer](https://github.com/d4mation/botw-unexplored-viewer) — original data, research, and guide references.

BotW Unexplored is an unofficial homebrew project and is not affiliated with Nintendo.
