/**
 * WinmmProxy.cpp
 * Virtua Cop 2 Accessibility Mod — WINMM.DLL Proxy
 *
 * How it works:
 *   1. This DLL is placed in the game folder named WINMM.dll
 *   2. When the game loads, it loads our DLL instead of the real WINMM.dll
 *   3. We forward all WINMM function calls to the real Windows WINMM.dll
 *   4. We hook timeGetTime() (called every frame) to poll game state and
 *      announce changes via Tolk to the screen reader
 *
 * Build: Win32 (x86) DLL — must be 32-bit to match the game!
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include "ScreenReader.h"
#include "GameState.h"
#include "KeyboardHook.h"
#include "Announcer.h"
#include "DebugLogger.h"
#include "Loc.h"

// -----------------------------------------------------------------------
// Real WINMM.dll handle
// -----------------------------------------------------------------------
static HMODULE g_hRealWinmm = nullptr;

// -----------------------------------------------------------------------
// Forward declarations for all exported WINMM functions we proxy
// -----------------------------------------------------------------------
typedef DWORD(WINAPI* PFN_timeGetTime)();
typedef MMRESULT(WINAPI* PFN_timeBeginPeriod)(UINT uPeriod);
typedef MMRESULT(WINAPI* PFN_timeEndPeriod)(UINT uPeriod);
typedef BOOL(WINAPI* PFN_PlaySoundA)(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
// Add more as needed — only forward what the game actually calls

static PFN_timeGetTime      real_timeGetTime      = nullptr;
static PFN_timeBeginPeriod  real_timeBeginPeriod  = nullptr;
static PFN_timeEndPeriod    real_timeEndPeriod    = nullptr;
static PFN_PlaySoundA       real_PlaySoundA       = nullptr;

// -----------------------------------------------------------------------
// State
// -----------------------------------------------------------------------
static bool g_initialized = false;
static DWORD g_lastPollTime = 0;
static const DWORD POLL_INTERVAL_MS = 100; // Poll game state every 100ms

// -----------------------------------------------------------------------
// Load real WINMM.dll from System32
// -----------------------------------------------------------------------
static bool LoadRealWinmm()
{
    char sysDir[MAX_PATH];
    GetSystemDirectoryA(sysDir, MAX_PATH);
    std::string path = std::string(sysDir) + "\\WINMM.dll";

    g_hRealWinmm = LoadLibraryA(path.c_str());
    if (!g_hRealWinmm) {
        DebugLogger::Log("ERROR: Failed to load real WINMM.dll from " + path);
        return false;
    }

    real_timeGetTime     = (PFN_timeGetTime)    GetProcAddress(g_hRealWinmm, "timeGetTime");
    real_timeBeginPeriod = (PFN_timeBeginPeriod)GetProcAddress(g_hRealWinmm, "timeBeginPeriod");
    real_timeEndPeriod   = (PFN_timeEndPeriod)  GetProcAddress(g_hRealWinmm, "timeEndPeriod");
    real_PlaySoundA      = (PFN_PlaySoundA)     GetProcAddress(g_hRealWinmm, "PlaySoundA");

    DebugLogger::Log("Real WINMM.dll loaded successfully");
    return true;
}

// -----------------------------------------------------------------------
// Accessibility mod initialization (runs once on first timeGetTime call)
// -----------------------------------------------------------------------
static void InitAccessibilityMod()
{
    if (g_initialized) return;
    g_initialized = true;

    DebugLogger::Init("VC2_AccessMod_Debug.log");
    DebugLogger::Log("=== Virtua Cop 2 Accessibility Mod v1.0 ===");

    Loc::Init("en");
    ScreenReader::Init();
    GameState::Init();
    KeyboardHook::Init();

    DebugLogger::Log("Mod initialized. Announcing startup.");
    ScreenReader::Say(Loc::Get("mod_loaded")); // "Virtua Cop 2 Accessibility Mod loaded"
}

// -----------------------------------------------------------------------
// Per-frame poll — called inside our timeGetTime hook
// -----------------------------------------------------------------------
static void PollGameState(DWORD now)
{
    if (now - g_lastPollTime < POLL_INTERVAL_MS) return;
    g_lastPollTime = now;

    GameStateSnapshot prev = GameState::GetCurrent();
    GameState::Update();
    GameStateSnapshot curr = GameState::GetCurrent();

    Announcer::AnnounceChanges(prev, curr);
}

// -----------------------------------------------------------------------
// Exported functions — forwarded to real WINMM.dll
// -----------------------------------------------------------------------

extern "C" __declspec(dllexport)
DWORD WINAPI timeGetTime()
{
    DWORD result = real_timeGetTime ? real_timeGetTime() : GetTickCount();

    // Use this frequent call as our "heartbeat"
    if (!g_initialized) InitAccessibilityMod();
    PollGameState(result);

    return result;
}

extern "C" __declspec(dllexport)
MMRESULT WINAPI timeBeginPeriod(UINT uPeriod)
{
    return real_timeBeginPeriod ? real_timeBeginPeriod(uPeriod) : TIMERR_NOERROR;
}

extern "C" __declspec(dllexport)
MMRESULT WINAPI timeEndPeriod(UINT uPeriod)
{
    return real_timeEndPeriod ? real_timeEndPeriod(uPeriod) : TIMERR_NOERROR;
}

extern "C" __declspec(dllexport)
BOOL WINAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound)
{
    return real_PlaySoundA ? real_PlaySoundA(pszSound, hmod, fdwSound) : FALSE;
}

// -----------------------------------------------------------------------
// DLL entry point
// -----------------------------------------------------------------------
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        LoadRealWinmm();
        break;

    case DLL_PROCESS_DETACH:
        KeyboardHook::Shutdown();
        ScreenReader::Shutdown();
        DebugLogger::Log("Mod unloaded.");
        if (g_hRealWinmm) {
            FreeLibrary(g_hRealWinmm);
            g_hRealWinmm = nullptr;
        }
        break;
    }
    return TRUE;
}
