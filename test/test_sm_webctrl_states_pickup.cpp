/*
 * test.cpp
 *
 *  Created on: Feb 5, 2022
 *      Author: martin
 *
 *  state machine using pick-up active switch input
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "mock_sm_webctrl.h"

#include "src/sm_webctrl.h"

using namespace webctrl;
using namespace ::testing;

class TestStatesPickup : public ::testing::Test
{
  public:

    TestStatesPickup()
    {
    }

    NiceMock<MockInterface> mock_interface;

    const uint32_t debounce_pos_edges = 3;
    const uint32_t debounce_neg_edges = 3;

    void SetUp()
    {
      ON_CALL(mock_interface, get_debounce_sample_count)
          .WillByDefault(DoAll(SetArgReferee<0>(debounce_pos_edges), SetArgReferee<1>(debounce_neg_edges)));
      ON_CALL(mock_interface, get_window_size)
          .WillByDefault(Return(100));
    }

    void TearDown()
    {
    }

    void ExpectNoEvents()
    {
      EXPECT_CALL(mock_interface, event_left).Times(0);
      EXPECT_CALL(mock_interface, event_far_left).Times(0);
      EXPECT_CALL(mock_interface, event_right).Times(0);
      EXPECT_CALL(mock_interface, event_far_right).Times(0);
      EXPECT_CALL(mock_interface, event_radio_is_active).Times(0);
    }
};

TEST_F(TestStatesPickup, TestSetup)
{
  Sm dut(mock_interface);

  ExpectNoEvents();

  EXPECT_EQ(dut.get_state(), State::RADIO);
}

/*
 * Radio active on power-up
 */
TEST_F(TestStatesPickup, TestStartRadio)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillOnce(DoAll(SetArgReferee<0>(false), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);
}

/*
 * pickup active on power-up
 */
TEST_F(TestStatesPickup, TestStartPickup)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillOnce(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);
}

/*
 * Radio active, switch to pick-up
 */
TEST_F(TestStatesPickup, TestSwitchRadioPickup)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillOnce(DoAll(SetArgReferee<0>(false), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);
}

/*
 * Pick-up is active, switch to radio
 */
TEST_F(TestStatesPickup, TestSwitchPickupRadio)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillOnce(DoAll(SetArgReferee<0>(true), Return(true)))
      .WillOnce(DoAll(SetArgReferee<0>(false), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);
}

/*
 * Radio active, switch to dial control
 */
TEST_F(TestStatesPickup, TestSwitchRadioCtrl)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING))
      .WillRepeatedly(SetArgReferee<0>(dial::State::SEARCHING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);

  ExpectNoEvents();
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);
}

/*
 * Pick-up active, switch to dial control
 */
TEST_F(TestStatesPickup, TestSwitchPickupCtrl)
{
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING))
      .WillRepeatedly(SetArgReferee<0>(dial::State::SEARCHING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);

  ExpectNoEvents();
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);
}

/*
 * Radio mode remains stable without input changes
 */
TEST_F(TestStatesPickup, TestRadioNoEvents)
{
  uint32_t i;
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);

  ExpectNoEvents();

  for (i = 0; i < 10000; ++i) {
    dut.tick();
    EXPECT_EQ(dut.get_state(), State::RADIO);
  }
}

/*
 * Pickup mode remains stable without input changes
 */
TEST_F(TestStatesPickup, TestPickupNoEvents)
{
  uint32_t i;
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);

  ExpectNoEvents();

  for (i = 0; i < 10000; ++i) {
    dut.tick();
    EXPECT_EQ(dut.get_state(), State::PICKUP);
  }
}

/*
 * Control mode remains stable without input changes
 */
TEST_F(TestStatesPickup, TestControlNoEvents)
{
  uint32_t i;
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING))
      .WillRepeatedly(SetArgReferee<0>(dial::State::MOVING));

  //startup pickup
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);

  //switch to ctrl
  ExpectNoEvents();
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);

  //and stay there
  for (i = 0; i < 10000; ++i) {
    dut.tick();
    EXPECT_EQ(dut.get_state(), State::CONTROL);
  }
}

/*
 * Radio active, switch to dial control and back
 */
TEST_F(TestStatesPickup, TestSwitchRadioCtrlBack)
{
  uint32_t i;
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING))
      .WillOnce(SetArgReferee<0>(dial::State::SEARCHING))
      .WillOnce(SetArgReferee<0>(dial::State::MOVING))
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  //startup radio
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::RADIO);

  //switch to ctrl
  ExpectNoEvents();
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);
  //stay there
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);

  //back to radio
  //has quite some debouncing time. This is intentional as we assume ctrl mode to be only used
  //when using am/fm radio (as it doesn't make any sense to control the web radio with hardware
  //inputs from web interface...)
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).
      Times(AtLeast(0));
  for (i = 0; i < 10000; ++i) {
    dut.tick();
    auto state = dut.get_state();
    if (state == State::RADIO) {
      break;
    }
  }
  EXPECT_EQ(dut.get_state(), State::RADIO);
}

/*
 * Pickup active, switch to dial control and leave afterwards
 */
