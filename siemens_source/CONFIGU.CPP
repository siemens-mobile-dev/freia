//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "configu.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#define __FROM_CPP__

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "regpub.h"
#include "cmdpub.h"

#include "mainu.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OvalBtn"
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"

Tconfigurationfunctions      *configurationfunctions;

//---------------------------------------------------------------------------
__fastcall Tconfigurationfunctions::Tconfigurationfunctions (TComponent *
                                                             Owner):TForm
    (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::OvalButton6Click (TObject * Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::OvalButton2Click (TObject * Sender)
{
    ModalResult = mrOk;
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::COMportappClick (TObject * Sender)
{
    mainfunctions->setcomapp (COMportapp->ItemIndex + 1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::COMportdngClick (TObject * Sender)
{
    mainfunctions->setcomdng (COMportdng->ItemIndex + 1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::debuglevelClick (TObject * Sender)
{
    mainfunctions->setdebuglevel (debuglevel->ItemIndex + 1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::COMspeedClick (TObject * Sender)
{
    int speeds[] = { 57600, 115200, 230400, 460800 };

    mainfunctions->setcomspeed (speeds[COMspeed->ItemIndex], false);
}


void __fastcall Tconfigurationfunctions::bootviabootcoreClick (TObject * Sender)
{
    mainfunctions->setboottype (boottype->ItemIndex+1, false);
}

void __fastcall Tconfigurationfunctions::com_rtsClick(TObject *Sender)
{
    mainfunctions->setcomtype (com_dtr->Checked, com_rts->Checked, false);
}
//---------------------------------------------------------------------------

void __fastcall Tconfigurationfunctions::com_dtrClick(TObject *Sender)
{
    mainfunctions->setcomtype (com_dtr->Checked, com_rts->Checked, false);
}
//---------------------------------------------------------------------------

