using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using SimpleHsm;

namespace Calc
{
    public partial class MainForm : Form
    {
        CalcHsm hsm; 

        public MainForm()
        {
            InitializeComponent();

            hsm = new CalcHsm(tbDisplay);
        }

        private void btnClick(object sender, EventArgs e)
        {
            if (sender == btn0)
                hsm.SignalCurrentState(new StateEvent((Signal)CalcSignal.Digit0));
            else if (sender == btn1)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '1'));
            else if (sender == btn2)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '2'));
            else if (sender == btn3)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '3'));
            else if (sender == btn4)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '4'));
            else if (sender == btn5)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '5'));
            else if (sender == btn6)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '6'));
            else if (sender == btn7)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '7'));
            else if (sender == btn8)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '8'));
            else if (sender == btn9)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Digit1_9, '9'));
            else if (sender == btnDot)
                hsm.SignalCurrentState(new StateEvent((Signal)CalcSignal.Point));
            else if (sender == btnPercent)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Operator, '%'));
            else if (sender == btnDivide)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Operator, '/'));
            else if (sender == btnMultiply)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Operator, '*'));
            else if (sender == btnSubtract)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Operator, '-'));
            else if (sender == btnAdd)
                hsm.SignalCurrentState(new CalcEvent(CalcSignal.Operator, '+'));
            else if (sender == btnClear)
                hsm.SignalCurrentState(new StateEvent((Signal)CalcSignal.Clear));
            else if (sender == btnClearEntry)
                hsm.SignalCurrentState(new StateEvent((Signal)CalcSignal.ClearEntry));
            else if (sender == btnEquals)
                hsm.SignalCurrentState(new StateEvent((Signal)CalcSignal.Equals));
        }
    }
}