/**
 * Announcer.h / Announcer.cpp
 * Formats and announces Virtua Cop 2 game state changes via ScreenReader.
 *
 * Pattern: mirrors the Handler class pattern from the accessibility template.
 * Each type of announcement is a separate method, called only when the
 * relevant state has actually changed (prev != curr).
 *
 * Announcement format follows the template's guidelines:
 *   - Status: "[What] [direction] [value]" e.g. "Lives dropped to 2"
 *   - Screen transitions: "[ScreenName]" e.g. "Stage Select"
 *   - Actions: "[Action] [Object] [Result]" e.g. "Continue used, Stage 2"
 */

#pragma once
#ifndef ANNOUNCER_H
#define ANNOUNCER_H

#include "GameState.h"

/**
 * @brief Compares previous and current game state snapshots,
 *        announces any changes to the screen reader.
 */
class Announcer
{
public:
    /// <summary>
    /// Compare prev and curr state. Announce anything that changed.
    /// Call this after GameState::Update() on every poll cycle.
    /// </summary>
    static void AnnounceChanges(
        const GameStateSnapshot& prev,
        const GameStateSnapshot& curr);

    /// <summary>
    /// Announce full current status (for F1 hotkey).
    /// </summary>
    static void AnnounceFullStatus(const GameStateSnapshot& curr);

    /// <summary>
    /// Announce score only (for F3 hotkey).
    /// </summary>
    static void AnnounceScore(const GameStateSnapshot& curr);

    /// <summary>
    /// Announce stage and difficulty (for F4 hotkey).
    /// </summary>
    static void AnnounceStageDifficulty(const GameStateSnapshot& curr);

    /// <summary>
    /// Announce lives only (for F5 hotkey).
    /// </summary>
    static void AnnounceLives(const GameStateSnapshot& curr);

private:
    static void HandleModeChange(GameMode prev, GameMode curr);
    static void HandleLivesChange(int prevLives, int currLives, int player);
    static void HandleStageChange(int prevStage, int currStage);
    static void HandleGameOver(const GameStateSnapshot& curr);
};

#endif // ANNOUNCER_H
