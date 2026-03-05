# Virtua Cop 2 — Game API Reference

## Game Overview

- **Title:** Virtua Cop 2
- **Platform:** PC Windows (1999 SEGA port)
- **Genre:** On-rails light-gun shooter
- **Architecture:** 32-bit Win32, DirectX 5/6 era
- **Main executable:** PPJ2DD.EXE (PE32 for Intel 80386)

## Executable & DLL Structure

- PPJ2DD.EXE — main game executable (32-bit)
- VC2.EXE — launcher / DOS stub that runs PPJ2DD.EXE
- DPCTRL.DLL — DirectPlay multiplayer controller
- DDGL.DLL — DirectDraw graphics layer
- HGL_D3D.DLL / HGL_DD.DLL — SEGA HGL (Hardware Graphics Library) for D3D and DirectDraw
- PG_STG1.DLL / PG_STG2.DLL / PG_STG3.DLL — Stage logic per stage
- WINCPUID.DLL — CPU detection utility

## DLLs Loaded by Game (from import table)

- DINPUT.dll — DirectInput for mouse/gun/controller
- DSOUND.dll — DirectSound for audio
- WINMM.dll — Windows Multimedia (timeGetTime, etc.)
- COMCTL32.dll — Windows common controls
- USER32.dll — Window messages, keyboard state
- KERNEL32.dll — Core Win32

## Strings Extracted from PPJ2DD.EXE

### Game State Strings
- "1PLife" — Player 1 life count label
- "2PLife" — Player 2 life count label
- "ClearCnt" — Clear count (stages cleared)
- "Shoot" — Shoot action
- "GunSelect" — Gun selection menu
- "GunSpecial" — Special gun mode
- "Continue" — Continue screen
- "Difficulty" — Difficulty setting
- "DeviceSelect" — Input device selection
- "Ranking%d" — Ranking screen (numbered)
- "Rank%d" — Rank value

### Stage/Menu Strings
- "STAGE:SELECT" — Stage select menu
- "STAGE:1" through "STAGE:4" — Stage identifiers
- "ENDING::INTERMISSION" — Ending/intermission screen
- "RAILLINE:SHOOTOUT" — Rail line shootout (gameplay mode)
- "GUN:SELECT" — Gun select screen
- "::::GUN:ADJUST:" — Gun calibration
- ":::::CONTINUE" — Continue screen
- ":::::::LIFE" — Life display
- ":::GUN:LOOSENESS" — Gun looseness setting
- "::::DIFFICULTY" — Difficulty menu
- "::CALIBRATE:GUN" — Gun calibration
- ":RANKING" — Ranking screen
- ":HARD:" — Hard difficulty
- "NORMAL" — Normal difficulty
- ":EASY:" — Easy difficulty
- "::PLAYER1::GAMES" — Player 1 games counter
- "::PLAYER2::GAMES" — Player 2 games counter
- "GUN:SELECT" — Gun select screen

### Multiplayer (DPCTRL.DLL exports)
- ChatStart
- CheckLobby
- CloseProvider
- DestroyDirectPlay
- GetPlayerName
- GetRivalName
- InitialDirectPlay
- ReceiveDirectPlay
- SWDataSyncReceive
- SWDataSyncSend
- SendDirectPlay
- SendDirectPlaySWResetMessage
- SendDirectPlayWaitMessage

### Player Name Formats
- "Player%d" (English)
- "Spieler%d" (German)
- "Joueur%d" (French)
- "Jugador%d" (Spanish)

## Game Mechanics Summary

### On-Rails Shooter Gameplay
1. The camera moves automatically through 3D environments
2. Player aims with mouse (or light gun) — a crosshair appears on screen
3. Left-click to shoot enemies
4. Shooting off-screen reloads the gun (standard light-gun mechanic)
5. Enemies shoot back; getting hit costs a life
6. Stage clears when all required enemies are defeated
7. Boss fights at end of each stage

### Lives System
- Player starts with a set number of lives (configurable in Options)
- Getting hit = -1 life
- Zero lives = Game Over / Continue screen
- Continue: Insert coin (press key) to continue from checkpoint

### Scoring
- Base points per enemy kill
- Headshot bonus
- Time bonus at stage end
- Accuracy rating at stage end
- Ranking screen at game end

### Menus (in order)
1. Title Screen — Press Start
2. Player Count (1P / 2P)
3. Stage Select (Stage 1, 2, 3, or ADV mode)
4. Difficulty (Easy / Normal / Hard)
5. Gun Select (pistol types with different looseness/rate of fire)
6. (Gameplay)
7. Continue screen (on Game Over)
8. Ranking (end of game)
9. Options (from title: calibrate gun, set lives, difficulty default)

## Memory Map (UNKNOWN — needs Cheat Engine analysis)

These memory addresses are NOT yet known and require sighted assistance
with Cheat Engine or similar tool to discover:

- Player 1 Lives: UNKNOWN
- Player 2 Lives: UNKNOWN
- Player 1 Score: UNKNOWN
- Player 2 Score: UNKNOWN
- Current Stage: UNKNOWN
- Current Game State (menu / gameplay / game over): UNKNOWN
- Difficulty setting: UNKNOWN
- Time remaining (if any): UNKNOWN

### Strategy for Finding Addresses
1. Open Cheat Engine, attach to PPJ2DD.EXE
2. Search for initial value (e.g., 3 lives = search for value 3)
3. Lose a life, search for new value (2)
4. Repeat until single address found
5. Verify by changing value and seeing effect in game
6. Note the static offset from module base (for ASLR-stable addressing)

## Input System

- DirectInput (DINPUT.dll) for mouse, gun controller, joystick
- GetKeyState (USER32.dll) possibly used for keyboard shortcuts
- Mouse position maps to screen coordinates for crosshair
- Off-screen mouse position triggers reload

## Safe Mod Keys

These function keys are NOT used by the game (based on string analysis):
- F1 — safe
- F2 — safe
- F3 — safe
- F4 — safe
- F5 — safe
- F6 — safe
- F7 — safe
- F8 — safe
- F9 — safe
- F10 — potentially used by Windows menus (avoid)
- F11 — safe
- F12 — safe

## Hook Points for Accessibility

### Best Hook: WINMM.dll proxy via timeGetTime

The game calls timeGetTime() regularly for frame timing. By proxying WINMM.dll:
1. Our timeGetTime hook runs every frame (~60fps)
2. We read game state from memory
3. If state changed → speak via Tolk
4. Forward original call to real WINMM.dll

### Alternative Hook: Direct3D / DirectDraw Present

If WINMM proxy causes issues, hooking the Present() call of HGL_D3D.DLL would
give the same per-frame heartbeat.

### Keyboard Hook: SetWindowsHookEx (WH_KEYBOARD_LL)

For mod hotkeys (F1-F12), install a low-level keyboard hook. This runs in our
DLL's code, catching keystrokes before the game processes them.

## BIN File Format

The BIN folder contains binary game data:
- L_*.BIN — likely level/layout data
- P_*.BIN — likely polygon/model data
- MOTADV.BIN / MOTSTG*.BIN — motion/animation data
- CAMMOV*.BIN — camera movement paths

These are compiled binary formats. No text strings for accessibility extraction.
The SE (sound effects) folder likely contains .wav or proprietary audio.

## Known Limitations

- No scripting layer (Lua, Python, C#) — pure C++
- No official mod API or SDK
- Memory addresses must be found via Cheat Engine (requires sighted help initially)
- 32-bit process — all injected code must also be 32-bit
- DirectX 5/6 era — HGL graphics layer wraps DirectDraw and Direct3D
