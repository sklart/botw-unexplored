#pragma once

#include <cstdint>
#include <string>

namespace Localization
{
    enum class Language
    {
        English,
        Russian,
        Spanish
    };

    enum class Text
    {
        Exit,
        ChooseAnotherProfile,
        NoSaveDataTitle,
        NoSaveDataDescription,
        GameRunningTitle,
        GameRunningDescription,
        GameRunningDescription2,
        No,
        Yes,
        MasterModeTitle,
        MasterModeDescription,
        Legend,
        Close,
        Koroks,
        Shrines,
        Hinoxes,
        Taluses,
        Moldugas,
        Locations,
        ShowCompleted,
        MarkFound,
        OpenLegend,
        GameRunning,
        LoadedOlderSave,
        LoadMasterMode,
        LoadNormalMode,
        Controls,
        LanguageName
    };

    const std::string& Get(Text text);
    Language GetLanguage();
    void ToggleLanguage();
    void Load();
    void Save();
    const std::string& GetKorokGuide(int id, const std::string& fallback);
    const std::string& GetLocationName(uint32_t hash, const std::string& fallback);
}
