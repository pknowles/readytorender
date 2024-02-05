// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <ranges>
#include <stdint.h>
#include <algorithm>

namespace rtr {

namespace ktx {

// Only supports KTX 2!
struct Identifier : std::array<uint8_t, 12> {
    constexpr Identifier() { std::copy_n("\xABKTX 20\xBB\r\n\x1A\n", 12, this->begin()); }
};

template<class T>
class Iterator {
public:

private:
    const uint8_t* m_ptr;
};

struct LevelIndex {
    uint64_t byteOffset;
    uint64_t byteLength;
    uint64_t uncompressedByteLength;
};

class Header {
public:
    Identifier identifier;
    uint32_t   vkFormat;
    uint32_t   typeSize;
    uint32_t   pixelWidth;
    uint32_t   pixelHeight;
    uint32_t   pixelDepth;
    uint32_t   layerCount;
    uint32_t   faceCount;
    uint32_t   levelCount;
    uint32_t   supercompressionScheme;

private:
    uint32_t   dfdByteOffset;
    uint32_t   dfdByteLength;
    uint32_t   kvdByteOffset;
    uint32_t   kvdByteLength;
    uint64_t   sgdByteOffset;
    uint64_t   sgdByteLength;

public:
    auto levelsRaw() const {
        return levelIndices() | std::ranges::views::transform([this](const LevelIndex& index) {
            return std::span{headerBegin() + index.byteOffset, index.byteLength}; });
    }

    bool validateIdentifier() const
    {
        Identifier expected;
        bool result = (identifier == expected);
        return result;
    }

private:
    const std::byte* headerBegin() const {
        return reinterpret_cast<const std::byte*>(this);
    }
    const std::byte* headerEnd() const {
        return headerBegin() + sizeof(*this);
    }
    std::span<const LevelIndex> levelIndices() const {
        return {reinterpret_cast<const LevelIndex*>(headerEnd()), levelCount};
    }
};

} // namespace ktx

} // namespace rtr