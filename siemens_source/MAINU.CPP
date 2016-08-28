//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

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
#include "processinfou.h"
#include "phoneinfou.h"
#include "unlocku.h"
#include "miscu.h"
#include "flashu.h"
#include "configu.h"
#include "dongleu.h"
#include "freiau.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "UPlasmaForm"
#pragma link "OvalBtn"
#pragma resource "*.dfm"

Tmainfunctions                       *mainfunctions;

#define DI_FILTER "dongle info files (*.di)|*.DI"
#define DU_FILTER "dongle update files (*.du)|*.DU"
#define FLS_FILTER "FLS flash files (*.fls)|*.FLS"
#define MAP_FILTER "MAP files (*.map)|*.MAP"
#define LOG_FILTER "LOG files (*.log)|*.LOG"
#define FREIA_HEADER "Freia - Engine build %d / Frontend build %d"

int                           debuglevel;
int                           phonemodel;
int                           comport_app;
int                           comport_dtr;
int                           comport_rts;
int                           comport_dng;
int                           comspeed;
int                           boottype;

static int                    progress_start;
static int                    progress_end;
static int                    redirection;
static int                    provider1 = 0, provider2 = 0;
static int                    updateflashimei;

int RasX, RasY;

//---------------------------------------------------------------------------
__fastcall Tmainfunctions::Tmainfunctions (TComponent * Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::OvalButton6Click (TObject * Sender)
{
    Application->Terminate ();
}

//---------------------------------------------------------------------------



static void tokenize (char *tmp, int *argc, char *argv[])
{
    int j = 0;
    char                         *ptr = tmp;

    argv[j++] = ptr;
    while (*ptr != '\0')
    {
        while (*ptr > 32 && *ptr != '\0')   /* find end of data */
        {
            if (*ptr == CMDLINE_STRING_SEP) /* strings must be between apostrophes */
            {
                ptr++;          /* Skip start */
                while (*ptr != CMDLINE_STRING_SEP && *ptr != '\0')  /* find end of string */
                    ptr++;
            }
            ptr++;
        }

        *ptr = '\0';            /* terminate token */
        ptr++;

        while (*ptr <= 32 && *ptr != '\0')  /* find end of whitespace */
            ptr++;

        if (*ptr != '\0')
            argv[j++] = ptr;
    }

    *argc = j;
}

void mylog (char *logstr, BOOL Biserror, BOOL Bshowaserror)
{
    processinfo->memo->Tag++;
    if (processinfo->memo->Tag > 400)
    {
        processinfo->memo->Clear ();
        processinfo->memo->Lines->Clear ();
        processinfo->memo->Repaint ();
        processinfo->memo->Tag = 0;
    }
    processinfo->memo->Lines->Add (logstr);

    if (Biserror)
        MessageDlg (logstr, (Bshowaserror ? mtError : mtInformation),
                    TMsgDlgButtons () << mbOK, 0);
}

void progressbardone (void)
{
    processinfo->progressbar->Progress = 0;
    processinfo->progressbar->Enabled = false;
}

void progressbarinit (UINT32 U32start, UINT32 U32end)
{
    processinfo->progressbar->Enabled = true;
    processinfo->progressbar->Progress = 0;
    progress_start = U32start;
    progress_end = U32end;
}

void progressbarupdate (UINT32 U32addr)
{
    int pos =
        (U32addr - progress_start) * 100 / (progress_end - progress_start);
    // pos shall be between 0 and 100
    processinfo->progressbar->Progress = pos;
    Application->ProcessMessages();
}

void guiupdate (void)
{
    Application->ProcessMessages();
}


void __fastcall Tmainfunctions::runfreia (char *arglist)
{
    int argc;
    char                         *argv[64];
    char tmp[256], cmdlinestr[256];

    memset (tmp, 0, sizeof (tmp));
    memset (cmdlinestr, 0, sizeof (cmdlinestr));

    strcpy (tmp, "freia.exe ");
    Trim (arglist);
    strcat (tmp, arglist);

    memset (argv, 0, sizeof (argv));

    processinfo->memo->Clear ();
    processinfo->memo->Lines->Clear ();
    processinfo->memo->Repaint ();
    processinfo->memo->Tag = 0;
    tokenize (tmp, &argc, argv);

    TCursor Save_Cursor = Screen->Cursor;

    Screen->Cursor = crHourGlass;   // Show hourglass cursor
    Application->ProcessMessages();
    cmd_main (argc, argv, mylog, progressbarinit, progressbarupdate,
              progressbardone, guiupdate);
    Screen->Cursor = Save_Cursor;   // always restore the cursor
}

void __fastcall Tmainfunctions::setboottype (int value, bool force)
{
    if (value == boottype && !force)
    {
        return;
    }

    boottype = value;
    if (boottype == 1)
    {
        boottype1->Checked = true;
        boottype2->Checked = false;
        boottype3->Checked = false;
    }
    else if (boottype == 2)
    {
        boottype1->Checked = false;
        boottype2->Checked = true;
        boottype3->Checked = false;
    }
    else
    {
        boottype1->Checked = false;
        boottype2->Checked = false;
        boottype3->Checked = true;
    }

    configurationfunctions->boottype->ItemIndex = boottype-1;
}

void __fastcall Tmainfunctions::AdjustStatus (char *modelname, int index)
{
    bool Bfound;
    tMISC_PHONE_MODEL            *pmodel;
    char                         *ptr = strchr (modelname, ' ');
    char line[128];

    if (ptr == NULL)
    {
        return;
    }

    Bfound = MISC_GetPhoneModel (ptr + 1, &pmodel);
    if (!Bfound)
    {
        return;
    }


    phonemodel = index;

    sprintf (line, "Firmware : 0x%06X-0xFFFFFF", pmodel->U32mcustartaddress);
    phoneinfo->fwaddress->Caption = line;

    if (pmodel->bootcore.U32size > 0)
    {
        sprintf (line, "Bootcore : 0x%06X-0x%06X", pmodel->bootcore.U32start,
                 pmodel->bootcore.U32start + pmodel->bootcore.U32size - 1);
    }
    else
    {
        strcpy (line, "Bootcore : None");
    }
    phoneinfo->bootcore->Caption = line;

    if (pmodel->eeprom[1].U32size > 0)
    {
        sprintf (line, "1st EEPROM : 0x%06X-0x%06X",
                 pmodel->eeprom[0].U32start,
                 pmodel->eeprom[0].U32start + pmodel->eeprom[0].U32size - 1);
        phoneinfo->eeprom1->Caption = line;

        sprintf (line, "2nd EEPROM : 0x%06X-0x%06X",
                 pmodel->eeprom[1].U32start,
                 pmodel->eeprom[1].U32start + pmodel->eeprom[1].U32size - 1);
        phoneinfo->eeprom2->Caption = line;
    }
    else
    {
        sprintf (line, "1st EEPROM : 0x%06X-0x%06X", pmodel->eeprom[0].U32start,
                 pmodel->eeprom[0].U32start + pmodel->eeprom[0].U32size - 1);
        phoneinfo->eeprom1->Caption = line;
        phoneinfo->eeprom2->Caption = "2nd EEPROM : None";
    }
}

void __fastcall Tmainfunctions::FormActivate (TObject * Sender)
{
    freia->Top = mainfunctions->Top+mainfunctions->Height;
    freia->Left = mainfunctions->Left - phoneinfo->Width - 10;

    processinfo->Top = mainfunctions->Top+mainfunctions->Height + 10;
    processinfo->Left = mainfunctions->Left - (processinfo->Width-mainfunctions->Width)/2;

    phoneinfo->Top = mainfunctions->Top + (mainfunctions->Height-phoneinfo->Height)/2;
    phoneinfo->Left = mainfunctions->Left - phoneinfo->Width - 40;

    phoneinfo->phonemodel->ItemIndex = phonemodel;
    phoneinfo->phonemodel->Text =
        phoneinfo->phonemodel->Items->Strings[phonemodel];

    AdjustStatus (phoneinfo->phonemodel->Items->Strings[phonemodel].c_str (),
                  phonemodel);

    setcomapp (comport_app, true);
    setcomdng (comport_dng, true);
    setcomspeed (comspeed, true);
    setdebuglevel (debuglevel, true);
    setcomtype (comport_dtr, comport_rts, true);

    setboottype (boottype, true);

    setredirection ((eREDIRECTION_TYPE) redirection, true);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::OvalButton5Click (TObject * Sender)
{
    TModalResult result;

    unlockingfunctions->Tag = -1;
    result = unlockingfunctions->ShowModal ();

    if (result != mrOk && result != mrIgnore && result != mrCancel)
    {
        return;
    }

    switch (unlockingfunctions->unlockaction->ItemIndex)
    {
    case 0:
        Createlogfromphone1Click (Sender);
        break;

    case 1:
        Createmapfromlog1Click (Sender);
        break;

    case 2:
        Createmapsfromlogs1Click (Sender);
        break;

    case 3:
        Createmapfromlog2Click (Sender);
        break;

    case 4:
        Directunlockandmapisalsosaved1Click (Sender);
        break;

    case 5:
        Directunlockandmapisnotsaved1Click (Sender);
        break;

    case 6:
        Createbackupmap1Click (Sender);
        break;

    case 7:
        Loadmaptophone1Click (Sender);
        break;

    default:
        MessageDlg ("Invalid action selected!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        break;
    }
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::OvalButton4Click (TObject * Sender)
{
    if (miscellanousfunctions->ShowModal () != mrOk)
    {
        return;
    }

    switch (miscellanousfunctions->miscellanousaction->ItemIndex)
    {
    case 0:
        Decrypt1Click (Sender);
        break;

    case 1:
        ReadandpatchbootcoreinFLSformat1Click (Sender);
        break;

    case 2:
        Backupbatteryparametersfromphone1Click (Sender);
        break;

    case 3:
        Restorebatteryparametersfromfile1Click (Sender);
        break;

    default:
        MessageDlg ("Invalid action selected!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        break;
    };
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::OvalButton2Click (TObject * Sender)
{
    configurationfunctions->ShowModal ();
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::OvalButton1Click (TObject * Sender)
{
    if (donglefunctions->ShowModal () != mrOk)
    {
        return;
    }

    switch (donglefunctions->dongleaction->ItemIndex)
    {
    case 0:
        Testdongle1Click (Sender);
        break;

    case 1:
        Getdongleinfo1Click (Sender);
        break;

    case 2:
        Uploaddongleinfo1Click (Sender);
        break;

    case 3:
        Updatedongle1Click (Sender);
        break;

    default:
        MessageDlg ("Invalid action selected!", mtError,
                    TMsgDlgButtons () << mbOK, 0);
        break;
    };
}

//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::FormCreate (TObject * Sender)
{
    char buffer[SYS_MAX_STRING_LEN];
    AnsiString tmpstr;
    int issaveonexit, model;

    sprintf (buffer, FREIA_HEADER, MISC_GetBuild (), CMDLINE_BUILD);
    mainfunctions->Caption = buffer;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "saveonexit", &issaveonexit))
        issaveonexit = 1;

    saveconfigurationonexit->Checked = issaveonexit == 1;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "model", &phonemodel))
        phonemodel = 0;

    if (!REG_Load
        (HKEY_CURRENT_USER, "freia", "boottype", &boottype))
        boottype = 1;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "comdtr", &comport_dtr))
        comport_dtr = 1;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "comrts", &comport_rts))
        comport_rts = 1;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "comapp", &comport_app))
        comport_app = 1;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "comdng", &comport_dng))
        comport_dng = 2;

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "comspeed", &comspeed))
        comspeed = 115200;

    REG_Load (HKEY_CURRENT_USER, "freia", "debuglevel", &debuglevel);

    if (!REG_Load (HKEY_CURRENT_USER, "freia", "redirectlog", &redirection))
        redirection = REDIRECTION_TYPE_NONE;
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::setcomapp (int U8port, bool force)
{
    if (U8port == comport_app && !force)
    {
        return;
    }

    if (U8port == 2)
    {
        COMport1app->Checked = false;
        COMport2app->Checked = true;
        COMport3app->Checked = false;
        COMport4app->Checked = false;
        configurationfunctions->COMportapp->ItemIndex = 1;
        comport_app = 2;
    }
    else if (U8port == 3)
    {
        COMport1app->Checked = false;
        COMport2app->Checked = false;
        COMport3app->Checked = true;
        COMport4app->Checked = false;
        configurationfunctions->COMportapp->ItemIndex = 2;
        comport_app = 3;
    }
    else if (U8port == 4)
    {
        COMport1app->Checked = false;
        COMport2app->Checked = false;
        COMport3app->Checked = false;
        COMport4app->Checked = true;
        configurationfunctions->COMportapp->ItemIndex = 3;
        comport_app = 4;
    }
    else
    {
        COMport1app->Checked = true;
        COMport2app->Checked = false;
        COMport3app->Checked = false;
        COMport4app->Checked = false;
        configurationfunctions->COMportapp->ItemIndex = 0;
        comport_app = 1;
    }
}

