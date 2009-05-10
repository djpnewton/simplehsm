//
// Boolean definitions
//

#ifndef BOOL
typedef int BOOL;
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

//
// Generic State machine signals
//

enum
{
  SIG_NULL  = 0,
  SIG_INIT  = 1,
  SIG_ENTRY = 2,
  SIG_EXIT  = 3,
  SIG_USER  = 4,
} SimpleHsmSignals;

//
// State machine definitions
//

#define stnone 0;
typedef int stnext;
typedef stnext (*stfunc)(int, void*);
typedef stfunc (*ptrstfunc)(int, void*);

typedef struct
{
  stfunc curState;
} SimpleHsm;

//
// State utility function definitions
//

void InitialState(SimpleHsm* hsm, stfunc newState);
void TransitionState(SimpleHsm* hsm, stfunc newState);
void InitTransitionState(SimpleHsm* hsm, stfunc newState);
void SignalCurrentState(SimpleHsm* hsm, int signal, void* param);
BOOL IsInState(SimpleHsm* hsm, stfunc state);
