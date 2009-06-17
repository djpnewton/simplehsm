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
 * The calc state machine example program
 */

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iup.h>

#include "calcHsm.h"

/**
* Handle any button clicks and send the relevant signal to the calc state machine
* 
* @param self The button that was clicked
* 
*/
int btnClick(Ihandle* self)
{
  char* c = IupGetAttribute(self, "TITLE");

  if (strcmp(c, "0") == 0)
    calchsm_signal(SIG_DIGIT0, NULL);
  else if (strcmp(c, "1") == 0 ||
           strcmp(c, "2") == 0 ||
           strcmp(c, "3") == 0 ||
           strcmp(c, "4") == 0 ||
           strcmp(c, "5") == 0 ||
           strcmp(c, "6") == 0 ||
           strcmp(c, "7") == 0 ||
           strcmp(c, "8") == 0 ||
           strcmp(c, "9") == 0)
    calchsm_signal(SIG_DIGIT1_9, c);
  else if (strcmp(c, ".") == 0)
    calchsm_signal(SIG_POINT, NULL);
  else if (strcmp(c, "+") == 0 ||
           strcmp(c, "-") == 0 ||
           strcmp(c, "*") == 0 ||
           strcmp(c, "/") == 0)
    calchsm_signal(SIG_OPERATOR, c);
  else if (strcmp(c, "C") == 0)
    calchsm_signal(SIG_CLEAR, NULL);
  else if (strcmp(c, "CE") == 0)
    calchsm_signal(SIG_CLEARENTRY, NULL);
  else if (strcmp(c, "=") == 0)
    calchsm_signal(SIG_EQUALS, NULL);

  return EXIT_SUCCESS;
}

/**
* Attach button click event handlers to all the child buttons of a control
* 
* @param ih The parent control
* 
*/
void attach_button_events(Ihandle* ih)
{
  int i;
  // recurse through all controls
  for (i = 0; i < IupGetChildCount(ih); i++)
    attach_button_events(IupGetChild(ih, i));
  // if control is a button attach the event
  if (strcmp(IupGetClassName(ih), "button") == 0)
    IupSetCallback(ih, "ACTION", btnClick);
}

/**
* The main function of this program
*/
int main(int argc, char **argv)
{
  // initialise iup library
  IupOpen(&argc, &argv);
  // load calc user interface file
  IupLoad("calc.led");
  // setup button handlers 
  attach_button_events(IupGetHandle("main"));
  // show main window
  IupShow(IupGetHandle("main"));
  // Initialse calc state machine
  calchsm_init(IupGetHandle("textBox"));
  // start mainloop
  IupMainLoop();
  // close iup library
  IupClose();  

  return EXIT_SUCCESS;
}