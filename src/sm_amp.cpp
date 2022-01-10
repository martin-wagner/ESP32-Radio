/*
 * sm_amp.cpp
 *
 *  Created on: Jan 10, 2022
 *      Author: martin
 */


#include "sm_amp.h"

namespace amp {

Time_ms Abstract_state::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::AMP_STOPVOL:
      stop();
      break;
    default:
      break;
  }
  return -1;
}

Time_ms Abstract_state::timer()
{
  set_state(&sm->timeout);
  return sm->h.get_save_decrease();
}

void Abstract_state::set_state (Abstract_state *state)
{
  if (state == nullptr) {
    dbgprint("invalid state switch");
    return;
  }

#ifdef SM_AMP_PRINTING
  dbgprint("SM amp switch %s -> %s", sm->state->get_state_name(), state->get_state_name());
#endif
  sm->state = state;
  state->init_state();
}

void Abstract_state::stop()
{
  sm->h.set_amp_relay(0, 0);
  set_state(&sm->wait);

  dbgprint ( "stopping amp movement" ) ;
}

Time_ms State_wait::cmd(Saba_remote_control &request)
{
  switch (request.cmd)
  {
    case Saba_remote_control_cmd::AMP_UPVOL:
      set_state(&sm->volume_up);
      return request.time;
    case Saba_remote_control_cmd::AMP_DOWNVOL:
      set_state(&sm->volume_down);
      return request.time;
    case Saba_remote_control_cmd::AMP_MUTE:
      toggle_mute();
      break;
    default:
      //delegate to base class
      return Abstract_state::cmd(request);
  }
  return -1;
}

const char* State_wait::get_state_name()
{
  static const char *c = "wait";
  return c;
}

void State_wait::toggle_mute()
{
  auto in = sm->h.get_mute();
  if (in == true) {
    sm->h.set_mute_relay(0);
  } else {
    sm->h.set_mute_relay(1);
  }
  dbgprint ( "volume mute" ) ;
}

void State_volume::init_state()
{
  Abstract_state::init_state();
  set_amp_relay();
}

const char* State_volume_up::get_state_name()
{
  static const char *c = "vol up";
  return c;
}

void State_volume_up::set_amp_relay()
{
  sm->h.set_amp_relay(1, 0);
  dbgprint ( "volume up" ) ;
}

Time_ms State_volume_down::timer()
{
  //ignore timeout, volume already down
  stop();
  return -1;
}

const char* State_volume_down::get_state_name()
{
  static const char *c = "vol down";
  return c;
}

void State_volume_down::set_amp_relay()
{
  sm->h.set_amp_relay(0, 1);
  dbgprint ( "volume down" ) ;
}

Time_ms State_timeout::timer()
{
  stop();
  return -1;
}

void State_timeout::init_state()
{
  sm->h.set_amp_relay(0, 1);
  dbgprint ( "volume down (timer)" ) ;
}

const char* State_timeout::get_state_name()
{
  static const char *c = "timeout";
  return c;
}

Sm::Sm(Interface &h) :
    wait(this), volume_up(this), volume_down(this), timeout(this), h(h)
{
}

Time_ms Sm::cmd(Saba_remote_control &request)
{
  if (request.time > h.get_movement_limit())
  {
    request.time = h.get_movement_limit();
  }

  return state->cmd(request);
}

Time_ms Sm::timer()
{
  return state->timer();
}

State Sm::get_state()
{
  return state->get_state();
}

}


