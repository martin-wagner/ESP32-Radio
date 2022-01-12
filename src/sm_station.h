/*
 * sm_station.h
 *
 *  Created on: Jan 10, 2022
 *      Author: Martin Wagner
 *
 *  This state machine monitors analog signal level and time, and uses those information
 *  to detect when the signal is
 *  - strong and consistent enough to be considered an active station
 *  - weak enough to be considered noise
 *
 */

#ifndef SRC_SM_STATION_H_
#define SRC_SM_STATION_H_

#include <vector>
#include "sm_common.h"

//#define SM_STATION_PRINTING

namespace station {

class Sm;

enum class State
{
  ACTIVE,
  INACTIVE,
  NOISE,
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
    virtual void init_state() {};
    virtual const char *get_state_name() = 0;

  protected:

    bool consider_active();
    bool consider_inactive();
    bool consider_noisy();

};

class State_inactive : public Abstract_state
{
  public:
    State_inactive(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_inactive() {};

    virtual void tick();

    virtual State get_state() { return State::INACTIVE; };

  protected:

    virtual const char *get_state_name();

};

class State_active : public Abstract_state
{
  public:
    State_active(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_active() {};

    virtual void tick();

    virtual State get_state() { return State::ACTIVE; };

  protected:

    virtual const char *get_state_name();

};

class State_noise : public Abstract_state
{
  public:
    State_noise(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_noise() {};

    virtual void tick();

    virtual State get_state() { return State::NOISE; };

  protected:

    virtual const char *get_state_name();

};

/**
 * interface to connect state machine user
 */
class Interface
{
  public:

    /**
     * get analog value, raw 0 ... 100% of AD scale
     */
    virtual float read_analog_value() = 0;

    /**
     * get upper / lower threshold, 0 ... 100%
     */
    virtual void get_threshold(float &active, float &inactive) = 0;

    /**
     * get window size for signal analysis
     */
    virtual Time_ms get_window_size() = 0;

    /**
     * is called when station changed to active
     */
    virtual void event_active() {};

    /**
     * is called when station changed to inactive
     */
    virtual void event_inactive() {};

    /**
     * is called when signal level is unstable
     */
    virtual void event_noisy() {};

};

class Sm
{
  friend class Abstract_state;
  friend class State_inactive;
  friend class State_active;
  friend class State_noise;

  public:

    Sm(Interface &h);
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

    State_inactive inactive;
    State_active active;
    State_noise noise;
    Abstract_state *state = &inactive;

    Interface &h;

  private:

    Time_ms t_diff_incremental = 0;

  private:

    uint32_t samples_taken = 0;

  protected:

    std::vector<float> *history = nullptr; //most recent history[0]

    void clear_history();
    bool is_history_valid();
    void add_sample(const float &s);

};


}

#endif /* SRC_SM_STATION_H_ */
