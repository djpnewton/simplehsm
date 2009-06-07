//---------------------------------------------------------------------------
#ifndef calcHsmH
#define calcHsmH
//---------------------------------------------------------------------------

#include <iup.h>

#include "simplehsm.h"

//
// Calc State Machine Signals
//

enum
{
    SIG_CLEAR = SIG_USER,
    SIG_CLEARENTRY,
    SIG_DIGIT1_9,
    SIG_DIGIT0,
    SIG_OPERATOR,
    SIG_POINT,
    SIG_EQUALS
} calc_signals_t;


//
// Calc state machine public definitions
//

void calchsm_init(Ihandle* tb);
void calchsm_signal(int signal, void* param);


//---------------------------------------------------------------------------
#endif  //calcHsmH