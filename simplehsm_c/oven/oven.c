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
// Oven state machine object
//

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
    case SIG_EXIT:
      printf("  heating: exiting state\n");
      return stnone;
    case SIG_OPEN_DOOR:
      printf("  heating: OPEN_DOOR signal!\n");
      simplehsm_transition_state(&hsm, doorOpen);
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

int _tmain(int argc, _TCHAR* argv[])
{
  simplehsm_initial_state(&hsm, oven);
  simplehsm_signal_current_state(&hsm, SIG_OPEN_DOOR, NULL);
  show_status(&hsm);
  return 0;
}