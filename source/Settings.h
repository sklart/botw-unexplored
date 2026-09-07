#pragma once

#include <array>
#include <iosfwd>

#include "LegendCategories.h"

namespace SettingsIO
{
    constexpr int CurrentVersion = 2;
    // Legend categories include the display-mode control, so this count is deliberately distinct from ObjectType.
    constexpr size_t CategoryCount = LegendCategories::ButtonCount;

    struct Settings
    {
        float cameraX = 0.0f;
        float cameraY = 0.0f;
        float zoom = 0.25f;
        bool legendOpen = true;
        std::array<bool, CategoryCount> visible = {{true, true, false, false, false, false, false}};
        int showMode = 0;
        int language = -1;
    };

    enum class LoadResult
    {
        Missing,
        Current,
        Legacy,
        Invalid,
        FutureVersion
    };

    LoadResult Load(std::istream& input, Settings& settings);
    bool Save(std::ostream& output, const Settings& settings);
}
