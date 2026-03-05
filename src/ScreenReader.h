/**
 * ScreenReader.h / ScreenReader.cpp
 * Tolk-based screen reader wrapper for Virtua Cop 2 Accessibility Mod
 *
 * Mirrors the ScreenReader.cs pattern from the Unity accessibility template,
 * adapted for native Win32 C++.
 *
 * Tolk supports: NVDA, JAWS, SAPI (fallback)
 * Requires: Tolk.dll + nvdaControllerClient32.dll (32-bit) in game folder
 */

#pragma once
#ifndef SCREEN_READER_H
#define SCREEN_READER_H

#include <string>

/**
 * @brief Wrapper around Tolk screen reader library.
 *
 * Usage:
 *   ScreenReader::Init();
 *   ScreenReader::Say("Hello"); // interrupts current speech
 *   ScreenReader::SayQueued("Press F1 for help"); // queues after current speech
 *   ScreenReader::Shutdown();
 */
class ScreenReader
{
public:
    /// <summary>Initialize Tolk. Call once at mod startup.</summary>
    static void Init();

    /// <summary>Speak text immediately, interrupting any current speech.</summary>
    static void Say(const std::string& text);

    /// <summary>Queue text to speak after current speech finishes.</summary>
    static void SayQueued(const std::string& text);

    /// <summary>Repeat the last announcement.</summary>
    static void RepeatLast();

    /// <summary>Shut down Tolk. Call on mod unload.</summary>
    static void Shutdown();

    /// <summary>Returns true if a screen reader is active.</summary>
    static bool IsActive();

private:
    static std::string s_lastAnnouncement;
    static bool s_initialized;
};

#endif // SCREEN_READER_H
