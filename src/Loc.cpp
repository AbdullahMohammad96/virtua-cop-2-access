/**
 * Loc.cpp
 * Localization strings — English.
 */

#include "Loc.h"
#include "DebugLogger.h"

std::unordered_map<std::string, std::string> Loc::s_strings;

void Loc::Init(const std::string& language)
{
    s_strings.clear();
    if (language == "en" || language.empty()) {
        LoadEnglish();
        DebugLogger::Log("Localization loaded: English");
    } else {
        // Fallback to English for unknown language codes
        LoadEnglish();
        DebugLogger::Log("WARNING: Unknown language '" + language + "', using English.");
    }
}

std::string Loc::Get(const std::string& key)
{
    auto it = s_strings.find(key);
    if (it != s_strings.end()) {
        return it->second;
    }
    DebugLogger::Log("WARNING: Missing localization key: " + key);
    return "[" + key + "]"; // Return key as fallback so missing strings are obvious
}

void Loc::LoadEnglish()
{
    // ----------------------------------------------------------------
    // Startup
    // ----------------------------------------------------------------
    s_strings["mod_loaded"]         = "Virtua Cop 2 Accessibility Mod loaded. Press F1 for status.";

    // ----------------------------------------------------------------
    // Game states / modes
    // ----------------------------------------------------------------
    s_strings["title_screen"]       = "Title Screen. Press Enter to start.";
    s_strings["stage_select"]       = "Stage Select.";
    s_strings["difficulty_select"]  = "Difficulty Select.";
    s_strings["gun_select"]         = "Gun Select.";
    s_strings["gameplay"]           = "Gameplay started.";
    s_strings["continue_screen"]    = "Continue screen.";
    s_strings["game_over"]          = "Game Over.";
    s_strings["ranking"]            = "Ranking screen.";
    s_strings["options"]            = "Options.";

    // ----------------------------------------------------------------
    // Hints shown after mode transitions
    // ----------------------------------------------------------------
    s_strings["gameplay_hint"]      = "F1 for status, F2 to repeat, F5 for lives.";
    s_strings["continue_hint"]      = "Press Enter to continue or Escape to quit.";
    s_strings["stage_select_hint"]  = "Arrow keys to select, Enter to confirm.";

    // ----------------------------------------------------------------
    // Lives
    // ----------------------------------------------------------------
    s_strings["player"]             = "Player ";
    s_strings["lives"]              = "lives: ";
    s_strings["lives_dropped_to"]   = "hit! Lives: ";
    s_strings["lives_increased_to"] = "extra life! Lives: ";
    s_strings["last_life_warning"]  = "Warning: last life!";
    s_strings["lives_unknown"]      = "Lives: unknown. Memory addresses not yet configured.";

    // ----------------------------------------------------------------
    // Score
    // ----------------------------------------------------------------
    s_strings["score"]              = "score: ";
    s_strings["score_milestone"]    = "Score: ";
    s_strings["score_unknown"]      = "Score: unknown. Memory addresses not yet configured.";

    // ----------------------------------------------------------------
    // Stage / difficulty
    // ----------------------------------------------------------------
    s_strings["stage"]              = "Stage ";
    s_strings["difficulty"]         = "Difficulty: ";
    s_strings["stage_unknown"]      = "Stage unknown";
    s_strings["stage_cleared"]      = "Stage ";
    s_strings["entering_stage"]     = " cleared. Entering stage ";

    // ----------------------------------------------------------------
    // Game over / continue
    // ----------------------------------------------------------------
    s_strings["game_over_hint"]     = "All lives lost. Press Enter to continue or Escape for title.";

    // ----------------------------------------------------------------
    // Status summary (F1)
    // ----------------------------------------------------------------
    s_strings["status_prefix"]      = "Status: ";
    s_strings["not_in_game"]        = "Not currently in gameplay.";
}
