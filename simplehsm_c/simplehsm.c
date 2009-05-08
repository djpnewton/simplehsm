// simplehsm.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

//
// Current state variable
//

stfunc curState = NULL;

//
// State utility function implementations
//

void InitialState(stfunc newState)
{
  InitTransitionState(newState);
}

BOOL IsParent(stfunc parentState, stfunc childState)
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

void TransitionState(stfunc newState)
{
  // exit signal to current state
  if (curState != NULL)
  {
    stfunc parentState;
    curState(SIG_EXIT, NULL);
    parentState = (stfunc)curState(SIG_NULL, NULL);
    while (!IsParent(parentState, newState))
    {
      parentState(SIG_EXIT, NULL);
      parentState = (stfunc)parentState(SIG_NULL, NULL);
    }
  }
  else
    printf("TransitionState: ERROR - current state is invalid!");
  
  InitTransitionState(newState);
}

void InitTransitionState(stfunc newState)
{
  // set new state
  curState = newState;
  // entry signal to current state
  SignalCurrentState(SIG_ENTRY, NULL);
  // init signal to current state
  SignalCurrentState(SIG_INIT, NULL);
}

void SignalCurrentState(int signal, void* param)
{
  if (curState != NULL)
  {
    stfunc parentState = curState;
    do
      parentState = (stfunc)parentState(signal, param);
    while (parentState != NULL);
  }
  else
    printf("SignalCurrentState: ERROR - current state is invalid!\n");
}

BOOL IsInState(stfunc state)
{
  stfunc parentState = curState;
  do
  {
    if (state == parentState)
      return TRUE;
    parentState = (stfunc)parentState(SIG_NULL, NULL);
  }
  while (parentState != NULL);
  return FALSE;
}