TEST_F(TestStatesPickup, TestSwitchPickupCtrlBack)
{
  uint32_t i;
  Sm dut(mock_interface);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  EXPECT_CALL(mock_interface, read_switch_inputs)
      .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillOnce(SetArgReferee<0>(dial::State::WAITING))
      .WillOnce(SetArgReferee<0>(dial::State::SEARCHING))
      .WillOnce(SetArgReferee<0>(dial::State::MOVING))
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));

  //startup pickup
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::PICKUP);

  //switch to ctrl
  ExpectNoEvents();
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);
  //stay there
  dut.tick();
  EXPECT_EQ(dut.get_state(), State::CONTROL);

  //leave ctrl mode
  //does not make sense in PU mode, see previous test case. We check that SM does not
  //get stuck
  EXPECT_CALL(mock_interface, event_radio_is_active(_)).
      Times(AtLeast(0));
  for (i = 0; i < 10000; ++i) {
    dut.tick();
    auto state = dut.get_state();
    if (state != State::CONTROL) {
      break;
    }
  }
  EXPECT_NE(dut.get_state(), State::CONTROL);
}

/*
 * Radio mode active -> don't generate any events
 *
 * testcase 1 -- not stable for long enough
 */
TEST_F(TestStatesPickup, TestRadioEvents1)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(1), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(1)))
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 20; ++i) {
    dut.tick();
  }
}

/*
 * Radio mode active -> don't generate any events
 *
 * testcase 2 -- still not not stable for long enough
 */
TEST_F(TestStatesPickup, TestRadioEvents2)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Radio mode active -> don't generate any events
 *
 * enough events to trigger "left"
 */
TEST_F(TestStatesPickup, TestRadioEventsLeft)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Radio mode active -> don't generate any events
 *
 * enough events to trigger "far left"
 */
TEST_F(TestStatesPickup, TestRadioEventsFarLeft)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Radio mode active -> don't generate any events
 *
 * enough events to trigger "right"
 */
TEST_F(TestStatesPickup, TestRadioEventsRight)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Radio mode active -> don't generate any events
 *
 * enough events to trigger "far right"
 */
TEST_F(TestStatesPickup, TestRadioEventsFarRight)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(false), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(true))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}


/*
 * Pickup mode active
 *
 * testcase 1 -- not stable for long enough
 */
TEST_F(TestStatesPickup, TestPickupEvents1)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(1), SetArgReferee<2>(0)))
        .WillOnce(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(1)))
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * testcase 2 -- still not stable for long enough
 */
TEST_F(TestStatesPickup, TestPickupEvents2)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(2)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * enough events to trigger "left". clean event generation, no bouncing/noise involved
 */
TEST_F(TestStatesPickup, TestPickupEventsLeft)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(1);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * enough events to trigger "far left". clean event generation, no bouncing/noise involved
 */
TEST_F(TestStatesPickup, TestPickupEventsFarLeft)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(1);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * enough events to trigger "right". clean event generation, no bouncing/noise involved
 */
TEST_F(TestStatesPickup, TestPickupEventsRight)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(1);
  EXPECT_CALL(mock_interface, event_far_right).Times(0);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * enough events to trigger "far right". clean event generation, no bouncing/noise involved
 */
TEST_F(TestStatesPickup, TestPickupEventsFarRight)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(1);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 25; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * multiple events within one detection time window. Second event can be detected
 * but is not required. Mis-detection is not allowed.
 */
TEST_F(TestStatesPickup, TestPickupEventsMulti1)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //first (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //second (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(AtLeast(1));
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 30; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * multiple events within one detection time window. Second event can be detected
 * but is not required. Mis-detection is not allowed.
 */
TEST_F(TestStatesPickup, TestPickupEventsMulti2)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //first (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //second (left)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(AtLeast(0));
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(AtLeast(1));
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 30; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * multiple events within one detection time window. Second event can be detected
 * but is not required. Mis-detection is not allowed.
 */
TEST_F(TestStatesPickup, TestPickupEventsMulti3)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //first (left)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(0), SetArgReferee<2>(1)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //second (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(AtLeast(1));
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(AtLeast(0));
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 30; ++i) {
    dut.tick();
  }
}

/*
 * Pickup mode active
 *
 * multiple events some time apart. Second event must be detected
 */
TEST_F(TestStatesPickup, TestPickupEventsMulti4)
{
  uint32_t i;
  Sm dut(mock_interface, false);

  EXPECT_CALL(mock_interface, read_pickup_input)
      .WillRepeatedly(DoAll(SetArgReferee<0>(true), Return(true)));
  {
    InSequence s;
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(10)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //first (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(1000)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
    //second (far right)
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .Times(5)
        .WillRepeatedly(DoAll(SetArgReferee<0>(1), SetArgReferee<1>(1), SetArgReferee<2>(0)));
    EXPECT_CALL(mock_interface, read_switch_inputs)
        .WillRepeatedly(DoAll(SetArgReferee<0>(0), SetArgReferee<1>(0), SetArgReferee<2>(0)));
  }
  EXPECT_CALL(mock_interface, get_dial_sm_state)
      .WillRepeatedly(SetArgReferee<0>(dial::State::WAITING));
  EXPECT_CALL(mock_interface, event_left).Times(0);
  EXPECT_CALL(mock_interface, event_far_left).Times(0);
  EXPECT_CALL(mock_interface, event_right).Times(0);
  EXPECT_CALL(mock_interface, event_far_right).Times(2);
  EXPECT_CALL(mock_interface, event_radio_is_active(Eq(false))).Times(1);

  for (i = 0; i < 10000; ++i) {
    dut.tick();
  }
}
