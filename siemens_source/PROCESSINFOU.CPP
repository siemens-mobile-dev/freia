//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "processinfou.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UPlasmaForm"
#pragma link "CGAUGES"
#pragma resource "*.dfm"
Tprocessinfo                 *processinfo;

//---------------------------------------------------------------------------
__fastcall Tprocessinfo::Tprocessinfo (TComponent * Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tprocessinfo::FormCloseQuery(TObject *Sender,
      bool &CanClose)
{
  CanClose = false;        
}
//---------------------------------------------------------------------------

