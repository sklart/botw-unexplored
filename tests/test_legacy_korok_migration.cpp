#include <cassert>
#include <sstream>
#include <vector>

#include "LegacyKorokMigration.h"

int main()
{
    std::vector<bool> found = {true, false};
    std::stringstream valid("0\n1\n0\n");
    assert(LegacyKorokMigration::Parse(valid, 3, found));
    assert(found.size() == 3 && !found[0] && found[1] && !found[2]);

    const std::vector<bool> before = found;
    std::stringstream malformed("0\n2\n0\n");
    assert(!LegacyKorokMigration::Parse(malformed, 3, found));
    assert(found == before);
    std::stringstream truncated("0\n1\n");
    assert(!LegacyKorokMigration::Parse(truncated, 3, found));
    assert(found == before);
    std::stringstream extra("0\n1\n0\n0\n");
    assert(!LegacyKorokMigration::Parse(extra, 3, found));
    assert(found == before);
}
