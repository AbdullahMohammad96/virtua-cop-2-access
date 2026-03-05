/**
 * GameState.cpp
 * Reads Virtua Cop 2 game state from process memory.
 *
 * !! PLACEHOLDER OFFSETS !!
 * The TODO offsets below must be discovered with Cheat Engine.
 * See docs/game-api.md — "Memory Map" and "How to Find Addresses".
 * Until they are filled in, the mod will announce "unknown" for most values,
 * but screen reader output and hotkeys will still work.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>
#include "GameState.h"
#include "DebugLogger.h"

// -----------------------------------------------------------------------
// Memory offsets — FILL THESE IN after Cheat Engine analysis
// These are offsets from the PPJ2DD.EXE module base address.
// -----------------------------------------------------------------------
namespace Offsets {
    // TODO: Replace 0x00000000 with real offsets from Cheat Engine
    static const uintptr_t Lives1P      = 0x00000000;  // TODO
    static const uintptr_t Lives2P      = 0x00000000;  // TODO
    static const uintptr_t Score1P      = 0x00000000;  // TODO
    static const uintptr_t Score2P      = 0x00000000;  // TODO
    static const uintptr_t Stage        = 0x00000000;  // TODO
    static const uintptr_t Difficulty   = 0x00000000;  // TODO
    static const uintptr_t GameModeVal  = 0x00000000;  // TODO
}

// -----------------------------------------------------------------------
// Difficulty value mapping (as found in game strings: Easy/Normal/Hard)
// -----------------------------------------------------------------------
namespace DifficultyValue {
    static const int Easy   = 0;
    static const int Normal = 1;
    static const int Hard   = 2;
}

// -----------------------------------------------------------------------
// Static members
// -----------------------------------------------------------------------
GameStateSnapshot GameState::s_current;
uintptr_t         GameState::s_moduleBase = 0;

// -----------------------------------------------------------------------
// Init — find the game's base module address
// -----------------------------------------------------------------------
void GameState::Init()
{
    s_moduleBase = (uintptr_t)GetModuleHandleA("PPJ2DD.EXE");
    if (s_moduleBase == 0) {
        // Might be loaded under a different name; try without extension
        s_moduleBase = (uintptr_t)GetModuleHandleA(nullptr);
    }

    if (s_moduleBase != 0) {
        char msg[128];
        sprintf_s(msg, "PPJ2DD.EXE base address: 0x%08X", (unsigned)s_moduleBase);
        DebugLogger::Log(msg);
    } else {
        DebugLogger::Log("WARNING: Could not find PPJ2DD.EXE base address. Memory reads will fail.");
    }
}

// -----------------------------------------------------------------------
// Safe memory read — returns defaultVal on any access failure
// -----------------------------------------------------------------------
int GameState::ReadInt32(uintptr_t offset)
{
    if (s_moduleBase == 0 || offset == 0) return -1;

    uintptr_t addr = s_moduleBase + offset;
    // Use SEH for safe read (alternative: VirtualQuery to check page is readable)
    __try {
        return *(int*)addr;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DebugLogger::Log("WARNING: Memory read exception at offset 0x" + 
                         std::to_string(offset));
        return -1;
    }
}

bool GameState::ReadBool(uintptr_t offset)
{
    return ReadInt32(offset) != 0;
}

// -----------------------------------------------------------------------
// Update — poll all game state from memory
// -----------------------------------------------------------------------
void GameState::Update()
{
    // Lives
    int lives1P = ReadInt32(Offsets::Lives1P);
    int lives2P = ReadInt32(Offsets::Lives2P);

    // Score
    int score1P = ReadInt32(Offsets::Score1P);
    int score2P = ReadInt32(Offsets::Score2P);

    // Stage (1-4; 0 might mean menu/title)
    int stage = ReadInt32(Offsets::Stage);

    // Difficulty (0=Easy, 1=Normal, 2=Hard)
    int difficulty = ReadInt32(Offsets::Difficulty);

    // Game mode raw value — map to enum
    int modeRaw = ReadInt32(Offsets::GameModeVal);
    GameMode mode = GameMode::Unknown;
    // TODO: map modeRaw values to GameMode enum once offsets are known
    // Example:
    // if (modeRaw == 0) mode = GameMode::TitleScreen;
    // if (modeRaw == 1) mode = GameMode::Gameplay;
    // etc.

    // Derive GameOver / Continue from lives
    bool isGameOver = (lives1P == 0);

    // Update snapshot (only if values look plausible — sanity check)
    s_current.lives1P    = (lives1P >= 0 && lives1P <= 9) ? lives1P : s_current.lives1P;
    s_current.lives2P    = (lives2P >= 0 && lives2P <= 9) ? lives2P : s_current.lives2P;
    s_current.score1P    = (score1P >= 0) ? score1P : s_current.score1P;
    s_current.score2P    = (score2P >= 0) ? score2P : s_current.score2P;
    s_current.stage      = (stage >= 1 && stage <= 4) ? stage : s_current.stage;
    s_current.difficulty = (difficulty >= 0 && difficulty <= 2) ? difficulty : s_current.difficulty;
    s_current.mode       = mode;
    s_current.isGameOver = isGameOver;
}

// -----------------------------------------------------------------------
// Getters
// -----------------------------------------------------------------------
const GameStateSnapshot& GameState::GetCurrent()
{
    return s_current;
}

std::string GameState::ModeName(GameMode mode)
{
    switch (mode) {
        case GameMode::TitleScreen:     return "Title Screen";
        case GameMode::StageSelect:     return "Stage Select";
        case GameMode::DifficultySelect:return "Difficulty Select";
        case GameMode::GunSelect:       return "Gun Select";
        case GameMode::Gameplay:        return "Gameplay";
        case GameMode::Continue:        return "Continue";
        case GameMode::GameOver:        return "Game Over";
        case GameMode::Ranking:         return "Ranking";
        case GameMode::Options:         return "Options";
        default:                        return "Unknown";
    }
}

std::string GameState::DifficultyName(int difficulty)
{
    switch (difficulty) {
        case 0:  return "Easy";
        case 1:  return "Normal";
        case 2:  return "Hard";
        default: return "Unknown";
    }
}
