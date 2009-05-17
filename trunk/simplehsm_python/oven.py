import simplehsm

#
# Oven State machine signals
#

SIG_OPEN_DOOR = simplehsm.SIG_USER,

#
# Oven State hierachy
#

#  oven
#    heating
#      toasting
#      baking
#    doorOpen

#
# Oven state machine
#

class Oven(simplehsm.SimpleHsm):

    def __init__(self):
        self.InitialState(self.oven);

    #
    # Oven State implementations
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
        return self.oven;

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

    def doorOpen(self, state_event):
        if (state_event.sig == simplehsm.SIG_ENTRY):
            print "  doorOpen: entering state"
            return None;
        elif (state_event.sig == simplehsm.SIG_INIT):
            return None;
        elif (state_event.sig == simplehsm.SIG_EXIT):
            print "  doorOpen: exiting state"
            return None;
        return self.oven

    def ShowStatus(self):
        print "\nChecking States:"
        print "  in oven state: %d\n  in heating state: %d\n  in toasting state: %d\n  in baking state: %d\n  in doorOpen state: %d\n\n" % (self.IsInState(self.oven), self.IsInState(self.heating), self.IsInState(self.toasting), self.IsInState(self.baking), self.IsInState(self.doorOpen))

def main():
    oven = Oven()
    oven.SignalCurrentState(simplehsm.StateEvent(SIG_OPEN_DOOR))
    oven.ShowStatus()

main()
