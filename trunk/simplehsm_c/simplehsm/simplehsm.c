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
 * The simplehsm C implementation
 */

/** \mainpage
 *
 * \section intro_sec Introduction
 *
 * simplehsm is designed to be a very simple implementation of a hierarchical state machine.
 *
 * From http://www.eventhelix.com/RealtimeMantra/HierarchicalStateMachine.htm:
 *
 * \verbatim
In conventional state machine design, all states are considered at the same level. 
The design does not capture the commonality that exists among states. In real life, 
many states handle most  messages in similar fashion and differ only in handling of 
few key messages. Even when the actual handling differs, there is still some 
commonality. Hierarchical state machine design captures the commonality by organizing 
the states as a hierarchy. The states at the higher level in hierarchy perform the 
common message handling, while the lower level states inherit the commonality from 
higher level ones and perform the state specific functions.
\endverbatim
 * 
 * @todo A nice helpful overview like: http://ehiti.de/machine_objects/
 */

#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

//
// State utility function implementations
//

/**
* Initialise a simplehsm state machine.
* 
* @param hsm The state machine to initialise
* @param top_state The topmost (and initial) state of the hierarchy
* 
*/
void simplehsm_initialize(simplehsm_t* hsm, stfunc top_state)
{
  int i;
  // init top state
  hsm->top_state = top_state;
#ifdef SHSM_DEEPHIST
  // init deep history table
  for (i = 0; i < MAX_HISTORY; i++)
  {
    hsm->deep_history_table[i].parent = stnone;
    hsm->deep_history_table[i].child = stnone;
  }
#endif
  // transition to initial top state
  simplehsm_init_transition_state(hsm, top_state);
}

/**
* Check is one state is the parent of another.
* 
* @param hsm The state machine to check
* @param parent_state The parent state to check
* @param child_state The child state to check
* @return True if the parent_state parameter is a parent to the child_state parameter
* 
*/
BOOL _is_parent(simplehsm_t* hsm, stfunc parent_state, stfunc child_state)
{
  do 
  {
    child_state = (stfunc)child_state(SIG_NULL, NULL);
    if (child_state == parent_state)
      return TRUE;
  }
  while (child_state != NULL);
  return FALSE;
}

