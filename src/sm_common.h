/*
 * sm_common.h
 *
 *  Created on: Jan 9, 2022
 *      Author: martin
 *
 *
 * https://stackoverflow.com/questions/14676709/c-code-for-state-machine
 * or https://github.com/hbarcelos/cpp-state-machine
 * but with static state allocation instead of dynamic
 *
 */

#ifndef SRC_SM_COMMON_H_
#define SRC_SM_COMMON_H_

#include <stdint.h>
#include <memory>

#include <Arduino.h>

//c++11 make_unique
//https://stackoverflow.com/a/17903225
#if __cplusplus == 201103L
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
#endif

//arduino.h "#define FALLING xyz". WTF???
#undef FALLING
#undef RISING

enum class Edge {
  NONE,
  FALLING,
  RISING,
};

using Time_ms = int32_t;   // time in milliseconds

// Saba Remote Control Stuff
enum class Saba_remote_control_cmd {
  DIAL_FASTMOVELEFT,        // Fast move dial pointer
  DIAL_FASTMOVERIGHT,
  DIAL_MOVELEFT,            // Move dial pointer
  DIAL_MOVERIGHT,
  DIAL_SEARCHLEFT,          // Move dial pointer, then search for station
  DIAL_SEARCHRIGHT,
  DIAL_STOP,                // Stop moving dial pointer
  DIAL_STATION_ACTIVE,      // Radio station now active at current dial position
  DIAL_STATION_INACTIVE,    // Radio station not active at current dial position
  AMP_UPVOL,                // Rotate volume knob to increase current volume
  AMP_DOWNVOL,              // Rotate volume knob to decrease current volume
  AMP_STOPVOL,              // Stop rotating knob
  AMP_MUTE,                 // Mute/unmute the audio amp (toggle)
};

struct Saba_remote_control {
  Saba_remote_control_cmd cmd;
  Time_ms time;
};

//Saba front inputs Stuff
enum class Saba_input {
  FAST_LEFT,
  LEFT,
  NONE,
  RIGHT,
  FAST_RIGHT,
};


#endif /* SRC_SM_COMMON_H_ */
