#pragma once

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace LegacyKorokMigration
{
    bool Parse(std::istream& input, size_t expectedCount, std::vector<bool>& found);
}
