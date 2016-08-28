//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "dongleu.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OvalBtn"
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tdonglefunctions             *donglefunctions;

//---------------------------------------------------------------------------
__fastcall Tdonglefunctions::Tdonglefunctions (TComponent * Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tdonglefunctions::OvalButton6Click (TObject * Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------

void __fastcall Tdonglefunctions::OvalButton2Click (TObject * Sender)
{
    ModalResult = mrOk;
}

//---------------------------------------------------------------------------
