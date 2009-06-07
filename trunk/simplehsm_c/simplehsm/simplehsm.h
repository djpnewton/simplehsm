//---------------------------------------------------------------------------
#ifndef simplehsmH
#define simplehsmH
//---------------------------------------------------------------------------

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
} simplehsm_signals_t;

//
// State machine definitions
//

// TODO - make these definitions better? (simplehsm_state_t etc?)
#define stnone NULL;
typedef void* stnext;
typedef stnext (*stfunc)(int, void*);
typedef stfunc (*ptrstfunc)(int, void*);

typedef struct
{
  stfunc current_state;
} simplehsm_t;

//
// State utility function definitions
//

void simplehsm_initial_state(simplehsm_t* hsm, stfunc new_state);
void simplehsm_transition_state(simplehsm_t* hsm, stfunc new_state);
void simplehsm_init_transition_state(simplehsm_t* hsm, stfunc new_state);
void simplehsm_signal_current_state(simplehsm_t* hsm, int signal, void* param);
BOOL simplehsm_is_in_state(simplehsm_t* hsm, stfunc state);

//---------------------------------------------------------------------------
#endif  //simplehsmH