/*
 * sm_webctrl.h
 *
 *  Created on: Jan 10, 2022
 *      Author: Martin Wagner
 *
 * This state machine monitors digital inputs from rocker switch and uses them to control
 * the web radio.
 * We have three digital inputs for the switch
 * - direction
 * - (slow) movement
 * - fast movement.
 * By mechanics, "movement" is active as soon as the rocker switch is moved in any direction.
 * "fast movement" can be activated in addition to "movement" by pushing the switch beyond the
 * indentation. Circuit design makes direction input default "right". Any event on this input
 * means switch has been pushed to the left.
 * We also use the dial movement control state machine as input. This is required because
 * the control feeds back to the inputs inside hardware.
 *
 * From those inputs we have to detect the following information
 * - is the switch in idle position?
 * - is the switch pushed to the left/right first indentation?
 *  -> event left / right
 * - is the switch pushed to the left/right limit?
 *  -> event far left / right
 * - is pick-up input enabled?
 *  -> event radio / pick-up active
 * - is the movement control state machine non-idle?
 *  -> suppress any event /todo assume radio and generate event radio active?
 *
 * Knowing pick-up mode is somewhat essential for proper funcion
 * - wireless communication leads to distortion in fm reception
 *  -> disable web radio when pick-up is not enabled, nobody is listening anyway...
 * - when no switch is active and radio is active, part of the audio signal is coupled
 *   on top of the high-Z search dc signal, leading to erroneous detection of input changes
 *  -> disable event generation when pick-up is not enabled
 * However we don't have a digital input for that, so some guesswork is implemented in
 * software. //todo was ist implementiert...
 * /todo input is on pcb, requires modification
 * in radio and implementing in software.
 */

#ifndef SRC_SM_WEBCTRL_H_
#define SRC_SM_WEBCTRL_H_

#include <vector>
#include <functional>

#include "sm_common.h"
#include "sm_dial.h"

//#define SM_WEBCTRL_PRINTING //todo

namespace webctrl {

//assume this tick time
//fixme not the proper way to do this, but makes correcting this also easier
static constexpr uint32_t MS_TO_TICK_DIVIDER = 5;

class Sm;
class History;
class Statistics;

enum class State
{
  RADIO,
  PICKUP,
  CONTROL,
};

class Abstract_state
{
  public:
    Abstract_state(Sm *sm) : sm(sm) {};
    virtual ~Abstract_state() {};

    virtual void tick() = 0;

    virtual State get_state() = 0;

  protected:

    Sm *sm;

  protected:

    void set_state(Abstract_state *state);
    virtual void init_state();
    virtual const char *get_state_name() = 0;

  protected:

    bool tick_precheck_handle();

};

class State_radio : public Abstract_state
{
  public:
    State_radio(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_radio() {};

    virtual State get_state() { return State::RADIO; };

};

class State_radio_entry : public State_radio
{
  public:
    State_radio_entry(Sm *sm) : State_radio(sm) {};
    virtual ~State_radio_entry() {};

    virtual void tick();

  protected:

    virtual void init_state(); //todo hier und weitere notwendig?
    virtual const char *get_state_name();

  private:

    bool is_stable();
    bool is_instable();

    //time to monitor before switching to pick-up
    const uint32_t TICKS_LEAVE = 500 / MS_TO_TICK_DIVIDER;
    //maximum events for leaving
    const uint32_t EVENTS_LEAVE = 2;
    //time to monitor to assume stable operation in radio state
    const uint32_t TICKS_STABLE = 2500 / MS_TO_TICK_DIVIDER;
    //minimum events necessary for assumption
    const uint32_t EVENTS_STABLE = 10;

};

class State_radio_stay : public State_radio
{
  public:
    State_radio_stay(Sm *sm) : State_radio(sm) {};
    virtual ~State_radio_stay() {};

    virtual void tick();

  protected:

    virtual void init_state();
    virtual const char *get_state_name();

  private:

    bool is_stable();
    bool is_instable();

    //time to monitor to assume stable operation in radio state
    const uint32_t TICKS_STABLE = 1000 / MS_TO_TICK_DIVIDER;
    //minimum events necessary for assumption
    const uint32_t EVENTS_STABLE = 2;
    //direct leaving if always only absolute minimum of events happen
    //time to monitor before switching to pick-up
    const uint32_t TICKS_LEAVE = 5 * TICKS_STABLE;
    //maximum events for direct leaving
    const uint32_t EVENTS_LEAVE = 5 * EVENTS_STABLE;
};

class State_pickup : public Abstract_state
{
  public:
    State_pickup(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_pickup() {};

    virtual State get_state() { return State::PICKUP; };

};

class State_pickup_entry : public State_pickup
{
  public:
    State_pickup_entry(Sm *sm) : State_pickup(sm) {};
    virtual ~State_pickup_entry() {};

    virtual void tick();

  protected:

    virtual void init_state();
    virtual const char *get_state_name();

  private:

    bool is_stable();
    bool is_instable();

    //time to monitor before switching to radio
    const uint32_t TICKS_LEAVE = 500 / MS_TO_TICK_DIVIDER;
    //minimum events for leaving
    const uint32_t EVENTS_LEAVE = 6;
    //time to monitor to assume stable operation in pickup state
    const uint32_t TICKS_STABLE = 2500 / MS_TO_TICK_DIVIDER;
    //maximum events for assumption
    const uint32_t EVENTS_STABLE = 0;

};

class State_pickup_stay : public State_pickup
{
  public:
    State_pickup_stay(Sm *sm) : State_pickup(sm) {};
    virtual ~State_pickup_stay() {};

