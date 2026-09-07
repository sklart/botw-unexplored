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

    bool ParseVersion(const std::string& line, int& version)
    {
        std::istringstream parser(line);
        return static_cast<bool>(parser >> version) && (parser >> std::ws).eof();
    }

    bool ParseEntry(const std::string& line, ManualProgress::Entry& entry)
    {
        std::istringstream parser(line);
        int master = 0;
        int type = 0;
        if (!(parser >> entry.context.profileId1 >> entry.context.profileId2 >> master >> type >> entry.completionHash) ||
            !(parser >> std::ws).eof() || (master != 0 && master != 1) ||
            type < 0 || type >= static_cast<int>(Data::ObjectType::Count))
            return false;
        entry.context.masterMode = master != 0;
        entry.type = static_cast<Data::ObjectType>(type);
        return true;
    }

    void RemoveCarriageReturn(std::string& line)
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
    }
}

ManualProgress::LoadResult ManualProgress::Load(std::istream& input)
{
    std::string header;
    if (!std::getline(input, header))
        return LoadResult::Invalid;
    RemoveCarriageReturn(header);
    if (header != Header)
        return LoadResult::Invalid;

    std::string versionLine;
    int version = 0;
    if (!std::getline(input, versionLine))
        return LoadResult::Invalid;
    RemoveCarriageReturn(versionLine);
    if (!ParseVersion(versionLine, version))
        return LoadResult::Invalid;
    if (version > Version)
    {
        preserveFutureFile = true;
        return LoadResult::FutureVersion;
    }
    if (version != Version)
        return LoadResult::Invalid;

    std::vector<Entry> parsedEntries;
    std::string line;
    while (std::getline(input, line))
    {
        RemoveCarriageReturn(line);
        Entry entry;
        if (!ParseEntry(line, entry))
            return LoadResult::Invalid;
        if (std::find_if(parsedEntries.begin(), parsedEntries.end(), [&](const Entry& existing)
        {
            return SameKey(existing, entry.context, entry.type, entry.completionHash);
        }) == parsedEntries.end())
            parsedEntries.push_back(entry);
    }
    if (!input.eof())
        return LoadResult::Invalid;

    entries = std::move(parsedEntries);
    dirty = false;
    preserveFutureFile = false;
    return LoadResult::Current;
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

ManualProgress::State ManualProgress::CaptureState()
{
    State state;
    state.entries = entries;
    state.dirty = dirty;
    return state;
}

void ManualProgress::RestoreState(const State& state)
{
    entries = state.entries;
    dirty = state.dirty;
}

const std::vector<ManualProgress::Entry>& ManualProgress::Entries()
{
    return entries;
}
