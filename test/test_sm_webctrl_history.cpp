/*
 * test.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: martin
 */

#include <gtest/gtest.h>
#include "src/sm_webctrl.h"

using namespace webctrl;

class TestHistory : public ::testing::Test
{
  public:

    TestHistory() : dut(3)
    {
    }


    History dut;

    void add_sample(bool m_s, bool f_s, bool l_s, dial::State d_s)
    {
      History::Entry s;

      s.movement.state = m_s;
      s.movement.edge = Edge::NONE;
      s.fast.state = f_s;
      s.fast.edge = Edge::NONE;
      s.left.state = l_s;
      s.left.edge = Edge::NONE;
      s.dial_state = d_s;
      dut.add_sample(s);
    }
};

TEST_F(TestHistory, TestEmpty)
{
  EXPECT_EQ(0, dut.size());
  EXPECT_TRUE(dut.is_valid(0));
  EXPECT_FALSE(dut.is_valid(1));
  EXPECT_FALSE(dut.is_valid());
  EXPECT_EQ(nullptr, dut.get(0));
  EXPECT_EQ(nullptr, dut.current());
}

TEST_F(TestHistory, TestNonEmpty)
{
  add_sample(true, false, false, dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING);

  EXPECT_EQ(2, dut.size());
  EXPECT_TRUE(dut.is_valid(2));
  EXPECT_FALSE(dut.is_valid());
  EXPECT_NE(nullptr, dut.get(0));
  EXPECT_NE(nullptr, dut.current());
}

TEST_F(TestHistory, TestFilled)
{
  add_sample(true, false, false, dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING);

  EXPECT_EQ(3, dut.size());
  EXPECT_TRUE(dut.is_valid(3));
  EXPECT_TRUE(dut.is_valid());
  EXPECT_NE(nullptr, dut.get(0));
  EXPECT_NE(nullptr, dut.current());
}

TEST_F(TestHistory, TestRotate)
{
  add_sample(true, false, false, dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING);
  add_sample(true, false, false, dial::State::WAITING);

  EXPECT_EQ(3, dut.size());
  EXPECT_TRUE(dut.is_valid(3));
  EXPECT_TRUE(dut.is_valid());
  auto s = dut.get(2);
  ASSERT_NE(nullptr, s);
  EXPECT_EQ(Edge::FALLING, s->movement.edge);
  s = dut.current();
  ASSERT_NE(nullptr, s);
  EXPECT_EQ(Edge::RISING, s->movement.edge);
}

TEST_F(TestHistory, TestValues)
{
  add_sample(true, true, true, dial::State::SEARCHING);
  add_sample(false, true, true, dial::State::WAITING);
  add_sample(false, false, false, dial::State::MOVING);
  add_sample(true, false, false, dial::State::SEARCHING);

  auto &s = dut();

  EXPECT_FALSE(s.at(2).movement.state);
  EXPECT_EQ(Edge::FALLING, s.at(2).movement.edge);
  EXPECT_TRUE(s.at(2).fast.state);
  EXPECT_EQ(Edge::NONE, s.at(2).fast.edge);
  EXPECT_TRUE(s.at(2).left.state);
  EXPECT_EQ(Edge::NONE, s.at(2).left.edge);
  EXPECT_EQ(dial::State::WAITING, s.at(2).dial_state);

  EXPECT_FALSE(s.at(1).movement.state);
  EXPECT_EQ(Edge::NONE, s.at(1).movement.edge);
  EXPECT_FALSE(s.at(1).fast.state);
  EXPECT_EQ(Edge::FALLING, s.at(1).fast.edge);
  EXPECT_FALSE(s.at(1).left.state);
  EXPECT_EQ(Edge::FALLING, s.at(1).left.edge);
  EXPECT_EQ(dial::State::MOVING, s.at(1).dial_state);

  EXPECT_TRUE(s.at(0).movement.state);
  EXPECT_EQ(Edge::RISING, s.at(0).movement.edge);
  EXPECT_FALSE(s.at(0).fast.state);
  EXPECT_EQ(Edge::NONE, s.at(0).fast.edge);
  EXPECT_FALSE(s.at(0).left.state);
  EXPECT_EQ(Edge::NONE, s.at(0).left.edge);
  EXPECT_EQ(dial::State::SEARCHING, s.at(0).dial_state);
}


TEST_F(TestHistory, TestValuesPartlyFilled)
{
  add_sample(false, true, false, dial::State::MOVING);
  add_sample(true, true, true, dial::State::SEARCHING);

  ASSERT_EQ(2, dut.size());

  auto &s = dut();

  EXPECT_FALSE(s.at(1).movement.state);
  EXPECT_EQ(Edge::NONE, s.at(1).movement.edge);
  EXPECT_TRUE(s.at(1).fast.state);
  EXPECT_EQ(Edge::NONE, s.at(1).fast.edge);
  EXPECT_FALSE(s.at(1).left.state);
  EXPECT_EQ(Edge::NONE, s.at(1).left.edge);
  EXPECT_EQ(dial::State::MOVING, s.at(1).dial_state);

  EXPECT_TRUE(s.at(0).movement.state);
  EXPECT_EQ(Edge::RISING, s.at(0).movement.edge);
  EXPECT_TRUE(s.at(0).fast.state);
  EXPECT_EQ(Edge::NONE, s.at(0).fast.edge);
  EXPECT_TRUE(s.at(0).left.state);
  EXPECT_EQ(Edge::RISING, s.at(0).left.edge);
  EXPECT_EQ(dial::State::SEARCHING, s.at(0).dial_state);
}

