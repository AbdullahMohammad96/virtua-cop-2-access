/**
 * KeyboardHook.cpp
 * Low-level keyboard hook implementation.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "KeyboardHook.h"
#include "Announcer.h"
#include "ScreenReader.h"
#include "GameState.h"
#include "DebugLogger.h"

HHOOK KeyboardHook::s_hHook = nullptr;

// -----------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------
void KeyboardHook::Init()
{
    s_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (s_hHook) {
        DebugLogger::Log("Keyboard hook installed successfully.");
    } else {
        DWORD err = GetLastError();
        DebugLogger::Log("WARNING: Failed to install keyboard hook. Error: " + 
                         std::to_string(err));
    }
}

// -----------------------------------------------------------------------
// Shutdown
// -----------------------------------------------------------------------
void KeyboardHook::Shutdown()
{
    if (s_hHook) {
        UnhookWindowsHookEx(s_hHook);
        s_hHook = nullptr;
        DebugLogger::Log("Keyboard hook removed.");
    }
}

// -----------------------------------------------------------------------
// Hook callback — runs for every keypress system-wide
// -----------------------------------------------------------------------
LRESULT CALLBACK KeyboardHook::LowLevelKeyboardProc(
    int nCode, WPARAM wParam, LPARAM lParam)
{
    // nCode < 0 means we must pass to next hook without processing
    if (nCode < 0) {
        return CallNextHookEx(s_hHook, nCode, wParam, lParam);
    }

    // Only handle WM_KEYDOWN (not WM_KEYUP or WM_SYSKEYDOWN)
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kb = (KBDLLHOOKSTRUCT*)lParam;
        const GameStateSnapshot& curr = GameState::GetCurrent();

        switch (kb->vkCode) {
        case VK_F1:
            // Full status announcement
            Announcer::AnnounceFullStatus(curr);
            // Don't consume — let the game process F1 too (it likely ignores it)
            break;

        case VK_F2:
            // Repeat last announcement
            ScreenReader::RepeatLast();
            break;

        case VK_F3:
            // Score only
            Announcer::AnnounceScore(curr);
            break;

        case VK_F4:
            // Stage and difficulty
            Announcer::AnnounceStageDifficulty(curr);
            break;

        case VK_F5:
            // Lives only
            Announcer::AnnounceLives(curr);
            break;

        case VK_F12:
            // Toggle debug logging
            DebugLogger::ToggleDebug();
            if (DebugLogger::IsDebugEnabled()) {
                ScreenReader::Say("Debug logging enabled");
            } else {
                ScreenReader::Say("Debug logging disabled");
            }
            break;

        default:
            break;
        }
    }

    // Always pass to next hook — never consume keystrokes
    return CallNextHookEx(s_hHook, nCode, wParam, lParam);
}
