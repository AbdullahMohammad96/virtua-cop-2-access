# Setup Guide — Virtua Cop 2 Accessibility Mod (Win32)

This guide explains how to build and install the accessibility mod for Virtua Cop 2.

---

## What Is a DLL Proxy?

Virtua Cop 2 is a 1999 Win32 C++ game. It loads certain Windows DLLs at startup,
including WINMM.dll (Windows Multimedia). We:

1. Rename WINMM.dll (the real Windows one in the game folder, if present — see below)
   to WINMM_orig.dll
2. Place our custom WINMM.dll in the game folder
3. When the game loads WINMM.dll, it loads ours instead
4. Our DLL forwards all function calls to the real Windows WINMM
5. AND runs our accessibility code on every frame

This is the same technique used by many PC game mods and patches.

---

## Requirements

### Software

- Windows 10 or 11 (64-bit OS is fine — we target 32-bit output)
- Visual Studio 2019 or 2022 (Community edition is free)
  - During install, select "Desktop development with C++"
  - Make sure "MSVC v142/v143 — VS 2019/2022 C++ x86/x64 build tools" is included
  - Also select "Windows 10 SDK" or "Windows 11 SDK"
- Git (optional, for version control)

### Tolk — Screen Reader Library

Tolk is the same screen reader bridge used by Unity accessibility mods. It supports NVDA, JAWS, and SAPI.

Download: https://github.com/ndarilek/tolk/releases

You need the 32-bit versions (because the game is 32-bit):
- Tolk.dll (32-bit)
- nvdaControllerClient32.dll (for NVDA support)
- Tolk.h (header file for compiling)
- Tolk.lib (import library for linking)

Copy Tolk.dll and nvdaControllerClient32.dll to the game folder alongside VC2.EXE.

---

## Build Steps

### Step 1: Set Up the Visual Studio Project

1. Open Visual Studio
2. Create new project: "Dynamic-Link Library (DLL)" → C++
3. Name it: VC2AccessibilityMod
4. Platform: Win32 (x86) — NOT x64! The game is 32-bit.
5. Configuration: Release (or Debug for testing)

### Step 2: Add Source Files

Copy the files from the src/ folder of this mod package into your project:
- WinmmProxy.cpp — Main proxy DLL entry point and WINMM forwarding
- ScreenReader.cpp / .h — Tolk wrapper (adapted from template)
- GameState.cpp / .h — Memory polling and state tracking
- KeyboardHook.cpp / .h — Hotkey handler (F1-F5, F12)
- Announcer.cpp / .h — Formats and announces game state changes
- Loc.cpp / .h — Localization (English strings)
- DebugLogger.cpp / .h — Debug file logging

### Step 3: Configure Project Settings

In Project Properties (right-click project → Properties):

- Configuration: All Configurations
- Platform: Win32

Under C/C++ → General → Additional Include Directories:
- Add path to Tolk.h

Under Linker → General → Additional Library Directories:
- Add path to Tolk.lib

Under Linker → Input → Additional Dependencies:
- Add: Tolk.lib, user32.lib, kernel32.lib, winmm.lib

Under Linker → Advanced → Entry Point:
- Leave blank (DllMain is the entry point)

### Step 4: Build

- Build → Build Solution (or Ctrl+Shift+B)
- Look in Release\VC2AccessibilityMod.dll (or Debug\)
- This DLL must be renamed to WINMM.dll for deployment

### Step 5: Deploy to Game Folder

Run the deploy script (scripts/Deploy-Mod.ps1) OR do manually:

1. Navigate to game folder (e.g., C:\Games\VirtuaCop2\VirtuaCop2)
2. Check if WINMM.dll already exists there
   - If yes: rename it to WINMM_orig.dll (backup!)
   - If no: the game uses the system WINMM.dll — our proxy will load it from System32
3. Copy your built VC2AccessibilityMod.dll to the game folder
4. Rename it to WINMM.dll
5. Copy Tolk.dll and nvdaControllerClient32.dll to the game folder
6. Launch the game — your screen reader should announce "Virtua Cop 2 Accessibility Mod loaded"

---

## How to Find Memory Addresses (Requires Sighted Help)

The mod needs to read game memory to know the player's current lives, score, and stage.
These addresses must be found with Cheat Engine. This step requires a sighted collaborator
if you are a blind user, because Cheat Engine's interface is not accessible.

### What to find with Cheat Engine

Ask your sighted helper to:

1. Open Cheat Engine, click the computer icon, attach to PPJ2DD.EXE
2. **Player Lives:** Start game with 3 lives → search for "3" (4-byte integer)
   Lose a life → search for "2" → repeat → find the stable address
3. **Score:** Start game with 0 score → search for "0" → shoot enemy → search for new value
4. **Current Stage:** Varies — try searching for 1 when in Stage 1, 2 in Stage 2, etc.
5. For each found address: right-click → "Find out what accesses this address"
   This reveals the base pointer + offset for stable addressing across game restarts

### Once addresses are found

Update GameState.cpp with the real offsets:

```cpp
// Example — replace with real values from Cheat Engine
#define OFFSET_1P_LIVES    0x00000000  // TODO: fill in real offset
#define OFFSET_SCORE       0x00000000  // TODO: fill in real offset
#define OFFSET_STAGE       0x00000000  // TODO: fill in real offset
#define OFFSET_GAME_STATE  0x00000000  // TODO: fill in real offset
```

---

## PowerShell Deploy Script

Save as scripts/Deploy-Mod.ps1:

```powershell
param(
    [string]$GameDir = "C:\Games\VirtuaCop2\VirtuaCop2",
    [string]$BuildDir = ".\Release"
)

$proxyDll = Join-Path $BuildDir "VC2AccessibilityMod.dll"
$targetDll = Join-Path $GameDir "WINMM.dll"
$backupDll = Join-Path $GameDir "WINMM_orig.dll"

if (-not (Test-Path $proxyDll)) {
    Write-Host "ERROR: Build output not found at $proxyDll" -ForegroundColor Red
    exit 1
}

# Backup original WINMM.dll if present
if ((Test-Path $targetDll) -and -not (Test-Path $backupDll)) {
    Write-Host "Backing up original WINMM.dll..."
    Copy-Item $targetDll $backupDll
}

Write-Host "Deploying mod DLL..."
Copy-Item $proxyDll $targetDll -Force

Write-Host "Done! Launch VC2.EXE to test." -ForegroundColor Green
```

Run with: `powershell -ExecutionPolicy Bypass -File scripts\Deploy-Mod.ps1 -GameDir "C:\Path\To\Game"`

---

## Troubleshooting

**Game won't start after installing mod:**
- Verify mod DLL is 32-bit (not 64-bit)
- Check that Tolk.dll and nvdaControllerClient32.dll are in the game folder
- Try renaming WINMM.dll back to the original and see if the game starts without it
- Look for a VC2_AccessMod_Debug.log file in the game folder (created in Debug mode)

**No speech output:**
- Ensure NVDA or JAWS is running before launching the game
- Check that nvdaControllerClient32.dll is present (32-bit version!)
- Try SAPI fallback: Tolk auto-detects screen readers; if NVDA/JAWS not found, it uses SAPI

**Mod loads but announces wrong information:**
- Memory addresses are probably wrong or not yet filled in
- Enable debug logging (F12) and check VC2_AccessMod_Debug.log

**Hotkeys not responding:**
- The WH_KEYBOARD_LL hook may have a timing issue at startup
- Check debug log for "Keyboard hook installed" message
