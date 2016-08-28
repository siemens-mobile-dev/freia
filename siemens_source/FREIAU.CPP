//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "freiau.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tfreia *freia;
//---------------------------------------------------------------------------
__fastcall Tfreia::Tfreia(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall Tfreia::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = false;        
}
//---------------------------------------------------------------------------

