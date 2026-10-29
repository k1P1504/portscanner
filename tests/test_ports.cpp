#include "portscanner/ports.hpp"

#include <gtest/gtest.h>

TEST(ParsePort, AcceptsValidPorts)
{
    EXPECT_EQ(parse_port("80"), 80);
    EXPECT_EQ(parse_port("1"), 1);
    EXPECT_EQ(parse_port("65535"), 65535);
}

TEST(ParsePort, RejectsGarbage)
{
    EXPECT_FALSE(parse_port("0"));
    EXPECT_FALSE(parse_port("65536"));
    EXPECT_FALSE(parse_port("-5"));
    EXPECT_FALSE(parse_port("abc"));
    EXPECT_FALSE(parse_port("80abc"));
    EXPECT_FALSE(parse_port(""));
}
TEST(ParseRange, ExpandsRange)
{
    std::vector<std::uint16_t> expected{8000, 8001, 8002};
    EXPECT_EQ(parse_range("8000-8002"), expected);
}

TEST(ParseRange, RejectsReversedRange)
{
    EXPECT_TRUE(parse_range("8005-8000").empty());
}

TEST(ParseList, CombinesPortsAndRanges)
{
    std::vector<std::uint16_t> expected{22, 80, 8000, 8001, 8002};
    EXPECT_EQ(parse_list("22,80,8000-8002"), expected);
}

TEST(ParseList, OneBadPieceRejectsAll)
{
    EXPECT_TRUE(parse_list("22,abc,80").empty());
    EXPECT_TRUE(parse_list("22,,80").empty());
}