void __fastcall Tmainfunctions::setcomdng (int U8port, bool force)
{
    if (U8port == comport_dng && !force)
    {
        return;
    }

    if (U8port == 2)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = true;
        COMport3dng->Checked = false;
        COMport4dng->Checked = false;
        COMport5dng->Checked = false;
        COMport6dng->Checked = false;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 1;
        comport_dng = 2;
    }
    else if (U8port == 3)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = false;
        COMport3dng->Checked = true;
        COMport4dng->Checked = false;
        COMport5dng->Checked = false;
        COMport6dng->Checked = false;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 2;
        comport_dng = 3;
    }
    else if (U8port == 4)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = false;
        COMport3dng->Checked = false;
        COMport4dng->Checked = true;
        COMport5dng->Checked = false;
        COMport6dng->Checked = false;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 3;
        comport_dng = 4;
    }
    else if (U8port == 5)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = false;
        COMport3dng->Checked = false;
        COMport4dng->Checked = false;
        COMport5dng->Checked = true;
        COMport6dng->Checked = false;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 4;
        comport_dng = 5;
    }
    else if (U8port == 6)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = false;
        COMport3dng->Checked = false;
        COMport4dng->Checked = false;
        COMport5dng->Checked = false;
        COMport6dng->Checked = true;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 5;
        comport_dng = 6;
    }
    else if (U8port == 7)
    {
        COMport1dng->Checked = false;
        COMport2dng->Checked = false;
        COMport3dng->Checked = false;
        COMport4dng->Checked = false;
        COMport5dng->Checked = false;
        COMport6dng->Checked = false;
        COMport7dng->Checked = true;
        configurationfunctions->COMportdng->ItemIndex = 6;
        comport_dng = 7;
    }
    else
    {
        COMport1dng->Checked = true;
        COMport2dng->Checked = false;
        COMport3dng->Checked = false;
        COMport4dng->Checked = false;
        COMport5dng->Checked = false;
        COMport6dng->Checked = false;
        COMport7dng->Checked = false;
        configurationfunctions->COMportdng->ItemIndex = 0;
        comport_dng = 1;
    }
}

