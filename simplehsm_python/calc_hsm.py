from simplehsm import *
from Tkinter import *

#
# Calc State Machine Signals
#

SIG_CLEAR = SIG_USER
SIG_CLEAR_ENTRY = SIG_USER + 1
SIG_DIGIT_1_9 = SIG_USER + 2
SIG_DIGIT_0 = SIG_USER + 3
SIG_OPERATOR = SIG_USER + 4
SIG_POINT = SIG_USER + 5
SIG_EQUALS = SIG_USER + 6

#
# Calc State Machine event
#

class CalcEvent(StateEvent):

    key = None

    def __init__(self, sig, key):
        StateEvent.__init__(self, sig)
        self.key = key;

#
# Calc State Machine
#

class CalcHsm(SimpleHsm):

    entry = None

    def __init__(self, entry):
        self.entry = entry
        self.InitialState(self.on)

    #
    # State variables
    #

    clearOnNext, negative = (False, False)
    _operand1 = None
    _operator = None

    #
    # Helper functions
    #

    def Zero(self):
        self.entry.delete(0, END)
        self.entry.insert(END, "0")
        self.negative = False

    def Negate(self):
        self.entry.delete(0, END)
        self.entry.insert(END, "-0")
        self.negative = True

    def Insert(self, c):
        if self.clearOnNext:
            self.entry.delete(0, END)
            if self.negative:
                self.entry.insert(END, "-")
            self.clearOnNext = False
        self.entry.insert(END, c)

    def Update(self, operand1, operand2, operator_):
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

    def on(self, state_event):
        if state_event.sig == SIG_INIT:
            self.InitTransitionState(self.ready)
            return None
        elif state_event.sig == SIG_CLEAR:
            self.TransitionState(self.on)
            return None
        return None

    def ready(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.clearOnNext = True
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
            self.Insert('0')
            self.clearOnNext = True
            self.TransitionState(self.zero1)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac1)
            return None
        return self.on

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

    def negated1(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.Negate()
            return None
        elif state_event.sig == SIG_DIGIT_0:
            self.TransitionState(self.zero1)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac1)
            return None
        elif state_event.sig == SIG_CLEAR_ENTRY:
            self.TransitionState(self.begin)
            return None
        return self.on

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

    def zero1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int1)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac1)
            return None
        return self.operand1

    def int1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.Insert('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac1)
            return None
        return self.operand1

    def frac1(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.Insert('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            return None
        return self.operand1

    def error(self, state_event):
        return self.on

    def opEntered(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.clearOnNext = True
            self.negative = False
            return None
        elif state_event.sig == SIG_OPERATOR:
            if state_event.key == '-':
                self.TransitionState(self.negated2)
                return None
        elif state_event.sig == SIG_DIGIT_0:
            self.Insert('0')
            self.clearOnNext = True
            self.TransitionState(self.zero2)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac2)
            return None
        return self.on

    def negated2(self, state_event):
        if state_event.sig == SIG_ENTRY:
            self.Negate()
            return None
        elif state_event.sig == SIG_DIGIT_0:
            self.TransitionState(self.zero2)
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac2)
            return None
        elif state_event.sig == SIG_CLEAR_ENTRY:
            self.Zero()
            self.TransitionState(self.opEntered)
            return None
        return self.on

    def operand2(self, state_event):
        if state_event.sig == SIG_CLEAR_ENTRY:
            self.Zero()
            self.TransitionState(self.opEntered)
            return None
        elif state_event.sig == SIG_OPERATOR:
            try:
                self._operand1 = self.Update(self._operand1, float(self.entry.get()), self._operator)
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
                self._operand1 = self.Update(self._operand1, float(self.entry.get()), self._operator)
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

    def zero2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            self.TransitionState(self.int2)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac2)
            return None
        return self.operand2

    def int2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.Insert('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            return None
        elif state_event.sig == SIG_POINT:
            self.Insert('.')
            self.TransitionState(self.frac2)
            return None
        return self.operand2

    def frac2(self, state_event):
        if state_event.sig == SIG_DIGIT_0:
            self.Insert('0')
            return None
        elif state_event.sig == SIG_DIGIT_1_9:
            self.Insert(state_event.key)
            return None
        return self.operand2
