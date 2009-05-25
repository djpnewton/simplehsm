#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iup.h>

#include "simplehsm.h"

int btnClick(Ihandle* self)
{
  printf("self: %d\n", self);
  return EXIT_SUCCESS;
}

void AttachButtonEvents(Ihandle* ih)
{
  int i;
  // recurse through all controls
  for (i = 0; i < IupGetChildCount(ih); i++)
    AttachButtonEvents(IupGetChild(ih, i));
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
  AttachButtonEvents(IupGetHandle("main"));
  // show main window
  IupShow(IupGetHandle("main"));
  // start mainloop
  IupMainLoop();
  // close iup library
  IupClose();  

  return EXIT_SUCCESS;
}