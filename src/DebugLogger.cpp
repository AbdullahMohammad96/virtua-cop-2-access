/**
 * DebugLogger.cpp
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <fstream>
#include <ctime>
#include "DebugLogger.h"

std::string DebugLogger::s_filename;
bool        DebugLogger::s_debugEnabled = false; // Off by default (F12 to enable)
bool        DebugLogger::s_initialized  = false;

void DebugLogger::Init(const std::string& filename)
{
    s_filename    = filename;
    s_initialized = true;

    // Always write startup message regardless of debug mode
    std::ofstream f(s_filename, std::ios::trunc);
    if (f.is_open()) {
        f << "=== VC2 Accessibility Mod Log ===" << std::endl;
        f << "Debug mode: OFF (press F12 in-game to enable)" << std::endl;
        f.close();
    }
}

void DebugLogger::Log(const std::string& message)
{
    if (!s_initialized || !s_debugEnabled) return;

    // Get timestamp
    time_t now = time(nullptr);
    char timebuf[32];
    struct tm tm_info;
    localtime_s(&tm_info, &now);
    strftime(timebuf, sizeof(timebuf), "%H:%M:%S", &tm_info);

    std::ofstream f(s_filename, std::ios::app);
    if (f.is_open()) {
        f << "[" << timebuf << "] " << message << std::endl;
        f.close();
    }
}

void DebugLogger::ToggleDebug()
{
    s_debugEnabled = !s_debugEnabled;

    // Always write this toggle message regardless of previous state
    std::ofstream f(s_filename, std::ios::app);
    if (f.is_open()) {
        f << "--- Debug mode " << (s_debugEnabled ? "ENABLED" : "DISABLED") << " ---" << std::endl;
        f.close();
    }
}

bool DebugLogger::IsDebugEnabled()
{
    return s_debugEnabled;
}
