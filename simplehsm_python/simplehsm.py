#
# Generic State machine signals
#

SIG_NULL  = 0
SIG_INIT  = 1
SIG_ENTRY = 2
SIG_EXIT  = 3
SIG_USER  = 4

#
# State Event class
#

class StateEvent:

    sig = None

    def __init__(self, sig):
        self.sig = sig

#
# SimpleHsm class
#

class SimpleHsm:

    #
    # Current state variable
    #

    __cur_state = None

    #
    # State utility function implementations
    #

    def InitialState(self, new_state):
        self.InitTransitionState(new_state)

    def __IsParent(self, parent_state, child_state):
        while True:
            child_state = child_state(StateEvent(SIG_NULL))
            if child_state == parent_state:
                return True
            if not child_state: break
        return False

    def TransitionState(self, new_state):
        # exit signal to current state
        if (self.__cur_state):
            self.__cur_state(StateEvent(SIG_EXIT))
            parent_state = self.__cur_state(StateEvent(SIG_NULL))
            while (not self.__IsParent(parent_state, new_state)):
                parent_state(StateEvent(SIG_EXIT))
                parent_state = parent_state(StateEvent(SIG_NULL))
            # set current state to parent state
            self.__cur_state = parent_state
        else:
            print "TransitionState: ERROR - current state is invalid!"

        # entry signal to new state
        while (self.__cur_state != new_state):
            parent_state = new_state
            last_child = new_state
            while (True):
                parent_state = parent_state(StateEvent(SIG_NULL))
                if (parent_state == self.__cur_state):
                    last_child(StateEvent(SIG_ENTRY))
                    # set current state to last child state
                    self.__cur_state = last_child
                    break;
                last_child = parent_state

        # init signal to new state
        new_state(StateEvent(SIG_INIT))

    def InitTransitionState(self, new_state):
        # set new state
        self.__cur_state = new_state
        if (self.__cur_state):
            # entry signal to current state
            self.__cur_state(StateEvent(SIG_ENTRY))
            # init signal to current state
            self.__cur_state(StateEvent(SIG_INIT))
        else:
            print "InitTransitionState: ERROR - current state is invalid!\n"

    def SignalCurrentState(self, state_event):
        if (self.__cur_state != None):
            parent_state = self.__cur_state
            while (True):
                parent_state = parent_state(state_event)
                if (not parent_state): break;
        else:
            print "SignalCurrentState: ERROR - current state is invalid!\n"

    def IsInState(self, state):
        parent_state = self.__cur_state
        while (True):
            if (state == parent_state):
                return True
            parent_state = parent_state(StateEvent(SIG_NULL))
            if (not parent_state): break
        return False





