#include "SaveParser.h"

namespace
{
    uint32_t ReadU32(const uint8_t* data, size_t offset)
    {
        return static_cast<uint32_t>(data[offset]) |
               (static_cast<uint32_t>(data[offset + 1]) << 8) |
               (static_cast<uint32_t>(data[offset + 2]) << 16) |
               (static_cast<uint32_t>(data[offset + 3]) << 24);
    }
}

bool SaveParser::ParseSaveBuffer(const uint8_t* data, size_t size, ParsedSaveState& result)
{
    if (data == nullptr || size < HeaderSize)
        return false;

    const size_t trailingDataSize = (size - HeaderSize) % RecordSize;
    // Switch game_data.sav files may end with a four-byte trailer. Other partial
    // record sizes cannot be distinguished from corruption and are rejected.
    if (trailingDataSize != 0 && trailingDataSize != SupportedTrailingDataSize)
        return false;

    const size_t recordCount = (size - HeaderSize) / RecordSize;
    if (recordCount < MinimumRecordCount)
        return false;

    ParsedSaveState candidate;
    candidate.records.reserve(recordCount);
    bool foundPlaytime = false;
    size_t nonZeroHashCount = 0;

    for (size_t offset = HeaderSize; offset + RecordSize <= size; offset += RecordSize)
    {
        const Record record = {ReadU32(data, offset), ReadU32(data, offset + 4)};
        candidate.records.push_back(record);
        if (record.hash != 0)
            ++nonZeroHashCount;
        if (record.hash == PlaytimeHash && record.value != 0)
        {
            candidate.playtime = record.value;
            foundPlaytime = true;
        }
        if (record.hash == BalladOfHeroesReadyHash)
            candidate.hasDLC = record.value == 1;
    }

    // A real game_data.sav has a non-empty record set and a non-zero playtime control record.
    if (!foundPlaytime || nonZeroHashCount < MinimumRecordCount)
        return false;

    result = candidate;
    return true;
}
