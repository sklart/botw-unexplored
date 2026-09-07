#include <cassert>
#include <sstream>

#include "Settings.h"
#include "Utf8.h"

int main()
{
    SettingsIO::Settings expected;
    expected.cameraX = 12.5f;
    expected.cameraY = -8.0f;
    expected.zoom = 1.5f;
    expected.legendOpen = false;
    expected.visible[3] = true;
    expected.showMode = 2;
    expected.language = 1;

    std::stringstream current;
    assert(SettingsIO::Save(current, expected));
    SettingsIO::Settings actual;
    assert(SettingsIO::Load(current, actual) == SettingsIO::LoadResult::Current);
    assert(actual.cameraX == expected.cameraX && actual.cameraY == expected.cameraY && actual.zoom == expected.zoom);
    assert(!actual.legendOpen && actual.visible[3] && actual.showMode == 2 && actual.language == 1);

    std::stringstream legacy("4\n-5\n0.5\n1\n1\n0\n0\n0\n0\n0\n0\n");
    assert(SettingsIO::Load(legacy, actual) == SettingsIO::LoadResult::Legacy);
    assert(actual.cameraX == 4.0f && actual.visible[0] && !actual.visible[1]);

    std::stringstream empty;
    assert(SettingsIO::Load(empty, actual) == SettingsIO::LoadResult::Missing);
    std::stringstream invalid("BOTW_UNEXPLORED_SETTINGS\n2\nnan\n");
    assert(SettingsIO::Load(invalid, actual) == SettingsIO::LoadResult::Invalid);
    std::stringstream future("BOTW_UNEXPLORED_SETTINGS\n99\n");
    assert(SettingsIO::Load(future, actual) == SettingsIO::LoadResult::FutureVersion);

    assert(Utf8::Decode("ASCII") == U"ASCII");
    assert(Utf8::Decode("Русский") == U"Русский");
    assert(Utf8::Decode("Español") == U"Español");
    assert(Utf8::Decode("\xF0\x9F\x97\xBA") == U"🗺");
    assert(Utf8::Decode("\xF0\x28\x8C\x28") == U"\uFFFD(\uFFFD(");
}
