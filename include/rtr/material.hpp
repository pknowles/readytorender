// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <decodeless/header.hpp>
#include <decodeless/offset_span.hpp>
#include <glm/glm.hpp>
#include <rtr/types.hpp>
#include <rtr/ktx.hpp>
#include <optional>

namespace rtr {

// Generated by gitversion.cmake
#include <readytorender_gitversion.h>

namespace common {

struct Texture {
    decodeless::offset_ptr<ktx::Header> ktx;
};

struct MaterialFactors {
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f};
    float     metallic{0.0f};
    float     roughness{0.5f};
};

struct MaterialTextures {
    optional_index32 color;
    optional_index32 metallic;
    optional_index32 roughness;
    optional_index32 normal;
};

struct Material {
    MaterialFactors  factors;
    MaterialTextures textures;
};

struct MaterialHeader : decodeless::Header {
    static constexpr decodeless::Magic   HeaderIdentifier{"RTR:COMMON:MAT"};
    static constexpr decodeless::Version VersionSupported{1, 0, 0};
    MaterialHeader();
    decodeless::offset_span<Material>          materials;
    decodeless::offset_span<Texture>           textures;
    decodeless::offset_span<offset_string>     materialNames;

    // Optional external textures. Cast the data in these files to ktx::Header.
    //
    // IMPORTANT: either
    // 1. This array is empty and all textures are embedded in 'textures'
    // 2. There are no embedded textures and each is in its own ktx file
    //
    // This rather rigid constraint is chosen to simplify what could otherwise
    // be an explosion of complexity in indexing.
    optional<external_items> texturesExternal;
};

inline MaterialHeader::MaterialHeader()
    : decodeless::Header{
          .identifier = HeaderIdentifier,
          .version = VersionSupported,
          .gitHash = readytorender_GIT_HASH,
      } {}

} // namespace common

} // namespace rtr