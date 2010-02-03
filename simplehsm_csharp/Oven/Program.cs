using System;
using System.Collections.Generic;
using System.Text;
using SimpleHsm;

namespace Oven
{
    //
    // Oven State machine signals
    //

    public enum OvenSignal
    {
      OpenDoor = Signal.User,
      CloseDoor,
      Toasting,
      Baking
    };

    //
    // Oven State Hierachy
    //

    // oven
      // heating
        // toasting
        // baking
      // doorOpen

    class Oven : SimpleHsm.SimpleHsm
    {
        public Oven()
        {
            Initialize(oven);
        }

        //
        // Oven State function implementations
        //

        StateDelegate oven(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    System.Console.WriteLine("oven: entering state");
                    return null;
                case Signal.Init:
                    InitTransitionState(heating);
                    return null;
                case Signal.Exit:
                    System.Console.WriteLine("oven: exiting state");
                    return null;
            }
            return null;
        }

        [DeepHistory]
        StateDelegate heating(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    System.Console.WriteLine("  heating: entering state");
                    return null;
                case Signal.Init:
                    InitTransitionState(toasting);
                    return null;
                case Signal.Exit:
                    System.Console.WriteLine("  heating: exiting state");
                    return null;
                case (Signal)OvenSignal.OpenDoor:
                    System.Console.WriteLine("  heating: OPEN_DOOR signal!");
                    TransitionState(doorOpen);
                    return null;
                case (Signal)OvenSignal.Toasting:
                    System.Console.WriteLine("  heating: TOASTING signal!");
                    TransitionState(toasting);
                    return null;
                case (Signal)OvenSignal.Baking:
                    System.Console.WriteLine("  heating: BAKING signal!");
                    TransitionState(baking);
                    return null;
            }
            return oven;
        }

        StateDelegate toasting(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    System.Console.WriteLine("    toasting: entering state");
                    return null;
                case Signal.Init:
                    return null;
                case Signal.Exit:
                    System.Console.WriteLine("    toasting: exiting state");
                    return null;
            }
            return heating;
        }

        StateDelegate baking(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    System.Console.WriteLine("    baking: entering state");
                    return null;
                case Signal.Init:
                    return null;
                case Signal.Exit:
                    System.Console.WriteLine("    baking: exiting state");
                    return null;
            }
            return heating;
        }

        StateDelegate doorOpen(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    System.Console.WriteLine("  doorOpen: entering state");
                    return null;
                case Signal.Init:
                    return null;
                case Signal.Exit:
                    System.Console.WriteLine("  doorOpen: exiting state");
                    return null;
                case (Signal)OvenSignal.CloseDoor:
                    System.Console.WriteLine("  doorOpen: CLOSE_DOOR signal!");
                    TransitionState(heating, true);
                    return null;
            }
            return oven;
        }

        public void ShowStatus()
        {
            System.Console.WriteLine("\nChecking States:");
            string s = string.Format("  in oven state: {0}\n  in heating state: {1}\n  in toasting state: {2}\n  in baking state: {3}\n  in doorOpen state: {4}\n\n",
                 IsInState(oven), IsInState(heating), IsInState(toasting), IsInState(baking), IsInState(doorOpen));
            System.Console.WriteLine(s);
        }
    }

    class Program
    {
        static void Main(string[] args)
        {
            Oven oven = new Oven();
            oven.SignalCurrentState(new StateEvent((Signal)OvenSignal.Baking));
            oven.SignalCurrentState(new StateEvent((Signal)OvenSignal.OpenDoor));
            oven.SignalCurrentState(new StateEvent((Signal)OvenSignal.CloseDoor));
            oven.ShowStatus();
        }
    }
}
