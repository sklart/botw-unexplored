#pragma once

#include <cstdint>
#include <string>

#include "ObjectTypes.h"

namespace ObjectModel
{
    struct TypeMetadata
    {
        const char* iconPath;
        bool hasName;
        int legendButtonIndex;
    };

    const TypeMetadata& GetMetadata(Data::ObjectType type);
    bool IsValidType(Data::ObjectType type);
}
