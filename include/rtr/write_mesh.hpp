// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <concepts>
#include <decodeless/allocator.hpp>
#include <decodeless/allocator_construction.hpp>
#include <ranges>
#include <rtr/mesh.hpp>

namespace rtr {

namespace common {

// Utility call to facilitate writing a MeshHeader
template <class Allocator, std::ranges::input_range Meshes, std::ranges::input_range Names>
    requires std::convertible_to<std::ranges::range_value_t<Meshes>, Mesh>
[[nodiscard]] MeshHeader* createMeshHeader(const Allocator& allocator, Meshes&& meshes,
                                           Names&& meshNames) {
    MeshHeader* result = decodeless::create::object<MeshHeader>(allocator);
    result->meshes = decodeless::create::array<Mesh>(allocator, meshes);
    const std::byte* firstByte = reinterpret_cast<const std::byte*>(result->meshes.data());
    const std::byte* lastByte = 0;
#define RTR_ARRAY(ATTR_TYPE, ATTR_NAME)                                                            \
    {                                                                                              \
        using T = typename decltype(Mesh::ATTR_NAME)::value_type;                                  \
        size_t i = 0;                                                                              \
        T*     elementsBegin = nullptr;                                                            \
        T*     elementsEnd = nullptr;                                                              \
        for (auto& mesh : meshes) {                                                                \
            auto& span = result->meshes[i++].ATTR_NAME;                                            \
            span = decodeless::create::array<T>(allocator, mesh.ATTR_NAME);                        \
            if (!lastByte)                                                                         \
                firstByte = reinterpret_cast<const std::byte*>(span.data());                       \
            if (!elementsEnd)                                                                      \
                elementsBegin = span.data();                                                       \
            lastByte =                                                                             \
                reinterpret_cast<const std::byte*>(elementsEnd = span.data() + span.size());       \
        }                                                                                          \
        if (elementsEnd)                                                                           \
            result->all.ATTR_NAME = std::span{elementsBegin, size_t(elementsEnd - elementsBegin)}; \
    }
    RTR_COMMON_MESH_FOREACH_ARRAY
#undef RTR_ARRAY
    if (lastByte)
        result->raw = std::span{firstByte, size_t(lastByte - firstByte)};
    result->meshNames = decodeless::create::array<decodeless::offset_span<const char>>(
        allocator, std::ranges::size(meshNames));
    auto meshNameIt = result->meshNames.begin();
    for (auto& meshName : meshNames)
        *meshNameIt++ = decodeless::create::array<const char>(allocator, meshName);
    return result;
}

} // namespace common

} // namespace rtr
