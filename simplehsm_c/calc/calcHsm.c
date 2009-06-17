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
 * The calc state machine
 */

#include "calcHsm.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * The Calc state machine object
 */
simplehsm_t hsm = {NULL};

//
// Calc State function definitions
//

stnext on(int signal, void* param);
  stnext ready(int signal, void* param);
    stnext result(int signal, void* param);
    stnext begin(int signal, void* param);
  stnext negated1(int signal, void* param);
  stnext operand1(int signal, void* param);
    stnext zero1(int signal, void* param);
    stnext int1(int signal, void* param);
    stnext frac1(int signal, void* param);
  stnext error(int signal, void* param);
  stnext opEntered(int signal, void* param);
  stnext negated2(int signal, void* param);
  stnext operand2(int signal, void* param);
    stnext zero2(int signal, void* param);
    stnext int2(int signal, void* param);
    stnext frac2(int signal, void* param);

//
// State variables
//

/**
 * The IUP textbox widget
 */
Ihandle* textbox = NULL;
/**
 * If this flag is set we clear the textbox and replace text with new character on the next signal
 */
BOOL clear_on_next;
/**
 * If this flag is set the current operand will be negative
 */
BOOL negative;
/**
 * The first operand
 */
double _operand1;
/**
 * The current operator
 */
char _operator;

//
// public functions
//

/**
 * Initialize the calc state machine
 *
 * @param tb The IUP textbox widget that the calc state machine will use
 */
void calchsm_init(Ihandle* tb)
{
    textbox = tb;
    simplehsm_initial_state(&hsm, on);
}

/**
 * Send a signal to the calc state machine
 *
 * @param signal The signal to send
 * @param param An optional parameter
 */
void calchsm_signal(int signal, void* param)
{
    simplehsm_signal_current_state(&hsm, signal, param);
}

//
// Helper functions
//

/**
 * Clear the textbox input and replace with a '0' string also reset the #negative variable
 */
void zero(void)
{
    IupSetAttribute(textbox, "VALUE", "0");
    negative = FALSE;
}

/**
 * Clear the textbox input and replace with a '-0' string also set the #negative variable
 */
void negate(void)
{
    IupSetAttribute(textbox, "VALUE", "-0");
    negative = TRUE;
}

/**
 * Append a character to the textbox input, if the #clear_on_next variable is set then 
 * clear the textbox first 
 *
 * @param c The character to insert
 */
void append(char c)
{
    if (clear_on_next)
    {
        IupSetAttribute(textbox, "VALUE", "");
        if (negative)
            IupSetAttribute(textbox, "VALUE", "-");
        clear_on_next = FALSE;
    }
    IupSetAttribute(textbox, "APPEND", &c);
}

/**
 * Calculate the result of a function
 *
 * @param operand1 The first operand
 * @param operand2 The second operand
 * @param operator_ The operator (+, -, *, /) to use
 * @return The result of the function
 */
double calc(double operand1, double operand2, char operator_)
{
    switch (operator_)
    {
        case '+':
            return operand1 + operand2;
        case '-':
            return operand1 - operand2;
        case '*':
            return operand1 * operand2;
        case '/':
            return operand1 / operand2;
    }
    return 0;
}

//
// Calc state functions
//

/**
 * The 'on' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return Always NULL as this is the top level state function
 */
stnext on(int signal, void* param)
{
    switch (signal)
    {
        case SIG_INIT:
            simplehsm_init_transition_state(&hsm, ready);
            return NULL;
        case SIG_CLEAR:
            simplehsm_transition_state(&hsm, on);
            return NULL;
    }
    return NULL;
}

/**
 * The 'ready' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext ready(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            clear_on_next = TRUE;
            negative = FALSE;
            return NULL;
        case SIG_INIT:
            simplehsm_init_transition_state(&hsm, begin);
            return NULL;
        case SIG_OPERATOR:
            _operator = *((char*)param);
            simplehsm_transition_state(&hsm, opEntered);
            return NULL;
        case SIG_DIGIT0:
            append('0');
            clear_on_next = TRUE;
            simplehsm_transition_state(&hsm, zero1);
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
    }
    return on;
}

/**
 * The 'result' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return ready() (the parent state)
 */
stnext result(int signal, void* param)
{
    return ready;
}

/**
 * The 'begin' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise ready() (the parent state)
 */
stnext begin(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            zero();
            _operand1 = 0;
            return NULL;
        case SIG_OPERATOR:
            if (*((char*)param) == '-')
            {
                simplehsm_transition_state(&hsm, negated1);
                return NULL;
            }
            break;
      }
      return ready;
}

