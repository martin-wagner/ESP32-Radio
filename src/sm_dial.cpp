/*
 * sm_dial.cpp
 *
 *  Created on: Jan 9, 2022
 *      Author: martin
 */

#include "sm_dial.h"

namespace dial {

const char* TAG = "dial" ;

void Abstract_state::timer()
{
  ESP_LOGI ( TAG, "timer" ) ;
  stop();
}

void Abstract_state::set_state (Abstract_state *state)
{
  if (state == nullptr) {
    ESP_LOGI ( TAG, "invalid state switch");
    return;
  }

#ifdef SM_DIAL_PRINTING
  ESP_LOGI ( TAG, "SM Dial switch %s -> %s", sm->state->get_state_name(), state->get_state_name());
#endif
  sm->state = state;
  state->init_state();
}

void Abstract_state::stop()
{
  sm->h.set_dial_relay(0, 0, 0);
  set_state(&sm->wait);

  ESP_LOGI ( TAG, "stopping dial movement" ) ;
}

Time_ms State_wait::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::DIAL_FASTMOVELEFT:
      set_state(&sm->move_fast_left);
      return request.time;
    case Saba_remote_control_cmd::DIAL_FASTMOVERIGHT:
      set_state(&sm->move_fast_right);
      return request.time;
    case Saba_remote_control_cmd::DIAL_MOVELEFT:
      set_state(&sm->move_slow_left);
      return request.time;
    case Saba_remote_control_cmd::DIAL_MOVERIGHT:
      set_state(&sm->move_slow_right);
      return request.time;
    case Saba_remote_control_cmd::DIAL_SEARCHLEFT:
      set_state(&sm->search_inactive_left);
      return request.time;
    case Saba_remote_control_cmd::DIAL_SEARCHRIGHT:
      set_state(&sm->search_inactive_right);
      return request.time;
    case Saba_remote_control_cmd::DIAL_STATION_INACTIVE:
      break;
    case Saba_remote_control_cmd::DIAL_STATION_ACTIVE:
      break;
    case Saba_remote_control_cmd::DIAL_STOP:
      stop();
      break;
    default:
      break;
  }
  return -1;
}

const char* State_wait::get_state_name()
{
  static const char *c = "wait";
  return c;
}

Time_ms State_move::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::DIAL_STOP:
      stop();
      break;
    case Saba_remote_control_cmd::DIAL_SEARCHLEFT:
      set_state(&sm->search_inactive_left);
      return request.time;
    case Saba_remote_control_cmd::DIAL_SEARCHRIGHT:
      set_state(&sm->search_inactive_right);
      return request.time;
    default:
      break;
  }
  return -1;
}

void State_move::init_state()
{
  Abstract_state::init_state();
  set_dial_relay();
}

const char* State_move_slow_left::get_state_name()
{
  static const char *c = "move slow left";
  return c;
}

void State_move_slow_left::set_dial_relay()
{
  sm->h.set_dial_relay(1, 0, 0);
  ESP_LOGI ( TAG, "moving dial to the left" ) ;
}

const char* State_move_slow_right::get_state_name()
{
  static const char *c = "move slow right";
  return c;
}

void State_move_slow_right::set_dial_relay()
{
  sm->h.set_dial_relay(0, 1, 0);
  ESP_LOGI ( TAG, "moving dial to the right" ) ;
}

const char* State_move_fast_left::get_state_name()
{
  static const char *c = "move fast left";
  return c;
}

void State_move_fast_left::set_dial_relay()
{
  sm->h.set_dial_relay(1, 0, 1);
  ESP_LOGI ( TAG, "fast moving dial to the left" ) ;
}

const char* State_move_fast_right::get_state_name()
{
  static const char *c = "move fast right";
  return c;
}

void State_move_fast_right::set_dial_relay()
{
  sm->h.set_dial_relay(0, 1, 1);
  ESP_LOGI ( TAG, "fast moving dial to the right" ) ;
}

Time_ms State_search::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::DIAL_STOP:
      stop();
      break;
    default:
      break;
  }
  return -1;
}

Time_ms State_search_leave::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::DIAL_STATION_INACTIVE:
      set_state(&sm->search_active);
      return sm->h.get_movement_limit();
    default:
      //delegate to base class
      return State_search::cmd(request);
  }
  return -1;
}

void State_search_leave::init_state()
{
  Abstract_state::init_state();
  set_dial_relay();
}

const char* State_search_leave_left::get_state_name()
{
  static const char *c = "search left leave";
  return c;
}

void State_search_leave_left::set_dial_relay()
{
  sm->h.set_dial_relay(1, 0, 0);
  ESP_LOGI ( TAG, "searching for station to the left" ) ;
}

const char* State_search_leave_right::get_state_name()
{
  static const char *c = "search right leave";
  return c;
}

void State_search_leave_right::set_dial_relay()
{
  sm->h.set_dial_relay(0, 1, 0);
  ESP_LOGI ( TAG, "searching for station to the right" ) ;
}

Time_ms State_search_next::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::DIAL_STATION_ACTIVE:
      ESP_LOGI ( TAG, "station found!" ) ;
      stop();
      break;
    default:
      //delegate to base class
      return State_search::cmd(request);
  }
  return -1;
}

const char* State_search_next::get_state_name()
{
  static const char *c = "search next";
  return c;
}

Sm::Sm(Interface &h) :
    wait(this), move_slow_left(this), move_slow_right(this), move_fast_left(this), move_fast_right(this),
    search_inactive_left(this), search_inactive_right(this), search_active(this), h(h)
{
}

Time_ms Sm::cmd(Saba_remote_control &request)
{
  if (request.time > h.get_movement_limit())
  {
    request.time = h.get_movement_limit();
  }

  auto timeout = state->cmd(request);
  if (timeout < 0) {
    timeout = h.get_movement_limit();
  }
  return timeout;
}

void Sm::timer()
{
  state->timer();
}

State Sm::get_state()
{
  return state->get_state();
}

}
