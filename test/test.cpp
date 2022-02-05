/*
 * test.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: martin
 */

#include <gtest/gtest.h>

class TestClass : public ::testing::Test
{
};

TEST_F(TestClass, str)
{
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
}

TEST_F(TestClass, eq)
{
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}

// Demonstrate some basic assertions.
TEST(HelloTest, BasicAssertions) {
  // Expect two strings not to be equal.
  EXPECT_STRNE("hello", "world");
  // Expect equality.
  EXPECT_EQ(7 * 6, 42);
}
