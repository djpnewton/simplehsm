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
# The simplehsm C implementation

## \mainpage
#
# \section intro_sec Introduction
#
# simplehsm is designed to be a very simple implementation of a hierarchical state machine.
#
# From http://www.eventhelix.com/RealtimeMantra/HierarchicalStateMachine.htm:
#
# \verbatim
#In conventional state machine design, all states are considered at the same level. 
#The design does not capture the commonality that exists among states. In real life, 
#many states handle most  messages in similar fashion and differ only in handling of 
#few key messages. Even when the actual handling differs, there is still some 
#commonality. Hierarchical state machine design captures the commonality by organizing 
#the states as a hierarchy. The states at the higher level in hierarchy perform the 
#common message handling, while the lower level states inherit the commonality from 
#higher level ones and perform the state specific functions.
#\endverbatim
# 
# @todo A nice helpful overview like: http://ehiti.de/machine_objects/


#
# The base state machine signals
#

## Null signal, all state functions should ignore this signal and return their parent state (or None if the top level state) 
SIG_NULL  = 0
## Initialisation signal, a state function should transition to a default substate (SimpleHsm::InitTransitionState()) if it has substates
SIG_INIT  = 1
## Entry signal, a state function should perform its entry actions (if any)
SIG_ENTRY = 2
## Exit signal, a state function should perform its exit actions (if any)
SIG_EXIT  = 3
## User signals should start from this index
SIG_USER  = 4

##
# Add a deep history psuedostate to a composite state
#
# @param state The state to recive a deep history child state
#
def deep_history(state):
    state.deep_hist = True
    return state


## StateEvent class
# 
# The state event carries the details about the event (what signal caused the event) and can be subclassed to provide extra parameters
class StateEvent:

    ## The signal associated with this event
    sig = None

    ## StateEvent constructor
    #
    # @param sig The signal associated with this event
    def __init__(self, sig):
        self.sig = sig

## SimpleHsm class
# 
# The basic state machine implementation. This class should be subclassed to create a state machine.
class SimpleHsm:

    ## Current state variable
    __cur_state = None

    ## The topmost state of the state machine hierachy
    __top_state = None

    ## The table of current deep history psuedostates 
    __deep_hist_table = []

    #
    # State utility function implementations
    #

    ##
    # Initialise a simplehsm state machine.
    # 
    # @param top_state The topmost (and initial) state of the hierachy
    # 
    def Initialize(self, top_state):
        # init top state
        self.__top_state = top_state
        # init deep history table
        self.__deep_hist_table = []
        # transition to initial top state
        self.InitTransitionState(top_state)

    ##
    # Check is one state is the parent of another.
    # 
    # @param parent_state The parent state to check
    # @param child_state The child state to check
    # @return True if the parent_state parameter is a parent to the child_state parameter
    # 
    def __IsParent(self, parent_state, child_state):
        while True:
            child_state = child_state(StateEvent(SIG_NULL))
            if child_state == parent_state:
                return True
            if not child_state: break
        return False

    ##
    # Check if a state has the deep history attribute
    #
    # 
    # @param state The state to check
    # @return True if the method refered to by the delegate has the DeepHistory attribute
    #
    def __HasDeepHistAttrib(self, state):
        return state.__dict__.has_key("deep_hist")

    ##
    # Initiate a transition to a new state.
    # This function will:
    #   - replace the target state with the deep history target if required
    #   - store a deep history target state if exiting a composite state with a deep history child
    #   - perform the exit/entry chain from the current state to the target state
    # 
    # @param new_state The state to transition to
    # @param to_deep_hist Whether to transition to the states internal deep history psuedostate
    # 
    def TransitionState(self, new_state, to_deep_hist = False):

        # retrive historical state if appropriate
        if to_deep_hist:
            new_state = self.RetrieveDeephist(new_state)
            if new_state == None:
                print "TransitionState: ERROR - RetrieveDeephist failed!"
                return

        # exit signal to current state
        if (self.__cur_state):
            # record the deep history state if appropriate
            parent_state = self.__cur_state(StateEvent(SIG_NULL))
            while parent_state != None:
                # the top state cannot have a history sub state and returns stnone to all 
                # unhandled signals anyhow
                if parent_state == self.__top_state:
                    break
                # if the parentState has the DeepHist attribute and is not a parent of the new 
                # state then record the deep history
                elif (self.__HasDeepHistAttrib(parent_state) and
                      (not self.__IsParent(parent_state, new_state))):
                    self.RecordDeephist(parent_state, self.__cur_state)
                    break
                parent_state = parent_state(StateEvent(SIG_NULL))
            # state exit chain 
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

    ##
    # Initiate an initial transition to a new state (this function should only be used from a #SIG_INIT state event).
    # 
    # @param new_state The state to transition to
    # 
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

    ##
    # Send a signal to the state machine.
    # 
    # @param state_event The event to send to the state machine
    # 
    def SignalCurrentState(self, state_event):
        if (self.__cur_state != None):
            parent_state = self.__cur_state
            while (True):
                parent_state = parent_state(state_event)
                if (not parent_state): break;
        else:
            print "SignalCurrentState: ERROR - current state is invalid!\n"

    ##
    # Check if a state machine is currently in a certain state.
    # 
    # @param state The state to check
    # @return True if the state machine is currently in the specified state
    # 
    def IsInState(self, state):
        parent_state = self.__cur_state
        while (True):
            if (state == parent_state):
                return True
            parent_state = parent_state(StateEvent(SIG_NULL))
            if (not parent_state): break
        return False


    ##
    # Record deep history psuedostate.
    # 
    # @param history_parent The parent state of the deep history psuedostate
    # @param history_state The state to return to if transitioning to the deep history psuedostate
    # 
    #
    def RecordDeephist(self, history_parent, history_state):
        for i in range(len(self.__deep_hist_table)):
            if self.__deep_hist_table[i][0] == history_parent:
                self.__deep_hist_table[i] = (history_parent, history_state)
                return
        self.__deep_hist_table.append((history_parent, history_state))

    ##
    # Retrive deep history psuedostate.
    # 
    # @param history_parent The parent state of the deep history psuedostate
    # @return The state to transition to via the deep history psuedostate
    # 
    #
    def RetrieveDeephist(self, history_parent):
        for i in range(len(self.__deep_hist_table)):
            if self.__deep_hist_table[i][0] == history_parent:
                res = self.__deep_hist_table[i][1]
                # remove history state from deep history table
                self.__deep_hist_table.remove(self.__deep_hist_table[i])
                # return deep history target
                return res
        return None
