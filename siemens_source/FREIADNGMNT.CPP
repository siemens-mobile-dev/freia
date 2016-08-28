//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USERES("freiadngmnt.res");
USEFORM("dngmntu.cpp", Form1);
USEUNIT("dng.c");
USEUNIT("rc4.c");
USEUNIT("misc.c");
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
        try
        {
                 Application->Initialize();
                 Application->CreateForm(__classid(TForm1), &Form1);
                 Application->Run();
        }
        catch (Exception &exception)
        {
                 Application->ShowException(&exception);
        }
        return 0;
}
//---------------------------------------------------------------------------
