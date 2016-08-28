//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
USEUNIT("UPlasmaForm.pas");
USERES("UPlasmaForm.dcr");
USEUNIT("TransparentComponents.pas");
USEFORMNS("UPlasmaEd.pas", Uplasmaed, MaskForm);
USEUNIT("OvalBtn.pas");
USEPACKAGE("VCL50.bpi");
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Package source.
//---------------------------------------------------------------------------

#pragma argsused
int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void*)
{
        return 1;
}
//---------------------------------------------------------------------------
