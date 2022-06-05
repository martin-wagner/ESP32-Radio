/*
 * mock_sm_webctrl.h
 *
 *  Created on: Feb 6, 2022
 *      Author: martin
 */

#ifndef TEST_MOCK_SM_WEBCTRL_H_
#define TEST_MOCK_SM_WEBCTRL_H_


#include <gmock/gmock.h>
#include "src/sm_webctrl.h"

namespace webctrl {

class MockInterface : public Interface
{
 public:

  MOCK_METHOD(void, read_switch_inputs, (uint8_t &movement, uint8_t &fast, uint8_t &left), (override));
  MOCK_METHOD(bool, read_pickup_input, (uint8_t &active), (override));
  MOCK_METHOD(void, get_dial_sm_state, (dial::State &state), (override));
  MOCK_METHOD(uint32_t, get_window_size, (), (override));
  MOCK_METHOD(void, get_debounce_sample_count, (uint32_t &stable, uint32_t &delay), (override));
  MOCK_METHOD(void, event_left, (), (override));
  MOCK_METHOD(void, event_far_left, (), (override));
  MOCK_METHOD(void, event_right, (), (override));
  MOCK_METHOD(void, event_far_right, (), (override));
  MOCK_METHOD(void, event_radio_is_active, (bool active), (override));


};

}  // namespace webctrl




#endif /* TEST_MOCK_SM_WEBCTRL_H_ */
