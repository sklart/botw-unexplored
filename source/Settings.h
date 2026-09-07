#pragma once

#include <array>
#include <iosfwd>

namespace SettingsIO
{
    constexpr int CurrentVersion = 2;
    constexpr int CategoryCount = 7;

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
