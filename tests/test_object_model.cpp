#include <cassert>

#include "ObjectModel.h"

int main()
{
    const char* expectedIcons[] = {
        "romfs:/korokseed.png", "romfs:/shrine.png", "romfs:/dlcshrine.png", "",
        "romfs:/hinox.png", "romfs:/talus.png", "romfs:/molduga.png"
    };
    const int expectedLegendButtons[] = {0, 1, 1, 5, 2, 3, 4};

    for (int value = 0; value < static_cast<int>(Data::ObjectType::Count); ++value)
    {
        const Data::ObjectType type = static_cast<Data::ObjectType>(value);
        assert(ObjectModel::IsValidType(type));
        const ObjectModel::TypeMetadata& metadata = ObjectModel::GetMetadata(type);
        assert(std::string(metadata.iconPath) == expectedIcons[value]);
        assert(metadata.legendButtonIndex == expectedLegendButtons[value]);
    }
    assert(!ObjectModel::IsValidType(Data::ObjectType::Count));
    return 0;
}
