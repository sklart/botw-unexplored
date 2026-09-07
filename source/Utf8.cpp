#include "Utf8.h"

#include <cstdint>

std::u32string Utf8::Decode(const std::string& text)
{
    std::u32string result;
    for (size_t i = 0; i < text.size();)
    {
        const uint8_t first = static_cast<uint8_t>(text[i]);
        uint32_t codepoint = 0xFFFD;
        size_t length = 1;
        if (first < 0x80)
            codepoint = first;
        else
        {
            size_t expected = 0;
            uint32_t minimum = 0;
            uint32_t value = 0;
            if ((first & 0xE0) == 0xC0) { expected = 2; minimum = 0x80; value = first & 0x1F; }
            else if ((first & 0xF0) == 0xE0) { expected = 3; minimum = 0x800; value = first & 0x0F; }
            else if ((first & 0xF8) == 0xF0) { expected = 4; minimum = 0x10000; value = first & 0x07; }
            if (expected != 0 && i + expected <= text.size())
            {
                bool valid = true;
                for (size_t j = 1; j < expected; ++j)
                {
                    const uint8_t next = static_cast<uint8_t>(text[i + j]);
                    if ((next & 0xC0) != 0x80) { valid = false; break; }
                    value = (value << 6) | (next & 0x3F);
                }
                if (valid && value >= minimum && value <= 0x10FFFF && !(value >= 0xD800 && value <= 0xDFFF))
                {
                    codepoint = value;
                    length = expected;
                }
            }
        }
        result.push_back(static_cast<char32_t>(codepoint));
        i += length;
    }
    return result;
}
