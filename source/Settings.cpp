#include "Settings.h"

#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <istream>
#include <ostream>
#include <string>

namespace
{
    const char* Header = "BOTW_UNEXPLORED_SETTINGS";

    bool ReadLine(std::istream& input, std::string& value)
    {
        return static_cast<bool>(std::getline(input, value));
    }

    bool ParseInt(const std::string& value, int& result)
    {
        if (value.empty())
            return false;
        char* end = nullptr;
        errno = 0;
        const long parsed = std::strtol(value.c_str(), &end, 10);
        if (errno != 0 || end == value.c_str() || *end != '\0' || parsed < -2147483647L - 1L || parsed > 2147483647L)
            return false;
        result = static_cast<int>(parsed);
        return true;
    }

    bool ParseFloat(const std::string& value, float& result)
    {
        if (value.empty())
            return false;
        char* end = nullptr;
        errno = 0;
        const float parsed = std::strtof(value.c_str(), &end);
        if (errno != 0 || end == value.c_str() || *end != '\0' || !std::isfinite(parsed))
            return false;
        result = parsed;
        return true;
    }

    bool ParseBool(const std::string& value, bool& result)
    {
        int parsed = 0;
        if (!ParseInt(value, parsed) || (parsed != 0 && parsed != 1))
            return false;
        result = parsed != 0;
        return true;
    }

    bool ReadCurrent(std::istream& input, SettingsIO::Settings& settings)
    {
        std::string value;
        if (!ReadLine(input, value) || !ParseFloat(value, settings.cameraX)) return false;
        if (!ReadLine(input, value) || !ParseFloat(value, settings.cameraY)) return false;
        if (!ReadLine(input, value) || !ParseFloat(value, settings.zoom)) return false;
        if (!ReadLine(input, value) || !ParseBool(value, settings.legendOpen)) return false;
        for (int i = 0; i < SettingsIO::CategoryCount; ++i)
            if (!ReadLine(input, value) || !ParseBool(value, settings.visible[i])) return false;
        if (!ReadLine(input, value) || !ParseInt(value, settings.showMode) || settings.showMode < 0 || settings.showMode > 2) return false;
        if (!ReadLine(input, value) || !ParseInt(value, settings.language) || settings.language < -1 || settings.language > 2) return false;
        return true;
    }

    bool ReadLegacy(std::istream& input, const std::string& firstLine, SettingsIO::Settings& settings)
    {
        std::string value;
        if (!ParseFloat(firstLine, settings.cameraX)) return false;
        if (!ReadLine(input, value) || !ParseFloat(value, settings.cameraY)) return false;
        if (!ReadLine(input, value) || !ParseFloat(value, settings.zoom)) return false;
        if (!ReadLine(input, value) || !ParseBool(value, settings.legendOpen)) return false;
        for (int i = 0; i < SettingsIO::CategoryCount && ReadLine(input, value); ++i)
        {
            if (!ParseBool(value, settings.visible[i]))
                return false;
        }
        return true;
    }
}

SettingsIO::LoadResult SettingsIO::Load(std::istream& input, Settings& settings)
{
    std::string firstLine;
    if (!ReadLine(input, firstLine))
        return LoadResult::Missing;

    if (firstLine != Header)
        return ReadLegacy(input, firstLine, settings) ? LoadResult::Legacy : LoadResult::Invalid;

    std::string versionLine;
    int version = 0;
    if (!ReadLine(input, versionLine) || !ParseInt(versionLine, version))
        return LoadResult::Invalid;
    if (version > CurrentVersion)
        return LoadResult::FutureVersion;
    if (version != CurrentVersion || !ReadCurrent(input, settings))
        return LoadResult::Invalid;
    return LoadResult::Current;
}

bool SettingsIO::Save(std::ostream& output, const Settings& settings)
{
    output << Header << '\n' << CurrentVersion << '\n'
           << settings.cameraX << '\n' << settings.cameraY << '\n' << settings.zoom << '\n'
           << static_cast<int>(settings.legendOpen) << '\n';
    for (int i = 0; i < CategoryCount; ++i)
        output << static_cast<int>(settings.visible[i]) << '\n';
    output << settings.showMode << '\n' << settings.language << '\n';
    return static_cast<bool>(output);
}
