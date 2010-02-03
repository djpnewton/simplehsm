/**
 * @file
 * @author  Daniel Newton <djpnewton@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2009 Daniel Newton
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * A contrived oven state machine. A simple demo of the simplehsm C implementation.
 */

#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

/** \enum oven_signals_t
 * The oven state machine signals
 */
enum oven_signals_t
{
  SIG_OPEN_DOOR = SIG_USER, /**< The oven door has been opened */
  SIG_CLOSE_DOOR,           /**< The oven door has been closed */
  SIG_TOASTING,             /**< The toasting mode has been selected */
  SIG_BAKING                /**< The baking mode has been selected */
};

/**
 * The Oven state machine object
 */
simplehsm_t hsm = {NULL};


//
// Oven State function definitions
//

stnext oven(int signal, void* param);
  stnext heating(int signal, void* param);
    stnext toasting(int signal, void* param);
    stnext baking(int signal, void* param);
  stnext doorOpen(int signal, void* param);

//
// Oven State function implementations
//

/**
 * The top level oven state
 * 
 * @param signal The signal to send to this state
 * @param param A signal specific parameter
 * @return Aways returns #stnone indicating that this is the top level state
 */
stnext oven(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("oven: entering state\n");
      return stnone;
    case SIG_INIT:
        simplehsm_init_transition_state(&hsm, heating);
      return stnone;
    case SIG_EXIT:
      printf("oven: exiting state\n");
      return stnone;
  }
  return stnone;
}

/**
 * The heating state. This state contains a deep history psuedostate.
 * 
 * @param signal The signal to send to this state
 * @param param A signal specific parameter
 * @return #stnone/#stdeephist if the signal is handled, otherwise the parent state (oven())
 */
stnext heating(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("  heating: entering state\n");
      return stnone;
    case SIG_INIT:
      simplehsm_init_transition_state(&hsm, toasting);
      return stnone;
    case SIG_DEEPHIST:
      return stdeephist; // tell framework to record deep history state
    case SIG_EXIT:
      printf("  heating: exiting state\n");
      return stnone;
    case SIG_OPEN_DOOR:
      printf("  heating: OPEN_DOOR signal!\n");
      simplehsm_transition_state(&hsm, doorOpen);
      return stnone;
    case SIG_TOASTING:
      printf("  heating: TOASTING signal!\n");
      simplehsm_transition_state(&hsm, toasting);
      return stnone;
    case SIG_BAKING:
      printf("  heating: BAKING signal!\n");
      simplehsm_transition_state(&hsm, baking);
      return stnone;
  }
  return (stnext)oven;
}

/**
 * The toasting state
 * 
 * @param signal The signal to send to this state
 * @param param A signal specific parameter
 * @return #stnone if the signal is handled, otherwise the parent state (heating())
 */
stnext toasting(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("    toasting: entering state\n");
      return stnone;
    case SIG_INIT:
      return stnone;
    case SIG_EXIT:
      printf("    toasting: exiting state\n");
      return stnone;
  }
  return (stnext)heating;
}

/**
 * The baking state
 * 
 * @param signal The signal to send to this state
 * @param param A signal specific parameter
 * @return #stnone if the signal is handled, otherwise the parent state (heating())
 */
stnext baking(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("    baking: entering state\n");
      return stnone;
    case SIG_INIT:
      return stnone;
    case SIG_EXIT:
      printf("    baking: exiting state\n");
      return stnone;
  }
  return (stnext)heating;
}

/**
 * The doorOpen state
 * 
 * @param signal The signal to send to this state
 * @param param A signal specific parameter
 * @return #stnone if the signal is handled, otherwise the parent state (oven())
 */
stnext doorOpen(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("  doorOpen: entering state\n");
      return stnone;
    case SIG_INIT:
      return stnone;
    case SIG_EXIT:
      printf("  doorOpen: exiting state\n");
      return stnone;
    case SIG_CLOSE_DOOR:
      printf("  doorOpen: CLOSE_DOOR signal!\n");
      simplehsm_transition_state_ex(&hsm, heating, TRUE);
      return stnone;
  }
  return (stnext)oven;
}

/**
 * Show the status of the oven state machine by showing what states it is currently in
 * 
 * @param hsm The oven state machine
 */
void show_status(simplehsm_t* hsm)
{
  printf("\nChecking States:\n");
  printf("  in oven state: %d\n  in heating state: %d\n  in toasting state: %d\n  in baking state: %d\n  in doorOpen state: %d\n\n",
         simplehsm_is_in_state(hsm, oven),
         simplehsm_is_in_state(hsm, heating),
         simplehsm_is_in_state(hsm, toasting),
         simplehsm_is_in_state(hsm, baking),
         simplehsm_is_in_state(hsm, doorOpen));
}

/**
 * Main function of the oven program
 * 
 * Sets the initial state, sends a signal and then shows the state machine status
 */
int _tmain(int argc, _TCHAR* argv[])
{
  simplehsm_initialize(&hsm, oven);
  simplehsm_signal_current_state(&hsm, SIG_BAKING, NULL);
  simplehsm_signal_current_state(&hsm, SIG_OPEN_DOOR, NULL);
  simplehsm_signal_current_state(&hsm, SIG_CLOSE_DOOR, NULL);
  show_status(&hsm);
  return 0;
}