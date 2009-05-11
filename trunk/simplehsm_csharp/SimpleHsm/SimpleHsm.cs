using System;
using System.Collections.Generic;
using System.Text;

namespace SimpleHsm
{
    //
    // Generic State machine signals
    //

    public enum Signal
    {
        Null  = 0,
        Init  = 1,
        Entry = 2,
        Exit  = 3,
        User  = 4,
    };

    //
    // State machine definitions
    //

    public class StateEvent
    {
        public Signal Signal;

        public StateEvent(Signal sig)
        {
            Signal = sig;
        }
    }

    public delegate StateDelegate StateDelegate(StateEvent e);

    public class SimpleHsm
    {
        //
        // Current state variable
        //

        StateDelegate curState = null;

        //
        // State utility function implementations
        //

        protected void InitialState(StateDelegate newState)
        {
            InitTransitionState(newState);
        }

        protected Boolean IsParent(StateDelegate parentState, StateDelegate childState)
        {
            do
            {
                childState = childState(new StateEvent(Signal.Null));
                if (childState == parentState)
                    return true;
            }
            while (childState != null);
            return false;
        }

        protected void TransitionState(StateDelegate newState)
        {
            // exit signal to current state
            if (curState != null)
            {
                StateDelegate parentState;
                curState(new StateEvent(Signal.Exit));
                parentState = curState(new StateEvent(Signal.Null));
                while (!IsParent(parentState, newState))
                {
                    parentState(new StateEvent(Signal.Exit));
                    parentState = parentState(new StateEvent(Signal.Null));
                }
                // set current state to parent state
                curState = parentState;
            }
            else
                System.Console.WriteLine("TransitionState: ERROR - current state is invalid!");

            // entry signal to new state
            while (curState != newState)
            {
                StateDelegate parentState = newState;
                StateDelegate lastChild = newState;
                while (true)
                {
                    parentState = parentState(new StateEvent(Signal.Null));
                    if (parentState == curState)
                    {
                        lastChild(new StateEvent(Signal.Entry));
                        // set current state to last child state
                        curState = lastChild;
                        break;
                    }
                    lastChild = parentState;
                }
            }

            // init signal to new state
            newState(new StateEvent(Signal.Init));
        }

        protected void InitTransitionState(StateDelegate newState)
        {
            // set new state
            curState = newState;
            if (curState != null)
            {
                // entry signal to current state
                curState(new StateEvent(Signal.Entry));
                // init signal to current state
                curState(new StateEvent(Signal.Init));
            }
            else
                System.Console.WriteLine("InitTransitionState: ERROR - current state is invalid!\n");
        }

        public void SignalCurrentState(StateEvent e)
        {
            if (curState != null)
            {
                StateDelegate parentState = curState;
                do
                    parentState = parentState(e);
                while (parentState != null);
            }
            else
                System.Console.WriteLine("SignalCurrentState: ERROR - current state is invalid!\n");
        }

        protected Boolean IsInState(StateDelegate state)
        {
            StateDelegate parentState = curState;
            do
            {
                if (state == parentState)
                    return true;
                parentState = parentState(new StateEvent(Signal.Null));
            }
            while (parentState != null);
            return false;
        }
        
    }
}
