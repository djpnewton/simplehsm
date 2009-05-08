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
# Oven State function implementations
#

def oven(signal, param):
    if (signal == simplehsm.SIG_ENTRY):
        print "oven: entering state"
        return None
    elif (signal == simplehsm.SIG_INIT):
        simplehsm.InitTransitionState(heating)
        return None
    elif (signal == simplehsm.SIG_EXIT):
        print "oven: exiting state"
        return None
    return None

def heating(signal, param):
    global SIG_OPEN_DOOR
    if (signal == simplehsm.SIG_ENTRY):
        print "  heating: entering state"
        return None
    elif (signal == simplehsm.SIG_INIT):
        simplehsm.InitTransitionState(toasting);
        return None
    elif (signal == simplehsm.SIG_EXIT):
        print "  heating: exiting state"
        return None
    elif (signal == SIG_OPEN_DOOR):
        print "  heating: OPEN_DOOR signal!"
        simplehsm.TransitionState(doorOpen)
        return None;
    return oven;

def toasting(signal, param):
    if (signal == simplehsm.SIG_ENTRY):
        print "    toasting: entering state"
        return None
    elif (signal == simplehsm.SIG_INIT):
        return None
    elif (signal == simplehsm.SIG_EXIT):
        print "    toasting: exiting state"
        return None
    return heating

def baking(signal, param):
    if (signal == simplehsm.SIG_ENTRY):
        print "    baking: entering state"
        return None
    elif (signal == simplehsm.SIG_INIT):
        return None
    elif (signal == simplehsm.SIG_EXIT):
        print "    baking: exiting state"
        return None
    return heating

def doorOpen(signal, param):
    if (signal == simplehsm.SIG_ENTRY):
        print "  doorOpen: entering state"
        return None;
    elif (signal == simplehsm.SIG_INIT):
        return None;
    elif (signal == simplehsm.SIG_EXIT):
        print "  doorOpen: exiting state"
        return None;
    return oven

def ShowStatus():
    print "\nChecking States:"
    print "  in oven state: %d\n  in heating state: %d\n  in toasting state: %d\n  in baking state: %d\n  in doorOpen state: %d\n\n" % (simplehsm.IsInState(oven), simplehsm.IsInState(heating), simplehsm.IsInState(toasting), simplehsm.IsInState(baking), simplehsm.IsInState(doorOpen))

def main():
    simplehsm.InitialState(oven);
    simplehsm.SignalCurrentState(SIG_OPEN_DOOR, None)
    ShowStatus()

main()
