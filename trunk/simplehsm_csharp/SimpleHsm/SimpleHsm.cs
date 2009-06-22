/**
 * @file
 * @author  Daniel Newton <djpnewton@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright (c) 2009 Daniel Newton
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * The simplehsm C# implementation
 */

/** \mainpage
 *
 * \section intro_sec Introduction
 *
 * simplehsm is designed to be a very simple implementation of a hierarchical state machine.
 *
 * From http://www.eventhelix.com/RealtimeMantra/HierarchicalStateMachine.htm:
 *
 * \verbatim
In conventional state machine design, all states are considered at the same level. 
The design does not capture the commonality that exists among states. In real life, 
many states handle most  messages in similar fashion and differ only in handling of 
few key messages. Even when the actual handling differs, there is still some 
commonality. Hierarchical state machine design captures the commonality by organizing 
the states as a hierarchy. The states at the higher level in hierarchy perform the 
common message handling, while the lower level states inherit the commonality from 
higher level ones and perform the state specific functions.
\endverbatim
 * 
 * @todo A nice helpful overview like: http://ehiti.de/machine_objects/
 */

using System;
using System.Collections.Generic;
using System.Text;

namespace SimpleHsm
{
    //
    // Generic State machine signals
    //

    /** \enum Signal
     * The base state machine signals
     */
    public enum Signal
    {
        Null  = 0, /**< Null signal, all state functions should ignore this signal and return their parent state (or null if the top level state) */
        Init  = 1, /**< Initialisation signal, a state function should transition to a default substate (SimpleHsm::SimpleHsm::InitTransitionState()) if it has substates */
        Entry = 2, /**< Entry signal, a state function should perform its entry actions (if any) */
        Exit  = 3, /**< Exit signal, a state function should perform its exit actions (if any) */
        User  = 4, /**< User signals should start from this index */
    };

    //
    // State machine definitions
    //

    /** 
     * StateEvent class
     * 
     * The state event carries the details about the event (what signal caused the event) and can be subclassed to provide extra parameters
     */
    public class StateEvent
    {
        /** 
         * The signal associated with this event
         */
        public Signal Signal;

        /**
         * StateEvent constructor
         * 
         * @param sig The signal associated with this event
         */
        public StateEvent(Signal sig)
        {
            Signal = sig;
        }
    }

    /**
     * A state function
     */
    public delegate StateDelegate StateDelegate(StateEvent e);

    /**
     * SimpleHsm class
     *
     * The basic state machine implementation. This class should be subclassed to create a state machine.
     */
    public class SimpleHsm
    {
        /** Current state variable */
        StateDelegate curState = null;

        //
        // State utility function implementations
        //

        /**
        * Initialise a simplehsm state machine.
        * 
        * @param newState The initial or starting state
        * 
        */
        protected void InitialState(StateDelegate newState)
        {
            InitTransitionState(newState);
        }

        /**
        * Check is one state is the parent of another.
        * 
        * @param parentState The parent state to check
        * @param childState The child state to check
        * @return True if the parentState parameter is a parent to the childState parameter
        * 
        */
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

        /**
        * Initiate a transition to a new state.
        * 
        * @param newState The state to transition to
        * 
        */
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

        /**
        * Initiate an initial transition to a new state (this function should only be used from a #SimpleHsm::Signal::Init state event).
        * 
        * @param newState The state to transition to
        * 
        */
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

        /**
        * Send a signal to the state machine.
        * 
        * @param e The event to send to the state machine
        * 
        */
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

        /**
        * Check if a state machine is currently in a certain state.
        * 
        * @param state The state to check
        * @return True if the state machine is currently in the specified state
        * 
        */
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
