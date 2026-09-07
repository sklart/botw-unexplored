#pragma once

#include <cstdint>
#include <array>
#include <cstddef>
#include <string>

#include "ObjectTypes.h"

namespace ObjectModel
{
    constexpr size_t ObjectTypeCount = static_cast<size_t>(Data::ObjectType::Count);

    struct TypeMetadata
    {
        const char* iconPath;
        bool hasName;
        int legendButtonIndex;
    };

    const TypeMetadata* TryGetMetadata(Data::ObjectType type);
    const TypeMetadata& GetMetadata(Data::ObjectType type);
    bool IsValidType(Data::ObjectType type);
}
