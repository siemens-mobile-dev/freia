//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "miscu.h"

#define __FROM_CPP__

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"

#include "mainu.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OvalBtn"
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tmiscellanousfunctions       *miscellanousfunctions;

//---------------------------------------------------------------------------
__fastcall Tmiscellanousfunctions::Tmiscellanousfunctions (TComponent *
                                                           Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tmiscellanousfunctions::OvalButton6Click (TObject * Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------

void __fastcall Tmiscellanousfunctions::OvalButton2Click (TObject * Sender)
{
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------
