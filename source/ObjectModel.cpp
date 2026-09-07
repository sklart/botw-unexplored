#include "ObjectModel.h"
#include "LegendCategories.h"

namespace
{
    const std::array<ObjectModel::TypeMetadata, ObjectModel::ObjectTypeCount> metadata = {{
        {"romfs:/korokseed.png", false, static_cast<int>(LegendCategories::ButtonTypes::Koroks)},
        {"romfs:/shrine.png", true, static_cast<int>(LegendCategories::ButtonTypes::Shrines)},
        {"romfs:/dlcshrine.png", false, static_cast<int>(LegendCategories::ButtonTypes::Shrines)},
        {"romfs:/village.png", true, static_cast<int>(LegendCategories::ButtonTypes::Locations)},
        {"romfs:/hinox.png", false, static_cast<int>(LegendCategories::ButtonTypes::Hinoxes)},
        {"romfs:/talus.png", false, static_cast<int>(LegendCategories::ButtonTypes::Taluses)},
        {"romfs:/molduga.png", false, static_cast<int>(LegendCategories::ButtonTypes::Moldugas)}
    }};
    static_assert(metadata.size() == static_cast<size_t>(Data::ObjectType::Count), "Object metadata must cover every type");
}

const ObjectModel::TypeMetadata* ObjectModel::TryGetMetadata(Data::ObjectType type)
{
    if (!IsValidType(type))
        return nullptr;
    return &metadata[static_cast<size_t>(type)];
}

const ObjectModel::TypeMetadata& ObjectModel::GetMetadata(Data::ObjectType type)
{
    const TypeMetadata* metadataForType = TryGetMetadata(type);
    return metadataForType != nullptr ? *metadataForType : metadata[0];
}

bool ObjectModel::IsValidType(Data::ObjectType type)
{
    return static_cast<int>(type) >= 0 && static_cast<int>(type) < static_cast<int>(Data::ObjectType::Count);
}
