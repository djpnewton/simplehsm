#
# Generic State machine signals
#

SIG_NULL  = 0
SIG_INIT  = 1
SIG_ENTRY = 2
SIG_EXIT  = 3
SIG_USER  = 4

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
            child_state = child_state(SIG_NULL, None)
            if child_state == parent_state:
                return True
            if not child_state: break
        return False

    def TransitionState(self, new_state):
        # exit signal to current state
        if (self.__cur_state):
            self.__cur_state(SIG_EXIT, None)
            parent_state = self.__cur_state(SIG_NULL, None)
            while (not self.__IsParent(parent_state, new_state)):
                parent_state(SIG_EXIT, None)
                parent_state = parent_state(SIG_NULL, None)
        else:
            print "TransitionState: ERROR - current state is invalid!"
        self.InitTransitionState(new_state)

    def InitTransitionState(self, new_state):
        # set new state
        self.__cur_state = new_state
        # entry signal to current state
        self.SignalCurrentState(SIG_ENTRY, None)
        # init signal to current state
        self.SignalCurrentState(SIG_INIT, None)

    def SignalCurrentState(self, signal, param):
        if (self.__cur_state != None):
            parent_state = self.__cur_state
            while (True):
                parent_state = parent_state(signal, param)
                if (not parent_state): break;
        else:
            print "SignalCurrentState: ERROR - current state is invalid!\n"

    def IsInState(self, state):
        parent_state = self.__cur_state
        while (True):
            if (state == parent_state):
                return True
            parent_state = parent_state(SIG_NULL, None)
            if (not parent_state): break
        return False





