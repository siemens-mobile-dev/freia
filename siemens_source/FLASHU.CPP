//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#define __FROM_CPP__

#include <stdio.h>
#include "sysdef.h"
#include "config.h"
#include "freiapub.h"
#include "miscpub.h"

#include "flashu.h"
#include "phoneinfou.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OvalBtn"
#pragma link "UPlasmaForm"
#pragma resource "*.dfm"
Tflashingfunctions           *flashingfunctions;

//---------------------------------------------------------------------------
__fastcall Tflashingfunctions::Tflashingfunctions (TComponent *
                                                   Owner):TForm (Owner)
{
}

//---------------------------------------------------------------------------

void __fastcall Tflashingfunctions::OvalButton6Click (TObject * Sender)
{
    ModalResult = mrCancel;
}

//---------------------------------------------------------------------------

void __fastcall Tflashingfunctions::OvalButton2Click (TObject * Sender)
{
    ModalResult = mrOk;
}

//---------------------------------------------------------------------------

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
void __fastcall Tflashingfunctions::validatevalues (TObject * Sender)
{
    int i, start = 0, count = 0;
    char buffer[256];
    bool ishexstart, ishexcount;

    memset (buffer, 0, sizeof (buffer));
    Trim (startoffs->Text);
    startoffs->GetTextBuf (buffer, sizeof (buffer));
    for (i = 0; i < (UINT16) strlen (buffer); i++)
        buffer[i] = UpCase (buffer[i]);
    ishexstart = strchr (buffer, 'X');
    misc_readarg (buffer, &start, TRUE);

    memset (buffer, 0, sizeof (buffer));
    Trim (countfile->Text);
    countfile->GetTextBuf (buffer, sizeof (buffer));
    for (i = 0; i < (UINT16) strlen (buffer); i++)
        buffer[i] = UpCase (buffer[i]);
    ishexcount = strchr (buffer, 'X');

    misc_readarg (buffer, &count, TRUE);
    if (start + count > Tag)
        count = Tag - start;

    if (ishexstart)
        sprintf (buffer, "0x%lX", start);
    else
        sprintf (buffer, "%ld", start);
    startoffs->Text = buffer;

    if (ishexcount)
        sprintf (buffer, "0x%lX", count);
    else
        sprintf (buffer, "%ld", count);
    countfile->Text = buffer;
}

void __fastcall Tflashingfunctions::startoffsExit (TObject * Sender)
{
    validatevalues (Sender);
}

//---------------------------------------------------------------------------

void __fastcall Tflashingfunctions::countfileExit (TObject * Sender)
{
    validatevalues (Sender);
}

//---------------------------------------------------------------------------

bool __fastcall Tflashingfunctions::GetSelectedAddress (int entry_index,
                                                        long *startaddr,
                                                        long *endaddr)
{
    bool Bfound;
    tMISC_PHONE_MODEL            *pmodel;
    char                         *ptr;
    char line[128];

    strcpy (line,
            phoneinfo->phonemodel->Items->Strings[phoneinfo->phonemodel->
                                                  ItemIndex].c_str ());
    ptr = strchr (line, ' ');
    *startaddr = *endaddr = 0;

    if (ptr == NULL)
    {
        return false;
    }

    Bfound = MISC_GetPhoneModel (ptr + 1, &pmodel);
    if (!Bfound)
    {
        return false;
    }

    switch (entry_index)
    {
    default:
    case 0:                    /* firmware */
        *startaddr = pmodel->U32mcustartaddress;
        *endaddr = 0xFFFFFF;
        break;

    case 1:                    /* eeprom1 */
        if (pmodel->eeprom[0].U32size == 0)
        {
            return false;
        }

        *startaddr = pmodel->eeprom[0].U32start;
        *endaddr = pmodel->eeprom[0].U32start + pmodel->eeprom[0].U32size - 1;
        break;

    case 2:                    /* eeprom2 */
        if (pmodel->eeprom[1].U32size == 0)
        {
            return false;
        }

        *startaddr = pmodel->eeprom[1].U32start;
        *endaddr = pmodel->eeprom[1].U32start + pmodel->eeprom[1].U32size - 1;
        break;

    case 3:                    /* bootcore */
        if (pmodel->bootcore.U32size == 0)
        {
            return false;
        }

        *startaddr = pmodel->bootcore.U32start;
        *endaddr = pmodel->bootcore.U32start + pmodel->bootcore.U32size - 1;
        break;
    }

    return true;
}

void __fastcall Tflashingfunctions::presetClick (TObject * Sender)
{
    long start, end;
    int cnt;

    if (preset->ItemIndex > 3)
    {
        return;
    }

    if (!GetSelectedAddress (preset->ItemIndex, &start, &end))
    {
        MessageDlg ("This phone lacks of selected area", mtInformation,
                    TMsgDlgButtons () << mbOK, 0);
        return;
    }

    startaddr->Text = "0x" + IntToHex ((int) start, 6);
    endaddr->Text = "0x" + IntToHex ((int) end, 6);
    startoffs->Text = "0";
    countfile->Text =
        IntToStr ((end - start + 1 > Tag ? Tag : end - start + 1));
}
