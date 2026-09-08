#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

namespace SaveParser
{
    constexpr uint32_t PlaytimeHash = 0x73c29681;
    constexpr uint32_t BalladOfHeroesReadyHash = 1186840637;
    constexpr size_t HeaderSize = 0x0c;
    constexpr size_t RecordSize = 8;
    constexpr size_t MinimumRecordCount = 32;

    struct Record
    {
        uint32_t hash = 0;
        uint32_t value = 0;
    };

    struct ParsedSaveState
    {
        uint32_t playtime = 0;
        bool hasDLC = false;
        std::vector<Record> records;
    };

    bool ParseSaveBuffer(const uint8_t* data, size_t size, ParsedSaveState& result);
}