#ifdef SHSM_DEEPHIST
/**
* Initiate a transition to a new state.
*
* This function will:
*   - replace the target state with the deep history target if required
*   - store a deep history target state if exiting a composite state with a deep history child
*   - perform the exit/entry chain from the current state to the target state
* 
* @param hsm The state machine to transition
* @param new_state The state to transition to
* @param to_deep_hist Whether to transition to the states internal deep history psuedostate
* 
*/
void simplehsm_transition_state_ex(simplehsm_t* hsm, stfunc new_state, BOOL to_deep_hist)
{
  // retrive historical state if appropriate
  if (to_deep_hist)
  {
    new_state = simplehsm_retrieve_deephist(hsm, new_state);
    if (new_state == stnone)
    {
      printf("simplehsm_transition_state_ex: ERROR - simplehsm_retrieve_deephist failed!");
      return;
    }
  }
#else
/**
* Initiate a transition to a new state.
* 
* @param hsm The state machine to transition
* @param new_state The state to transition to
* 
*/
void simplehsm_transition_state(simplehsm_t* hsm, stfunc new_state)
{
#endif
  // exit signal to current state
  if (hsm->current_state != NULL)
  {
    stfunc parent_state;
#ifdef SHSM_DEEPHIST
    // record the deep history state if appropriate
    parent_state = (stfunc)hsm->current_state(SIG_NULL, NULL);
    while (parent_state != stnone)
    {
      // the top state cannot have a history sub state and returns stnone to all 
      // unhandled signals anyhow
      if (parent_state == hsm->top_state)            
        break;
      // if the parent_state handles SIG_DEEPHIST and is not a parent of the new 
      // state then record the deep history
      else if ((stfunc)parent_state(SIG_DEEPHIST, NULL) == stdeephist &&
               !_is_parent(hsm, parent_state, new_state))
      {
        simplehsm_record_deephist(hsm, parent_state, hsm->current_state);
      }
      parent_state = (stfunc)parent_state(SIG_NULL, NULL);
    }
#endif
    // state exit chain 
    hsm->current_state(SIG_EXIT, NULL);
    parent_state = (stfunc)hsm->current_state(SIG_NULL, NULL);
    while (!_is_parent(hsm, parent_state, new_state))
    {
      parent_state(SIG_EXIT, NULL);
      parent_state = (stfunc)parent_state(SIG_NULL, NULL);
    }
    // set current state to parent state
    hsm->current_state = parent_state;
  }
  else
    printf("simplehsm_transition_state_ex: ERROR - current state is invalid!");
  
  // entry signal to new state
  while (hsm->current_state != new_state)
  {
    stfunc parent_state = new_state;
    stfunc lastChild = new_state;
    while (TRUE)
    {
      parent_state = parent_state(SIG_NULL, NULL);
      if (parent_state == hsm->current_state)
      {
        lastChild(SIG_ENTRY, NULL);
        // set current state to last child state
        hsm->current_state = lastChild;
        break;
      }
      lastChild = parent_state;
    }
  }

  // init signal to new state
  new_state(SIG_INIT, NULL);
}

#ifdef SHSM_DEEPHIST
/**
* Initiate a transition to a new state.
* 
* @param hsm The state machine to transition
* @param new_state The state to transition to
* 
*/
void simplehsm_transition_state(simplehsm_t* hsm, stfunc new_state)
{
  simplehsm_transition_state_ex(hsm, new_state, FALSE);
}
#endif

/**
* Initiate an initial transition to a new state (this function should only be used from a #SIG_INIT state event).
* 
* @param hsm The state machine to transition
* @param new_state The state to transition to
* 
*/
void simplehsm_init_transition_state(simplehsm_t* hsm, stfunc new_state)
{
  // set new state
  hsm->current_state = new_state;
  if (hsm->current_state != NULL)
  {
    // entry signal to current state
    hsm->current_state(SIG_ENTRY, NULL);
    // init signal to current state
    hsm->current_state(SIG_INIT, NULL);
  }
  else
    printf("simplehsm_init_transition_state: ERROR - current state is invalid!\n");
}

/**
* Send a signal to the state machine.
* 
* @param hsm The state machine to signal
* @param signal The signal flag
* @param param An optional parameter to accompany the signal
* 
*/
void simplehsm_signal_current_state(simplehsm_t* hsm, int signal, void* param)
{
  if (hsm->current_state != NULL)
  {
    stfunc parent_state = hsm->current_state;
    do
      parent_state = (stfunc)parent_state(signal, param);
    while (parent_state != NULL);
  }
  else
    printf("simplehsm_signal_current_state: ERROR - current state is invalid!\n");
}

/**
* Check if a state machine is currently in a certain state.
* 
* @param hsm The state machine to check
* @param state The state to check
* @return True if the state machine is currently in the specified state
* 
*/
BOOL simplehsm_is_in_state(simplehsm_t* hsm, stfunc state)
{
  stfunc parent_state = hsm->current_state;
  do
  {
    if (state == parent_state)
      return TRUE;
    parent_state = (stfunc)parent_state(SIG_NULL, NULL);
  }
  while (parent_state != NULL);
  return FALSE;
}

#ifdef SHSM_DEEPHIST
/**
* Record deep history psuedostate.
* 
* @param hsm The state machine
* @param history_parent The parent state of the deep history psuedostate
* @param history_state The state to return to if transitioning to the deep history psuedostate
* 
*/
void simplehsm_record_deephist(simplehsm_t* hsm, stfunc history_parent, stfunc history_state)
{
  int i;
  for (i = 0; i < MAX_HISTORY; i++)
  {
    if (hsm->deep_history_table[i].parent == stnone || hsm->deep_history_table[i].parent == history_parent)
    {
      hsm->deep_history_table[i].parent = history_parent;
      hsm->deep_history_table[i].child = history_state;
      return;
    }
  }
  printf("simplehsm_record_deephist: ERROR - ran out of deep history table space!\n");
}

/**
* Retrive deep history psuedostate.
* 
* @param hsm The state machine
* @param history_parent The parent state of the deep history psuedostate
* @return The state to transition to via the deep history psuedostate
* 
*/
stfunc simplehsm_retrieve_deephist(simplehsm_t* hsm, stfunc history_parent)
{
  int i;
  for (i = 0; i < MAX_HISTORY; i++)
  {
    if (hsm->deep_history_table[i].parent == history_parent)
    {
      stfunc res = hsm->deep_history_table[i].child;
      // remove history state from deep history table
      hsm->deep_history_table[i].parent = stnone;
      hsm->deep_history_table[i].child = stnone;
      // return deep history target
      return res;
    }
  }   
  return stnone;
}
#endif
