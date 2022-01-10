/*
 * sm_dial.h
 *
 *  Created on: Jan 9, 2022
 *      Author: martin
 */

#ifndef SRC_SM_DIAL_H_
#define SRC_SM_DIAL_H_

#include "sm_common.h"

#define SM_DIAL_PRINTING

namespace dial {

class Sm;

enum class State
{
  WAITING,
  MOVING,
  SEARCHING
};

class Abstract_state
{
  public:
    Abstract_state(Sm *sm) : sm(sm) {};
    virtual ~Abstract_state() {};

    virtual Time_ms cmd(Saba_remote_control &request) = 0;
    virtual void timer();

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
    virtual void timer() {};

    virtual State get_state() { return State::WAITING; };

  protected:

    virtual const char *get_state_name();

};

class State_move : public Abstract_state
{
  public:
    State_move(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_move() {};

    virtual Time_ms cmd(Saba_remote_control &request);

    virtual State get_state() { return State::MOVING; };

  protected:

    virtual void init_state();
    virtual void set_dial_relay() = 0;

};

class State_move_slow_left : public State_move
{
  public:
    State_move_slow_left(Sm *sm) : State_move(sm) {};
    virtual ~State_move_slow_left() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

class State_move_slow_right : public State_move
{
  public:
    State_move_slow_right(Sm *sm) : State_move(sm) {};
    virtual ~State_move_slow_right() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

class State_move_fast_left : public State_move
{
  public:
    State_move_fast_left(Sm *sm) : State_move(sm) {};
    virtual ~State_move_fast_left() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

class State_move_fast_right : public State_move
{
  public:
    State_move_fast_right(Sm *sm) : State_move(sm) {};
    virtual ~State_move_fast_right() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

//station search:
//- slow motor movement
//- leave current station -> wait until event "gone"
//- wait until event "found"
class State_search : public Abstract_state
{
  public:
    State_search(Sm *sm) : Abstract_state(sm) {};
    virtual ~State_search() {};

    virtual Time_ms cmd(Saba_remote_control &request);

    virtual State get_state() { return State::SEARCHING; };

};

class State_search_leave : public State_search
{
  public:
    State_search_leave(Sm *sm) : State_search(sm) {};
    virtual ~State_search_leave() {};

    virtual Time_ms cmd(Saba_remote_control &request);

  protected:

    virtual void init_state();
    virtual void set_dial_relay() = 0;
};

class State_search_leave_left : public State_search_leave
{
  public:
    State_search_leave_left(Sm *sm) : State_search_leave(sm) {};
    virtual ~State_search_leave_left() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

class State_search_leave_right : public State_search_leave
{
  public:
    State_search_leave_right(Sm *sm) : State_search_leave(sm) {};
    virtual ~State_search_leave_right() {};

  protected:

    virtual const char *get_state_name();

  protected:

    virtual void set_dial_relay();

};

class State_search_next : public State_search
{
  public:
    State_search_next(Sm *sm) : State_search(sm) {};
    virtual ~State_search_next() {};

    virtual Time_ms cmd(Saba_remote_control &request);

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
     * set output relais
     *
     * this is the state machine output
     */
    virtual void set_dial_relay(uint8_t left, uint8_t right, uint8_t fast) = 0;

    /**
     * time for moving dial pointer from one end to the other
     */
    virtual Time_ms get_movement_limit() = 0;

};

class Sm
{
  friend class Abstract_state;
  friend class State_wait;
  friend class State_move;
  friend class State_move_slow;
  friend class State_move_slow_left;
  friend class State_move_slow_right;
  friend class State_move_fast;
  friend class State_move_fast_left;
  friend class State_move_fast_right;
  friend class State_search;
  friend class State_search_leave;
  friend class State_search_leave_left;
  friend class State_search_leave_right;
  friend class State_search_next;

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
     */
    void timer();

    /**
     * get current machine state
     *
     * @return state
     */
    State get_state();


  private:

    State_wait wait;
    State_move_slow_left move_slow_left;
    State_move_slow_right move_slow_right;
    State_move_fast_left move_fast_left;
    State_move_fast_right move_fast_right;
    State_search_leave_left search_inactive_left;
    State_search_leave_right search_inactive_right;
    State_search_next search_active;
    Abstract_state *state = &wait;

    Interface &h;

};






}

#endif /* SRC_SM_DIAL_H_ */
