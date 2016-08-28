//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include "dngmntu.h"

#define __FROM_CPP__

#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "dngpub.h"

bool                          deviceinfo_is_loaded = false;
int                           update_counter;

#define DI_FILTER "dongle info files (*.di)|*.DI"
#define BIN_FILTER "binary dongle update files (*.bin)|*.BIN"
#define DU_FILTER "dongle update files (*.du)|*.DU"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1                       *Form1;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1 (TComponent * Owner):TForm (Owner)
{
}

void mylog (char *logstr, BOOL Biserror, BOOL Bshowaserror)
{
    Form1->memo->Tag++;
    if (Form1->memo->Tag > 400)
    {
        Form1->memo->Clear ();
        Form1->memo->Lines->Clear ();
        Form1->memo->Repaint ();
        Form1->memo->Tag = 0;
    }
    Form1->memo->Lines->Add (logstr);

    if (Biserror)
        MessageDlg (logstr, (Bshowaserror ? mtError : mtInformation),
                    TMsgDlgButtons () << mbOK, 0);
}

void __fastcall TForm1::setupopendialog (char *ext, char *filter)
{
    AnsiString tmp;

    tmp = ExtractFilePath (opendialog->FileName);
    if (tmp.Length () == 0)
    {
        tmp = ExtractFilePath (Application->ExeName);
    }

    opendialog->InitialDir = tmp;

    opendialog->DefaultExt = ext;
    opendialog->Filter = filter;
}

//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click (TObject * Sender)
{
    char filename[SYS_MAX_PATHNAME_LEN];
    tDNG_DEVINFO devinfo;

    setupopendialog (FREIA_DI_EXTENSION, DI_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    strcpy (filename, ExpandFileName (opendialog->FileName).c_str ());
    if (!DNG_DecryptDevinfo (filename, &devinfo))
    {
        MessageDlg ("Invalid device info!", mtError, TMsgDlgButtons () << mbOK,
                    0);
        return;
    }

    deviceinfo_is_loaded = true;
    DNG_SetDeviceID (devinfo.U16devidlo, devinfo.U16devidhi);

    devidlo->Text = IntToHex (devinfo.U16devidlo, 4);
    devidhi->Text = IntToHex (devinfo.U16devidhi, 4);
    version->Text = IntToHex (devinfo.U16version, 4);
    blocked->Text = IntToHex (devinfo.Bblocked, 1);
    misusage_counter->Text = IntToHex (devinfo.U16misusage_counter, 4);
    update_counter = devinfo.U32update_counter;

    devidlo->Enabled = devinfo.U16devidlo == 0xFFFF
        && devinfo.U16devidhi == 0xFFFF;
    devidhi->Enabled = devinfo.U16devidlo == 0xFFFF
        && devinfo.U16devidhi == 0xFFFF;
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button2Click (TObject * Sender)
{
    tDNG_DEVINFO devinfo;

    if (!deviceinfo_is_loaded)
    {
        MessageDlg ("No device info is loaded yet!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        return;
    }
    
    devinfo.U16devidlo = MISC_HexToInt (devidlo->Text.c_str ());
    devinfo.U16devidhi = MISC_HexToInt (devidhi->Text.c_str ());
    devinfo.U16version = MISC_HexToInt (version->Text.c_str ());
    devinfo.Bblocked = MISC_HexToInt (blocked->Text.c_str ());
    devinfo.U16misusage_counter =
        MISC_HexToInt (misusage_counter->Text.c_str ());
    devinfo.U32update_counter = update_counter + 1;

    if (!DNG_EncryptDevinfo (&devinfo))
    {
        MessageDlg ("Could not save device info!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        return;
    }

    MessageDlg ("Device info is saved!", mtInformation,
                TMsgDlgButtons () << mbOK, 0);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::FormCreate (TObject * Sender)
{
    MISC_SetLogCallout (mylog);
}

//---------------------------------------------------------------------------

void __fastcall TForm1::Button3Click (TObject * Sender)
{
    char filename[SYS_MAX_PATHNAME_LEN];

    if (!deviceinfo_is_loaded)
    {
        MessageDlg ("No device info is loaded yet!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        return;
    }

    setupopendialog (BIN_FILTER, "BIN");

    if (!opendialog->Execute ())
    {
        return;
    }

    strcpy (filename, ExpandFileName (opendialog->FileName).c_str ());
    if (!DNG_CreateUpdate (filename))
    {
        MessageDlg ("Could not create update file!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        return;
    }

    MessageDlg ("Update file is created.", mtInformation,
                TMsgDlgButtons () << mbOK, 0);
}

//---------------------------------------------------------------------------
