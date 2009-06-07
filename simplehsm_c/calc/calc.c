#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iup.h>

#include "calcHsm.h"

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