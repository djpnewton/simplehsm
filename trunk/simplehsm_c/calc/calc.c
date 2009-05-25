#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>

#include <iup.h>

#include "simplehsm.h"

int main(int argc, char **argv)
{
  IupOpen(&argc, &argv);
  IupLoad("calc.led");
  IupShow(IupGetHandle("main"));
  IupMainLoop();
  IupClose();  
  return EXIT_SUCCESS;
}