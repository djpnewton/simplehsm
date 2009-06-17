##
 # @file
 # @author  Daniel Newton <djpnewton@gmail.com>
 # @version 1.0
 #
 # @section LICENSE
 #
 # Copyright (c) 2009 Daniel Newton
 # 
 # Permission is hereby granted, free of charge, to any person obtaining a copy
 # of this software and associated documentation files (the "Software"), to deal
 # in the Software without restriction, including without limitation the rights
 # to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 # copies of the Software, and to permit persons to whom the Software is
 # furnished to do so, subject to the following conditions:
 # 
 # The above copyright notice and this permission notice shall be included in
 # all copies or substantial portions of the Software.
 # 
 # THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 # IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 # FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 # AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 # LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 # OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 # THE SOFTWARE.
 #
 # @section DESCRIPTION
 #
 # The calc state machine
 #

from simplehsm import *
from Tkinter import *

#
# Calc State Machine Signals
#

## Clear signal, user has pressed the clear button
SIG_CLEAR = SIG_USER
## Clear Entry signal, user has pressed the clear entry button
SIG_CLEAR_ENTRY = SIG_USER + 1
## Digit 1-9 signal, user has pressed a digit button ranging from 1 to 9
SIG_DIGIT_1_9 = SIG_USER + 2
## Digit 0 signal, user has pressed the 0 button
SIG_DIGIT_0 = SIG_USER + 3
## Operator signal, user has pressed an operator (add, subtract, multiply, divide) button
SIG_OPERATOR = SIG_USER + 4
## Point signal, user has pressed the decimal point button
SIG_POINT = SIG_USER + 5
## Equals signal, user has pressed the equals button
SIG_EQUALS = SIG_USER + 6

#
# Calc State Machine event
#

## Calc state machine events
class CalcEvent(StateEvent):

    ## The calculator key pressed that caused the event
    key = None

    ## Calc state machine event constructor
    # @param sig The signal associated with this event
    # @param key The key pressed that caused the event
    def __init__(self, sig, key):
        StateEvent.__init__(self, sig)
        self.key = key;

#
# Calc State Machine
#

