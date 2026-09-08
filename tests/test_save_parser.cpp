#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

#include "SaveParser.h"

namespace
{
    void AppendU32(std::vector<uint8_t>& buffer, uint32_t value)
    {
        buffer.push_back(static_cast<uint8_t>(value));
        buffer.push_back(static_cast<uint8_t>(value >> 8));
        buffer.push_back(static_cast<uint8_t>(value >> 16));
        buffer.push_back(static_cast<uint8_t>(value >> 24));
    }

    void AppendRecord(std::vector<uint8_t>& buffer, uint32_t hash, uint32_t value)
    {
        AppendU32(buffer, hash);
        AppendU32(buffer, value);
    }

    std::vector<uint8_t> MakeValidBuffer(bool includeDlc)
    {
        std::vector<uint8_t> buffer(SaveParser::HeaderSize, 0);
        AppendRecord(buffer, SaveParser::PlaytimeHash, 12345);
        if (includeDlc)
            AppendRecord(buffer, SaveParser::BalladOfHeroesReadyHash, 1);
        const size_t targetRecordCount = SaveParser::MinimumRecordCount;
        for (size_t index = includeDlc ? 2 : 1; index < targetRecordCount; ++index)
            AppendRecord(buffer, 0x10000000u + static_cast<uint32_t>(index), static_cast<uint32_t>(index));
        return buffer;
    }
}

int main()
{
    SaveParser::ParsedSaveState parsed;
    const std::vector<uint8_t> representative = MakeValidBuffer(true);
    assert(SaveParser::ParseSaveBuffer(representative.data(), representative.size(), parsed));
    assert(parsed.playtime == 12345);
    assert(parsed.hasDLC);
    assert(parsed.records.size() == SaveParser::MinimumRecordCount);

    std::vector<uint8_t> random(SaveParser::HeaderSize + SaveParser::RecordSize * SaveParser::MinimumRecordCount, 0xa5);
    assert(!SaveParser::ParseSaveBuffer(random.data(), random.size(), parsed));

    assert(!SaveParser::ParseSaveBuffer(nullptr, 0, parsed));

    std::vector<uint8_t> playtimeOnly(SaveParser::HeaderSize, 0);
    AppendRecord(playtimeOnly, SaveParser::PlaytimeHash, 1);
    assert(playtimeOnly.size() == 20);
    assert(!SaveParser::ParseSaveBuffer(playtimeOnly.data(), playtimeOnly.size(), parsed));

    std::vector<uint8_t> truncatedAfterPlaytime = MakeValidBuffer(false);
    truncatedAfterPlaytime.resize(20);
    assert(!SaveParser::ParseSaveBuffer(truncatedAfterPlaytime.data(), truncatedAfterPlaytime.size(), parsed));

    std::vector<uint8_t> truncatedMiddleRecord = MakeValidBuffer(false);
    truncatedMiddleRecord.resize(truncatedMiddleRecord.size() - 4);
    assert(!SaveParser::ParseSaveBuffer(truncatedMiddleRecord.data(), truncatedMiddleRecord.size(), parsed));

    std::vector<uint8_t> malformedAlignment = MakeValidBuffer(false);
    malformedAlignment.push_back(0);
    assert(!SaveParser::ParseSaveBuffer(malformedAlignment.data(), malformedAlignment.size(), parsed));

    const std::vector<uint8_t> unknownHashes = MakeValidBuffer(false);
    assert(SaveParser::ParseSaveBuffer(unknownHashes.data(), unknownHashes.size(), parsed));
    assert(!parsed.hasDLC);

    SaveParser::ParsedSaveState previousState;
    assert(SaveParser::ParseSaveBuffer(representative.data(), representative.size(), previousState));
    const SaveParser::ParsedSaveState expectedState = previousState;
    assert(!SaveParser::ParseSaveBuffer(malformedAlignment.data(), malformedAlignment.size(), previousState));
    assert(previousState.playtime == expectedState.playtime);
    assert(previousState.hasDLC == expectedState.hasDLC);
    assert(previousState.records.size() == expectedState.records.size());
    assert(std::equal(previousState.records.begin(), previousState.records.end(), expectedState.records.begin(),
        [](const SaveParser::Record& left, const SaveParser::Record& right)
        {
            return left.hash == right.hash && left.value == right.value;
        }));
}
