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
# A contrived oven state machine. A simple demo of the simplehsm C implementation.

import simplehsm

#
# Oven State machine signals
#

## The oven door has been opened
SIG_OPEN_DOOR = simplehsm.SIG_USER
SIG_CLOSE_DOOR = simplehsm.SIG_USER + 1
SIG_TOASTING = simplehsm.SIG_USER + 2
SIG_BAKING = simplehsm.SIG_USER + 3

#
# Oven State hierachy
#

#  oven
#    heating
#      toasting
#      baking
#    doorOpen

##
# Oven state machine
#
class Oven(simplehsm.SimpleHsm):

    ## Oven state machine constructor
    #
    # Sets the initial state
    def __init__(self):
        self.Initialize(self.oven);

    #
    # Oven State implementations
    #

    ##
    # The top level oven state
    # 
    # @param state_event The signal to send to this state
    # @return Aways returns None indicating that this is the top level state
    #
    def oven(self, state_event):
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "oven: entering state"
            return None
        elif (state_event.sig == simplehsm.SIG_INIT):
            self.InitTransitionState(self.heating)
            return None
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "oven: exiting state"
            return None
        return None

    ##
    # The heating state
    # 
    # @param state_event The signal to send to this state
    # @return None if the signal is handled, otherwise the parent state (oven())
    #
    @simplehsm.deep_history
    def heating(self, state_event):
        global SIG_OPEN_DOOR
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "  heating: entering state"
            return None
        elif (state_event.sig == simplehsm.SIG_INIT):
            self.InitTransitionState(self.toasting);
            return None
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "  heating: exiting state"
            return None
        elif (state_event.sig == SIG_OPEN_DOOR):
            print "  heating: OPEN_DOOR signal!"
            self.TransitionState(self.doorOpen)
            return None;
        elif (state_event.sig == SIG_TOASTING):
            print "  heating: TOASTING signal!"
            self.TransitionState(self.toasting)
            return None;
        elif (state_event.sig == SIG_BAKING):
            print "  heating: BAKING signal!"
            self.TransitionState(self.baking)
            return None;
        return self.oven;

    ##
    # The toasting state
    # 
    # @param state_event The signal to send to this state
    # @return None if the signal is handled, otherwise the parent state (heating())
    #
    def toasting(self, state_event):
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "    toasting: entering state"
            return None
        elif (state_event.sig == simplehsm.SIG_INIT):
            return None
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "    toasting: exiting state"
            return None
        return self.heating

    ##
    # The baking state
    # 
    # @param state_event The signal to send to this state
    # @return None if the signal is handled, otherwise the parent state (heating())
    #
    def baking(self, state_event):
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "    baking: entering state"
            return None
        elif (state_event.sig == simplehsm.SIG_INIT):
            return None
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "    baking: exiting state"
            return None
        return self.heating

    ##
    # The doorOpen state
    # 
    # @param state_event The signal to send to this state
    # @return None if the signal is handled, otherwise the parent state (oven())
    #
    def doorOpen(self, state_event):
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "  doorOpen: entering state"
            return None;
        elif (state_event.sig == simplehsm.SIG_INIT):
            return None;
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "  doorOpen: exiting state"
            return None;
        elif (state_event.sig == SIG_CLOSE_DOOR):
            print "  doorOpen: CLOSE_DOOR signal!"
            self.TransitionState(self.heating, to_deep_hist = True)
        return self.oven

    ##
    # Show the status of the oven state machine by showing what states it is currently in
    # 
    def ShowStatus(self):
        print "\nChecking States:"
        print "  in oven state: %d\n  in heating state: %d\n  in toasting state: %d\n  in baking state: %d\n  in doorOpen state: %d\n\n" % (self.IsInState(self.oven), self.IsInState(self.heating), self.IsInState(self.toasting), self.IsInState(self.baking), self.IsInState(self.doorOpen))

##
# Main function of the oven program
# 
# Creates the oven state machine, sends a signal and then shows the state machine status
#
def main():
    oven = Oven()
    oven.SignalCurrentState(simplehsm.StateEvent(SIG_BAKING))
    oven.SignalCurrentState(simplehsm.StateEvent(SIG_OPEN_DOOR))
    oven.SignalCurrentState(simplehsm.StateEvent(SIG_CLOSE_DOOR))
    oven.ShowStatus()

main()
