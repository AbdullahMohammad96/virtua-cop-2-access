/**
 * KeyboardHook.h / KeyboardHook.cpp
 * Low-level keyboard hook for accessibility mod hotkeys.
 *
 * Uses WH_KEYBOARD_LL (low-level keyboard hook) via SetWindowsHookEx.
 * This captures F1-F5 and F12 globally, without interfering with the
 * game's own input processing.
 *
 * Mod hotkeys:
 *   F1  — Announce full status (mode, stage, difficulty, lives, score)
 *   F2  — Repeat last announcement
 *   F3  — Announce score
 *   F4  — Announce stage and difficulty
 *   F5  — Announce lives
 *   F12 — Toggle debug logging
 */

#pragma once
#ifndef KEYBOARD_HOOK_H
#define KEYBOARD_HOOK_H

/**
 * @brief Installs and manages the low-level keyboard hook for mod hotkeys.
 */
class KeyboardHook
{
public:
    /// <summary>Install the keyboard hook. Call once at mod startup.</summary>
    static void Init();

    /// <summary>Remove the keyboard hook. Call on mod shutdown.</summary>
    static void Shutdown();

private:
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK s_hHook;
};

#endif // KEYBOARD_HOOK_H
