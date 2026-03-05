/**
 * ScreenReader.cpp
 * Tolk screen reader wrapper implementation
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ScreenReader.h"
#include "DebugLogger.h"

// -----------------------------------------------------------------------
// Tolk function pointer types (loaded dynamically from Tolk.dll)
// -----------------------------------------------------------------------
typedef void  (*PFN_Tolk_Load)();
typedef void  (*PFN_Tolk_Unload)();
typedef bool  (*PFN_Tolk_IsLoaded)();
typedef bool  (*PFN_Tolk_HasSpeech)();
typedef bool  (*PFN_Tolk_Output)(const wchar_t* str, bool interrupt);
typedef bool  (*PFN_Tolk_Silence)();
typedef bool  (*PFN_Tolk_DetectScreenReader)();

static HMODULE                  g_hTolk = nullptr;
static PFN_Tolk_Load            fn_Load = nullptr;
static PFN_Tolk_Unload          fn_Unload = nullptr;
static PFN_Tolk_IsLoaded        fn_IsLoaded = nullptr;
static PFN_Tolk_HasSpeech       fn_HasSpeech = nullptr;
static PFN_Tolk_Output          fn_Output = nullptr;

// -----------------------------------------------------------------------
// Static member definitions
// -----------------------------------------------------------------------
std::string ScreenReader::s_lastAnnouncement;
bool        ScreenReader::s_initialized = false;

// -----------------------------------------------------------------------
// Helper: convert UTF-8 std::string to wide string for Tolk
// -----------------------------------------------------------------------
static std::wstring ToWide(const std::string& str)
{
    if (str.empty()) return L"";
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring result(size - 1, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &result[0], size);
    return result;
}

// -----------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------
void ScreenReader::Init()
{
    if (s_initialized) return;

    g_hTolk = LoadLibraryA("Tolk.dll");
    if (!g_hTolk) {
        DebugLogger::Log("WARNING: Tolk.dll not found. Screen reader output disabled.");
        return;
    }

    fn_Load      = (PFN_Tolk_Load)     GetProcAddress(g_hTolk, "Tolk_Load");
    fn_Unload    = (PFN_Tolk_Unload)   GetProcAddress(g_hTolk, "Tolk_Unload");
    fn_IsLoaded  = (PFN_Tolk_IsLoaded) GetProcAddress(g_hTolk, "Tolk_IsLoaded");
    fn_HasSpeech = (PFN_Tolk_HasSpeech)GetProcAddress(g_hTolk, "Tolk_HasSpeech");
    fn_Output    = (PFN_Tolk_Output)   GetProcAddress(g_hTolk, "Tolk_Output");

    if (!fn_Load || !fn_Output) {
        DebugLogger::Log("ERROR: Tolk.dll loaded but required functions not found.");
        FreeLibrary(g_hTolk);
        g_hTolk = nullptr;
        return;
    }

    fn_Load();
    s_initialized = true;

    if (fn_HasSpeech && fn_HasSpeech()) {
        DebugLogger::Log("Tolk initialized. Screen reader detected.");
    } else {
        DebugLogger::Log("Tolk initialized. No screen reader detected; using SAPI fallback.");
    }
}

// -----------------------------------------------------------------------
// Say (interrupt = true — stops current speech and speaks immediately)
// -----------------------------------------------------------------------
void ScreenReader::Say(const std::string& text)
{
    if (text.empty()) return;
    s_lastAnnouncement = text;
    DebugLogger::Log("[SR] Say: " + text);

    if (!s_initialized || !fn_Output) return;
    fn_Output(ToWide(text).c_str(), true);
}

// -----------------------------------------------------------------------
// SayQueued (interrupt = false — waits for current speech to finish)
// -----------------------------------------------------------------------
void ScreenReader::SayQueued(const std::string& text)
{
    if (text.empty()) return;
    DebugLogger::Log("[SR] SayQueued: " + text);

    if (!s_initialized || !fn_Output) return;
    fn_Output(ToWide(text).c_str(), false);
}

// -----------------------------------------------------------------------
// RepeatLast
// -----------------------------------------------------------------------
void ScreenReader::RepeatLast()
{
    if (!s_lastAnnouncement.empty()) {
        Say(s_lastAnnouncement);
    }
}

// -----------------------------------------------------------------------
// IsActive
// -----------------------------------------------------------------------
bool ScreenReader::IsActive()
{
    return s_initialized && fn_HasSpeech && fn_HasSpeech();
}

// -----------------------------------------------------------------------
// Shutdown
// -----------------------------------------------------------------------
void ScreenReader::Shutdown()
{
    if (!s_initialized) return;
    if (fn_Unload) fn_Unload();
    if (g_hTolk) {
        FreeLibrary(g_hTolk);
        g_hTolk = nullptr;
    }
    s_initialized = false;
}
