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
 * The nested deep history state machine header
 */

#ifndef NESTED_DEEPHIST_H_
#define NESTED_DEEPHIST_H_

#ifndef NULL
#define NULL    (0)
#endif

enum {
    SIG_A2B = SIG_USER,
    SIG_B2A,
    SIG_C2D,
    SIG_D2C
} deephist_signal_t;

#endif /* NESTED_DEEPHIST_H_ */
