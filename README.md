# Virtua Cop 2 Accessibility Mod

Accessibility mod for **Virtua Cop 2** (PC Windows, 1999, SEGA).
Enables blind players using screen readers (NVDA, JAWS, SAPI) to receive audio
announcements of game state, lives, score, stage changes, and Game Over.

---

## What This Mod Does

- Announces game mode transitions (Title Screen, Stage Select, Difficulty, Gameplay, Game Over, etc.)
- Announces when you lose a life — "Player 1 hit! Lives: 2"
- Warns you on last life — "Warning: last life!"
- Announces stage clears and stage entry
- Provides on-demand status with F1 — "Gameplay. Stage 2. Normal. Player 1 lives: 3. Score: 45200"
- Announces score milestones (every 10,000 points, queued so it doesn't interrupt)
- Queues helpful key hints after mode transitions

## Hotkeys

- F1 — Full status (mode, stage, difficulty, lives, score)
- F2 — Repeat last announcement
- F3 — Score only
- F4 — Stage and difficulty only
- F5 — Lives only
- F12 — Toggle debug log file

## Installation

See `docs/setup-guide-win32.md` for full build and install instructions.

Quick summary:
1. Build the project in Visual Studio (Win32/x86 — must be 32-bit!)
2. Get Tolk.dll + nvdaControllerClient32.dll (32-bit) from https://github.com/ndarilek/tolk/releases
3. Copy both Tolk DLLs to the game folder
4. Run `scripts\Deploy-Mod.ps1 -GameDir "C:\Path\To\VirtuaCop2"`

## Important: Memory Addresses Needed

The lives/score/stage announcements require memory offsets that are game-specific.
**These are currently set to placeholder zeros and must be filled in.**

See `docs/game-api.md` → "Memory Map" section for instructions on using Cheat Engine
to find the correct offsets. This step requires sighted assistance.

Once offsets are found, update `src/GameState.cpp` in the `Offsets` namespace.

## Project Structure

- `src/` — C++ source files
  - `WinmmProxy.cpp` — Main DLL entry point (WINMM.dll proxy)
  - `ScreenReader.cpp/.h` — Tolk screen reader wrapper
  - `GameState.cpp/.h` — Memory polling and state tracking
  - `KeyboardHook.cpp/.h` — F1-F5, F12 hotkeys
  - `Announcer.cpp/.h` — Formats and announces state changes
  - `Loc.cpp/.h` — Localization strings
  - `DebugLogger.cpp/.h` — Debug file logging
- `docs/` — Documentation
  - `setup-guide-win32.md` — Full build and install guide
  - `game-api.md` — Game structure, strings, memory map
- `scripts/` — PowerShell helpers
  - `Deploy-Mod.ps1` — Deploy to game folder
- `project_status.md` — Current project status and progress tracking
- `VC2AccessibilityMod.vcxproj` — Visual Studio project file

## Screen Reader Compatibility

- NVDA — supported (requires nvdaControllerClient32.dll)
- JAWS — supported via Tolk
- Windows SAPI — automatic fallback if no screen reader detected

## License

MIT — free to use, share, and modify. Contributions welcome.
