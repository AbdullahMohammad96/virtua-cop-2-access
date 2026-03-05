/**
 * GameState.h
 * Reads Virtua Cop 2 game state from process memory.
 *
 * IMPORTANT: Memory offsets below are PLACEHOLDERS.
 * They must be filled in after Cheat Engine analysis.
 * See docs/game-api.md — "Memory Map" section.
 *
 * Pattern: mirrors the AccessStateManager concept from the template,
 * adapted for direct memory reading in C++.
 */

#pragma once
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <cstdint>
#include <string>

// -----------------------------------------------------------------------
// Game state enum — what screen/mode the game is currently in
// -----------------------------------------------------------------------
enum class GameMode {
    Unknown = 0,
    TitleScreen,
    StageSelect,
    DifficultySelect,
    GunSelect,
    Gameplay,
    Continue,
    GameOver,
    Ranking,
    Options
};

// -----------------------------------------------------------------------
// Snapshot of all readable game state
// -----------------------------------------------------------------------
struct GameStateSnapshot {
    GameMode    mode        = GameMode::Unknown;
    int         lives1P     = -1;   // Player 1 lives (-1 = not yet read)
    int         lives2P     = -1;   // Player 2 lives
    int         score1P     = -1;   // Player 1 score
    int         score2P     = -1;   // Player 2 score
    int         stage       = -1;   // Current stage (1-4)
    int         difficulty  = -1;   // 0=Easy, 1=Normal, 2=Hard
    bool        isGameOver  = false;
    bool        isContinue  = false;
};

/**
 * @brief Reads and tracks Virtua Cop 2 game state from process memory.
 */
class GameState
{
public:
    /// <summary>Initialize — find base module address.</summary>
    static void Init();

    /// <summary>
    /// Poll current game state from memory.
    /// Call this regularly (e.g., every 100ms from timeGetTime hook).
    /// </summary>
    static void Update();

    /// <summary>Get the most recently read game state snapshot.</summary>
    static const GameStateSnapshot& GetCurrent();

    /// <summary>
    /// Human-readable name of a GameMode.
    /// </summary>
    static std::string ModeName(GameMode mode);

    /// <summary>
    /// Human-readable difficulty name.
    /// </summary>
    static std::string DifficultyName(int difficulty);

private:
    static GameStateSnapshot s_current;
    static uintptr_t         s_moduleBase;

    static int  ReadInt32(uintptr_t offset);
    static bool ReadBool(uintptr_t offset);
};

#endif // GAME_STATE_H
