/*
 * sm_station.cpp
 *
 *  Created on: Jan 10, 2022
 *      Author: martin
 */


#include "sm_station.h"

namespace station {

const char* TAG = "station" ;

void Abstract_state::set_state(Abstract_state *state)
{
  if (state == nullptr) {
    ESP_LOGI ( TAG, "invalid state switch");
    return;
  }

#ifdef SM_STATION_PRINTING
  ESP_LOGI ( TAG, "SM station switch %s -> %s", sm->state->get_state_name(), state->get_state_name());
#endif
  sm->state = state;
  state->init_state();
}

void Abstract_state::init_state()
{
  sm->h.get_threshold(active_threshold, inactive_threshold);
}

bool Abstract_state::consider_active()
{
  if ( ! sm->is_history_valid()) {
    return false;
  }

  auto av = get_average();

  if (av >= active_threshold) {
    ESP_LOGI ( TAG, "station active" ) ;
    return true;
  }
  return false;
}

bool Abstract_state::consider_inactive()
{
  if ( ! sm->is_history_valid()) {
    return true;
  }

  auto av = get_average();

  if (av <= inactive_threshold) {
    ESP_LOGI ( TAG, "station gone" ) ;
    return true;
  }
  return false;
}

bool Abstract_state::consider_noisy()
{
  if ( ! sm->is_history_valid()) {
    return false;
  }

  auto av = get_average();

  if ((av < active_threshold) && (av > inactive_threshold)) {
    return true;
  }
  return false;
}

float Abstract_state::get_average()
{
  float av;

  av = 0;
  for (const auto &s : *sm->history) {
    av = av + s;
  }
  av = av / sm->history->size();
  return av;
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