void __fastcall Tmainfunctions::setcomspeed (int U32speed, bool force)
{
    if (U32speed == comspeed && !force)
    {
        return;
    }

    if (U32speed == 115200)
    {
        COMspeed57600->Checked = false;
        COMspeed115200->Checked = true;
        COMspeed230400->Checked = false;
        COMspeed460800->Checked = false;
        configurationfunctions->COMspeed->ItemIndex = 1;
        comspeed = 115200;
    }
    else if (U32speed == 230400)
    {
        COMspeed57600->Checked = false;
        COMspeed115200->Checked = false;
        COMspeed230400->Checked = true;
        COMspeed460800->Checked = false;
        configurationfunctions->COMspeed->ItemIndex = 2;
        comspeed = 230400;
    }
    else if (U32speed == 460800)
    {
        COMspeed57600->Checked = false;
        COMspeed115200->Checked = false;
        COMspeed230400->Checked = false;
        COMspeed460800->Checked = true;
        configurationfunctions->COMspeed->ItemIndex = 3;
        comspeed = 460800;
    }
    else
    {
        COMspeed57600->Checked = true;
        COMspeed115200->Checked = false;
        COMspeed230400->Checked = false;
        COMspeed460800->Checked = false;
        configurationfunctions->COMspeed->ItemIndex = 0;
        comspeed = 57600;
    }
}

