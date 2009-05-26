#include "calcHsm.h"

#include <stdlib.h>
#include <stdio.h>

//
// Calc state machine object
//

SimpleHsm hsm = {NULL};

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

Ihandle* textBox = NULL;
BOOL clearOnNext, negative;
double _operand1;
char _operator;

//
// public functions
//

void InitCalcHsm(Ihandle* tb)
{
    textBox = tb;
    InitialState(&hsm, on);
}

void SignalCalcHsm(int signal, void* param)
{
    SignalCurrentState(&hsm, signal, param);
}

//
// Helper functions
//

void Zero(void)
{
    IupSetAttribute(textBox, "VALUE", "0");
    negative = FALSE;
}

void Negate(void)
{
    IupSetAttribute(textBox, "VALUE", "-0");
    negative = TRUE;
}

void Insert(char c)
{
    if (clearOnNext)
    {
        IupSetAttribute(textBox, "VALUE", "");
        if (negative)
            IupSetAttribute(textBox, "VALUE", "-");
        clearOnNext = FALSE;
    }
    IupSetAttribute(textBox, "APPEND", &c);
}

double Update(double operand1, double operand2, char operator_)
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

stnext on(int signal, void* param)
{
    switch (signal)
    {
        case SIG_INIT:
            InitTransitionState(&hsm, ready);
            return NULL;
        case SIG_CLEAR:
            TransitionState(&hsm, on);
            return NULL;
    }
    return NULL;
}

stnext ready(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            clearOnNext = TRUE;
            negative = FALSE;
            return NULL;
        case SIG_INIT:
            InitTransitionState(&hsm, begin);
            return NULL;
        case SIG_OPERATOR:
            _operator = *((char*)param);
            TransitionState(&hsm, opEntered);
            return NULL;
        case SIG_DIGIT0:
            Insert('0');
            clearOnNext = TRUE;
            TransitionState(&hsm, zero1);
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int1);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac1);
            return NULL;
    }
    return on;
}

stnext result(int signal, void* param)
{
    return ready;
}

stnext begin(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            Zero();
            _operand1 = 0;
            return NULL;
        case SIG_OPERATOR:
            if (*((char*)param) == '-')
            {
                TransitionState(&hsm, negated1);
                return NULL;
            }
            break;
      }
      return ready;
}

stnext negated1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            Negate();
            return NULL;
        case SIG_DIGIT0:
            TransitionState(&hsm, zero1);
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int1);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac1);
            return NULL;
        case SIG_CLEARENTRY:
            TransitionState(&hsm, begin);
            return NULL;
    }
    return on;
}

stnext operand1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_CLEARENTRY:
            TransitionState(&hsm, ready);
            return NULL;
        case SIG_OPERATOR:
            _operand1 = atof(IupGetAttribute(textBox, "VALUE"));
            _operator = *((char*)param);
            TransitionState(&hsm, opEntered);
            return NULL;
    }
    return on;
}

stnext zero1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int1);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

stnext int1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            Insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

stnext frac1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            Insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            return NULL;
    }
    return operand1;
}

stnext error(int signal, void* param)
{
    return on;
}

stnext opEntered(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            clearOnNext = TRUE;
            negative = FALSE;
            return NULL;
        case SIG_OPERATOR:
            if (*((char*)param) == '-')
            {
                TransitionState(&hsm, negated2);
                return NULL;
            }
            break;
        case SIG_DIGIT0:
            Insert('0');
            clearOnNext = TRUE;
            TransitionState(&hsm, zero2);
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int2);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac2);
            return NULL;
    }
    return on;
}

stnext negated2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_ENTRY:
            Negate();
            return NULL;
        case SIG_DIGIT0:
            TransitionState(&hsm, zero2);
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int2);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac2);
            return NULL;
        case SIG_CLEARENTRY:
            Zero();
            TransitionState(&hsm, opEntered);
            return NULL;
    }
    return on;
}

stnext operand2(int signal, void* param)
{
    char dblstr[_CVTBUFSIZE];
    switch (signal)
    {
        case SIG_CLEARENTRY:
            Zero();
            TransitionState(&hsm, opEntered);
            return NULL;
        case SIG_OPERATOR:
            _operand1 = Update(_operand1, atof(IupGetAttribute(textBox, "VALUE")), _operator);
            _operator = *((char*)param);
            if (_operand1 == (double)((int)_operand1))
                IupSetAttribute(textBox, "VALUE", itoa((int)_operand1, dblstr, 10));
            else
            {
                sprintf(dblstr, "%f", _operand1);
                IupSetAttribute(textBox, "VALUE", dblstr);
            }
            TransitionState(&hsm, opEntered);
            // TODO - handle error like so
            //IupSetAttribute(textBox, "VALUE", "ERROR");
            //TransitionState(&hsm, error);
            return NULL;
        case SIG_EQUALS:
            _operand1 = Update(_operand1, atof(IupGetAttribute(textBox, "VALUE")), _operator);
            if (_operand1 == (double)((int)_operand1))
                IupSetAttribute(textBox, "VALUE", itoa((int)_operand1, dblstr, 10));
            else
            {
                sprintf(dblstr, "%f", _operand1);
                IupSetAttribute(textBox, "VALUE", dblstr);
            }
            TransitionState(&hsm, result);
            // TODO - handle error like so
            //IupSetAttribute(textBox, "VALUE", "ERROR");
            //TransitionState(&hsm, error);
            return NULL;
    }
    return on;
}

stnext zero2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            TransitionState(&hsm, int2);
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

stnext int2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            Insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            return NULL;
        case SIG_POINT:
            Insert('.');
            TransitionState(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

stnext frac2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            Insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            Insert(*((char*)param));
            return NULL;
    }
    return operand2;
}