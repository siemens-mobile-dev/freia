//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>

#define __FROM_CPP__

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"

#include "phoneinfou.h"
#include "mainu.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tphoneinfo                   *phoneinfo;

//---------------------------------------------------------------------------
__fastcall Tphoneinfo::Tphoneinfo (TComponent * Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall Tphoneinfo::phonemodelChange (TObject * Sender)
{
    mainfunctions->AdjustStatus (phonemodel->Items->Strings[phonemodel->ItemIndex].
                         c_str (), phonemodel->ItemIndex);
}

//---------------------------------------------------------------------------
void __fastcall Tphoneinfo::FormCloseQuery(TObject *Sender, bool &CanClose)
{
  CanClose = false;
}
//---------------------------------------------------------------------------

