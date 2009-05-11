// simplehsm.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

//
// State utility function implementations
//

void InitialState(SimpleHsm* hsm, stfunc newState)
{
  InitTransitionState(hsm, newState);
}

BOOL IsParent(SimpleHsm* hsm, stfunc parentState, stfunc childState)
{
  do 
  {
    childState = (stfunc)childState(SIG_NULL, NULL);
    if (childState == parentState)
      return TRUE;
  }
  while (childState != NULL);
  return FALSE;
}

void TransitionState(SimpleHsm* hsm, stfunc newState)
{
  // exit signal to current state
  if (hsm->curState != NULL)
  {
    stfunc parentState;
    hsm->curState(SIG_EXIT, NULL);
    parentState = (stfunc)hsm->curState(SIG_NULL, NULL);
    while (!IsParent(hsm, parentState, newState))
    {
      parentState(SIG_EXIT, NULL);
      parentState = (stfunc)parentState(SIG_NULL, NULL);
    }
    // set current state to parent state
    hsm->curState = parentState;
  }
  else
    printf("TransitionState: ERROR - current state is invalid!");
  
  // entry signal to new state
  while (hsm->curState != newState)
  {
    stfunc parentState = newState;
    stfunc lastChild = newState;
    while (TRUE)
    {
      parentState = parentState(SIG_NULL, NULL);
      if (parentState == hsm->curState)
      {
        lastChild(SIG_ENTRY, NULL);
        // set current state to last child state
        hsm->curState = lastChild;
        break;
      }
      lastChild = parentState;
    }
  }

  // init signal to new state
  newState(SIG_INIT, NULL);
}

void InitTransitionState(SimpleHsm* hsm, stfunc newState)
{
  // set new state
  hsm->curState = newState;
  if (hsm->curState != NULL)
  {
    // entry signal to current state
    hsm->curState(SIG_ENTRY, NULL);
    // init signal to current state
    hsm->curState(SIG_INIT, NULL);
  }
  else
    printf("InitTransitionState: ERROR - current state is invalid!\n");
}

void SignalCurrentState(SimpleHsm* hsm, int signal, void* param)
{
  if (hsm->curState != NULL)
  {
    stfunc parentState = hsm->curState;
    do
      parentState = (stfunc)parentState(signal, param);
    while (parentState != NULL);
  }
  else
    printf("SignalCurrentState: ERROR - current state is invalid!\n");
}

BOOL IsInState(SimpleHsm* hsm, stfunc state)
{
  stfunc parentState = hsm->curState;
  do
  {
    if (state == parentState)
      return TRUE;
    parentState = (stfunc)parentState(SIG_NULL, NULL);
  }
  while (parentState != NULL);
  return FALSE;
}





