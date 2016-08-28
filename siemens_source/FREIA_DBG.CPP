//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("freia_dbg.res");
USEFORM("mainu.cpp", mainfunctions);
USEFORM("phoneinfou.cpp", phoneinfo);
USEFORM("processinfou.cpp", processinfo);
USEFORM("unlocku.cpp", unlockingfunctions);
USEFORM("miscu.cpp", miscellanousfunctions);
USEFORM("flashu.cpp", flashingfunctions);
USEFORM("configu.cpp", configurationfunctions);
USEFORM("dongleu.cpp", donglefunctions);
USEUNIT("reg.c");
USEUNIT("Comm.c");
USEUNIT("dng.c");
USEUNIT("map.c");
USEUNIT("misc.c");
USEUNIT("cmdhc.c");
USEUNIT("sec.c");
USEUNIT("tty.c");
USEFORM("freiau.cpp", freia);
//---------------------------------------------------------------------------
#define __FROM_CPP__

#include <io.h>
#include <fcntl.h>
#include <stdio.h>

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "regpub.h"
#include "cmdpub.h"

WINAPI WinMain (HINSTANCE, HINSTANCE, LPSTR, int)
{
    int                           i;

    if (_argc > 1)
    {
        if (MISC_RedirectIOToConsole())
        {
            i = cmd_main (_argc, _argv, NULL, NULL, NULL, NULL, NULL);
            FreeConsole ();
            return i;
        }
    }

    try
    {
        Application->Initialize ();
        Application->CreateForm(__classid(Tmainfunctions), &mainfunctions);
                 Application->CreateForm(__classid(Tphoneinfo), &phoneinfo);
                 Application->CreateForm(__classid(Tprocessinfo), &processinfo);
                 Application->CreateForm(__classid(Tfreia), &freia);
                 Application->CreateForm(__classid(Tunlockingfunctions), &unlockingfunctions);
                 Application->CreateForm(__classid(Tmiscellanousfunctions), &miscellanousfunctions);
                 Application->CreateForm(__classid(Tflashingfunctions), &flashingfunctions);
                 Application->CreateForm(__classid(Tconfigurationfunctions), &configurationfunctions);
                 Application->CreateForm(__classid(Tdonglefunctions), &donglefunctions);
                 Application->Run ();
    }
    catch (Exception & exception)
    {
        Application->ShowException (&exception);
    }
    return 0;
}

//---------------------------------------------------------------------------
