/*
 * test.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: martin
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_sm_webctrl.h"

#include "src/sm_webctrl.h"

using namespace webctrl;
using namespace ::testing;

class TestStatistics : public ::testing::Test
{
  public:

    TestStatistics() :  history(history_size)
    {
    }

    NiceMock<MockInterface> mock_interface;

    const uint32_t history_size = 1000;
    History history;

    const uint32_t debounce_pos_edges = 4;
    const uint32_t debounce_neg_edges = 5;

    void add_sample(bool m_s = false, Edge m_e = Edge::NONE, bool f_s = false,
        Edge f_e = Edge::NONE, bool l_s = false, Edge l_e = Edge::NONE,
        dial::State d_s = dial::State::WAITING)
    {
      History::Entry s;

      s.movement.state = m_s;
      s.movement.edge = m_e;
      s.fast.state = f_s;
      s.fast.edge = f_e;
      s.left.state = l_s;
      s.left.edge = l_e;
      s.dial_state = d_s;
      history.add_sample(s);
    }

    void SetUp()
    {
      ON_CALL(mock_interface, get_debounce_sample_count)
          .WillByDefault(DoAll(SetArgReferee<0>(debounce_pos_edges), SetArgReferee<1>(debounce_neg_edges)));
    }

    void TearDown()
    {
    }
};

TEST_F(TestStatistics, TestEmpty)
{
  Statistics dut(mock_interface, history, history_size);

  EXPECT_FALSE(dut.are_valid());
}

TEST_F(TestStatistics, TestNonFull)
{
  uint32_t i;

  for (i = 0; i < history_size - 1; ++i) {
    add_sample();
  }

  Statistics dut(mock_interface, history, history_size);

  EXPECT_FALSE(dut.are_valid());
}

TEST_F(TestStatistics, TestReady)
{
  uint32_t i;

  for (i = 0; i < history_size; ++i) {
    add_sample();
  }

  Statistics dut(mock_interface, history, history_size);

  ASSERT_TRUE(dut.are_valid());
  //no events
  EXPECT_FALSE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(0, dut().movement.edge_count);
  EXPECT_EQ(0, dut().fast.edge_count);
  EXPECT_EQ(0, dut().left.edge_count);
}



