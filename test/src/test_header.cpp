// Copyright (c) 2024 Pyarelal Knowles, MIT License

#include <gtest/gtest.h>
#include <rtr/header.hpp>
#include <creator.hpp>

using namespace rtr;

TEST(RootHeader, Valid) {
    creator alloc(1024);
    rtr::RootHeader* root = alloc.construct<rtr::RootHeader>();
    EXPECT_TRUE(root->validate());
}

TEST(RootHeader, Invalid) {
    creator alloc(1024);
    rtr::RootHeader* root = alloc.construct<rtr::RootHeader>();
    root->identifier[3] = 'X';
    EXPECT_FALSE(root->validate());
}
