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

    const uint32_t history_size = 20000;
    History history;

    const uint32_t debounce_samples = 4;

    void add_sample(bool m_s = false, bool f_s = false, bool l_s = false,
        dial::State d_s = dial::State::WAITING)
    {
      History::Entry s;

      s.movement.state = m_s;
      s.movement.edge = Edge::NONE;
      s.fast.state = f_s;
      s.fast.edge = Edge::NONE;
      s.left.state = l_s;
      s.left.edge = Edge::NONE;
      s.dial_state = d_s;
      history.add_sample(s);
    }
    void SetUp()
    {
      ON_CALL(mock_interface, get_debounce_sample_count)
          .WillByDefault(DoAll(SetArgReferee<0>(debounce_samples), SetArgReferee<1>(0)));
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

TEST_F(TestStatistics, TestMovementRisingEdge)
{
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(true,   false, false, dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_TRUE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(1, dut().movement.edge_count);
  EXPECT_EQ(0, dut().fast.edge_count);
  EXPECT_EQ(0, dut().left.edge_count);
}

TEST_F(TestStatistics, TestMovementFallingEdge)
{
  add_sample(true,  false, false, dial::State::WAITING);
  add_sample(true,  false, false, dial::State::WAITING);
  add_sample(false,  false, false, dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_FALSE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(1, dut().movement.edge_count);
  EXPECT_EQ(0, dut().fast.edge_count);
  EXPECT_EQ(0, dut().left.edge_count);
}

TEST_F(TestStatistics, TestFastRisingEdge)
{
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(false,  true,  false, dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_FALSE(dut().movement.state);
  EXPECT_TRUE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(0, dut().movement.edge_count);
  EXPECT_EQ(1, dut().fast.edge_count);
  EXPECT_EQ(0, dut().left.edge_count);
}

TEST_F(TestStatistics, TestFastFallingEdge)
{
  add_sample(false,  true,  false, dial::State::WAITING);
  add_sample(false,  true,  false, dial::State::WAITING);
  add_sample(false,  false, false, dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_FALSE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(0, dut().movement.edge_count);
  EXPECT_EQ(1, dut().fast.edge_count);
  EXPECT_EQ(0, dut().left.edge_count);
}

TEST_F(TestStatistics, TestLeftRisingEdge)
{
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(false,  false, false, dial::State::WAITING);
  add_sample(false,  false, true,  dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_FALSE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_TRUE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(0, dut().movement.edge_count);
  EXPECT_EQ(0, dut().fast.edge_count);
  EXPECT_EQ(1, dut().left.edge_count);
}

TEST_F(TestStatistics, TestLeftFallingEdge)
{
  add_sample(false, false, true,  dial::State::WAITING);
  add_sample(false, false, true,  dial::State::WAITING);
  add_sample(false, false, false, dial::State::WAITING); // <-- newest sample

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_FALSE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_FALSE(dut().movement.valid_event);
  EXPECT_FALSE(dut().fast.valid_event);
  EXPECT_FALSE(dut().left.valid_event);
  EXPECT_EQ(0, dut().movement.edge_count);
  EXPECT_EQ(0, dut().fast.edge_count);
  EXPECT_EQ(1, dut().left.edge_count);
}

TEST_F(TestStatistics, TestCounting)
{
#include "samples/basis.txt"

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_TRUE(dut().movement.state);
  EXPECT_FALSE(dut().fast.state);
  EXPECT_FALSE(dut().left.state);
  EXPECT_EQ(37, dut().movement.edge_count);
  EXPECT_EQ(15, dut().fast.edge_count);
  EXPECT_EQ(8,  dut().left.edge_count);
}

TEST_F(TestStatistics, TestEvents)
{
  EXPECT_CALL(mock_interface, get_debounce_sample_count)
      .WillRepeatedly(DoAll(SetArgReferee<0>(2), SetArgReferee<1>(0)));

#include "samples/basis.txt"

  /*
  detection works as following, with the assumption:
    valid event two samples high, detection begins on falling edge

    add_sample(true,  ..., ..., ...); // <-- oldest sample
    add_sample(true,  ..., ..., ...); // 1
    add_sample(true,  ..., ..., ...); // 2
    add_sample(true,  ..., ..., ...); // 3
    add_sample(true,  ..., ..., ...); // 4
                     <--- falling edge from time point of view
    add_sample(false, ..., ..., ...); //edge detection, valid event
    add_sample(false, ..., ..., ...);
    add_sample(false, ..., ..., ...);
    add_sample(false, ..., ..., ...);
    add_sample(false, ..., ..., ...);
    add_sample(false, ..., ..., ...);
    add_sample(false, ..., ..., ...);
                     <--- rising edge from time point of view
    add_sample(true,  ..., ..., ...); //edge detection
    add_sample(true,  ..., ..., ...);
    add_sample(true,  ..., ..., ...); // <-- newest sample

  */

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_TRUE(dut().movement.valid_event);
  EXPECT_TRUE(dut().fast.valid_event);
  EXPECT_TRUE(dut().left.valid_event);
}

TEST_F(TestStatistics, TestDebounce)
{
  EXPECT_CALL(mock_interface, get_debounce_sample_count)
      .WillRepeatedly(DoAll(SetArgReferee<0>(5), SetArgReferee<1>(0)));

#include "samples/debounce.txt"

  Statistics dut(mock_interface, history, history.size());

  ASSERT_TRUE(dut.are_valid());
  EXPECT_TRUE(dut().movement.valid_event);
}

//todo test statistics for pick-up detection without pick-up enabled input
//test data in test/samples dir

