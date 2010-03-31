/**
 * @file
 * @author  Suriyan Laohaprapanon <suriyant@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2010 Suriyan Laohaprapanon
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
 * A nested deep history state machine. A nested deep history demo of the 
 * simplehsm C implementation.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simplehsm.h"
#include "nested_deephist.h"

#include <stdio.h>

/**
 * The state machine object
 */
simplehsm_t hsm = {NULL};

void deephist_signal(int signal, void* param);

/**
 * State function definitions
 */
stnext top(int signal, void* param);
    stnext A(int signal, void* param);
        stnext C(int signal, void* param);
            stnext E(int signal, void* param);
        stnext D(int signal, void* param);
    stnext B(int signal, void* param);

/* State machine function */
stnext top(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_INIT:
        printf("%s: INIT to A\n", __FUNCTION__);
        simplehsm_init_transition_state(&hsm, A);
        return stnone;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    }
    return stnone;
}

stnext A(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_DEEPHIST:
        printf("%s: DEEPHIST\n", __FUNCTION__);
        return stdeephist;
    case SIG_INIT:
        printf("%s: INIT to C\n", __FUNCTION__);
        simplehsm_init_transition_state(&hsm, C);
        return stnone;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    case SIG_A2B:
        printf("%s: A to B\n", __FUNCTION__);
        simplehsm_transition_state(&hsm, B);
        return stnone;
    }
    return top;
}

stnext B(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_INIT:
        printf("%s: INIT\n", __FUNCTION__);
        return stnone;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    case SIG_B2A:
        printf("%s: B to A (H*)\n", __FUNCTION__);
        simplehsm_transition_state_ex(&hsm, A, TRUE);
        return stnone;
    }
    return top;
}

stnext C(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_INIT:
        printf("%s: INIT to E\n", __FUNCTION__);
        simplehsm_init_transition_state(&hsm, E);
        return stnone;
    case SIG_DEEPHIST:
        printf("%s: DEEPHIST\n", __FUNCTION__);
        return stdeephist;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    case SIG_C2D:
        printf("%s: C to D\n", __FUNCTION__);
        simplehsm_transition_state(&hsm, D);
        return stnone;
    }
    return A;
}

stnext D(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_INIT:
        printf("%s: INIT\n", __FUNCTION__);
        return stnone;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    case SIG_D2C:
        printf("%s: D to C (H*)\n", __FUNCTION__);
        simplehsm_transition_state_ex(&hsm, C, TRUE);
        return stnone;
    }
    return A;
}

stnext E(int signal, void* param)
{
    switch (signal)
    {
    case SIG_ENTRY:
        printf("%s: ENTRY\n", __FUNCTION__);
        return stnone;
    case SIG_INIT:
        printf("%s: INIT\n", __FUNCTION__);
        return stnone;
    case SIG_EXIT:
        printf("%s: EXIT\n", __FUNCTION__);
        return stnone;
    }
    return C;
}

/**
 * Show the status of the state machine by showing what states it is currently
 * in
 * 
 * @param hsm The state machine
 */
void show_status(simplehsm_t* hsm)
{
    printf("\nChecking States:\n");
    printf("  in top state: %d\n  in A state: %d\n  in B state: %d\n  in C state: %d\n  in D state: %d\n  in E state: %d\n\n",
        simplehsm_is_in_state(hsm, top),
        simplehsm_is_in_state(hsm, A),
        simplehsm_is_in_state(hsm, B),
        simplehsm_is_in_state(hsm, C),
        simplehsm_is_in_state(hsm, D),
        simplehsm_is_in_state(hsm, E));
}

/**
 * Main function of the program
 * 
 * Sets the initial state, sends a signal and then shows the state machine status
 */
int main(int argc, char* argv[])
{
    simplehsm_initialize(&hsm, top);
    show_status(&hsm);
    simplehsm_signal_current_state(&hsm, SIG_A2B, NULL);
    show_status(&hsm);
    simplehsm_signal_current_state(&hsm, SIG_B2A, NULL);
    show_status(&hsm);
    return 0;
}

