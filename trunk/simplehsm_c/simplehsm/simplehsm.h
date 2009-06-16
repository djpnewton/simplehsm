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
 * The simplehsm C implementation header
 */

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

/** \enum simplehsm_signals_t
 * The base state signals
 */
enum simplehsm_signals_t
{
  SIG_NULL  = 0, /**< Null signal, all state functions should ignore this signal and return their parent state (or NULL if the top level state) */
  SIG_INIT  = 1, /**< Initialisation signal, a state function should transition to a default substate (simplehsm_init_transition_state()) if it has substates */
  SIG_ENTRY = 2, /**< Entry signal, a state function should perform its entry actions (if any) */
  SIG_EXIT  = 3, /**< Exit signal, a state function should perform its exit actions (if any) */
  SIG_USER  = 4, /**< User signals should start from this index */
};

//
// State machine definitions
//

// TODO - make these definitions better? (simplehsm_state_t etc?)
/** 
 * Used by a state function when it returns no parent state
 */
#define stnone NULL;
/**
 * A generic pointer that points to a state function
 */
typedef void* stnext;
/**
 * A state function
 */
typedef stnext (*stfunc)(int, void*);

/**
 * The state machine object
 */
typedef struct
{
  stfunc current_state; /**< The current state of the state machine */
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