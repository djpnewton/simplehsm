// simplehsm.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

//
// State utility function implementations
//

void simplehsm_initial_state(simplehsm_t* hsm, stfunc new_state)
{
  simplehsm_init_transition_state(hsm, new_state);
}

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

void simplehsm_transition_state(simplehsm_t* hsm, stfunc new_state)
{
  // exit signal to current state
  if (hsm->current_state != NULL)
  {
    stfunc parent_state;
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
    printf("simplehsm_transition_state: ERROR - current state is invalid!");
  
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





