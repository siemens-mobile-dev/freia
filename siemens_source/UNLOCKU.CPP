//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "unlocku.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OvalBtn"
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tunlockingfunctions          *unlockingfunctions;

//---------------------------------------------------------------------------
__fastcall Tunlockingfunctions::Tunlockingfunctions (TComponent *
                                                     Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------
void __fastcall Tunlockingfunctions::locktoproviderClick (TObject * Sender)
{
    bool Bselected = locktoprovider->State == cbChecked;

    provider1->Enabled = Bselected;
    provider2->Enabled = Bselected;
    if (Bselected)
    {
        autolocktoprovider->State = cbUnchecked;
    }
}

//---------------------------------------------------------------------------

void __fastcall Tunlockingfunctions::FormActivate (TObject * Sender)
{
    if (Tag>0)
    {
        unlockaction->ItemIndex = Tag;
    }
    
    provider1->Text = "000-00";
    provider2->Text = "000-00";
    locktoprovider->State = cbUnchecked;
    locktoproviderClick (Sender);

    autolocktoprovider->State = cbUnchecked;
    updateflashIMEI->State = cbUnchecked;

    unlockactionClick (Sender);
}

//---------------------------------------------------------------------------
void __fastcall Tunlockingfunctions::updateflashIMEIClick (TObject * Sender)
{
    if (updateflashIMEI->State == cbChecked)
    {
        MessageDlg
            ("Please note that there is a HIGH RISK of flash IMEI corruption, which cannot be repaired when changing the OTP flash IMEI.",
             mtError, TMsgDlgButtons () << mbOK, 0);
    }
}

//---------------------------------------------------------------------------

void __fastcall Tunlockingfunctions::autolocktoproviderClick (TObject * Sender)
{
    bool Bselected = autolocktoprovider->State == cbChecked;

    if (Bselected)
    {
        locktoprovider->State = cbUnchecked;
    }
}

//---------------------------------------------------------------------------

void __fastcall Tunlockingfunctions::OvalButton6Click (TObject * Sender)
{
    ModalResult = mrNo;
}

//---------------------------------------------------------------------------
void __fastcall Tunlockingfunctions::OvalButton2Click (TObject * Sender)
{
    AnsiString tmp = unlockingfunctions->imei->Text.Trim ();
    bool bad;
    int i, cnt;

    /* are we unlocking? */
    if (OvalButton3->Visible && OvalButton4->Visible)
    {
        bad = tmp.Length () == 0;

        if (!bad)
        {
            for (i = 0, cnt = 0; i < tmp.Length (); i++)
            {
                if (tmp[i + 1] == '0')
                {
                    cnt++;
                }
            }

            bad = cnt >= 6;
        }
    }
    else
    {
        bad = FALSE;
    }

    if (bad)
    {
        if (MessageDlg ("Invalid IMEI is to be set. Continue?", mtConfirmation,
                        TMsgDlgButtons () << mbNo << mbYes, 0) != mrOk)
        {
            return;
        }
    }

    ModalResult = mrOk;
}

void __fastcall Tunlockingfunctions::unlockactionClick (TObject * Sender)
{
    switch (unlockaction->ItemIndex)
    {
    case 0:
    case 1:
    case 2:
    case 6:
    case 7:
        unlocksetup->Visible = false;
        OvalButton3->Visible = false;
        OvalButton4->Visible = false;
        break;

    default:
        unlocksetup->Visible = true;
        OvalButton3->Visible = true;
        OvalButton4->Visible = true;
        break;
    }
}

//---------------------------------------------------------------------------
void __fastcall Tunlockingfunctions::OvalButton3Click (TObject * Sender)
{
    ModalResult = mrIgnore;
}

//---------------------------------------------------------------------------

void __fastcall Tunlockingfunctions::OvalButton4Click (TObject * Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------
