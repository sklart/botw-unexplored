#include "ObjectModel.h"

namespace
{
    const ObjectModel::TypeMetadata metadata[] = {
        {"romfs:/korokseed.png", false, 0},
        {"romfs:/shrine.png", true, 1},
        {"romfs:/dlcshrine.png", false, 1},
        {"", true, 5},
        {"romfs:/hinox.png", false, 2},
        {"romfs:/talus.png", false, 3},
        {"romfs:/molduga.png", false, 4}
    };
}

const ObjectModel::TypeMetadata& ObjectModel::GetMetadata(Data::ObjectType type)
{
    return metadata[static_cast<int>(type)];
}

bool ObjectModel::IsValidType(Data::ObjectType type)
{
    return static_cast<int>(type) >= 0 && static_cast<int>(type) < static_cast<int>(Data::ObjectType::Count);
}
