// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <decodeless/header.hpp>
#include <rtr/mesh.hpp>

namespace rtr {

struct RootHeader : public decodeless::RootHeader
{
    static constexpr decodeless::Magic HeaderIdentifier{"READY-TO-RENDER"};
    RootHeader() : decodeless::RootHeader(HeaderIdentifier) {}

    bool validate() const
    {
        if(identifier != HeaderIdentifier)
            return false;
        return binaryCompatible();
    }
};

};