/**
 * Announcer.cpp
 * Formats and announces Virtua Cop 2 game state changes.
 */

#include "Announcer.h"
#include "ScreenReader.h"
#include "Loc.h"
#include "DebugLogger.h"
#include <string>
#include <sstream>

// -----------------------------------------------------------------------
// AnnounceChanges — main entry point called every poll cycle
// -----------------------------------------------------------------------
void Announcer::AnnounceChanges(
    const GameStateSnapshot& prev,
    const GameStateSnapshot& curr)
{
    // Screen/mode transitions — highest priority, interrupts
    if (prev.mode != curr.mode && curr.mode != GameMode::Unknown) {
        HandleModeChange(prev.mode, curr.mode);
    }

    // Game over — critical
    if (!prev.isGameOver && curr.isGameOver) {
        HandleGameOver(curr);
        return; // Don't announce other things on game over
    }

    // Stage change
    if (prev.stage != curr.stage && curr.stage > 0) {
        HandleStageChange(prev.stage, curr.stage);
    }

    // Player 1 lives change — important, interrupts
    if (prev.lives1P != curr.lives1P && curr.lives1P >= 0) {
        HandleLivesChange(prev.lives1P, curr.lives1P, 1);
    }

    // Player 2 lives change (2P mode)
    if (prev.lives2P != curr.lives2P && curr.lives2P >= 0) {
        HandleLivesChange(prev.lives2P, curr.lives2P, 2);
    }

    // Score milestones — announce every 10,000 points (queued, lower priority)
    // This avoids constant interruption while shooting
    if (curr.score1P >= 0 && prev.score1P >= 0) {
        int prevMilestone = prev.score1P / 10000;
        int currMilestone = curr.score1P / 10000;
        if (currMilestone > prevMilestone) {
            std::ostringstream oss;
            oss << Loc::Get("score_milestone") << curr.score1P;
            ScreenReader::SayQueued(oss.str());
        }
    }
}

// -----------------------------------------------------------------------
// HandleModeChange
// -----------------------------------------------------------------------
void Announcer::HandleModeChange(GameMode prev, GameMode curr)
{
    std::string modeName = GameState::ModeName(curr);
    ScreenReader::Say(modeName);
    DebugLogger::Log("[Announcer] Mode: " + GameState::ModeName(prev) + " -> " + modeName);

    // After announcing mode, queue available key hints
    if (curr == GameMode::Gameplay) {
        ScreenReader::SayQueued(Loc::Get("gameplay_hint")); // "F1 for status, F5 for lives"
    } else if (curr == GameMode::Continue) {
        ScreenReader::SayQueued(Loc::Get("continue_hint")); // "Press Enter to continue or Escape to quit"
    } else if (curr == GameMode::StageSelect) {
        ScreenReader::SayQueued(Loc::Get("stage_select_hint")); // "Arrow keys to select stage, Enter to confirm"
    }
}

// -----------------------------------------------------------------------
// HandleLivesChange
// -----------------------------------------------------------------------
void Announcer::HandleLivesChange(int prevLives, int currLives, int player)
{
    std::ostringstream oss;

    if (currLives < prevLives) {
        // Lost a life
        oss << Loc::Get("player") << player << " "
            << Loc::Get("lives_dropped_to") << currLives;
        ScreenReader::Say(oss.str()); // Interrupt — critical info

        if (currLives == 1) {
            ScreenReader::SayQueued(Loc::Get("last_life_warning")); // "Warning: last life"
        }
    } else if (currLives > prevLives) {
        // Gained a life (extra life bonus)
        oss << Loc::Get("player") << player << " "
            << Loc::Get("lives_increased_to") << currLives;
        ScreenReader::Say(oss.str());
    }

    DebugLogger::Log("[Announcer] P" + std::to_string(player) + 
                     " lives: " + std::to_string(prevLives) + 
                     " -> " + std::to_string(currLives));
}

// -----------------------------------------------------------------------
// HandleStageChange
// -----------------------------------------------------------------------
void Announcer::HandleStageChange(int prevStage, int currStage)
{
    std::ostringstream oss;
    oss << Loc::Get("stage_cleared");
    if (prevStage > 0) {
        oss << prevStage << ". ";
    }
    oss << Loc::Get("entering_stage") << currStage;
    ScreenReader::Say(oss.str());
    DebugLogger::Log("[Announcer] Stage: " + std::to_string(prevStage) + 
                     " -> " + std::to_string(currStage));
}

// -----------------------------------------------------------------------
// HandleGameOver
// -----------------------------------------------------------------------
void Announcer::HandleGameOver(const GameStateSnapshot& curr)
{
    ScreenReader::Say(Loc::Get("game_over")); // "Game Over"
    ScreenReader::SayQueued(Loc::Get("game_over_hint")); // "Press Enter to continue or Escape for title"
    DebugLogger::Log("[Announcer] Game Over. Score: " + std::to_string(curr.score1P));
}

// -----------------------------------------------------------------------
// AnnounceFullStatus — F1 hotkey
// -----------------------------------------------------------------------
void Announcer::AnnounceFullStatus(const GameStateSnapshot& curr)
{
    std::ostringstream oss;

    // Mode
    oss << GameState::ModeName(curr.mode) << ". ";

    // Stage
    if (curr.stage > 0) {
        oss << Loc::Get("stage") << curr.stage << ". ";
    }

    // Difficulty
    if (curr.difficulty >= 0) {
        oss << GameState::DifficultyName(curr.difficulty) << ". ";
    }

    // Lives
    if (curr.lives1P >= 0) {
        oss << Loc::Get("player") << "1 " 
            << Loc::Get("lives") << curr.lives1P << ". ";
    }

    // Score
    if (curr.score1P >= 0) {
        oss << Loc::Get("score") << curr.score1P << ". ";
    }

    ScreenReader::Say(oss.str());
}

// -----------------------------------------------------------------------
// AnnounceScore — F3 hotkey
// -----------------------------------------------------------------------
void Announcer::AnnounceScore(const GameStateSnapshot& curr)
{
    std::ostringstream oss;
    if (curr.score1P >= 0) {
        oss << Loc::Get("player") << "1 " << Loc::Get("score") << curr.score1P;
    } else {
        oss << Loc::Get("score_unknown");
    }
    ScreenReader::Say(oss.str());
}

// -----------------------------------------------------------------------
// AnnounceStageDifficulty — F4 hotkey
// -----------------------------------------------------------------------
void Announcer::AnnounceStageDifficulty(const GameStateSnapshot& curr)
{
    std::ostringstream oss;
    if (curr.stage > 0) {
        oss << Loc::Get("stage") << curr.stage << ". ";
    } else {
        oss << Loc::Get("stage_unknown") << ". ";
    }
    if (curr.difficulty >= 0) {
        oss << Loc::Get("difficulty") << GameState::DifficultyName(curr.difficulty);
    }
    ScreenReader::Say(oss.str());
}

// -----------------------------------------------------------------------
// AnnounceLives — F5 hotkey
// -----------------------------------------------------------------------
void Announcer::AnnounceLives(const GameStateSnapshot& curr)
{
    std::ostringstream oss;
    if (curr.lives1P >= 0) {
        oss << Loc::Get("player") << "1 " << Loc::Get("lives") << curr.lives1P;
    } else {
        oss << Loc::Get("lives_unknown");
    }
    ScreenReader::Say(oss.str());
}
