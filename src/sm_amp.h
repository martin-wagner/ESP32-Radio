/*
 * sm_amp.h
 *
 *  Created on: Jan 10, 2022
 *      Author: martin
 */

#ifndef SRC_SM_AMP_H_
#define SRC_SM_AMP_H_


#include "sm_common.h"

//#define SM_AMP_PRINTING

namespace amp {

class Sm;

enum class State
{
  WAITING,
  INCREASING,
  DECREASING
};

class Abstract_state
{
  public:
    Abstract_state(Sm *sm) : sm(sm) {};
    virtual ~Abstract_state() {};

    virtual Time_ms cmd(Saba_remote_control &request);
    virtual Time_ms timer();

    virtual State get_state() = 0;

  protected:

    Sm *sm;

  protected:

    void set_state(Abstract_state *state);
    virtual void init_state() {};
    virtual const char *get_state_name() = 0;

  protected:

    void stop();

};

class State_wait : public Abstract_state
{
  public:
    State_wait(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_wait() {};

    virtual Time_ms cmd(Saba_remote_control &request);
    virtual Time_ms timer() { return -1; };

    virtual State get_state() { return State::WAITING; };

  protected:

    virtual const char *get_state_name();

  private:

    //mute state is stored in gpio, not in software state
    void toggle_mute();

};

class State_volume : public Abstract_state
{
  public:
    State_volume(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_volume() {};

  protected:

    virtual void init_state();
    virtual void set_amp_relay() = 0;

};

class State_volume_up : public State_volume
{
  public:
    State_volume_up(Sm *sm) : State_volume(sm) {};
    virtual ~State_volume_up() {};

    virtual State get_state() { return State::INCREASING; };

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_amp_relay();

};

class State_volume_down : public State_volume
{
  public:
    State_volume_down(Sm *sm) : State_volume(sm) {};
    virtual ~State_volume_down() {};

    virtual Time_ms timer();

    virtual State get_state() { return State::DECREASING; };

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_amp_relay();

};

class State_timeout : public Abstract_state
{
  public:
    State_timeout(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_timeout() {};

    virtual Time_ms timer();

    virtual State get_state() { return State::DECREASING; };

  protected:

    virtual void init_state();
    virtual const char *get_state_name();

};

/**
 * interface to connect state machine user
 */
class Interface
{
  public:

    /**
     * set output relais
     *
     * this is the state machine output
     */
    virtual void set_amp_relay(uint8_t up, uint8_t down) = 0;

    /**
     * set output relay
     *
     * this is the state machine output
     */
    virtual void set_mute_relay(uint8_t mute) = 0;

    /**
     * get current "mute" state from hardware
     */
    virtual bool get_mute() = 0;

    /**
     * time for moving volume pot from one end to the other
     */
    virtual Time_ms get_movement_limit() = 0;

    /**
     * decrease volume for this time in case of timeout
     */
    virtual Time_ms get_save_decrease() = 0;

};

class Sm
{
  friend class Abstract_state;
  friend class State_wait;
  friend class State_volume;
  friend class State_volume_up;
  friend class State_volume_down;
  friend class State_timeout;

  public:

    Sm(Interface &h);

    /**
     * handles user command
     *
     * @param request cmd
     * @return call timer() after delay
     */
    Time_ms cmd(Saba_remote_control &request);

    /**
     * timer elapsed
     * @return call timer() after delay
     */
    Time_ms timer();

    /**
     * get current machine state
     *
     * @return state
     */
    State get_state();


  private:

    State_wait wait;
    State_volume_up volume_up;
    State_volume_down volume_down;
    State_timeout timeout;
    Abstract_state *state = &wait;

    Interface &h;

};


}

#endif /* SRC_SM_AMP_H_ */