    virtual void tick();

  protected:

    virtual void init_state();
    virtual const char *get_state_name();

  private:

    bool is_stable();
    bool is_instable();

    //time to monitor to assume stable operation in pickup state
    const uint32_t TICKS_STABLE = 500 / MS_TO_TICK_DIVIDER;
    //maximum events for assumption. Not zero as regular operation also generates events
    const uint32_t EVENTS_STABLE = 2;
    //direct leaving if too many events happen
    //time to monitor before switching to radio
    const uint32_t TICKS_LEAVE = 1000 / MS_TO_TICK_DIVIDER;
    //minimum events for direct leaving
    const uint32_t EVENTS_LEAVE = 15;

};

class State_internal_control : public Abstract_state
{
  public:
    State_internal_control(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_internal_control() {};

    virtual void tick();

    virtual State get_state() { return State::CONTROL; };

  protected:

    virtual void init_state();
    virtual const char *get_state_name();

  private:

    //remain in this state after switch condition occured
    const uint32_t TICKS_LEAVE = 500 / MS_TO_TICK_DIVIDER;

    uint32_t ticks_waiting = 0;

};

/**
 * interface to connect state machine user
 */
class Interface
{
  public:

    /**
     * read digital inputs
     *
     * 0 = inactive, !=0 = active
     */
    virtual void read_switch_inputs(uint8_t &movement, uint8_t &fast, uint8_t &left) = 0;

    /**
     * read pick-up mode input
     *
     * @return false - input not available
     */
    virtual bool read_pickup_input(uint8_t &active) { return false; };

    /**
     * get dial state machine state
     */
    virtual void get_dial_sm_state(dial::State &state) = 0;

    /**
     * get window size for signal analysis
     */
    virtual uint32_t get_window_size() = 0;

    /**
     * signal needs to be unchanged for this number of samples to be valid
     *
     * @remark no larger than window size
     */
    virtual void get_debounce_sample_count(uint32_t &pos, uint32_t &neg) = 0;

    /**
     * is called when button "left" is pressed to indentation
     */
    virtual void event_left() {};

    /**
     * is called when button "left" is pressed beyond indentation
     */
    virtual void event_far_left() {};

    /**
     * is called when button "right" is pressed to indentation
     */
    virtual void event_right() {};

    /**
     * is called when button "right" is pressed beyond indentation
     */
    virtual void event_far_right() {};

    /**
     * is called when radio is detected (in)active
     *
     * some guesswork involved, see comment on top.
     */
    virtual void event_radio_is_active(bool active) {}

};

class Sm
{
  friend class Abstract_state;
  friend class State_radio;
  friend class State_radio_entry;
  friend class State_radio_stay;
  friend class State_pickup;
  friend class State_pickup_entry;
  friend class State_pickup_stay;
  friend class State_internal_control;

  public:

    Sm(Interface &h, bool print = false);
    virtual ~Sm();

    /**
     * execute state machine
     */
    void tick();

    /**
     * get current machine state
     *
     * @return state
     */
    State get_state();


  private:

    State_radio_entry radio_entry;
    State_radio_stay radio_stay;
    State_pickup_entry pickup_entry;
    State_pickup_stay pickup_stay;
    State_internal_control internal_control;
    Abstract_state *state = &radio_entry;

    Interface &h;

  private:

    Time_ms t_diff_incremental = 0;

  private:

    uint32_t ticks_debounce_pos;
    uint32_t ticks_debounce_neg;

  protected:

    std::unique_ptr<History> history;
    const bool print;

};

/**
 * signal state archive
 */
class History
{
  public:

    History(uint32_t window_size);
    virtual ~History() {};

    struct Input {
      bool state;
      Edge edge;
    };

    struct Entry {
      Input movement;
      Input fast;
      Input left;
      dial::State dial_state;
    };

    using H = std::vector<Entry>;

    void clear_history();
    bool is_valid(uint32_t required_samples) const;
    bool is_valid() const;
    void add_sample(const Entry &s);
    uint32_t size() const;
    const Entry *get(uint32_t i) const;
    const Entry *current() const { return get(0); }
    const H &get() const;
    const H &operator()() const { return get(); };

    //print sample for creating unit test
    void print_sample(uint32_t i = 0) const;

  protected:

    uint32_t samples_taken = 0;

    std::unique_ptr<H> history; //most recent history[0]

  protected:

    Edge get_edge(uint8_t current, uint8_t previous);
};

/**
 * Statistics over given history
 */
class Statistics
{
  public:

    Statistics(Interface &h, const History &history, uint32_t for_n_samples);
    virtual ~Statistics() {}

    struct Input {
      bool state;           //current pin state
      bool valid_event;     //valid edge (according to /todo ... found in history
      uint32_t edge_count;  //<n> edges in history
    };

    struct S {
      Input movement;
      Input fast;
      Input left;
    };

    bool are_valid() { return valid; };
    const S &get();
    const S &operator()() { return get(); };

  protected:

    S stats;
    bool valid = false;

    const History &history;
    const uint32_t n;

    struct {
      uint32_t pos_edges;
      uint32_t neg_edges;
    } debounce;

  private:

    using Get_history_input = std::function<const History::Input &(const History::Entry &)>;
    void set_stats_single(Input &input, Get_history_input &get_history_input);



};

}

#endif /* SRC_SM_WEBCTRL_H_ */