void __fastcall Tmainfunctions::setdebuglevel (int U8debuglevel, bool force)
{
    if (debuglevel==U8debuglevel && !force)
    {
        return;
    }

    if (U8debuglevel == 1)
    {
        Low1->Checked = true;
        Medium1->Checked = false;
        High1->Checked = false;
        configurationfunctions->debuglevel->ItemIndex = 0;
        debuglevel = 1;
    }
    else if (U8debuglevel == 3)
    {
        Low1->Checked = false;
        Medium1->Checked = false;
        High1->Checked = true;
        configurationfunctions->debuglevel->ItemIndex = 2;
        debuglevel = 3;
    }
    else
    {
        Low1->Checked = false;
        Medium1->Checked = true;
        High1->Checked = false;
        configurationfunctions->debuglevel->ItemIndex = 1;
        debuglevel = 2;
    }
}

void __fastcall Tmainfunctions::setcomtype (bool Bdtr, bool Brts, bool force)
{
    if (Brts==comport_rts && Bdtr==comport_dtr && !force)
    {
        return;
    }

    comport_rts = Brts;
    comport_dtr = Bdtr;
    comport_RTS->Checked = Brts;
    comport_DTR->Checked = Bdtr;
    configurationfunctions->com_rts->Checked = Brts;
    configurationfunctions->com_dtr->Checked = Bdtr;    
}


