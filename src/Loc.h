/**
 * Loc.h / Loc.cpp
 * Localization system for accessibility announcements.
 *
 * All screen reader strings go through Loc::Get().
 * This matches the Loc.cs pattern from the accessibility template.
 * Even for a single-language mod, this makes future translation easy.
 *
 * Current languages: English (en)
 */

#pragma once
#ifndef LOC_H
#define LOC_H

#include <string>
#include <unordered_map>

/**
 * @brief Localization manager. All ScreenReader strings must use Loc::Get().
 */
class Loc
{
public:
    /// <summary>Initialize with a language code (e.g., "en").</summary>
    static void Init(const std::string& language);

    /// <summary>Get a localized string by key.</summary>
    static std::string Get(const std::string& key);

private:
    static std::unordered_map<std::string, std::string> s_strings;
    static void LoadEnglish();
};

#endif // LOC_H
