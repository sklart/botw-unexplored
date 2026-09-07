#include "ManualProgress.h"

#include <algorithm>
#include <fstream>
#include <istream>
#include <ostream>
#include <sstream>
#include <string>

namespace
{
    const char* Header = "BOTW_UNEXPLORED_MANUAL_PROGRESS";
    const int Version = 1;
    const char* FilePath = "sdmc:/switch/botw-unexplored/manual_progress.dat";
    std::vector<ManualProgress::Entry> entries;
    bool dirty = false;
    bool preserveFutureFile = false;

    bool SameKey(const ManualProgress::Entry& entry, const ManualProgress::Context& context,
                 Data::ObjectType type, uint32_t hash)
    {
        return entry.context.profileId1 == context.profileId1 && entry.context.profileId2 == context.profileId2 &&
               entry.context.masterMode == context.masterMode && entry.type == type && entry.completionHash == hash;
    }
}

ManualProgress::LoadResult ManualProgress::Load(std::istream& input)
{
    entries.clear();
    dirty = false;
    preserveFutureFile = false;
    std::string header;
    int version = 0;
    if (!std::getline(input, header) || header != Header || !(input >> version) || version != Version)
    {
        if (header == Header && version > Version)
        {
            preserveFutureFile = true;
            return LoadResult::FutureVersion;
        }
        return LoadResult::Invalid;
    }

    Entry entry;
    int master = 0;
    int type = 0;
    while (input >> entry.context.profileId1 >> entry.context.profileId2 >> master >> type >> entry.completionHash)
    {
        if ((master != 0 && master != 1) || type < 0 || type >= static_cast<int>(Data::ObjectType::Count))
        {
            entries.clear();
            return LoadResult::Invalid;
        }
        entry.context.masterMode = master != 0;
        entry.type = static_cast<Data::ObjectType>(type);
        if (!IsMarkedFound(entry.context, entry.type, entry.completionHash))
            entries.push_back(entry);
    }
    return input.eof() ? LoadResult::Current : LoadResult::Invalid;
}

bool ManualProgress::Save(std::ostream& output)
{
    output << Header << '\n' << Version << '\n';
    for (const Entry& entry : entries)
        output << entry.context.profileId1 << ' ' << entry.context.profileId2 << ' '
               << static_cast<int>(entry.context.masterMode) << ' ' << static_cast<int>(entry.type) << ' '
               << entry.completionHash << '\n';
    return static_cast<bool>(output);
}

void ManualProgress::Clear()
{
    entries.clear();
    dirty = false;
}

bool ManualProgress::MarkFound(const Context& context, Data::ObjectType type, uint32_t completionHash)
{
    if (!CanPersist() || static_cast<int>(type) < 0 || type >= Data::ObjectType::Count || completionHash == 0 ||
        IsMarkedFound(context, type, completionHash))
        return false;
    Entry entry;
    entry.context = context;
    entry.type = type;
    entry.completionHash = completionHash;
    entries.push_back(entry);
    dirty = true;
    return true;
}

bool ManualProgress::IsMarkedFound(const Context& context, Data::ObjectType type, uint32_t completionHash)
{
    return std::find_if(entries.begin(), entries.end(), [&](const Entry& entry)
    {
        return SameKey(entry, context, type, completionHash);
    }) != entries.end();
}

bool ManualProgress::ConfirmFromSave(const Context& context, Data::ObjectType type, uint32_t completionHash)
{
    const size_t before = entries.size();
    entries.erase(std::remove_if(entries.begin(), entries.end(), [&](const Entry& entry)
    {
        return SameKey(entry, context, type, completionHash);
    }), entries.end());
    const bool changed = entries.size() != before;
    dirty = dirty || changed;
    return changed;
}

bool ManualProgress::IsDirty()
{
    return dirty;
}

bool ManualProgress::CanPersist()
{
    return !preserveFutureFile;
}

bool ManualProgress::Flush()
{
    if (!dirty)
        return true;
    if (!CanPersist())
        return false;
    std::ofstream output(FilePath);
    if (!output.is_open() || !Save(output))
        return false;
    dirty = false;
    return true;
}

const std::vector<ManualProgress::Entry>& ManualProgress::Entries()
{
    return entries;
}
