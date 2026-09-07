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

    enum class LoadResult
    {
        Current,
        Invalid,
        FutureVersion
    };

    LoadResult Load(std::istream& input);
    bool Save(std::ostream& output);
    void Clear();
    bool MarkFound(const Context& context, Data::ObjectType type, uint32_t completionHash);
    bool IsMarkedFound(const Context& context, Data::ObjectType type, uint32_t completionHash);
    bool ConfirmFromSave(const Context& context, Data::ObjectType type, uint32_t completionHash);
    bool IsDirty();
    bool CanPersist();
    bool Flush();
    const std::vector<Entry>& Entries();
}
