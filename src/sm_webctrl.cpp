/*
 * sm_webctrl.cpp
 *
 *  Created on: Jan 10, 2022
 *      Author: martin
 */


#include <algorithm>

#include "sm_webctrl.h"

namespace webctrl {

void Abstract_state::set_state(Abstract_state *state)
{
  if (state == nullptr) {
    dbgprint("invalid state switch");
    return;
  }

#ifdef SM_WEBCTRL_PRINTING
  dbgprint("SM webctrl switch %s -> %s", sm->state->get_state_name(), state->get_state_name());
#endif
  sm->state = state;
  state->init_state();
}

void Abstract_state::init_state()
{
}

bool Abstract_state::tick_precheck_handle()
{
  bool have_pu_input;
  uint8_t input_active;

  //check for user control
  if (sm->history->get(0)->dial_state != dial::State::WAITING) {
    set_state(&sm->internal_control);
    return true;
  }
  //check for pickup switch input -- all the other stuff is not needed if we have that
  have_pu_input = sm->h.read_pickup_input(input_active);
  if (have_pu_input) {
    if (input_active) {
      if (sm->state != &sm->pickup_stay) {
        set_state(&sm->pickup_stay);
        sm->h.event_radio_is_active(false);
        dbgprint ( "switch to pickup (direct)" ) ;
      }
    } else {
      if (sm->state != &sm->radio_stay) {
        set_state(&sm->radio_stay);
        sm->h.event_radio_is_active(true);
        dbgprint ( "switch to radio (direct)" ) ;
      }
    }
    return true;
  }
  return false;
}

void State_radio_entry::tick()
{
  if (tick_precheck_handle()) {
    return;
  }

  if (is_stable()) {
    set_state(&sm->radio_stay);
    return;
  }
  if (is_instable()) {
    set_state(&sm->pickup_entry);
    sm->h.event_radio_is_active(false);
    dbgprint ( "switch to pickup (check)" ) ;
    return;
  }
}

void State_radio_entry::init_state()
{
  State_radio::init_state();
}

const char* State_radio_entry::get_state_name()
{
  static const char *c = "radio entry";
  return c;
}

bool State_radio_entry::is_stable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_STABLE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count + stats().fast.edge_count;
  if (events > EVENTS_STABLE) {
    return true;
  }
  return false;
}

bool State_radio_entry::is_instable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_LEAVE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count; //ignore "fast" -- is never noise
  if (events <= EVENTS_LEAVE) {
    return true;
  }
  return false;
}


void State_radio_stay::tick()
{
  if (tick_precheck_handle()) {
    return;
  }

  if (is_instable()) {
    set_state(&sm->pickup_entry);
    sm->h.event_radio_is_active(false);
    dbgprint ( "switch to pickup (check)" ) ;
    return;
  }
  if ( ! is_stable()) {
    set_state(&sm->radio_entry);
  }
}

void State_radio_stay::init_state()
{
  State_radio::init_state();
}

const char* State_radio_stay::get_state_name()
{
  static const char *c = "radio stay";
  return c;
}

bool State_radio_stay::is_stable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_STABLE);

  if ( ! stats.are_valid()) {
    return true;
  }

  events = stats().movement.edge_count + stats().left.edge_count + stats().fast.edge_count;
  if (events > EVENTS_STABLE) {
    return true;
  }
  return false;
}

bool State_radio_stay::is_instable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_LEAVE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count; //ignore "fast" -- is never noise
  if (events <= EVENTS_LEAVE) {
    return true;
  }
  return false;
}

void State_pickup::init_state()
{
  movement_event_occured = false;
  count = 0;
  Abstract_state::init_state();
}

