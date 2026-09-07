#pragma once

#include <cstdint>
#include <string>

#include "ObjectTypes.h"

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
        LanguageName,
        Missing,
        Completed,
        All,
        Position,
        Found,
        NotFound,
        ObjectInfo,
        NextMissing,
        Korok,
        Shrine,
        DLCShrine,
        Location,
        Hinox,
        StoneTalus,
        Molduga,
        Count
    };

    const std::string& Get(Text text);
    Language GetLanguage();
    void SetLanguage(Language language);
    const std::string& GetShowModeName(int showMode);
    const std::string& GetObjectTypeName(Data::ObjectType objectType);
    void ToggleLanguage();
    void Load();
    void Save();
    const std::string& GetKorokGuide(int id, const std::string& fallback);
    const std::string& GetLocationName(uint32_t hash, const std::string& fallback);
}
