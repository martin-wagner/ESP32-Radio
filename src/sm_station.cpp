/*
 * sm_station.cpp
 *
 *  Created on: Jan 10, 2022
 *      Author: martin
 */


#include "sm_station.h"

namespace station {

void Abstract_state::set_state(Abstract_state *state)
{
  if (state == nullptr) {
    dbgprint("invalid state switch");
    return;
  }

#ifdef SM_STATION_PRINTING
  dbgprint("SM station switch %s -> %s", sm->state->get_state_name(), state->get_state_name());
#endif
  sm->state = state;
  state->init_state();
}

bool Abstract_state::consider_active()
{
  uint32_t active_count = 0;
  uint32_t i;
  float active_threshold;
  float inactive_threshold;

  if ( ! sm->is_history_valid()) {
    return false;
  }
  sm->h.get_threshold(active_threshold, inactive_threshold);

  //calc stats
  //for (const auto &s : *sm->history) {
  for (i = 0; i < sm->history->size(); i++) {
    const auto &s = (*sm->history)[i];

    if (s > active_threshold) {
      active_count ++;
    }
  }

  //evaluate result
  if (active_count > (uint32_t)((float)sm->history->size() * 0.9)) {
    dbgprint ( "station active" ) ;
    return true;
  }
  return false;
}

bool Abstract_state::consider_inactive()
{
  uint32_t inactive_count = 0;
  uint32_t noise_count = 0;
  uint32_t i;
  float active_threshold;
  float inactive_threshold;

  if ( ! sm->is_history_valid()) {
    return true;
  }
  sm->h.get_threshold(active_threshold, inactive_threshold);

  //calc stats
  for (i = 0; i < sm->history->size(); i++) {
    const auto &s = (*sm->history)[i];

    if (s < inactive_threshold) {
      inactive_count ++;
    } else if (s > active_threshold) {
    } else {
      noise_count ++;
    }
  }

  //evaluate result
  if (inactive_count > (sm->history->size() / 4)) {
    dbgprint ( "station gone (1)" ) ;
    return true;
  }
  if ((inactive_count > (sm->history->size() / 8)) &&
      (noise_count > 0)) {
    dbgprint ( "station gone (2)" ) ;
    return true;
  }

  return false;
}

bool Abstract_state::consider_noisy()
{
  uint32_t active_count = 0;
  uint32_t inactive_count = 0;
  uint32_t noise_count = 0;
  uint32_t i;
  float active_threshold;
  float inactive_threshold;

  if ( ! sm->is_history_valid()) {
    return false;
  }
  sm->h.get_threshold(active_threshold, inactive_threshold);

  //calc stats
  for (i = 0; i < sm->history->size(); i++) {
    const auto &s = (*sm->history)[i];

    if (s < inactive_threshold) {
      inactive_count ++;
    } else if (s > active_threshold) {
      active_count ++;
    } else {
      noise_count ++;
    }
  }

  //evaluate result
  if (noise_count > (sm->history->size() / 2)) {
    return true;
  }
  if ((noise_count > (sm->history->size() / 4)) &&
      (active_count > 0) &&
      (inactive_count > 0)) {
    return true;
  }
  return false;
}

void State_inactive::tick()
{
  if (consider_active()) {
    set_state(&sm->active);
    sm->h.event_active();
  } else if (consider_noisy()) {
    set_state(&sm->noise);
    sm->h.event_noisy();
  }
}

const char* State_inactive::get_state_name()
{
  static const char *c = "inactive";
  return c;
}

void State_active::tick()
{
  if (consider_inactive()) {
    set_state(&sm->inactive);
    sm->h.event_inactive();
  } else if (consider_noisy()) {
    set_state(&sm->noise);
    sm->h.event_noisy();
  }
}

const char* State_active::get_state_name()
{
  static const char *c = "active";
  return c;
}

void State_noise::tick()
{
  if (consider_inactive() ||
      ! consider_noisy()) {
    set_state(&sm->inactive);
    sm->h.event_inactive();
  }
}

const char* State_noise::get_state_name()
{
  static const char *c = "noise";
  return c;
}

Sm::Sm(Interface &h) :
    inactive(this), active(this), noise(this), h(h)
{
  history = new std::vector<float> (h.get_window_size());
  clear_history();
}

Sm::~Sm()
{
  delete history;
}

void Sm::tick()
{
  auto s = h.read_analog_value();
  add_sample(s);

  state->tick();
}

State Sm::get_state()
{
  return state->get_state();
}

void Sm::clear_history()
{
  samples_taken = 0;
}

bool Sm::is_history_valid()
{
  if (samples_taken >= history->size()) {
    return true;
  }
  return false;
}

void Sm::add_sample(const float &s)
{
  int i;

  for (i = 1; i < history->size(); i++) {
    (*history)[i] = (*history)[i - 1];
  }
  (*history)[0] = s;
  if (samples_taken < UINT32_MAX) {
    samples_taken ++;
  }
}

}


