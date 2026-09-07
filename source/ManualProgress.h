#pragma once

#include <cstdint>
#include <iosfwd>
#include <vector>

#include "ObjectTypes.h"

namespace ManualProgress
{
    struct Context
    {
        uint64_t profileId1 = 0;
        uint64_t profileId2 = 0;
        bool masterMode = false;
    };

    struct Entry
    {
        Context context;
        Data::ObjectType type = Data::ObjectType::Korok;
        uint32_t completionHash = 0;
    };

    bool Load(std::istream& input);
    bool Save(std::ostream& output);
    void Clear();
    void MarkFound(const Context& context, Data::ObjectType type, uint32_t completionHash);
    bool IsMarkedFound(const Context& context, Data::ObjectType type, uint32_t completionHash);
    void ConfirmFromSave(const Context& context, Data::ObjectType type, uint32_t completionHash);
    const std::vector<Entry>& Entries();
}