void State_pickup::check_generate_events()
{
  //unit testing -- evaluate container event with less samples. Does not break anything.
  auto size = sm->history->size();
  if (size > TICKS_EVENT_WINDOW) {
    size = TICKS_EVENT_WINDOW;
  }
  Statistics stats(sm->h, *(sm->history), size);
  if ( ! stats.are_valid()) {
    return;
  }

  if ( ! stats().movement.valid_event) {
    //movement not detected
    movement_event_occured = false;
    count = 0;
    return;
  }
  count ++;
  if (count < sm->sync_delay) {
    return;
  }


  if (movement_event_occured == true) {
    //we can only handle _one_ event in time windows. Multiple events would require
    //us to check for correlation in time points (e.g. if we have two movement and one
    //left event -- does the left event correlate to the current movement event or to the
    //one before that).
    return;
  }
  movement_event_occured = true;

  auto fast = stats().fast.valid_event;
  auto left = stats().left.valid_event;
  if (fast && left) {
    sm->h.event_far_left();
    dbgprint ( "rocker left fast" ) ;
  } else if (fast && ! left) {
    sm->h.event_far_right();
    dbgprint ( "rocker right fast" ) ;
  } else if ( ! fast && left) {
    sm->h.event_left();
    dbgprint ( "rocker left" ) ;
  } else if ( ! fast && ! left) {
    sm->h.event_right();
    dbgprint ( "rocker right" ) ;
  }
}

void State_pickup_entry::tick()
{
  auto no_check = tick_precheck_handle();
  if (! no_check) {
    if (is_stable()) {
      set_state(&sm->pickup_stay);
      return;
    }
    if (is_instable()) {
      set_state(&sm->radio_entry);
      sm->h.event_radio_is_active(true);
      dbgprint ( "switch to radio (check)" ) ;
      return;
    }
  }

  check_generate_events();
}

void State_pickup_entry::init_state()
{
  State_pickup::init_state();
}

const char* State_pickup_entry::get_state_name()
{
  static const char *c = "pu entry";
  return c;
}

bool State_pickup_entry::is_stable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_STABLE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count;
  if (events <= EVENTS_STABLE) {
    return true;
  }
  return false;
}

bool State_pickup_entry::is_instable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_LEAVE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count;
  if (events > EVENTS_LEAVE) {
    return true;
  }
  return false;
}

void State_pickup_stay::tick()
{
  auto no_check = tick_precheck_handle();
  if (! no_check) {
    if (is_instable()) {
      set_state(&sm->radio_entry);
      sm->h.event_radio_is_active(true);
      dbgprint ( "switch to radio (check)" ) ;
      return;
    }
    if ( ! is_stable()) {
      set_state(&sm->pickup_entry);
      return;
    }
  }

  check_generate_events();
}

void State_pickup_stay::init_state()
{
  State_pickup::init_state();
}

const char* State_pickup_stay::get_state_name()
{
  static const char *c = "pu stay";
  return c;
}

bool State_pickup_stay::is_stable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_STABLE);

  if ( ! stats.are_valid()) {
    return true;
  }

  events = stats().movement.edge_count + stats().left.edge_count;
  if ((events <= EVENTS_STABLE) || stats().movement.valid_event) {
    return true;
  }
  return false;
}

bool State_pickup_stay::is_instable()
{
  uint32_t events;
  Statistics stats(sm->h, *(sm->history), TICKS_LEAVE);

  if ( ! stats.are_valid()) {
    return false;
  }

  events = stats().movement.edge_count + stats().left.edge_count;
  if (events > EVENTS_LEAVE) {
    return true;
  }
  return false;
}

void State_internal_control::tick()
{
  if (sm->history->get(0)->dial_state == dial::State::WAITING) {
    ticks_waiting ++;
  } else {
    //restart
    ticks_waiting = 0;
  }

  if (ticks_waiting > TICKS_LEAVE) {
    set_state(&sm->radio_entry);
    sm->h.event_radio_is_active(true);
    dbgprint ( "switch to radio (from control)" ) ;
  }
}

void State_internal_control::init_state()
{
  Abstract_state::init_state();
  ticks_waiting = 0;
}

const char* State_internal_control::get_state_name()
{
  static const char *c = "ctrl";
  return c;
}

Sm::Sm(Interface &h, bool print) :
    radio_entry(this), radio_stay(this), pickup_entry(this), pickup_stay(this), internal_control(
        this), h(h), print(print)
{
  uint32_t dummy;

  history = make_unique<History>(h.get_window_size());
  h.get_debounce_sample_count(dummy, sync_delay);
}

Sm::~Sm()
{
}