void __fastcall Tmainfunctions::Low1Click (TObject * Sender)
{
    setdebuglevel (1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Medium1Click (TObject * Sender)
{
    setdebuglevel (2, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::High1Click (TObject * Sender)
{
    setdebuglevel (3, false);
}

void __fastcall Tmainfunctions::setredirection (eREDIRECTION_TYPE redirectiontype, bool force)
{
    if (redirection == redirectiontype && !force)
    {
        return;
    }

    if (redirectiontype == REDIRECTION_TYPE_REDIRECT_TO_FILE)
    {
        redirect0->Checked = false;
        redirect1->Checked = true;
        redirect2->Checked = false;
        redirection = redirectiontype;
    }
    else if (redirectiontype == REDIRECTION_TYPE_SAVE_TO_FILE)
    {
        redirect0->Checked = false;
        redirect1->Checked = false;
        redirect2->Checked = true;
        redirection = redirectiontype;
    }
    else
    {
        redirect0->Checked = true;
        redirect1->Checked = false;
        redirect2->Checked = false;
        redirection = REDIRECTION_TYPE_NONE;
    }
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::redirect0Click (TObject * Sender)
{
    setredirection (REDIRECTION_TYPE_NONE, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::redirect1Click (TObject * Sender)
{
    setredirection (REDIRECTION_TYPE_REDIRECT_TO_FILE, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::redirect2Click (TObject * Sender)
{
    setredirection (REDIRECTION_TYPE_SAVE_TO_FILE, false);
}

void __fastcall Tmainfunctions::COMport1appClick (TObject * Sender)
{
    setcomapp (1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport2appClick (TObject * Sender)
{
    setcomapp (2, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport3appClick (TObject * Sender)
{
    setcomapp (3, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport4appClick (TObject * Sender)
{
    setcomapp (4, false);
}

void __fastcall Tmainfunctions::setupopendialog (char *ext, char *filter)
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
    opendialog->Filter += "|Any file (*.*)|*.*";    
}

void __fastcall Tmainfunctions::Clearlogwindow1Click (TObject * Sender)
{
    processinfo->memo->Clear ();
    processinfo->memo->Repaint ();
    processinfo->memo->Tag = 0;
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::SaveConfiguration1Click (TObject * Sender)
{
    int issaveonexit = saveconfigurationonexit->Checked;

    REG_Save (HKEY_CURRENT_USER, "freia", "saveonexit", &issaveonexit,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "comapp", (void *) &comport_app,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "comdtr", (void *) &comport_dtr,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "comrts", (void *) &comport_rts,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "comdng", (void *) &comport_dng,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "boottype",
              (void *) &boottype, rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "comspeed", (void *) &comspeed,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "debuglevel", (void *) &debuglevel,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "redirectlog", (void *) &redirection,
              rdInteger, TRUE);

    REG_Save (HKEY_CURRENT_USER, "freia", "model", (void *) &phonemodel,
              rdInteger, TRUE);
}

/********************************************************
*                                                       *
* FUNCTION NAME:                                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
*                                                       *
* RETURNS:                                              *
*                                                       *
*********************************************************/

static UINT8 misc_hexchartobyte (char hex)
{
    if (hex >= 'A' && hex <= 'F')
        return (UINT8) hex - 55;
    else if (hex >= 'a' && hex <= 'f')
        return (UINT8) hex - 87;
    else if (hex >= '0' && hex <= '9')
        return (UINT8) hex - 48;

    return 0;
}

/********************************************************
*                                                       *
* FUNCTION NAME:                                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
*                                                       *
* RETURNS:                                              *
*                                                       *
*********************************************************/

static UINT32 misc_hextodword (char *hex)
{
    UINT32 U32ret = 0;
    UINT16 U16hexlen = strlen (hex);
    SINT16 i;
    UINT32 U32exp = 1;

    for (i = U16hexlen - 1; i >= 0; i--, U32exp *= 16)
        U32ret += (U32exp * misc_hexchartobyte (hex[i]));

    return U32ret;
}


/********************************************************
*                                                       *
* FUNCTION NAME:                                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
*                                                       *
* RETURNS:                                              *
*                                                       *
*********************************************************/

static void misc_readarg (char *arg, void *val, BOOL Bnumber)
{
    UINT16 U16start = *((UINT16 *) arg);

    if (Bnumber)
    {
        /*
         * 0x or 0X 
         */
        if (U16start == 0x7830 || U16start == 0x5830)
            *((long *) val) = misc_hextodword (arg);
        else
            *((long *) val) = atol (arg);
    }
    else
        strcpy ((char *) val, arg);
}

//---------------------------------------------------------------------------
bool __fastcall Tmainfunctions::getaddresses (bool Bbinary,
                                      char *pathname, char *title,
                                      int *startaddr,
                                      int *endaddr,
                                      int *startofs, int *countfile)
{
    char buffer[256];
    UINT16 i;
    bool badstart, badend, good = false;
    int U32len;
    int handle;
    bool Bstartaddr, Bendaddr, Bstartoffs, Bcountfile;
    int U32top = 56;

    if (pathname)
    {
        handle = open (pathname, O_RDONLY);
        if (handle == -1)
            U32len = 0;
        else
        {
            U32len = filelength (handle);
            if (!Bbinary)
            {
                U32len -= sizeof (tFILE_HEADER);
            }
        }
        close (handle);
    }
    else
        U32len = 0;

    flashingfunctions->Tag = U32len;
    flashingfunctions->startaddr->Top = U32top;
    flashingfunctions->startaddr_l->Top = U32top;
    flashingfunctions->startaddr->Visible = Bstartaddr = startaddr != NULL;
    flashingfunctions->startaddr_l->Visible = Bstartaddr;
    if (Bstartaddr)
        U32top += (flashingfunctions->startaddr->Height + 11);

    flashingfunctions->endaddr->Top = U32top;
    flashingfunctions->endaddr_l->Top = U32top;
    flashingfunctions->endaddr->Visible = Bendaddr = endaddr != NULL;
    flashingfunctions->endaddr_l->Visible = Bendaddr;
    if (Bendaddr)
        U32top += (flashingfunctions->endaddr->Height + 11);

    flashingfunctions->startoffs->Top = U32top;
    flashingfunctions->startoffs_l->Top = U32top;
    flashingfunctions->startoffs->Visible = Bstartoffs = startofs != NULL;
    flashingfunctions->startoffs_l->Visible = Bstartoffs;
    if (Bstartoffs)
        U32top += (flashingfunctions->startoffs->Height + 11);

    flashingfunctions->countfile->Top = U32top;
    flashingfunctions->countfile_l->Top = U32top;
    flashingfunctions->countfile->Visible = Bcountfile = countfile != NULL;
    flashingfunctions->countfile_l->Visible = Bcountfile;

    flashingfunctions->startoffs->Text = "0";
    flashingfunctions->countfile->Text = IntToStr (U32len);

    if (Bstartoffs)
        *startofs = 0;

    if (Bcountfile)
        *countfile = 0;

    if (Bstartaddr)
        *startaddr = 0;

    if (Bendaddr)
        *endaddr = 0;

    if (title)
    {
        Trim (title);
        flashingfunctions->Caption = title;
    }

    if (flashingfunctions->ShowModal () == mrOk)
    {
        if (Bstartaddr)
        {
            memset (buffer, 0, sizeof (buffer));
            Trim (flashingfunctions->startaddr->Text);
            flashingfunctions->startaddr->GetTextBuf (buffer, sizeof (buffer));
            for (i = 0; i < (UINT16) strlen (buffer); i++)
                buffer[i] = UpCase (buffer[i]);

            misc_readarg (buffer, startaddr, TRUE);
            badstart = false;   /* 0 is allowed as start address */
        }
        else
            badstart = false;

        if (Bendaddr)
        {
            memset (buffer, 0, sizeof (buffer));
            Trim (flashingfunctions->endaddr->Text);
            flashingfunctions->endaddr->GetTextBuf (buffer, sizeof (buffer));
            for (i = 0; i < (UINT16) strlen (buffer); i++)
                buffer[i] = UpCase (buffer[i]);

            misc_readarg (buffer, endaddr, TRUE);
            badend = *endaddr == 0;
        }
        else
            badend = false;

        if (Bstartoffs)
        {
            memset (buffer, 0, sizeof (buffer));
            Trim (flashingfunctions->startoffs->Text);
            flashingfunctions->startoffs->GetTextBuf (buffer, sizeof (buffer));
            for (i = 0; i < (UINT16) strlen (buffer); i++)
                buffer[i] = UpCase (buffer[i]);

            misc_readarg (buffer, startofs, TRUE);
            if (*startofs > U32len)
                *startofs = U32len;
        }

        if (Bcountfile)
        {
            memset (buffer, 0, sizeof (buffer));
            Trim (flashingfunctions->countfile->Text);
            flashingfunctions->countfile->GetTextBuf (buffer, sizeof (buffer));
            for (i = 0; i < (UINT16) strlen (buffer); i++)
                buffer[i] = UpCase (buffer[i]);

            misc_readarg (buffer, countfile, TRUE);
            if (*startofs + *countfile > U32len)
                *countfile = U32len - *startofs;
        }

        good = !badstart && !badend;
    }

    return good;
}

void __fastcall Tmainfunctions::Unlock (bool Bshowform, bool BnoMAP, bool Bnoupdate)
{
    char param[128];
    char start[256];
    char newprovider[32], newIMEI[32];
    TModalResult result;
    AnsiString tmp;

    if (Bshowform)
    {
        result = unlockingfunctions->ShowModal ();
        if (result != mrOk && result != mrIgnore && result != mrCancel)
        {
            return;
        }
    }

    updateflashimei = unlockingfunctions->updateflashIMEI->State == cbChecked;

    if (unlockingfunctions->locktoprovider->State == cbChecked)
    {
        tmp = unlockingfunctions->provider1->Text;
        tmp.Delete (tmp.Pos ("-"), 1);
        tmp.Trim ();

        memset (newprovider, 0, sizeof (newprovider));
        strcpy (newprovider, tmp.c_str ());

        provider1 = atol (newprovider);

        tmp = unlockingfunctions->provider2->Text;
        tmp.Delete (tmp.Pos ("-"), 1);
        tmp.Trim ();

        memset (newprovider, 0, sizeof (newprovider));
        strcpy (newprovider, tmp.c_str ());

        provider2 = atol (newprovider);
    }
    else if (unlockingfunctions->autolocktoprovider->State == cbChecked)
    {
        provider1 = 1;
    }
    else
    {
        provider1 = provider2 = 0;
    }

    result = unlockingfunctions->ModalResult;

    memset (newIMEI, 0, sizeof (newIMEI));
    if (result==mrOk)
    {
      strcpy (newIMEI, unlockingfunctions->imei->Text.c_str ());
    }
    else if (result==mrIgnore)
    {
      strcpy (newIMEI, "E");
    }
    else /* if (result==mrCancel) */
    {
      strcpy (newIMEI, "O");
    }

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    if (BnoMAP)
    {
        strcat (start, "-nm ");
    }

    if (Bnoupdate)
    {
        strcat (start, "-nu ");
    }

    sprintf (param, "-u %s ", newIMEI);
    strcat (start, param);
    runfreia (start);

    updateflashimei = 0;
    provider1 = provider2 = 0;
}

void __fastcall Tmainfunctions::Loadmaptophone1Click (TObject * Sender)
{
    char param[256];
    char start[256];

    setupopendialog (FREIA_MAP_EXTENSION, MAP_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    sprintf (param, "-wm %c%s%c ", CMDLINE_STRING_SEP,
             ExpandFileName (opendialog->FileName).c_str (),
             CMDLINE_STRING_SEP);
    strcat (start, param);
    runfreia (start);
}

//---------------------------------------------------------------------------

bool __fastcall Tmainfunctions::getphonemodel (char *model)
{
    char buffer[64];
    char                         *ptr;

    strcpy (model, "");
    if (phoneinfo->phonemodel->ItemIndex == -1)
        return false;

    memset (buffer, 0, sizeof (buffer));
    strcpy (buffer,
            phoneinfo->phonemodel->Items->Strings[phoneinfo->phonemodel->
                                                  ItemIndex].c_str ());
    ptr = strchr (buffer, ' ');
    if (ptr)
    {
        sprintf (model, "%c%s%c", CMDLINE_STRING_SEP, ptr + 1,
                 CMDLINE_STRING_SEP);
    }
    return ptr != NULL;
}

bool __fastcall Tmainfunctions::buildstartofcommand (char *cmdline)
{
    char model[SYS_MAX_STRING_LEN], buffer[SYS_MAX_STRING_LEN];

    memset (model, 0, sizeof (model));

    if (!getphonemodel (model))
    {
        MessageDlg ("No phone model is set", mtInformation,
                    TMsgDlgButtons () << mbOK, 0);
        return FALSE;
    }

    strcpy (cmdline, "");
    sprintf (cmdline, "%s ", model);

    memset (buffer, 0, sizeof (buffer));
    sprintf (buffer, "-sd %d ", debuglevel);
    strcat (cmdline, buffer);

    if (redirection != REDIRECTION_TYPE_NONE)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-sr %d ", redirection);
        strcat (cmdline, buffer);
    }

    if (boottype!=1)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-bt %d ", boottype);
        strcat (cmdline, buffer);
    }

    if (comport_dtr!=1 || comport_rts!=1)        /* COM1 type*/
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-cat %d %d ", comport_dtr, comport_rts);
        strcat (cmdline, buffer);
    }

    if (comport_app!=1)        /* COM1 */
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-ca %d ", comport_app);
        strcat (cmdline, buffer);
    }

    if (comport_dng!=2)        /* COM2 */
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-cd %d ", comport_dng);
        strcat (cmdline, buffer);
    }

    if (comspeed != FREIA_DEFAULT_SPEED)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-s %d ", comspeed);
        strcat (cmdline, buffer);
    }

    if (provider1 > 0)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "-ltp %d %d ", provider1, provider2);
        strcat (cmdline, buffer);
    }

    if (updateflashimei)
    {
        strcat (cmdline, "-ufi ");
    }

    return TRUE;
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMspeed57600Click (TObject * Sender)
{
    setcomspeed (57600, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMspeed115200Click (TObject * Sender)
{
    setcomspeed (115200, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Createmapfromlog2Click (TObject * Sender)
{
    ShortString tmp = Sender->ClassName ();

    unlockingfunctions->Tag = 3;

    Unlock (CompareStr (tmp, "TMenuItem") == 0, false, true);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Directunlockandmapisalsosaved1Click (TObject * Sender)
{
    ShortString tmp = Sender->ClassName ();
    
    unlockingfunctions->Tag = 4;

    Unlock (CompareStr (tmp, "TMenuItem") == 0, false, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Directunlockandmapisnotsaved1Click (TObject * Sender)
{
    ShortString tmp = Sender->ClassName ();

    unlockingfunctions->Tag = 5;

    Unlock (CompareStr (tmp, "TMenuItem") == 0, true, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Createbackupmap1Click (TObject * Sender)
{
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    strcat (start, "-b ");
    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::readflash (bool Bbfb)
{
    int U32startaddr, U32endaddr;
    char start[256], cmdline[256];
    bool Bbinary = true;

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    if (getaddresses
        (Bbinary, NULL, "Enter read adresses:", &U32startaddr, &U32endaddr,
         NULL, NULL))
    {
        memset (cmdline, 0, sizeof (cmdline));
        if (Bbfb)
        {
            sprintf (cmdline, "%s %ld %ld", (Bbinary ? "-rrm" : "-rm"),
                     U32startaddr, U32endaddr - U32startaddr + 1);
        }
        else
        {
            sprintf (cmdline, "%s %ld %ld", (Bbinary ? "-rr" : "-r"),
                     U32startaddr, U32endaddr - U32startaddr + 1);
        }
        strcat (start, cmdline);
        runfreia (start);
    }
}


void __fastcall Tmainfunctions::ReadflashfromphoneandstoreasFLS1Click (TObject * Sender)
{
    readflash (false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::writeflash (void)
{
    int U32startaddr, U32startfile = 0, U32countfile = 0;
    char start[256], cmdline[256], filename[256];
    bool Bbinary = true;

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    setupopendialog (FREIA_FLS_EXTENSION, FLS_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    strcpy (filename, ExpandFileName (opendialog->FileName).c_str ());

    if (!getaddresses
        (Bbinary, filename, "Enter where to start from to write:",
         (Bbinary ? &U32startaddr : NULL), NULL, &U32startfile, &U32countfile))
    {
        return;
    }

    memset (cmdline, 0, sizeof (cmdline));

    if (Bbinary)
    {
        sprintf (cmdline, "-rw %c%s%c %ld %ld %ld", CMDLINE_STRING_SEP,
                 filename, CMDLINE_STRING_SEP, U32startaddr, U32startfile,
                 U32countfile);
    }
    else
    {
        sprintf (cmdline, "-w %c%s%c %ld %ld", CMDLINE_STRING_SEP, filename,
                 CMDLINE_STRING_SEP, U32startfile, U32countfile);
    }

    strcat (start, cmdline);
    runfreia (start);
}

void __fastcall Tmainfunctions::WriteFLSflashfiletophone1Click (TObject * Sender)
{
    writeflash ();
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Createmapfromlog1Click (TObject * Sender)
{
    char param[128];
    char start[256];

    setupopendialog (FREIA_LOG_EXTENSION, LOG_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    sprintf (param, "-ul %c%s%c ", CMDLINE_STRING_SEP,
             ExpandFileName (opendialog->FileName).c_str (),
             CMDLINE_STRING_SEP);
    strcat (start, param);
    runfreia (start);
}

void __fastcall Tmainfunctions::Decrypt (bool BfromMAP)
{
    char param[128];
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    if (BfromMAP)
    {
        setupopendialog (FREIA_MAP_EXTENSION, MAP_FILTER);

        if (!opendialog->Execute ())
        {
            return;
        }

        sprintf (param, "-dm %c%s%c ", CMDLINE_STRING_SEP,
                 ExpandFileName (opendialog->FileName).c_str (),
                 CMDLINE_STRING_SEP);

        strcat (start, param);
    }
    else
    {
        strcat (start, "-d ");
    }

    runfreia (start);
}


//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Decryptmapfromfile1Click (TObject * Sender)
{
    Decrypt (true);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Decryptmapfromphone1Click (TObject * Sender)
{
    Decrypt (false);
}

void __fastcall Tmainfunctions::CRCcheck (bool Bfix)
{
    char param[256], filename[SYS_MAX_PATHNAME_LEN], start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    setupopendialog (FREIA_FLS_EXTENSION, FLS_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    strcpy (filename, ExpandFileName (opendialog->FileName).c_str ());

    sprintf (param, "-%s %c%s%c ", (Bfix ? "fc" : "cc"), CMDLINE_STRING_SEP,
             filename, CMDLINE_STRING_SEP);

    strcat (start, param);

    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::saveconfigurationonexitClick (TObject * Sender)
{
    saveconfigurationonexit->Checked = !saveconfigurationonexit->Checked;
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::FormDestroy (TObject * Sender)
{
    if (saveconfigurationonexit->Checked)
    {
        SaveConfiguration1Click (Sender);
    }
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Backupbatteryparametersfromphone1Click (TObject *
                                                                Sender)
{
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    strcat (start, "-bb ");
    runfreia (start);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Restorebatteryparametersfromfile1Click (TObject *
                                                                Sender)
{
    char start[256], cmdline[256], filename[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    setupopendialog (FREIA_MAP_EXTENSION, MAP_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    strcpy (filename, ExpandFileName (opendialog->FileName).c_str ());

    memset (cmdline, 0, sizeof (cmdline));
    sprintf (cmdline, "-sb %c%s%c", CMDLINE_STRING_SEP,
             filename, CMDLINE_STRING_SEP);

    strcat (start, cmdline);
    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::COMspeed230400Click (TObject * Sender)
{
    setcomspeed (230400, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMspeed460800Click (TObject * Sender)
{
    setcomspeed (460800, false);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::ReadflashfromphoneviaBFBandstoreasFLS1Click (TObject *
                                                                     Sender)
{
    readflash (true);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Createlogfromphone1Click (TObject * Sender)
{
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    strcat (start, "-wl ");
    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Createmapsfromlogs1Click (TObject * Sender)
{
    char param[128];
    char start[256];

    setupopendialog (FREIA_LOG_EXTENSION, LOG_FILTER);

    if (!opendialog->Execute ())
    {
        return;
    }

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
        return;

    sprintf (param, "-uml %c%s%c ", CMDLINE_STRING_SEP,
             ExtractFileDir (opendialog->FileName).c_str (),
             CMDLINE_STRING_SEP);
    strcat (start, param);
    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::COMport1dngClick (TObject * Sender)
{
    setcomdng (1, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport2dngClick (TObject * Sender)
{
    setcomdng (2, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport3dngClick (TObject * Sender)
{
    setcomdng (3, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport4dngClick (TObject * Sender)
{
    setcomdng (4, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::patchbootcore (bool Bfromfile)
{
    char start[256], cmdline[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    if (Bfromfile)
    {
        setupopendialog (FREIA_FLS_EXTENSION, FLS_FILTER);

        if (!opendialog->Execute ())
            return;

        sprintf (cmdline, "-pb %c%s%c %d", CMDLINE_STRING_SEP,
                 ExpandFileName (opendialog->FileName).c_str (),
                 CMDLINE_STRING_SEP, 1);
    }
    else
    {
        sprintf (cmdline, "-pb ");
    }

    strcat (start, cmdline);
    runfreia (start);
}

void __fastcall Tmainfunctions::Decrypt1Click (TObject * Sender)
{
    patchbootcore (false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::ReadandpatchbootcoreinFLSformat1Click (TObject * Sender)
{
    patchbootcore (true);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Getdongleinfo1Click (TObject * Sender)
{
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    strcat (start, "-gdi ");
    runfreia (start);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Uploaddongleinfo1Click (TObject * Sender)
{
    char start[256], cmdline[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    setupopendialog (FREIA_DI_EXTENSION, DI_FILTER);

    if (!opendialog->Execute ())
        return;

    sprintf (cmdline, "-sdi %c%s%c", CMDLINE_STRING_SEP,
             ExpandFileName (opendialog->FileName).c_str (),
             CMDLINE_STRING_SEP);

    strcat (start, cmdline);
    runfreia (start);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::Updatedongle1Click (TObject * Sender)
{
    char start[256], cmdline[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    setupopendialog (FREIA_DU_EXTENSION, DU_FILTER);

    if (!opendialog->Execute ())
        return;

    sprintf (cmdline, "-ud %c%s%c", CMDLINE_STRING_SEP,
             ExpandFileName (opendialog->FileName).c_str (),
             CMDLINE_STRING_SEP);

    strcat (start, cmdline);
    runfreia (start);
}

//---------------------------------------------------------------------------
void __fastcall Tmainfunctions::Testdongle1Click (TObject * Sender)
{
    char start[256];

    memset (start, 0, sizeof (start));
    if (!buildstartofcommand (start))
    {
        return;
    }

    strcat (start, "-td ");
    runfreia (start);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport5dngClick (TObject * Sender)
{
    setcomdng (5, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport6dngClick (TObject * Sender)
{
    setcomdng (6, false);
}

//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::COMport7dngClick (TObject * Sender)
{
    setcomdng (7, false);
}

void __fastcall Tmainfunctions::comport_RTSClick(TObject *Sender)
{
     setcomtype(comport_DTR->Checked, !comport_RTS->Checked, false);
}
//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::comport_DTRClick(TObject *Sender)
{
     setcomtype(!comport_DTR->Checked, comport_RTS->Checked, false);
}
//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::boottype1Click(TObject *Sender)
{
   setboottype (1, false);
}
//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::boottype2Click(TObject *Sender)
{
   setboottype (2, false);
}
//---------------------------------------------------------------------------

void __fastcall Tmainfunctions::boottype3Click(TObject *Sender)
{
   setboottype (3, false);
}
//---------------------------------------------------------------------------

