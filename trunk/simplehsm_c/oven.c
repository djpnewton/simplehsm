#include <stdio.h>
#include <tchar.h>

#include "simplehsm.h"

//
// Oven State machine signals
//

enum
{
  SIG_OPEN_DOOR = SIG_USER,
} OvenSignals;

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

stnext oven(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("oven: entering state\n");
      return stnone;
    case SIG_INIT:
      InitTransitionState(heating);
      return stnone;
    case SIG_EXIT:
      printf("oven: exiting state\n");
      return stnone;
  }
  return stnone;
}

stnext heating(int signal, void* param)
{
  switch (signal)
  {
    case SIG_ENTRY:
      printf("  heating: entering state\n");
      return stnone;
    case SIG_INIT:
      InitTransitionState(toasting);
      return stnone;
    case SIG_EXIT:
      printf("  heating: exiting state\n");
      return stnone;
    case SIG_OPEN_DOOR:
      printf("  heating: OPEN_DOOR signal!\n");
      TransitionState(doorOpen);
      return stnone;
  }
  return (stnext)oven;
}

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
  }
  return (stnext)oven;
}

void ShowStatus(void)
{
  printf("\nChecking States:\n");
  printf("  in oven state: %d\n  in heating state: %d\n  in toasting state: %d\n  in baking state: %d\n  in doorOpen state: %d\n\n",
         IsInState(oven), IsInState(heating), IsInState(toasting), IsInState(baking), IsInState(doorOpen));
}

int _tmain(int argc, _TCHAR* argv[])
{
  InitialState(oven);
  SignalCurrentState(SIG_OPEN_DOOR, NULL);
  ShowStatus();
	return 0;
}