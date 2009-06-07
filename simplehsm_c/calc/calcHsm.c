#include "calcHsm.h"

#include <stdlib.h>
#include <stdio.h>

//
// Calc state machine object
//

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

Ihandle* textbox = NULL;
BOOL clear_on_next, negative;
double _operand1;
char _operator;

//
// public functions
//

void calchsm_init(Ihandle* tb)
{
    textbox = tb;
    simplehsm_initial_state(&hsm, on);
}

void calchsm_signal(int signal, void* param)
{
    simplehsm_signal_current_state(&hsm, signal, param);
}

//
// Helper functions
//

void zero(void)
{
    IupSetAttribute(textbox, "VALUE", "0");
    negative = FALSE;
}

void negate(void)
{
    IupSetAttribute(textbox, "VALUE", "-0");
    negative = TRUE;
}

void insert(char c)
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

double update(double operand1, double operand2, char operator_)
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
            simplehsm_init_transition_state(&hsm, ready);
            return NULL;
        case SIG_CLEAR:
            simplehsm_transition_state(&hsm, on);
            return NULL;
    }
    return NULL;
}

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
            insert('0');
            clear_on_next = TRUE;
            simplehsm_transition_state(&hsm, zero1);
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac1);
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
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
        case SIG_CLEARENTRY:
            simplehsm_transition_state(&hsm, begin);
            return NULL;
    }
    return on;
}

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

stnext zero1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int1);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

stnext int1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac1);
            return NULL;
    }
    return operand1;
}

stnext frac1(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
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
            insert('0');
            clear_on_next = TRUE;
            simplehsm_transition_state(&hsm, zero2);
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return on;
}

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
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
        case SIG_CLEARENTRY:
            zero();
            simplehsm_transition_state(&hsm, opEntered);
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
            zero();
            simplehsm_transition_state(&hsm, opEntered);
            return NULL;
        case SIG_OPERATOR:
            _operand1 = update(_operand1, atof(IupGetAttribute(textbox, "VALUE")), _operator);
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
            // TODO - handle error like so
            //IupSetAttribute(textbox, "VALUE", "ERROR");
            //simplehsm_transition_state(&hsm, error);
            return NULL;
        case SIG_EQUALS:
            _operand1 = update(_operand1, atof(IupGetAttribute(textbox, "VALUE")), _operator);
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
            // TODO - handle error like so
            //IupSetAttribute(textbox, "VALUE", "ERROR");
            //simplehsm_transition_state(&hsm, error);
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
            insert(*((char*)param));
            simplehsm_transition_state(&hsm, int2);
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

stnext int2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            return NULL;
        case SIG_POINT:
            insert('.');
            simplehsm_transition_state(&hsm, frac2);
            return NULL;
    }
    return operand2;
}

stnext frac2(int signal, void* param)
{
    switch (signal)
    {
        case SIG_DIGIT0:
            insert('0');
            return NULL;
        case SIG_DIGIT1_9:
            insert(*((char*)param));
            return NULL;
    }
    return operand2;
}