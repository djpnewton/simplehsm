using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using SimpleHsm;

namespace Calc
{
    //
    // Calc State Machine Signals
    //

    public enum CalcSignal
    {
        Clear = Signal.User,
        ClearEntry,
        Digit1_9,
        Digit0,
        Operator,
        Point,
        Equals
    }

    //
    // Calc State Machine event
    //

    public class CalcEvent : StateEvent
    {
        public Char Key;

        public CalcEvent(CalcSignal sig, Char key) : base((Signal)sig)
        {
            Key = key;
        }
    }

    //
    // Calc State Machine
    //

    class CalcHsm : SimpleHsm.SimpleHsm
    {
        TextBox textBox;

        public CalcHsm(TextBox tb)
        {
            textBox = tb;
            InitialState(on);
        }

        //
        // State variables
        //

        bool clearOnNext, negative;
        double _operand1;
        char _operator;

        //
        // Helper functions
        //

        void Zero()
        {
            textBox.Text = "0";
            negative = false;
        }

        void Negate()
        {
            textBox.Text = "-0";
            negative = true;
        }

        void Append(char c)
        {
            if (clearOnNext)
            {
                textBox.Text = "";
                if (negative)
                    textBox.Text = "-";
                clearOnNext = false;
            }
            textBox.Text += c;
        }

        double Calc(double operand1, double operand2, char operator_)
        {
            switch (operator_)
            {
                case '+':
                    return operand1 + operand2;
                case '-':
                    return operand1 - operand2;
                case '*':
                    return operand1 * operand2;
                case '/':
                    return operand1 / operand2;
            }
            return 0;
        }

        //
        // Calc state functions
        //

        StateDelegate on(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Init:
                    InitTransitionState(ready);
                    return null;
                case (Signal)CalcSignal.Clear:
                    TransitionState(on);
                    return null;
            }
            return null;
        }

        StateDelegate ready(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    clearOnNext = true;
                    negative = false;
                    return null;
                case Signal.Init:
                    InitTransitionState(begin);
                    return null;
                case (Signal)CalcSignal.Operator:
                    _operator = ((CalcEvent)e).Key;
                    TransitionState(opEntered);
                    return null;
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    clearOnNext = true;
                    TransitionState(zero1);
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int1);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac1);
                    return null;
            }
            return on;
        }

        StateDelegate result(StateEvent e)
        {
            return ready;
        }

        StateDelegate begin(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    Zero();
                    _operand1 = 0;
                    return null;
                case (Signal)CalcSignal.Operator:
                    if (((CalcEvent)e).Key == '-')
                    {
                        TransitionState(negated1);
                        return null;
                    }
                    break;
            }
            return ready;
        }

        StateDelegate negated1(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    Negate();
                    return null;
                case (Signal)CalcSignal.Digit0:
                    TransitionState(zero1);
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int1);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac1);
                    return null;
                case (Signal)CalcSignal.ClearEntry:
                    TransitionState(begin);
                    return null;
            }
            return on;
        }

        StateDelegate operand1(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.ClearEntry:
                    TransitionState(ready);
                    return null;
                case (Signal)CalcSignal.Operator:
                    _operand1 = Convert.ToDouble(textBox.Text);
                    _operator = ((CalcEvent)e).Key;
                    TransitionState(opEntered);
                    return null;
            }
            return on;
        }

        StateDelegate zero1(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int1);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac1);
                    return null;
            }
            return operand1;
        }

        StateDelegate int1(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac1);
                    return null;
            }
            return operand1;
        }

        StateDelegate frac1(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    return null;
            }
            return operand1;
        }

        StateDelegate error(StateEvent e)
        {
            return on;
        }

        StateDelegate opEntered(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    clearOnNext = true;
                    negative = false;
                    return null;
                case (Signal)CalcSignal.Operator:
                    if (((CalcEvent)e).Key == '-')
                    {
                        TransitionState(negated2);
                        return null;
                    }
                    break;
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    clearOnNext = true;
                    TransitionState(zero2);
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int2);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac2);
                    return null;
            }
            return on;
        }

        StateDelegate negated2(StateEvent e)
        {
            switch (e.Signal)
            {
                case Signal.Entry:
                    Negate();
                    return null;
                case (Signal)CalcSignal.Digit0:
                    TransitionState(zero2);
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int2);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac2);
                    return null;
                case (Signal)CalcSignal.ClearEntry:
                    Zero();
                    TransitionState(opEntered);
                    return null;
            }
            return on;
        }

        StateDelegate operand2(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.ClearEntry:
                    Zero();
                    TransitionState(opEntered);
                    return null;
                case (Signal)CalcSignal.Operator:
                    try
                    {
                        _operand1 = Calc(_operand1, Convert.ToDouble(textBox.Text), _operator);
                        _operator = ((CalcEvent)e).Key;
                        textBox.Text = _operand1.ToString();
                        TransitionState(opEntered);
                    }
                    catch
                    {
                        textBox.Text = "ERROR";
                        TransitionState(error);
                    }
                    return null;
                case (Signal)CalcSignal.Equals:
                    try
                    {
                        _operand1 = Calc(_operand1, Convert.ToDouble(textBox.Text), _operator);
                        textBox.Text = _operand1.ToString();
                        TransitionState(result);
                    }
                    catch
                    {
                        textBox.Text = "ERROR";
                        TransitionState(error);
                    }
                    return null;
            }
            return on;
        }

        StateDelegate zero2(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    TransitionState(int2);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac2);
                    return null;
            }
            return operand2;
        }

        StateDelegate int2(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    return null;
                case (Signal)CalcSignal.Point:
                    Append('.');
                    TransitionState(frac2);
                    return null;
            }
            return operand2;
        }

        StateDelegate frac2(StateEvent e)
        {
            switch (e.Signal)
            {
                case (Signal)CalcSignal.Digit0:
                    Append('0');
                    return null;
                case (Signal)CalcSignal.Digit1_9:
                    Append(((CalcEvent)e).Key);
                    return null;
            }
            return operand2;
        }
    }
}
