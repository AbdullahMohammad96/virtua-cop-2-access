# Project Status: Virtua Cop 2 Accessibility Mod

## Project Info

- **Game:** Virtua Cop 2 (PC/Windows port, 1999, SEGA)
- **Engine:** Custom SEGA Win32/DirectX engine
- **Architecture:** 32-bit (PPJ2DD.EXE is PE32 for Intel 80386)
- **Mod Loader:** None — Win32 DLL proxy approach (WINMM.dll or DINPUT.dll proxy)
- **Runtime:** C++ (native Win32
- **Game directory:** [User to fill in — e.g. C:\Games\VirtuaCop2\VirtuaCop2]
- **User experience level:** [To be determined]
- **User game familiarity:** [To be determined]

## What This Game Is

Virtua Cop 2 is a 1999 on-rails light-gun arcade shooter ported to PC. Key facts:

- Players shoot enemies as the camera moves automatically through stages
- Mouse or light-gun controls aiming; click to shoot
- Three stages (Stage 1, 2, 3) plus a final boss stage (Stage 4)
- Menus: Title screen, Stage Select, Difficulty Select, Gun Select, Ranking, Continue, Options
- Player has a life count; getting hit loses a life; losing all lives means Game Over or Continue
- Score is tracked; headshots and accuracy bonuses affect score
- Two-player mode supported (P1 and P2)
- Game strings found: Easy / Normal / Hard difficulty; 1PLife / 2PLife; GunSelect; Continue; Ranking

## Approach: Win32 DLL Proxy

Since Virtua Cop 2 is a compiled Win32 C++ game with no scripting layer, The accessibility mod uses the **DLL proxy technique**:

1. Rename an original DLL the game loads (e.g., WINMM.dll) to WINMM_orig.dll
2. Place a custom proxy DLL named WINMM.dll in the game folder
3. The proxy DLL forwards all original calls to WINMM_orig.dll AND runs our accessibility code
4. Our code hooks into Windows APIs (SetWindowsHookEx for keyboard/mouse, DirectInput hooks,
   or memory polling via ReadProcessMemory) to monitor game state and announce it via Tolk

The game loads: DINPUT.dll, DSOUND.dll, WINMM.dll, DPCTRL.DLL, HGL_D3D.DLL, PG_STG*.DLL

WINMM.dll is the best proxy target because:
- It's a standard Windows DLL (we can find its full export list)
- The game uses it for audio timing (timeGetTime is a key function we can hook)
- timeGetTime is called frequently, giving us a regular "heartbeat" to poll game state

## Screen Reader Integration

Using **Tolk** (Tolklib)
- Tolk is a C++ DLL that bridges to NVDA, JAWS, and SAPI
- Since our proxy DLL is also C++, Tolk integrates cleanly (no JNI/ctypes needed)
- Download Tolk from: https://github.com/ndarilek/tolk
- Required files: Tolk.dll + nvdaControllerClient32.dll (32-bit game!)

## Setup Progress

- [ ] Game directory confirmed
- [ ] Tolk DLLs obtained (Tolk.dll + nvdaControllerClient32.dll — 32-bit versions!)
- [ ] C++ build environment set up (Visual Studio 2019+ or MinGW-w64)
- [ ] WINMM proxy DLL skeleton compiled
- [ ] Tolk integrated — "mod loaded" spoken on game start
- [ ] Memory layout analysis done (find life/score/stage addresses)
- [ ] Game key bindings documented
- [ ] Menu state detection working
- [ ] In-game state announcements working

## Current Phase

**Phase:** Setup / Planning
**Currently working on:** Architecture and game analysis
**Blocked by:** Need C++ build environment + memory address research

## Game Key Bindings (Original)

From strings analysis of PPJ2DD.EXE:
- Mouse movement: Aim crosshair
- Left mouse button / trigger: Shoot
- Off-screen shot: Reload (standard light-gun mechanic)
- Keyboard: Likely Enter/Space for menus (to be confirmed)
- Escape: Likely pause or back
- Game uses DirectInput for controller/gun input

Safe mod keys (function keys are typically unused by game):
- F1: Read current status aloud
- F2: Re-announce last message
- F3: Announce score
- F4: Announce difficulty / stage
- F5: Announce lives remaining
- F12: Toggle debug mode

## Implemented Features

- (none yet — project in planning phase)

## Pending Tests

- (none yet)

## Known Issues

- Game is 32-bit only — all DLLs including Tolk must be 32-bit
- Memory addresses for game state (lives, score, stage) are not yet known
  and require memory analysis (Cheat Engine or similar) — this step may
  require sighted assistance
- The DLL proxy approach requires renaming original DLLs — make a backup first

## Architecture Decisions

- Win32 DLL proxy (WINMM.dll) chosen over other approaches
- Tolk used for screen reader output native C++ integration)
- Memory polling via ReadProcessMemory for game state (lives, score, stage)
  called on timeGetTime heartbeat (~60fps)
- OR: Hook DirectDraw/Direct3D present call for render-tick polling
- State management: Simple C++ struct tracking previous vs current state,
  announce only on change

## Key Bindings (Mod)

- F1: Announce current game state (lives, stage, score)
- F2: Repeat last announcement
- F3: Announce score
- F4: Announce current stage and difficulty
- F5: Announce lives remaining
- F12: Toggle debug logging to file

## current unknowns

- The main unknown is memory addresses for game state variables (1PLife, 2PLife,
  score, stage number). These need to be found via Cheat Engine (sighted help may
  be needed for initial address discovery).
- Once addresses are known, the polling approach is straightforward.
- See docs/game-api.md for all discovered strings and memory map notes.
- See docs/setup-guide-win32.md for full build instructions.