/**
 * The 'negated1' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext negated1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            negate();
            return NULL;
        case SIG_DIGIT0:
            simplehsm_transition_state(&hsm, zero1);
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
        case SIG_CLEARENTRY:
            simplehsm_transition_state(&hsm, begin);
            return NULL;
    }
    return on;
}

/**
 * The 'operand1' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext operand1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_CLEARENTRY:
            simplehsm_transition_state(&hsm, ready);
            return NULL;
        case SIG_OPERATOR:
            _operand1 = atof(IupGetAttribute(textbox, "VALUE"));
            _operator = *((char*)param);
            simplehsm_transition_state(&hsm, opEntered);
            return NULL;
    }
    return on;
}

/**
 * The 'zero1' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand1() (the parent state)
 */
stnext zero1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

/**
 * The 'int1' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand1() (the parent state)
 */
stnext int1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            append('0');
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

/**
 * The 'frac1' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand1() (the parent state)
 */
stnext frac1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            append('0');
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            return NULL;
    }
    return operand1;
}

/**
 * The 'error' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return on() (the parent state)
 */
stnext error(int signal, void* param)
{
    return on;
}

/**
 * The 'opEntered' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext opEntered(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            clear_on_next = TRUE;
            negative = FALSE;
            return NULL;
        case SIG_OPERATOR:
            if (*((char*)param) == '-')
            {
                simplehsm_transition_state(&hsm, negated2);
                return NULL;
            }
            break;
        case SIG_DIGIT0:
            append('0');
            clear_on_next = TRUE;
            simplehsm_transition_state(&hsm, zero2);
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return on;
}

/**
 * The 'negated2' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext negated2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            negate();
            return NULL;
        case SIG_DIGIT0:
            simplehsm_transition_state(&hsm, zero2);
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
        case SIG_CLEARENTRY:
            zero();
            simplehsm_transition_state(&hsm, opEntered);
            return NULL;
    }
    return on;
}

/**
 * The 'operand2' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise on() (the parent state)
 */
stnext operand2(int signal, void* param)
{
    char dblstr[_CVTBUFSIZE];
    switch (signal)
    {
        case SIG_CLEARENTRY:
            zero();
            simplehsm_transition_state(&hsm, opEntered);
            return NULL;
        case SIG_OPERATOR:
            _operand1 = calc(_operand1, atof(IupGetAttribute(textbox, "VALUE")), _operator);
            _operator = *((char*)param);
            if (_operand1 == (double)((int)_operand1))
            {
                _itoa_s((int)_operand1, dblstr, _CVTBUFSIZE, 10);
                IupSetAttribute(textbox, "VALUE", dblstr);
            }
            else
            {
                sprintf_s(dblstr, _CVTBUFSIZE, "%f", _operand1);
                IupSetAttribute(textbox, "VALUE", dblstr);
            }
            simplehsm_transition_state(&hsm, opEntered);
            /** @todo handle error like so:
             * \code
             * IupSetAttribute(textbox, "VALUE", "ERROR");
             * simplehsm_transition_state(&hsm, error);
             * \endcode
             */
            return NULL;
        case SIG_EQUALS:
            _operand1 = calc(_operand1, atof(IupGetAttribute(textbox, "VALUE")), _operator);
            if (_operand1 == (double)((int)_operand1))
            {
                _itoa_s((int)_operand1, dblstr, _CVTBUFSIZE, 10);
                IupSetAttribute(textbox, "VALUE", dblstr);
            }
            else
            {
                sprintf_s(dblstr, _CVTBUFSIZE, "%f", _operand1);
                IupSetAttribute(textbox, "VALUE", dblstr);
            }
            simplehsm_transition_state(&hsm, result);
            /** @todo handle error like so:
             * \code
             * IupSetAttribute(textbox, "VALUE", "ERROR");
             * simplehsm_transition_state(&hsm, error);
             * \endcode
             */
            return NULL;
    }
    return on;
}

/**
 * The 'zero2' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand2() (the parent state)
 */
stnext zero2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

/**
 * The 'int2' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand2() (the parent state)
 */
stnext int2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            append('0');
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            return NULL;
        case SIG_POINT:
            append('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

/**
 * The 'frac2' state functon
 *
 * @param signal The signal to handle
 * @param param The accompaning parameter
 * @return NULL if the signal is handled otherwise operand2() (the parent state)
 */
stnext frac2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            append('0');
            return NULL;
        case SIG_DIGIT1_9:
            append(*((char*)param));
            return NULL;
    }
    return operand2;
}