## Calc state machine
class CalcHsm(SimpleHsm):

    ## The Tkinter text entry widget    
    entry = None

    ## Calc state machine constructor
    # @param entry TheTkinter text entry widget that the calc state machine will use
    def __init__(self, entry):
        self.entry = entry
        self.InitialState(self.on)

    #
    # State variables
    #

    ## If this flag is set we clear the textbox and replace text with new character on the next signal
    clear_on_next = False
    ## If this flag is set the current operand will be negative
    negative = False
    ## The first operand
    _operand1 = None
    ## The current operator
    _operator = None

    #
    # Helper functions
    #

    ## Clear the textbox input and replace with a '0' string also reset the #negative variable
    def Zero(self):
        self.entry.delete(0, END)
        self.entry.insert(END, "0")
        self.negative = False

    ## Clear the textbox input and replace with a '-0' string also set the #negative variable
    def Negate(self):
        self.entry.delete(0, END)
        self.entry.insert(END, "-0")
        self.negative = True

    ## Append a character to the textbox input, if the #clear_on_next variable is set then 
    # clear the textbox first 
    #
    # @param c The character to insert
    def append(self, c):
        if self.clear_on_next:
            self.entry.delete(0, END)
            if self.negative:
                self.entry.insert(END, "-")
            self.clear_on_next = False
        self.entry.insert(END, c)

    ## Calculate the result of a function
    #
    # @param operand1 The first operand
    # @param operand2 The second operand
    # @param operator_ The operator (+, -, *, /) to use
    # @return The result of the function
    def calc(self, operand1, operand2, operator_):
        if (operator_ == '+'):
            return operand1 + operand2
        if (operator_ == '-'):
            return operand1 - operand2
        if (operator_ == '*'):
            return operand1 * operand2
        if (operator_ == '/'):
            return operand1 / operand2
        return 0

    #
    # Calc state functions
    #

    ##
    # The 'on' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return Always NULL as this is the top level state function
    #
    def on(self, state_event):
        if state_event.sig == SIG_INIT:
            self.InitTransitionState(self.ready)
            return None
        elif state_event.sig == SIG_CLEAR:
            self.TransitionState(self.on)
            return None
        return None

    ##
    # The 'ready' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def ready(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.clear_on_next = True
            self.negative = False
            return None
        elif state_event.sig == SIG_INIT:
            self.InitTransitionState(self.begin)
            return None
        elif state_event.sig == SIG_OPERATOR:
            self._operator = state_event.key
            self.TransitionState(self.opEntered)
            return None
        elif state_event.sig == SIG_DIGIT_0:
            self.append('0')
            self.clear_on_next = True
            self.TransitionState(self.zero1)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac1)
            return None
        return self.on

    ##
    # The 'result' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return ready() (the parent state)
    #
    def result(self, state_event):
        return self.ready

    def begin(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.Zero()
            self._operand1 = 0
            return None
        elif state_event.sig == SIG_OPERATOR:
            if state_event.key == '-':
                self.TransitionState(self.negated1)
                return None
        return self.ready

    ##
    # The 'negated1' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def negated1(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.Negate()
            return None
        elif state_event.sig == SIG_DIGIT_0:
            self.TransitionState(self.zero1)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac1)
            return None
        elif state_event.sig == SIG_CLEAR_ENTRY:
            self.TransitionState(self.begin)
            return None
        return self.on

    ##
    # The 'operand1' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def operand1(self, state_event):
        if state_event.sig == SIG_CLEAR_ENTRY:
            self.TransitionState(self.ready)
            return None
        elif state_event.sig == SIG_OPERATOR:
            self._operand1 = float(self.entry.get())
            self._operator = state_event.key
            self.TransitionState(self.opEntered)
            return None
        return self.on

    ##
    # The 'zero1' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand1() (the parent state)
    #
    def zero1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac1)
            return None
        return self.operand1

    ##
    # The 'int1' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand1() (the parent state)
    #
    def int1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.append('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac1)
            return None
        return self.operand1

    ##
    # The 'frac1' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand1() (the parent state)
    #
    def frac1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.append('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            return None
        return self.operand1

    ##
    # The 'error' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return on() (the parent state)
    #
    def error(self, state_event):
        return self.on

    ##
    # The 'opEntered' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def opEntered(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.clear_on_next = True
            self.negative = False
            return None
        elif state_event.sig == SIG_OPERATOR:
            if state_event.key == '-':
                self.TransitionState(self.negated2)
                return None
        elif state_event.sig == SIG_DIGIT_0:
            self.append('0')
            self.clear_on_next = True
            self.TransitionState(self.zero2)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac2)
            return None
        return self.on

    ##
    # The 'negated2' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def negated2(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.Negate()
            return None
        elif state_event.sig == SIG_DIGIT_0:
            self.TransitionState(self.zero2)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac2)
            return None
        elif state_event.sig == SIG_CLEAR_ENTRY:
            self.Zero()
            self.TransitionState(self.opEntered)
            return None
        return self.on

    ##
    # The 'operand2' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise on() (the parent state)
    #
    def operand2(self, state_event):
        if state_event.sig == SIG_CLEAR_ENTRY:
            self.Zero()
            self.TransitionState(self.opEntered)
            return None
        elif state_event.sig == SIG_OPERATOR:
            try:
                self._operand1 = self.calc(self._operand1, float(self.entry.get()), self._operator)
                self._operator = state_event.key
                self.entry.delete(0, END)
                val = str(self._operand1)
                if val[-2:] == ".0":
                    val = val[0:-2]
                self.entry.insert(END, val)
                self.TransitionState(self.opEntered)
            except:
                self.entry.delete(0, END)
                self.entry.insert(END, "ERROR")
                self.TransitionState(self.error)
            return None
        elif state_event.sig == SIG_EQUALS:
            try:
                self._operand1 = self.calc(self._operand1, float(self.entry.get()), self._operator)
                self.entry.delete(0, END)
                val = str(self._operand1)
                if val[-2:] == ".0":
                    val = val[0:-2]
                self.entry.insert(END, val)
                self.TransitionState(self.result)
            except:
                self.entry.delete(0, END)
                self.entry.insert(END, "ERROR")
                self.TransitionState(self.error)
            return None
        return self.on

    ##
    # The 'zero2' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand2() (the parent state)
    #
    def zero2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac2)
            return None
        return self.operand2

    ##
    # The 'int2' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand2() (the parent state)
    #
    def int2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.append('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            return None
        elif state_event.sig == SIG_POINT:
            self.append('.')
            self.TransitionState(self.frac2)
            return None
        return self.operand2

    ##
    # The 'frac2' state functon
    #
    # @param signal The signal to handle
    # @param param The accompaning parameter
    # @return NULL if the signal is handled otherwise operand2() (the parent state)
    #
    def frac2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.append('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.append(state_event.key)
            return None
        return self.operand2
