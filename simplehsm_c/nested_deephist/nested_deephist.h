/*
 * deephist.h
 *
 *  Created on: March 8, 2010
 *      Author: suriyan
 */

#ifndef DEEPHIST_H_
#define DEEPHIST_H_

#ifndef NULL
#define NULL    (0)
#endif

enum {
    SIG_A2B = SIG_USER,
    SIG_B2A,
    SIG_C2D,
    SIG_D2C
} deephist_signal_t;

#endif /* DEEPHIST_H_ */
