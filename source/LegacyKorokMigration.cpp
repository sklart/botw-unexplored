#include "LegacyKorokMigration.h"

#include <istream>
#include <string>

bool LegacyKorokMigration::Parse(std::istream& input, size_t expectedCount, std::vector<bool>& found)
{
    std::vector<bool> parsed;
    parsed.reserve(expectedCount);
    std::string line;
    while (std::getline(input, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (line == "0")
            parsed.push_back(false);
        else if (line == "1")
            parsed.push_back(true);
        else
            return false;
    }
    if (!input.eof() || parsed.size() != expectedCount)
        return false;
    found = std::move(parsed);
    return true;
}
