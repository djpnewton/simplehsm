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
 * The calc state machine header
 */

//---------------------------------------------------------------------------
#ifndef calcHsmH
#define calcHsmH
//---------------------------------------------------------------------------

#include <iup.h>

#include "simplehsm.h"

/** \enum calc_signals_t
 * Calc State Machine Signals
 */
enum
{
    SIG_CLEAR = SIG_USER, /**< Clear signal, user has pressed the clear button */
    SIG_CLEARENTRY,       /**< Clear Entry signal, user has pressed the clear entry button */
    SIG_DIGIT1_9,         /**< Digit 1-9 signal, user has pressed a digit button ranging from 1 to 9 */
    SIG_DIGIT0,           /**< Digit 0 signal, user has pressed the 0 button */
    SIG_OPERATOR,         /**< Operator signal, user has pressed an operator (add, subtract, multiply, divide) button */
    SIG_POINT,            /**< Point signal, user has pressed the decimal point button */
    SIG_EQUALS            /**< Equals signal, user has pressed the equals button */
} calc_signals_t;


//
// Calc state machine public definitions
//

void calchsm_init(Ihandle* tb);
void calchsm_signal(int signal, void* param);


//---------------------------------------------------------------------------
#endif  //calcHsmH