void Sm::tick()
{
  uint8_t movement;
  uint8_t fast;
  uint8_t left;
  dial::State dial_state;
  History::Entry s;

  h.read_switch_inputs(movement, fast, left);
  h.get_dial_sm_state(dial_state);

  s.movement.state = movement;
  s.movement.edge = Edge::NONE;
  s.fast.state = fast;
  s.fast.edge = Edge::NONE;
  s.left.state = left;
  s.left.edge = Edge::NONE;
  s.dial_state = dial_state;
  history->add_sample(s);
  if (print) {
    history->print_sample();
  }

  state->tick();
}

State Sm::get_state()
{
  return state->get_state();
}

History::History(uint32_t window_size)
{
  history = make_unique<H> (window_size);
  clear_history();
}

void History::clear_history()
{
  samples_taken = 0;
}

bool History::is_valid(uint32_t required_samples) const
{
  if ((required_samples <= history->size()) &&
      (samples_taken >= required_samples)) {
    return true;
  }
  return false;
}

bool History::is_valid() const
{
  if (samples_taken >= history->size()) {
    return true;
  }
  return false;
}

void History::add_sample(const Entry &s)
{
  Entry &current = (*history)[0];
  Entry &previous = (*history)[1];

  std::rotate(history->begin(), history->begin() + history->size() - 1, history->end()); //rotate right  by one. C++20 has shift...
  current = s;

  if (samples_taken < UINT32_MAX) {
    samples_taken ++;
  }

  if (samples_taken > 1) {
    current.movement.edge = get_edge(current.movement.state, previous.movement.state);
    current.fast.edge = get_edge(current.fast.state, previous.fast.state);
    current.left.edge = get_edge(current.left.state, previous.left.state);
  }
}

uint32_t History::size() const
{
  if (history->size() < samples_taken) {
    return history->size();
  }
  return samples_taken;
}

const History::Entry* History::get(uint32_t i) const
{
  if (is_valid(i + 1)) {
    return &(history->at(i));
  }
  return nullptr;
}

const History::H &History::get() const
{
  return *(history.get());
}

void History::print_sample(uint32_t i) const
{
  auto entry = get(i);
  if (entry == nullptr) {
    return;
  }

  dbgprint(
      "    add_sample((bool)%d, (bool)%d, (bool)%d, (dial::State)%d);",
      entry->movement.state, entry->fast.state, entry->left.state, entry->dial_state);
}

Edge History::get_edge(uint8_t current, uint8_t previous)
{
  if (current == previous) {
    return Edge::NONE;
  }
  if ((current == 0) && (previous > 0)) {
    return Edge::FALLING;
  }
  if ((current > 0) && (previous == 0)) {
    return Edge::RISING;
  }
  //here be dragons
  return Edge::NONE;
}

Statistics::Statistics(Interface &h, const History &history, uint32_t for_n_samples) :
    history(history), n(for_n_samples)
{
  uint32_t dummy;

  if (history.size() < n) {
    return;
  }

  h.get_debounce_sample_count(debounce_samples, dummy);

  //lambda accessors
  Get_history_input get_input_movement = [](const History::Entry &e)->const History::Input & { return e.movement; };
  Get_history_input get_input_fast     = [](const History::Entry &e)->const History::Input & { return e.fast;     };
  Get_history_input get_input_left     = [](const History::Entry &e)->const History::Input & { return e.left;     };

  set_stats_single(stats.movement, get_input_movement);
  set_stats_single(stats.fast,     get_input_fast);
  set_stats_single(stats.left,     get_input_left);

  valid = true;
}

const Statistics::S& Statistics::get()
{
  return stats;
}

void Statistics::set_stats_single(Input &input, Get_history_input &get_history_input)
{
  uint32_t i;
  uint32_t debounce_counter;

  input.valid_event = false;
  input.edge_count = 0;
  input.state = get_history_input(*history.current()).state;

  //search for valid events, count edges
  debounce_counter = 0;
  for (i = 0; i < n; i++) { //newest to oldest
    const auto &sample = get_history_input(*history.get(i));

    switch (sample.edge) {
      case Edge::RISING:
        input.edge_count ++;
        //event detection on falling edge -> start new detection on rising edge
        debounce_counter = 0;
        break;
      case Edge::FALLING:
        input.edge_count ++;
        break;
      case Edge::NONE:
      default:
        if (sample.state == true) {
          debounce_counter ++;
        }
        if (debounce_counter >= debounce_samples) {
          input.valid_event = true;
        }
        break;
    }
  }
}

}


