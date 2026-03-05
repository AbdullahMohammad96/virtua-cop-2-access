/**
 * DebugLogger.h / DebugLogger.cpp
 * File-based debug logger for the accessibility mod.
 *
 * Logs to VC2_AccessMod_Debug.log in the game folder.
 * Only writes when debug mode is enabled (toggle with F12 in-game).
 * Zero overhead when disabled.
 */

#pragma once
#ifndef DEBUG_LOGGER_H
#define DEBUG_LOGGER_H

#include <string>

/**
 * @brief Debug file logger. All logging goes through this class.
 *
 * Usage:
 *   DebugLogger::Init("VC2_AccessMod_Debug.log");
 *   DebugLogger::Log("Something happened");
 *   DebugLogger::ToggleDebug(); // Enable/disable at runtime (F12)
 */
class DebugLogger
{
public:
    /// <summary>Initialize logger with output filename.</summary>
    static void Init(const std::string& filename);

    /// <summary>Write a log message. No-op if debug is disabled.</summary>
    static void Log(const std::string& message);

    /// <summary>Toggle debug mode on/off.</summary>
    static void ToggleDebug();

    /// <summary>Returns true if debug logging is currently enabled.</summary>
    static bool IsDebugEnabled();

private:
    static std::string s_filename;
    static bool        s_debugEnabled;
    static bool        s_initialized;
};

#endif // DEBUG_LOGGER_H
