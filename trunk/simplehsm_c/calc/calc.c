#include <stdio.h>
#include <tchar.h>

//#define USE_TCL_STUBS
//#define USE_TK_STUBS
#include <tcl.h>
#include <tk.h>

#include "simplehsm.h"

/*
http://tcl.activestate.com/cgi-bin/tct/tip/66.txt
http://www.ratiosoft.com/tcl/borland/tcl_and_c.shtml
http://fringe.davesource.com/Fringe/Computers/Languages/tcl_tk/tcl_C.html
http://wiki.tcl.tk/2074
*/

/* tcltk script for creation of two buttons */
char *StartScript[] = {
    "button .b -text [eval NewText] -command {puts \"I like it!\"}\n",
    "button .q -text Quit -command exit\n",
    "pack .b .q\n",
    NULL
};

/* Interpreter for this application. */
static Tcl_Interp *interp;

/* initialize Tcl Tk Interpreter */
int Tcl_AppInit(Tcl_Interp* interp) {
    int status;

    Tcl_SetVar(interp, "tcl_library", "C:\\Tcl\\lib\\tcl8.5", TCL_GLOBAL_ONLY);
    Tcl_SetVar(interp, "TCLLIBPATH", "C:\\Tcl\\lib\\tcl8.5", TCL_GLOBAL_ONLY); 
    Tcl_SetVar(interp, "tcl_libPath", "C:\\Tcl\\lib\\tcl8.5", TCL_GLOBAL_ONLY);

    Tcl_SetVar(interp, "tk_library", "C:\\Tcl\\lib\\tk8.5", TCL_GLOBAL_ONLY);

    /* Initialize Tk values. */
    status = Tk_Init(interp);
    if (status != TCL_OK) 
        return TCL_ERROR;

    return TCL_OK;
}

int main(int argc, char* argv[])
{
    int i;
    Tcl_Obj *TempObject;

    /* Initialise the Tcl library thouroughly */
    Tcl_FindExecutable(argv[0]);

    Tcl_Main(argc, argv, Tcl_AppInit);  /* ??? */
    interp = Tcl_CreateInterp();
    if (Tcl_AppInit(interp) != TCL_OK)
        Tcl_Exit(1);
    TempObject = Tcl_NewStringObj(StartScript[0], -1);
    i = 1;
    while(StartScript[i] != NULL)
    {
        Tcl_AppendToObj(TempObject, StartScript[i], -1);
        i++;
    }

    if(Tcl_EvalObj(interp, TempObject) != TCL_OK) {
         printf("Error in StartScript!\n");
         Tcl_DeleteInterp(interp);
         Tcl_Exit(1);
         return 1;
    }
    Tk_MainLoop();
    Tcl_DeleteInterp(interp);
    Tcl_Exit(0);
    return 0;
}