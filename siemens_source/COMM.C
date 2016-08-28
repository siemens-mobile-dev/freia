
/********************************************************************
*                                                                   *
* FILE NAME:                                                        *
*                                                                   *
* PURPOSE:                                                          *
*                                                                   *
* FILE REFERENCES:                                                  *
*                                                                   *
* Name:                                                             *
*                                                                   *
* I/O:                                                              *
*                                                                   *
* Description:                                                      *
*                                                                   *
* EXTERNAL VARIABLES:                                               *
*                                                                   *
* Source:  <      >                                                 *
*                                                                   *
* Name:                                                             *
*                                                                   *
* Type:                                                             *
*                                                                   *
* I/O:                                                              *
*                                                                   *
* Description:                                                      *
*                                                                   *
*                                                                   *
* EXTERNAL REFERENCES:                                              *
*                                                                   *
* Name:                                                             *
*                                                                   *
* Description:                                                      *
*                                                                   *
*                                                                   *
* ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:      *
*                                                                   *
* ASSUMPTIONS, CONSTRAINTS, RESTRICTIONS:                           *
*                                                                   *
* NOTES:                                                            *
*                                                                   *
* REQUIREMENTS/FUNCTIONAL SPECIFICATIONS REFERENCES:                *
*                                                                   *
* DEVELOPMENT HISTORY:                                              *
*                                                                   *
* Date:                                                             *
*                                                                   *
* Author:                                                           *
*                                                                   *
* Change Id:                                                        *
*                                                                   *
* Release:                                                          *
*                                                                   *
* Description of change:                                            *
*                                                                   *
********************************************************************/

#include <Windows.h>
#include <stdio.h>
#include <ctype.h>

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "ttypub.h"
#include "secpub.h"
#include "dngpub.h"
#include "commloc.h"
#include "commpub.h"

extern eERROR                 freia_errno;
extern tFREIA_PHONEINFO       freia_phoneinfo;

static const UINT8            watermark[] =
    FREIA_WATERMARK_ID "1\x13\xfe\xe5\x07\x55\x10\x83\x47";

#include "loader.h"

/* Note that COMM_MAX_EEPROM_SIZE must be at least 0x10000, since
   flash sectors can be 65k as well (+1 is for checksum) */

static UINT8                  AU8iobuffer[COMM_MAX_EEPROM_SIZE + 1];

static BOOL                   Bbootisrunning = FALSE;
static BOOL                   Bbootspreloaded = FALSE;
static UINT8                  U8num_of_models = 0;
static UINT8                  U8comport_app;

static tCOMM_PHONEINFO        phoneinfo;
static UINT8                  xorruiner = 0x00;

static BOOL                   COMM_ReadFlashBlock (UINT32 U32startaddr,
                                                   UINT32 U32length);

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

static BOOL COMM_PreloadBoots (void)
{
    FILE                         *rfile;
    UINT16                        i;
    tENTRY                       *pentry = (tENTRY *) & AU8filebuffer[0];
    UINT32                        U32lastofs;

    if (Bbootspreloaded)
    {
        return TRUE;
    }

    i = 0;
    U32lastofs = 0;
    while (pentry->U32length > 0 && pentry->U32ofs >= U32lastofs)
    {
        i++;
        U32lastofs = pentry->U32ofs;
        pentry++;
    }

    U8num_of_models = i / COMM_MAX_NUM_OF_BOOTS_PER_MODEL;

    HDBGSTR1 ("COMM_PreloadBoots : number of phone models is %d",
              U8num_of_models);

    Bbootspreloaded = TRUE;
    return TRUE;
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

static BOOL COMM_WaitForCommandACK (UINT16 U16cmdack, UINT8 U16cmdnak,
                                    SINT8 S8acknakshift, UINT32 U32retry)
{
    BOOL                          Bfinished;
    BOOL                          Back = FALSE, Bnak = TRUE;
    BOOL                          Bshortack = HIBYTE (U16cmdack) == 0;
    BOOL                          Bshortnak = HIBYTE (U16cmdnak) == 0;

    do
    {
        MISC_GUIUpdate ();

        Bfinished =
            TTY_ReadComm (U8comport_app, AU8iobuffer, 0,
                          (Bshortack ? sizeof (UINT8) : sizeof (UINT16)));
        if (Bfinished)
        {
            Back =
                (Bshortack ? AU8iobuffer[S8acknakshift + 0] ==
                 U16cmdack : AU8iobuffer[S8acknakshift + 0] ==
                 LOBYTE (U16cmdack)
                 && AU8iobuffer[S8acknakshift + 1] == HIBYTE (U16cmdack));
            Bnak =
                (Bshortnak ? AU8iobuffer[S8acknakshift + 0] ==
                 U16cmdnak : AU8iobuffer[S8acknakshift + 0] ==
                 LOBYTE (U16cmdnak)
                 && AU8iobuffer[S8acknakshift + 1] == HIBYTE (U16cmdnak));

            if (U16cmdack == U16cmdnak && Back)
            {
                Bnak = FALSE;
            }

            Bfinished = Back && !Bnak;
        }
        if (!Bfinished)
        {
            MISC_Sleep (COMM_SMALL_DELAY);
        }
    } while (!Bfinished && U32retry--);

    return Bfinished && Back && !Bnak;
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

static BOOL COMM_SendRepeatableCommand (UINT32 U32retry, UINT8 U8cmd,
                                        UINT8 U8cmdack)
{
    BOOL                          Bfinished;

    do
    {
        MISC_GUIUpdate ();

        AU8iobuffer[0] = U8cmd;
        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
        {
            return FALSE;
        }

        Bfinished =
            TTY_ReadComm (U8comport_app, AU8iobuffer, 0, sizeof (AU8iobuffer));
        if (Bfinished)
        {
            Bfinished = AU8iobuffer[0] == U8cmdack;
        }
        else
        {
            MISC_Sleep (COMM_SMALL_DELAY);
        }
    } while (!Bfinished && U32retry--);

    return Bfinished;
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

static BOOL COMM_SendCommand (BOOL Bnoacknak, UINT32 U32requestedacksize,
                              UINT32 U32retry, UINT8 U8cmd, UINT16 U16cmdack,
                              UINT16 U16cmdnak, SINT8 S8acknakshift)
{
    BOOL                          Bfinished;
    BOOL                          Back = FALSE, Bnak = TRUE;
    BOOL                          Bshortack = HIBYTE (U16cmdack) == 0;
    BOOL                          Bshortnak = HIBYTE (U16cmdnak) == 0;

    AU8iobuffer[0] = U8cmd;
    if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
    {
        return FALSE;
    }

    MISC_Sleep (COMM_DEFAULT_DELAY);

    do
    {
        MISC_GUIUpdate ();

        Bfinished =
            TTY_ReadComm (U8comport_app, AU8iobuffer, U32requestedacksize,
                          sizeof (AU8iobuffer));

        if (Bfinished && !Bnoacknak)
        {
            Back =
                (Bshortack ? AU8iobuffer[S8acknakshift + 0] ==
                 U16cmdack : AU8iobuffer[S8acknakshift + 0] ==
                 LOBYTE (U16cmdack)
                 && AU8iobuffer[S8acknakshift + 1] == HIBYTE (U16cmdack));

            if (U16cmdnak == 0)
            {
                Bnak = !Back;
            }
            else
            {
                Bnak =
                    (Bshortnak ? AU8iobuffer[S8acknakshift + 0] ==
                     U16cmdnak : AU8iobuffer[S8acknakshift + 0] ==
                     LOBYTE (U16cmdnak)
                     && AU8iobuffer[S8acknakshift + 1] == HIBYTE (U16cmdnak));
            }
            Bfinished = Back || Bnak;
        }

        if (!Bfinished)
        {
            MISC_Sleep (COMM_DEFAULT_DELAY);
        }

    } while (!Bfinished && U32retry--);

    if (Bnoacknak)
    {
        Back =
            TTY_GetNumberOfReceivedBytes (U8comport_app) == U32requestedacksize;
        Bnak = !Back;
    }

    return Bfinished && Back && !Bnak;
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

static BOOL COMM_SendBoot (eMISC_MODEL_INDICES phonemodel, UINT8 U8file_index)
{
    tENTRY                       *pentry = (tENTRY *) & AU8filebuffer[0];
    UINT16                        U16length;
    UINT8                         U8checksum = 0;
    UINT32                        i;
    UINT16                        U16suggestedack;
    BOOL                          Bsendstrangepreboot;
    UINT8                        *ploader;
    UINT8                         AU8phuckedprebootend[] = {
        0x00, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00,
        0x04,
    };

    if (!COMM_PreloadBoots ())
    {
        return FALSE;
    }

    Bsendstrangepreboot = MISC_GetBootType () == MISC_BOOTTYPE_VIA_BOOTCORE_BUG
        && U8file_index == 0;

    if (phonemodel >= U8num_of_models)
    {
        LDBGSTR2 ("COMM_SendBoot : invalid phone model (%d/%d)", phonemodel,
                  U8num_of_models);
        return FALSE;
    }

    if (U8file_index >= COMM_MAX_NUM_OF_BOOTS_PER_MODEL)
    {
        LDBGSTR2 ("COMM_SendBoot : invalid file index (%d/%d)", U8file_index,
                  COMM_MAX_NUM_OF_BOOTS_PER_MODEL - 1);
        return FALSE;
    }

    HDBGSTR2 ("COMM_SendBoot : sending boot %d for model %d",
              U8file_index, phonemodel);

    pentry += (COMM_MAX_NUM_OF_BOOTS_PER_MODEL * phonemodel + U8file_index);

    /*
     * We don't really support bigger than 64k boots
     */
    U16length = pentry->U32length;

    if (HIBYTE (U16length) == 0)
    {
        if (!Bsendstrangepreboot)
        {
            AU8iobuffer[0] = U16length;
        }
        else
        {
            AU8iobuffer[0] = 0;
        }

        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
        {
            LDBGSTR ("COMM_SendBoot : cannot send file length");
            return FALSE;
        }
    }
    else
    {
        AU8iobuffer[0] = LOBYTE (U16length);
        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
        {
            LDBGSTR ("COMM_SendBoot : cannot send file length");
            return FALSE;
        }

        AU8iobuffer[0] = HIBYTE (U16length);
        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
        {
            LDBGSTR ("COMM_SendBoot : cannot send file length");
            return FALSE;
        }
    }

    ploader = &AU8filebuffer[pentry->U32ofs];
    if (Bsendstrangepreboot)
    {
        memcpy(AU8iobuffer, ploader, U16length);
        memset(&AU8iobuffer[U16length], 0xFF, 0x200-U16length);
        memcpy(&AU8iobuffer[0x200],AU8phuckedprebootend, sizeof(AU8phuckedprebootend));
        ploader = AU8iobuffer;
        U16length = COMM_SIZE_OF_MODIFIED_PREBOOT;
    }

    for (i = 0; i < (UINT32) U16length; i++)
    {
        U8checksum ^= ploader[i];
        ploader[i] ^= (UINT8) (xorruiner * i);
    }

    if (!TTY_WriteComm (U8comport_app, ploader, U16length))
    {
        LDBGSTR ("COMM_SendBoot : cannot send file");
        return FALSE;
    }

    if (!Bsendstrangepreboot)
    {
        AU8iobuffer[0] = U8checksum;

        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, 1))
        {
            LDBGSTR ("COMM_SendBoot : cannot send file checksum");
            return FALSE;
        }
    }

    if (U8file_index == 2)
    {
        U16suggestedack = COMM_2NDBOOT_ACK;
    }
    else if (U8file_index == 3)
    {
        U16suggestedack = COMM_3RDBOOT_ACK;
    }
    else
    {
        if (U8file_index == 0)
        {
            U16suggestedack = COMM_CMD_ACK;
        }
        else
        {
            U16suggestedack = COMM_CMD_ACK;
        }
    }

    HDBGSTR ("COMM_SendBoot : waiting for boot ack");
    if (!COMM_WaitForCommandACK
        (U16suggestedack, COMM_CMD_NAK, 0, COMM_DEFAULT_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_SendBoot : boot file is not accepted");
        return FALSE;
    }

    return TRUE;
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

static BOOL COMM_SendSpecialBoot (void)
{
    UINT8                         U8byte, U8checksum;
    UINT16                        i, U16length;
    UINT8                         AU8specialboot[COMM_SPECIALBOOT_LENGTH] =
        { 165, 90, 165, 165, 230, 88, 165, 0, 126, 182, 13, 255,
        83, 73, 69, 77, 69, 78, 83, 32, 66, 79, 79, 84,
        32, 80, 65, 82, 65, 77, 69, 84, 69, 82, 58, 1
    };

    memset (AU8iobuffer, 0, sizeof (AU8iobuffer));
    memcpy (AU8iobuffer, AU8specialboot, COMM_SPECIALBOOT_LENGTH);
    memcpy (&AU8iobuffer[COMM_SPECIALBOOT_LENGTH], "nutzoisthebest", 14);

    /*
     * We don't really support bigger than 64k boots 
     */

    U16length = COMM_SPECIALBOOT_LENGTH + 16;

    U8byte = U16length;
    if (!TTY_WriteComm (U8comport_app, &U8byte, 1))
    {
        LDBGSTR ("COMM_SendSpecialBoot : cannot send file length");
        return FALSE;
    }

    for (i = 0, U8checksum = 0; i < U16length; i++)
    {
        U8checksum ^= AU8iobuffer[i];
    }

    if (!TTY_WriteComm (U8comport_app, AU8iobuffer, U16length))
    {
        LDBGSTR ("COMM_SendSpecialBoot : cannot send file");
        return FALSE;
    }

    if (!TTY_WriteComm (U8comport_app, &U8checksum, 1))
    {
        LDBGSTR ("COMM_SendBoot : cannot send file checksum");
        return FALSE;
    }

    HDBGSTR ("COMM_SendSpecialBoot : waiting for boot ack");
    if (!COMM_WaitForCommandACK
        (COMM_CMD_ACK, COMM_CMD_NAK, 0, COMM_DEFAULT_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_SendSpecialBoot : boot file is not accepted");
        return FALSE;
    }

    if (!COMM_WaitForCommandACK
        (0x06, COMM_CMD_NAK, 0, COMM_DEFAULT_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_SendSpecialBoot : boot file is not accepted");
        return FALSE;
    }

    return TRUE;
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

BOOL COMM_UpdateIMEIBlocks (eMISC_MODEL_INDICES phonemodel, UINT8 * pB5009,
                            UINT8 * pB0001, UINT8 * pEnc5008, UINT8 * pEnc5077,
                            BOOL Bupdateflashimei)
{
    UINT8                         U8updatecmd = COMM_UPDATE_REQ;
    UINT16                        U16B5008Len, U16B5077Len;
    char                          desiredIMEI[FREIA_IMEI_LEN + 1];
    BOOL                          Bempty;

    SEC_GetBlockLen (phonemodel, &U16B5008Len, &U16B5077Len);

    if (!TTY_WriteComm (U8comport_app, &U8updatecmd, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_UpdateIMEIBlocks : cannot send update command");
        return FALSE;
    }

    if (!MISC_C30Like (phonemodel))
    {
        if (!TTY_WriteComm (U8comport_app, pB5009, FREIA_5009_AND_0001_LEN))
        {
            LDBGSTR ("COMM_UpdateIMEIBlocks : cannot send #5009");
            return FALSE;
        }

        if (!TTY_WriteComm (U8comport_app, pB0001, FREIA_5009_AND_0001_LEN))
        {
            LDBGSTR ("COMM_UpdateIMEIBlocks : cannot send #0001");
            return FALSE;
        }
    }

    if (!TTY_WriteComm (U8comport_app, pEnc5008, U16B5008Len))
    {
        LDBGSTR ("COMM_UpdateIMEIBlocks : cannot send #5008");
        return FALSE;
    }

    if (!MISC_C30Like (phonemodel))
    {
        if (!TTY_WriteComm (U8comport_app, pEnc5077, U16B5077Len))
        {
            LDBGSTR ("COMM_UpdateIMEIBlocks : cannot send #5077");
            return FALSE;
        }
    }

    if (!COMM_WaitForCommandACK
        (COMM_3RDBOOT_ACK, COMM_CMD_NAK, 0, COMM_UPDATE_COMMAND_RETRY))
    {
#ifndef LOGMODE
        LDBGSTR ("COMM_UpdateIMEIBlocks : update is not accepted");
#else
        printf ("phone update failed ;(\n");
#endif
        return FALSE;
    }

    if (Bupdateflashimei)
    {
#ifndef LOGMODE
        if (!MISC_PossibleToUpdateOriginalIMEI ())
        {
            LDBGSTR ("COMM_UpdateIMEIBlocks : original IMEI is locked");
            return TRUE;
        }

        if (!SEC_RecreateIMEI (phonemodel, pB5009, desiredIMEI, &Bempty))
        {
            LDBGSTR1 ("COMM_UpdateIMEIBlocks : IMEI '%s' is invalid",
                      desiredIMEI);
            return FALSE;
        }

        if (!Bempty)
        {
            if (!COMM_UpdateOriginalIMEI (desiredIMEI))
            {
                LDBGSTR ("COMM_UpdateIMEIBlocks : cannot update original IMEI");
                return FALSE;
            }
        }
        else
        {
            LDBGSTR ("COMM_UpdateIMEIBlocks : desired IMEI is empty");
        }
#endif
    }

#ifdef LOGMODE
    printf ("phone update OK.\n");
#endif
    return TRUE;
}


#ifndef LOGMODE

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

BOOL COMM_UpdateOriginalIMEI (char *IMEI)
{
    UINT8                         U8updatecmd = COMM_FLASHIMEI_REQ;
    UINT8                         AU8userarea[8];

    /*
     * Let's hope we have a cleared user register
     */
    memset (AU8userarea, 0xFF, sizeof (AU8userarea));

    if (!SEC_AdjustOriginalIMEI (IMEI))
    {
        return FALSE;
    }

    LDBGSTR1 ("COMM_UpdateOriginalIMEI : desired new original IMEI is '%s'",
              IMEI);

    SEC_NormalIMEIToBCDIMEI (IMEI, AU8userarea);

    if (!TTY_WriteComm (U8comport_app, &U8updatecmd, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_UpdateOriginalIMEI : cannot send update command");
        return FALSE;
    }

    if (!TTY_WriteComm (U8comport_app, AU8userarea, sizeof (AU8userarea)))
    {
        LDBGSTR ("COMM_UpdateOriginalIMEI : cannot send original IMEI");
        return FALSE;
    }

    if (!COMM_WaitForCommandACK
        (COMM_3RDBOOT_ACK, COMM_CMD_NAK, 0, COMM_UPDATE_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_UpdateOriginalIMEI : update is not accepted");
        return FALSE;
    }

    return TRUE;
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

BOOL COMM_UpdateBatteryBlock (eMISC_MODEL_INDICES phonemodel, UINT8 * pB67)
{
    UINT8                         U8updatecmd = COMM_SETBATTERY_REQ;

    if (!TTY_WriteComm (U8comport_app, &U8updatecmd, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_UpdateBatteryBlock : cannot send update command");
        return FALSE;
    }

    if (!MISC_C30Like (phonemodel))
    {
        if (!TTY_WriteComm (U8comport_app, pB67, FREIA_67_LEN))
        {
            LDBGSTR ("COMM_UpdateBatteryBlock : cannot send #67");
            return FALSE;
        }
    }

    if (!COMM_WaitForCommandACK
        (COMM_3RDBOOT_ACK, COMM_CMD_NAK, 0, COMM_UPDATE_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_UpdateBatteryBlock : update is not accepted");
        return FALSE;
    }

    return TRUE;
}
#endif

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

static BOOL COMM_IsBootRunning (void)
{
    if (!COMM_SendCommand
        (FALSE, 0, COMM_DEFAULT_COMMAND_RETRY, COMM_BOOTCODE_CHECK_REQ,
         COMM_BOOTCODE_CHECK_POS_REPLY, 0, 0))
    {
        Bbootisrunning = FALSE;
        if (*(UINT16 *) AU8iobuffer == COMM_UNKNOWN_FLASH_NAK)
        {
            LDBGSTR1 ("COMM_IsBootRunning : unknown flash id %04X",
                      *((UINT16 *) & AU8iobuffer[2]));
        }
        else
        {
            LDBGSTR
                ("COMM_IsBootRunning : bootcode does not seem to be running");
        }
        return FALSE;
    }

    Bbootisrunning = TRUE;
    return TRUE;
}

#ifndef LOGMODE

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

static BOOL COMM_GetVersion (UINT16 * pU16version)
{
    *pU16version = 0;
    if (!Bbootisrunning)
    {
        return FALSE;
    }

    if (!COMM_SendCommand
        (TRUE, sizeof (*pU16version), COMM_DEFAULT_COMMAND_RETRY,
         COMM_VERSION_REQ, COMM_BOOTCODE_CHECK_POS_REPLY, COMM_CMD_NAK, 1))
    {
        LDBGSTR ("COMM_GetVersion : cannot read loader version");
        return FALSE;
    }

    memcpy (pU16version, AU8iobuffer, sizeof (*pU16version));
    return TRUE;
}
#endif

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

static BOOL COMM_GetPhoneInfo (tCOMM_PHONEINFO * pphoneinfo)
{
#ifdef SAVE_PHONERAM
    FILE                         *rfile;
#endif
    memset (pphoneinfo, 0, sizeof (*pphoneinfo));
    if (!Bbootisrunning)
    {
        return FALSE;
    }

    if (!COMM_SendCommand
        (TRUE, sizeof (*pphoneinfo), COMM_DEFAULT_COMMAND_RETRY,
         COMM_PHONEINFO_REQ, COMM_BOOTCODE_CHECK_POS_REPLY, COMM_CMD_NAK, 1))
    {
        LDBGSTR ("COMM_GetPhoneInfo : cannot get phoneinfo");
        return FALSE;
    }

    memcpy (pphoneinfo, AU8iobuffer, sizeof (*pphoneinfo));
#ifdef SAVE_PHONERAM
    rfile = fopen ("phoneinfo.bin", "wb");
    if (rfile)
    {
        fwrite (pphoneinfo->AU8phoneinfo, 1, sizeof (pphoneinfo->AU8phoneinfo),
                rfile);
        fclose (rfile);
    }
#endif

    return TRUE;
}

#ifndef LOGMODE

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

static BOOL COMM_ChangeBaudrate (UINT32 U32baudrate)
{
    UINT8                         U8baudratecmd = COMM_CHANGESPEED_REQ;
    UINT8                         U8baudparam, U8baudreply;

    switch (U32baudrate)
    {
    case 57600:
        U8baudparam = 0;
        break;
    default:
        LDBGSTR1
            ("COMM_ChangeBaudrate : not supported baudrate %d, defaulting to 115200",
             U32baudrate);
    case 115200:
        U8baudparam = 1;
        break;
    case 230400:
        U8baudparam = 2;
        break;
    case 460800:
        U8baudparam = 3;
        break;
    }

    if (!TTY_WriteComm (U8comport_app, &U8baudratecmd, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_ChangeBaudrate : cannot send baudrate command");
        return FALSE;
    }

    if (!TTY_WriteComm (U8comport_app, &U8baudparam, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_ChangeBaudrate : cannot send baudrate param");
        return FALSE;
    }

    if (!TTY_ReadComm
        (U8comport_app, &U8baudreply, sizeof (U8baudreply),
         sizeof (U8baudreply)))
    {
        LDBGSTR1 ("COMM_ChangeBaudrate : not accepted baudrate %d",
                  U32baudrate);
        return FALSE;
    }

    if (U8baudreply != COMM_CHANGESPEED_PARAM_ACK)
    {
        LDBGSTR1 ("COMM_ChangeBaudrate : not accepted baudrate %d",
                  U32baudrate);
        return FALSE;
    }

    if (!TTY_SetSpeed (U8comport_app, U32baudrate))
    {
        LDBGSTR1
            ("COMM_ChangeBaudrate : cannot set baudrate %d, defaulting to 115200",
             U32baudrate);
        TTY_SetSpeed (U8comport_app, 115200);
        LDBGSTR1 ("COMM_ChangeBaudrate : Sleeping until timeout...",
                  U32baudrate);
        MISC_Sleep (1000 * 5);
        return TRUE;
    }

    U8baudparam = COMM_CHANGESPEED_CONFIRM;
    if (!TTY_WriteComm (U8comport_app, &U8baudparam, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_ChangeBaudrate : cannot send baudrate param");
        return FALSE;
    }

    if (!COMM_WaitForCommandACK
        (COMM_CHANGESPEED_REQ, COMM_CMD_NAK, 0, COMM_DEFAULT_COMMAND_RETRY))
    {
        LDBGSTR1 ("COMM_ChangeBaudrate : cannot set baudrate %d", U32baudrate);
        TTY_SetSpeed (U8comport_app, 115200);
        return FALSE;
    }

    return TRUE;
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
#ifdef USE_DONGLE
static BOOL COMM_DecryptLoaders (UINT8 * pKey)
{
    UINT8                         U8decryptcmd = COMM_DECRYPT_REQ;
    UINT16                        U16loaderkeylen =
        DNG_GetLoaderDecryptionKeyLen ();
    UINT16                        U16decryptofs = COMM_DECRYPT_OFS;
    UINT16                        U16decryptlen = COMM_DECRYPT_LEN;

    if (!TTY_WriteComm (U8comport_app, &U8decryptcmd, sizeof (UINT8)))
    {
        LDBGSTR ("COMM_DecryptLoaders : cannot send decrypt command");
        return FALSE;
    }

    MISC_Sleep (500);

    if (!TTY_WriteComm (U8comport_app, pKey, U16loaderkeylen))
    {
        LDBGSTR ("COMM_DecryptLoaders : cannot send decryption key");
        return FALSE;
    }

    MISC_Sleep (200);

    if (!TTY_WriteComm
        (U8comport_app, (UINT8 *) & U16decryptofs, sizeof (UINT16)))
    {
        LDBGSTR ("COMM_DecryptLoaders : cannot send decryption offset");
        return FALSE;
    }

    if (!TTY_WriteComm
        (U8comport_app, (UINT8 *) & U16decryptlen, sizeof (UINT16)))
    {
        LDBGSTR ("COMM_DecryptLoaders : cannot send decryption length");
        return FALSE;
    }

    if (!COMM_WaitForCommandACK
        (COMM_3RDBOOT_ACK, COMM_CMD_NAK, 0, COMM_DEFAULT_COMMAND_RETRY))
    {
        LDBGSTR ("COMM_DecryptLoaders : cannot decrypt loaders");
        return FALSE;
    }

    return TRUE;
}
#endif
#endif

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

BOOL COMM_LoadBoots (UINT8 U8comport, UINT32 U32speed,
                     eMISC_MODEL_INDICES phonemodel,
                     eMISC_MODEL_INDICES securitymodel)
{
    UINT16                        U16loaderversion;
    UINT8                         i;
    UINT8                         AU8localB5009[FREIA_5009_AND_0001_LEN];

#ifdef USE_DONGLE
    UINT8                         AU8loaderdecryptionkey[64];
#endif
    BOOL                          Bbootisalreadyrunning;
    BOOL                          Bempty;

    Bbootisrunning = FALSE;
    memset (&freia_phoneinfo, 0, sizeof (freia_phoneinfo));
    LDBGSTR1 ("COMM_LoadBoots : selected phone family is %d", phonemodel);

    if ((UINT32) sizeof (AU8iobuffer) < MISC_FLASH_BLOCKSIZE)
    {
        LDBGSTR2 ("COMM_LoadBoots : internal buffer is small (%d/%d)",
                  sizeof (AU8iobuffer), MISC_FLASH_BLOCKSIZE);
        return FALSE;
    }

#ifndef LOGMODE
    if (MISC_GetEmulation ())
    {
        Bbootisrunning = TRUE;
        freia_phoneinfo.U8version = 0;
        strcpy (freia_phoneinfo.languagepack, "lgX");
        strcpy (freia_phoneinfo.manufacturer, "SIEMENS");
        if (!MISC_GetPhoneModelName (freia_phoneinfo.model))
        {
            strcpy (freia_phoneinfo.model, "unknown");
        }

        freia_phoneinfo.AU32phoneid[0] = 0;
        freia_phoneinfo.AU32phoneid[1] = 0;
        freia_phoneinfo.U32flashid1 = 0;
        freia_phoneinfo.U32flashid2 = 0;
        freia_phoneinfo.U16flashid = 0;
        return TRUE;
    }
#endif

    xorruiner = MISC_SerialLoggerIsRunning ();

    if (!TTY_OpenCOMPort
        (U8comport, U32speed, COMM_DEFAULT_BOOTUP_DELAY, MISC_FLASH_BLOCKSIZE,
         MISC_GetAppCOMRTS (), MISC_GetAppCOMDTR ()))
    {
#ifndef LOGMODE
        LDBGSTR1 ("COMM_LoadBoots : cannot open COM %d", U8comport);
#else
        printf ("cannot open COM%d\n", U8comport);
#endif
        return FALSE;
    }

    Bbootisalreadyrunning = COMM_IsBootRunning ();

    if (!Bbootisalreadyrunning)
    {
#ifndef LOGMODE
        LDBGSTR ("COMM_LoadBoots : Waiting to power on the phone ...");
#else
        printf ("press power-on button shortly...\n");
#endif
        if (!COMM_SendRepeatableCommand
            (COMM_DEFAULT_BOOTUP_RETRY, COMM_BOOTUP_REQ, COMM_BOOTUP_ACK))
        {
#ifndef LOGMODE
            LDBGSTR ("COMM_LoadBoots : phone is not powered on");
#else
            printf ("phone is not powered on\n");
#endif
            return FALSE;
        }

        LDBGSTR ("COMM_LoadBoots : phone is powered on");
        if (!TTY_SetReadTimeout (U8comport_app, 40))
        {
            return FALSE;
        }

#ifdef LOGMODE
        printf ("sending boots...\n");
#endif

        if (MISC_GetBootType () == MISC_BOOTTYPE_VIA_PATCHED_BOOTCORE)
        {
            if (!COMM_SendSpecialBoot ())
            {
#ifdef LOGMODE
                printf ("preboot is not accepted\n");
#endif
                return FALSE;
            }
        }

        for (i = 0; i < COMM_MAX_NUM_OF_BOOTS_PER_MODEL; i++)
        {
            if (!COMM_SendBoot (phonemodel, i))
            {
#ifdef LOGMODE
                printf ("%d. boot is not accepted\n", i + 1);
#endif
                return FALSE;
            }
        }

        HDBGSTR ("COMM_LoadBoots : Checking bootcode..");
        Bbootisrunning = TRUE;
        if (!COMM_IsBootRunning ())
        {
#ifdef LOGMODE
            printf ("boot is not running\n");
#endif
            return FALSE;
        }
    }

#ifndef LOGMODE
    if (TTY_GetSpeed (U8comport_app) >= 57600)
    {
        COMM_ChangeBaudrate (TTY_GetSpeed (U8comport_app));
    }

    if (!COMM_GetVersion (&U16loaderversion))
    {
        return FALSE;
    }

    LDBGSTR2 ("COMM_LoadBoots : internal loader version is %02x.%02x",
              HIBYTE (U16loaderversion), LOBYTE (U16loaderversion));
    freia_phoneinfo.U16loaderversion = U16loaderversion;
#endif

    if (COMM_GetPhoneInfo (&phoneinfo))
    {
#ifndef LOGMODE
        freia_phoneinfo.U8version = phoneinfo.AU8phoneinfo[COMM_VERSION_OFFSET];
#endif

        freia_phoneinfo.AU32phoneid[0] = phoneinfo.U32phoneid1;
        freia_phoneinfo.AU32phoneid[1] = phoneinfo.U32phoneid2;
        freia_phoneinfo.U32flashid1 = XCHGWORDS (phoneinfo.U32flashhc1);
        freia_phoneinfo.U32flashid2 = XCHGWORDS (phoneinfo.U32flashhc2);
        freia_phoneinfo.U16flashid = phoneinfo.U16flashhcram;

        if (!MISC_C30Like (phonemodel))
        {
#ifndef LOGMODE
            strncpy (freia_phoneinfo.languagepack,
                     (char *) &phoneinfo.AU8phoneinfo[COMM_LANGUAGEPACK_OFFSET],
                     sizeof (freia_phoneinfo.languagepack) - 1);
            strncpy (freia_phoneinfo.manufacturer,
                     (char *) &phoneinfo.AU8phoneinfo[COMM_MANUFACTURER_OFFSET],
                     sizeof (freia_phoneinfo.manufacturer) - 1);
            strncpy (freia_phoneinfo.model,
                     (char *) &phoneinfo.AU8phoneinfo[COMM_MODEL_OFFSET],
                     sizeof (freia_phoneinfo.model) - 1);

            if (strstr (freia_phoneinfo.manufacturer, "SIEMENS") == NULL)
            {
                strcpy (freia_phoneinfo.languagepack, "lgX");
                strcpy (freia_phoneinfo.manufacturer, "SIEMENS");
                if (!MISC_GetPhoneModelName (freia_phoneinfo.model))
                {
                    strcpy (freia_phoneinfo.model, "unknown");
                }
            }
#endif
        }

        if (!SEC_RecreateIMEI
            (securitymodel, phoneinfo.B5009, freia_phoneinfo.IMEI, &Bempty))
        {
#ifndef LOGMODE
            LDBGSTR1 ("COMM_LoadBoots : EEPROM IMEI (%s) is corrupt",
                      freia_phoneinfo.IMEI);
#else
            printf ("EEPROM imei is bad, please change it in the log\n");
#endif
            memset (freia_phoneinfo.IMEI, 0, sizeof (freia_phoneinfo.IMEI));
        }
        else
        {
#ifndef LOGMODE
            if (Bempty)
            {
                LDBGSTR ("COMM_LoadBoots : EEPROM IMEI is empty");
            }
            else
            {
                LDBGSTR1 ("COMM_LoadBoots : EEPROM IMEI is %s",
                          freia_phoneinfo.IMEI);
            }
#else
            if (Bempty)
            {
                printf ("Found empty EEPROM IMEI\n");
            }
            else
            {
                printf ("Found EEPROM IMEI '%s'\n", freia_phoneinfo.IMEI);
            }
#endif
        }

        freia_phoneinfo.U16lockstatus = phoneinfo.U16lockstatus;
        memcpy (freia_phoneinfo.originalB5009, phoneinfo.originalB5009,
                sizeof (freia_phoneinfo.originalB5009));

        memset (AU8localB5009, 0, sizeof (AU8localB5009));
        memcpy (AU8localB5009, phoneinfo.originalB5009,
                sizeof (phoneinfo.originalB5009));
        AU8localB5009[7] = SEC_CreateCRCDigit (AU8localB5009);

        freia_phoneinfo.BoriginalIMEI = FALSE;

        if (!SEC_BCDIMEIToNormalIMEI
            (AU8localB5009, freia_phoneinfo.originalIMEI, &Bempty))
        {
#ifndef LOGMODE
            LDBGSTR1 ("COMM_LoadBoots : original IMEI (%s) is corrupt",
                      freia_phoneinfo.originalIMEI);
#else
            printf ("original imei is bad, please change it in the log\n");
#endif
            memset (freia_phoneinfo.originalIMEI, 0,
                    sizeof (freia_phoneinfo.originalIMEI));
        }
        else
        {
            if (Bempty)
            {
#ifndef LOGMODE
                LDBGSTR1 ("COMM_LoadBoots : original IMEI is empty (%s)",
                          (MISC_PossibleToUpdateOriginalIMEI ()?
                           "not locked" : "locked"));
#else
                printf ("original IMEI is empty (%s)\n",
                        (MISC_PossibleToUpdateOriginalIMEI ()? "not locked"
                         : "locked"));
#endif
            }
            else
            {
                freia_phoneinfo.BoriginalIMEI = TRUE;

#ifndef LOGMODE
                LDBGSTR2 ("COMM_LoadBoots : original IMEI is %s (%s)",
                          freia_phoneinfo.originalIMEI,
                          (MISC_PossibleToUpdateOriginalIMEI ()?
                           "not locked" : "locked"));
#else
                printf ("original IMEI is %s (%s)\n",
                        freia_phoneinfo.originalIMEI,
                        (MISC_PossibleToUpdateOriginalIMEI ()?
                         "not locked" : "locked"));
#endif
            }
        }

#ifndef LOGMODE
        if (!MISC_C30Like (phonemodel))
        {
            LDBGSTR5 ("COMM_LoadBoots : found %s %s %s v%x (%d)",
                      freia_phoneinfo.manufacturer, freia_phoneinfo.model,
                      freia_phoneinfo.languagepack, freia_phoneinfo.U8version,
                      phoneinfo.BC35_Type);
        }

        LDBGSTR3 ("COMM_LoadBoots : flash ids are " UINT32_FORMAT " ["
                  UINT32_FORMAT "] - %04X", freia_phoneinfo.U32flashid1,
                  freia_phoneinfo.U32flashid2, freia_phoneinfo.U16flashid);

        LDBGSTR2 ("COMM_LoadBoots : phone ids are " UINT32_FORMAT " ["
                  UINT32_FORMAT "]", freia_phoneinfo.AU32phoneid[0],
                  freia_phoneinfo.AU32phoneid[1]);
#else
        printf ("flash ids are " UINT32_FORMAT " ["
                UINT32_FORMAT "] - %04X\n", freia_phoneinfo.U32flashid1,
                freia_phoneinfo.U32flashid2, freia_phoneinfo.U16flashid);

        printf ("phone ids are " UINT32_FORMAT " ["
                UINT32_FORMAT "]\n", freia_phoneinfo.AU32phoneid[0],
                freia_phoneinfo.AU32phoneid[1]);
#endif
    }
    else
    {
        freia_phoneinfo.U8version = 0;
        strcpy (freia_phoneinfo.languagepack, "lgX");
        strcpy (freia_phoneinfo.manufacturer, "SIEMENS");
        if (!MISC_GetPhoneModelName (freia_phoneinfo.model))
        {
            strcpy (freia_phoneinfo.model, "unknown");
        }

        freia_phoneinfo.AU32phoneid[0] = 0;
        freia_phoneinfo.AU32phoneid[1] = 0;
        freia_phoneinfo.U32flashid1 = 0;
        freia_phoneinfo.U32flashid2 = 0;
        freia_phoneinfo.U16flashid = 0;
    }

#ifndef LOGMODE
#ifdef USE_DONGLE
    if (!Bbootisalreadyrunning)
    {
        if (DNG_GetLoaderDecryptionKey
            (freia_phoneinfo.AU32phoneid[0], AU8loaderdecryptionkey))
        {
            COMM_DecryptLoaders (AU8loaderdecryptionkey);
        }
    }
#endif
#endif

    return TRUE;
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

void COMM_Reboot (void)
{
    UINT8                         U8rebootcmd = COMM_REBOOT_REQ;

    if (Bbootisrunning && !MISC_GetEmulation ())
    {
        TTY_WriteComm (U8comport_app, &U8rebootcmd, sizeof (UINT8));
    }
}

#ifndef LOGMODE

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

static BOOL COMM_NormalReadFlashBlock (UINT32 U32startaddr, UINT32 U32length)
{
    UINT32                        U32idx = 0;
    UINT16                        U16blocklen;
    UINT32                        U32tmpnum;
    UINT16                        readack;
    struct
    {
        UINT8                         U8mode;
        UINT8                         U8startaddr[3];
        UINT8                         U8size[3];
    }
    readcmd;
    UINT16                        i, U16crc;
    UINT8                         U8checksum, U8num_of_retries;
    BOOL                          Bread;
    UINT32                        U32originalreadtimeout =
        TTY_GetReadTimeout (U8comport_app);
    readcmd.U8mode = 'R';
    U8num_of_retries = COMM_DEFAULT_RETRIES;
    while (U32idx < U32length)
    {
        MISC_GaugeUpdate (U32startaddr + U32idx);
        if (!TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout))
        {
            return FALSE;
        }

        U16blocklen = COMM_FLASHBLOCK_CHUNK_SIZE;
        if (U32length - U32idx < (UINT32) U16blocklen)
        {
            U16blocklen = U32length - U32idx;
        }

        U32tmpnum = MISC_SwapDword ((U32startaddr + U32idx)) >> 8;
        memcpy (readcmd.U8startaddr, &U32tmpnum, sizeof (readcmd.U8startaddr));
        U32tmpnum = MISC_SwapDword ((U16blocklen)) >> 8;
        memcpy (readcmd.U8size, &U32tmpnum, sizeof (readcmd.U8size));
        if (!TTY_WriteComm
            (U8comport_app, (UINT8 *) & readcmd, sizeof (readcmd)))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            return FALSE;
        }

        Bread = TRUE;
        TTY_SetReadTimeout (U8comport_app,
                            U32originalreadtimeout * (10 * U16blocklen / 1024));
        if (!TTY_ReadComm
            (U8comport_app, &AU8iobuffer[U32idx], U16blocklen, U16blocklen))
        {
            LDBGSTR1 ("COMM_NormalReadFlashBlock : cannot read flash block "
                      UINT32_FORMAT " retrying", U32startaddr + U32idx);
            Bread = FALSE;
        }

        if (Bread && !TTY_ReadComm
            (U8comport_app, (UINT8 *) & readack, sizeof (readack),
             sizeof (readack)))
        {
            LDBGSTR1 ("COMM_NormalReadFlashBlock : read error at "
                      UINT32_FORMAT " retrying", U32startaddr + U32idx);
            Bread = FALSE;
        }

        if (Bread && readack != COMM_3RDBOOT_ACK)   /* ?OK */
        {
            LDBGSTR1 ("COMM_NormalReadFlashBlock : read error at "
                      UINT32_FORMAT " retrying", U32startaddr + U32idx);
            Bread = FALSE;
        }

        if (Bread
            && !TTY_ReadComm (U8comport_app, (UINT8 *) & U16crc,
                              sizeof (U16crc), sizeof (U16crc)))
        {
            LDBGSTR1 ("COMM_NormalReadFlashBlock : read error at "
                      UINT32_FORMAT " retrying", U32startaddr + U32idx);
            Bread = FALSE;
        }

        for (i = 0, U8checksum = 0; i < U16blocklen; i++)
        {
            U8checksum ^= AU8iobuffer[U32idx + i];
        }

        if (Bread && U8checksum != U16crc)
        {
            LDBGSTR1 ("COMM_NormalReadFlashBlock : checksum error at "
                      UINT32_FORMAT " retrying", U32startaddr + U32idx);
            Bread = FALSE;
        }

        if (Bread)
        {
            U32idx += U16blocklen;
        }
        else
        {
            U8num_of_retries--;
            if (U8num_of_retries == 0)
            {
                LDBGSTR1 ("COMM_NormalReadFlashBlock : read error at "
                          UINT32_FORMAT, U32startaddr + U32idx);
                TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
                return FALSE;
            }
        }
    }

    TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
    return TRUE;
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

static BOOL COMM_EmulatedReadFlashBlock (UINT32 U32startaddr, UINT32 U32length)
{
    FILE                         *rfile;
    BOOL                          Bsuccess;

    if (!Bbootisrunning)
    {
        LDBGSTR ("COMM_EmulatedReadFlashBlock : boot is not running");
        return FALSE;
    }

    HDBGSTR1 ("COMM_EmulatedReadFlashBlock : reading block " UINT32_FORMAT,
              U32startaddr);
    if (!MISC_OpenEmulatedFlashFile (&rfile, "rb", U32startaddr))
    {
        return FALSE;
    }

    MISC_GaugeUpdate (U32startaddr);
    Bsuccess =
        (UINT32) fread (AU8iobuffer, sizeof (UINT8), U32length,
                        rfile) == U32length;
    fclose (rfile);
    return Bsuccess;
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

static BOOL COMM_ReadFlashBlock (UINT32 U32startaddr, UINT32 U32length)
{
    if (MISC_GetEmulation ())
    {
        return COMM_EmulatedReadFlashBlock (U32startaddr, U32length);
    }

    return COMM_NormalReadFlashBlock (U32startaddr, U32length);
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

static BOOL COMM_CopyBlockWithoutFF00 (UINT8 U8eepromidx,
                                       UINT32 U32eepromlen,
                                       UINT16 U16blockid,
                                       UINT8 * pAU8block, UINT16 U16destlen)
{
    UINT32                        U32idx = 0;
    tCOMM_BLOCKHEADER             blockheader;
    UINT16                        U16blocksegmin, U16blocksegmax;
    UINT32                        U32offs, U32start, U32size;

    if (!MISC_GetEEPROMAddress (U8eepromidx, &U32start, &U32size))
    {
        LDBGSTR1
            ("COMM_CopyBlock : cannot find eeprom block %d info", U8eepromidx);
        return FALSE;
    }

    U16blocksegmin = (U32start & 0xFFF0000) >> 16;
    U16blocksegmax = ((U32start + U32size) & 0xFFF0000) >> 16;
    do
    {
        memcpy (&blockheader, &AU8iobuffer[U32idx], sizeof (blockheader));
#if 0
        if (blockheader.U16blockid == U16blockid)
        {
            LDBGSTR5 ("COMM_CopyBlock : %d, %04X, %04X%04X,%04X",
                      blockheader.U16blocklength, blockheader.U16blockid,
                      blockheader.U16blockseg, blockheader.U16blockoffs,
                      blockheader.U16endid);
        }
#endif
        if ((blockheader.U16blockid == U16blockid
             || blockheader.U16blockid == COMM_DEFAULT_PHONEBLOCK_ID)
            && blockheader.U16blockseg >= U16blocksegmin
            && blockheader.U16blockseg <= U16blocksegmax
            && blockheader.U16blocklength == U16destlen
            && (blockheader.U16endid == COMM_EEPROM_BLOCKHEADER_END_ID1
                || blockheader.U16endid == COMM_EEPROM_BLOCKHEADER_END_ID2))
        {
            /*
             * it's a 0xFF000 based offset
             */

            U32offs =
                (UINT32) blockheader.U16blockseg * 0x10000 +
                blockheader.U16blockoffs - U32start;
            memcpy (pAU8block, &AU8iobuffer[U32offs], U16destlen);
            return TRUE;
        }

        U32idx++;
    } while (U32idx < U32eepromlen
             && (UINT32) (U32idx + sizeof (blockheader)) <
             (UINT32) sizeof (AU8iobuffer));
    return FALSE;
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

static BOOL COMM_CopyBlockWithFF00 (UINT8 U8eepromidx,
                                    UINT32 U32eepromlen,
                                    UINT16 U16blockid,
                                    UINT8 * pAU8block, UINT16 U16destlen)
{
    UINT32                        U32idx = 0;
    tCOMM_BLOCKHEADER             blockheader;
    UINT16                        U16blocksegmin, U16blocksegmax;
    UINT32                        U32offs, U32start, U32size;

    if (!MISC_GetEEPROMAddress (U8eepromidx, &U32start, &U32size))
    {
        LDBGSTR1
            ("COMM_CopyBlock : cannot find eeprom block %d info", U8eepromidx);
        return FALSE;
    }

    U16blocksegmin = (U32start & 0xFFF0000) >> 16;
    U16blocksegmax = ((U32start + U32size) & 0xFFF0000) >> 16;
    do
    {
        memcpy (&blockheader, &AU8iobuffer[U32idx], sizeof (blockheader));
#if 0
        if (blockheader.U16blockid == U16blockid)
        {
            LDBGSTR5 ("COMM_CopyBlock : %d, %04X, %04X%04X,%04X",
                      blockheader.U16blocklength, blockheader.U16blockid,
                      blockheader.U16blockseg, blockheader.U16blockoffs,
                      blockheader.U16endid);
        }
#endif
        if ((blockheader.U16blockid == U16blockid
             || blockheader.U16blockid == COMM_DEFAULT_PHONEBLOCK_ID)
            && blockheader.U16blockseg >= U16blocksegmin
            && blockheader.U16blockseg <= U16blocksegmax
            && blockheader.U16blocklength == U16destlen
            && blockheader.U16endid == COMM_EEPROM_BLOCKHEADER_END_ID3)
        {
            /*
             * it's a 0xFF000 based offset
             */

            U32offs =
                (UINT32) blockheader.U16blockseg * 0x10000 +
                blockheader.U16blockoffs - U32start;
            memcpy (pAU8block, &AU8iobuffer[U32offs], U16destlen);
            return TRUE;
        }

        U32idx++;
    } while (U32idx < U32eepromlen
             && (UINT32) (U32idx + sizeof (blockheader)) <
             (UINT32) sizeof (AU8iobuffer));
    return FALSE;
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

static BOOL COMM_CopyBlock (UINT8 U8eepromidx,
                            UINT32 U32eepromlen,
                            UINT16 U16blockid,
                            UINT8 * pAU8block, UINT16 U16destlen)
{
    if (COMM_CopyBlockWithoutFF00
        (U8eepromidx, U32eepromlen, U16blockid, pAU8block, U16destlen))
    {
        return TRUE;
    }

    return COMM_CopyBlockWithFF00
        (U8eepromidx, U32eepromlen, U16blockid, pAU8block, U16destlen);
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

static BOOL COMM_CopyX30Block (eMISC_MODEL_INDICES phonemodel,
                               UINT16 U16eeprom_offset,
                               UINT16 U16blockid1, UINT16 U16blockid2,
                               UINT8 * pAU8block, UINT16 U16destlen)
{
    tCOMM_BLOCKHEADER             blockheader;
    UINT16                        U16blocksegmin, U16blocksegmax;
    UINT32                        U32offs, U32start, U32size;
    UINT16                        r6, r4, r1, r7, *r2 =
        (UINT16 *) & AU8iobuffer[U16eeprom_offset + 6];

    r1 = 0;
    r7 = 0x2000;

    do
    {
        r4 = r2[1];
        r4++;
        r4 &= 0xFE;

        if (U16blockid1 == r2[0] && U16blockid2 == r2[1])
        {
            r6 = (phonemodel == MISC_MODEL_INDEX_C30 ? 0x2000 : 0x4000);
            r6 -= LOBYTE (U16blockid2);
            r6 -= r1;

            memcpy (pAU8block, &AU8iobuffer[U16eeprom_offset + r6], U16destlen);
            return TRUE;
        }

        r2 += 2;
        r1 += r4;
        r7 -= 4;
    } while (r7 > 0);

    return FALSE;
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

static UINT16 COMM_DetectX30StartOfEEPROM (eMISC_MODEL_INDICES phonemodel,
                                           UINT8 * pEEPROM)
{
    UINT16                       *pEEP = (UINT16 *) pEEPROM;

    if (pEEP[0] == 0xFFFF && pEEP[1] == 0xFFFF && pEEP[2] == 0xF0F0
        && pEEP[3] == 0xFFFF)
    {
        return (phonemodel == MISC_MODEL_INDEX_C30 ? 0x2000 : 0x4000);
    }

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

static BOOL COMM_BackupX30 (eMISC_MODEL_INDICES phonemodel,
                            char *IMEI, UINT8 * pEnc5008)
{
    const tCOMM_X30BLOCK          C30indices[] = {
        {0x08, 0x0C, 0x000A},
        {0x08, 0x0C, 0x09E4},
        {0x08, 0x22, 0x0F40},
        {0x08, 0x40, 0x0F62},
        {0x08, 0x10, 0x0FA2},
        {0x08, 0x2E, 0x0FB2},
        {0x08, 0x44, 0x0FE0},
        {0x06, 0x3E, 0x0A40},
        {0x06, 0x40, 0x0A7E},
        {0x06, 0x40, 0x0ABE},
        {0x06, 0x42, 0x0AFE},
        {0x06, 0x3E, 0x0B40},
        {0x06, 0x40, 0x0B7E},
        {0x06, 0x40, 0x0BBE},
        {0x06, 0x42, 0x0BFE},
        {0x06, 0x3E, 0x0C40},
        {0x06, 0x40, 0x0C7E},
        {0x06, 0x40, 0x0CBE},
        {0x06, 0x42, 0x0CFE},
        {0x06, 0x3E, 0x0D40},
        {0x06, 0x40, 0x0D7E},
        {0x06, 0x40, 0x0DBE},
        {0x06, 0x42, 0x0DFE},
        {0x06, 0x3E, 0x0E40},
        {0x06, 0x40, 0x0E7E},
        {0x06, 0x40, 0x0EBE},
        {0x06, 0x42, 0x0EFE},
        {0x06, 0x00, 0x0000}
    };

    const tCOMM_X30BLOCK          S40indices[] = {
        {0x08, 0x0C, 0x000A},
        {0x08, 0x24, 0x06E6},
        {0x08, 0x22, 0x0CFE},
        {0x08, 0x40, 0x0D20},
        {0x08, 0x10, 0x0D60},
        {0x08, 0x2E, 0x0D70},
        {0x08, 0x44, 0x0D9E},
        {0x06, 0x3E, 0x07FE},
        {0x06, 0x40, 0x083C},
        {0x06, 0x40, 0x087C},
        {0x06, 0x42, 0x08BC},
        {0x06, 0x3E, 0x08FE},
        {0x06, 0x40, 0x093C},
        {0x06, 0x40, 0x097C},
        {0x06, 0x42, 0x09BC},
        {0x06, 0x3E, 0x09FE},
        {0x06, 0x40, 0x0A3C},
        {0x06, 0x40, 0x0A7C},
        {0x06, 0x42, 0x0ABC},
        {0x06, 0x3E, 0x0AFE},
        {0x06, 0x40, 0x0B3C},
        {0x06, 0x40, 0x0B7C},
        {0x06, 0x42, 0x0BBC},
        {0x06, 0x3E, 0x0BFE},
        {0x06, 0x40, 0x0C3C},
        {0x06, 0x40, 0x0C7C},
        {0x06, 0x42, 0x0CBC},
        {0x06, 0x00, 0x0000}
    };

    const tCOMM_X30BLOCK         *eeprom_blocks =
        (phonemodel == MISC_MODEL_INDEX_C30 ? C30indices : S40indices);

    UINT16                        i, U16eeprom_offset;
    UINT16                        U16destlen, U16blockid1, U16blockid2;
    UINT32                        U32startaddress, U32size;
    UINT8                        *pblock = pEnc5008;
    BOOL                          Bempty;
    UINT8                         AU8tmp[32];
    BOOL                          BfoundIMEI = FALSE;

    if (!MISC_GetEEPROMAddress (0, &U32startaddress, &U32size))
    {
        LDBGSTR ("COMM_Backup : cannot find eeprom addresses");
        return FALSE;
    }

    if (U32size > sizeof (AU8iobuffer))
    {
        LDBGSTR2 ("COMM_Backup : detected EEPROM size is too big (%d/%d)",
                  U32size, COMM_MAX_EEPROM_SIZE);
        return FALSE;
    }

    MISC_GaugeInit (U32startaddress, U32startaddress + U32size);

    if (!COMM_ReadFlashBlock (U32startaddress, U32size))
    {
        MISC_GaugeDone ();
        return FALSE;
    }

    MISC_GaugeDone ();

    U16eeprom_offset = COMM_DetectX30StartOfEEPROM (phonemodel, AU8iobuffer);

    i = 0;
    do
    {
        U16blockid1 = eeprom_blocks[i].U16id;
        U16blockid2 = eeprom_blocks[i].U8id2;
        U16destlen = eeprom_blocks[i].U8id2;

        if (U16blockid1 == 0x00)
        {
            break;
        }

        if (!COMM_CopyX30Block
            (phonemodel, U16eeprom_offset, U16blockid1, U16blockid2 | 0x8000,
             pblock, U16destlen))
        {
            LDBGSTR2 ("COMM_Backup : cannot find block id %04X/%04X",
                      U16blockid1, U16blockid2);
            return FALSE;
        }

        if (!BfoundIMEI && U16blockid1 == 0x0A && U16blockid2 == 0x0C)
        {
            memcpy (AU8tmp, pblock, U16destlen);

            if (SEC_RecreateIMEI (phonemodel, AU8tmp, IMEI, &Bempty))
            {
                BfoundIMEI = TRUE;

                if (Bempty)
                {
                    LDBGSTR ("COMM_Backup : found EEPROM IMEI is empty");
                }
                else
                {
                    LDBGSTR1 ("COMM_Backup : found EEPROM IMEI is %s", IMEI);
                }
            }
        }

        pblock += U16destlen;
        i++;
    } while (U16blockid1 != 0);

    return TRUE;
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

BOOL COMM_Backup (eMISC_MODEL_INDICES phonemodel,
                  char *IMEI, UINT8 * pB5009,
                  UINT8 * pB0001, UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    BOOL                          B5009found = FALSE, B0001found =
        FALSE, Enc5008found = FALSE, Enc5077found = FALSE;
    UINT16                        U16B5008Len, U16B5077Len;
    UINT32                        U32startaddress, U32size;
    UINT8                         AU8B5009[FREIA_5009_AND_0001_LEN];
    UINT8                         U8eepromidx = 0;
    BOOL                          Bempty;

    if (!Bbootisrunning)
    {
        LDBGSTR ("COMM_Backup : boot is not running");
        return FALSE;
    }

    if (MISC_C30Like (phonemodel))
    {
        return COMM_BackupX30 (phonemodel, IMEI, pEnc5008);
    }

    SEC_GetBlockLen (phonemodel, &U16B5008Len, &U16B5077Len);
    if (!MISC_GetEEPROMAddress (U8eepromidx, &U32startaddress, &U32size))
    {
        LDBGSTR ("COMM_Backup : cannot find eeprom addresses");
        return FALSE;
    }

    if (U32size > sizeof (AU8iobuffer))
    {
        LDBGSTR2 ("COMM_Backup : detected EEPROM size is too big (%d/%d)",
                  U32size, COMM_MAX_EEPROM_SIZE);
        return FALSE;
    }

    MISC_GaugeInit (U32startaddress, U32startaddress + U32size);
    while ((!B5009found || !B0001found || !Enc5008found || !Enc5077found)
           && U8eepromidx < MISC_MAX_EEPROM_BLOCKS)
    {
        if (!COMM_ReadFlashBlock (U32startaddress, U32size))
        {
            MISC_GaugeDone ();
            return FALSE;
        }

        if (!B5009found)
        {
            if (COMM_CopyBlock
                (U8eepromidx, U32size, 5009, pB5009, FREIA_5009_AND_0001_LEN))
            {
                memcpy (AU8B5009, pB5009, sizeof (AU8B5009));
                if (SEC_RecreateIMEI (phonemodel, AU8B5009, IMEI, &Bempty))
                {
                    B5009found = TRUE;
                    if (Bempty)
                    {
                        LDBGSTR ("COMM_Backup : found EEPROM IMEI is empty");
                    }
                    else
                    {
                        LDBGSTR1 ("COMM_Backup : found EEPROM IMEI is %s",
                                  IMEI);
                    }
                }
            }
        }

        if (!B0001found)
        {
            if (COMM_CopyBlock
                (U8eepromidx, U32size, 76, pB0001, FREIA_5009_AND_0001_LEN))
            {
                B0001found = TRUE;
            }
        }

        if (!Enc5008found)
        {
            if (COMM_CopyBlock
                (U8eepromidx, U32size, 5008, pEnc5008, U16B5008Len))
            {
                Enc5008found = TRUE;
            }
        }

        if (!Enc5077found)
        {
            if (COMM_CopyBlock
                (U8eepromidx, U32size, 5077, pEnc5077, U16B5077Len))
            {
                Enc5077found = TRUE;
            }
        }

        if (++U8eepromidx < MISC_MAX_EEPROM_BLOCKS)
        {
            if (!MISC_GetEEPROMAddress
                (U8eepromidx, &U32startaddress, &U32size))
            {
                LDBGSTR ("COMM_Backup : cannot find eeprom addresses");
                return FALSE;
            }

            if (U32size > sizeof (AU8iobuffer))
            {
                LDBGSTR2
                    ("COMM_Backup : detected EEPROM size is too big (%d/%d)",
                     U32size, COMM_MAX_EEPROM_SIZE);
                return FALSE;
            }

            if (U32startaddress != 0)
            {
                MISC_GaugeDone ();
                MISC_GaugeInit (U32startaddress, U32startaddress + U32size);
            }
        }

    }

    MISC_GaugeDone ();
    if (!B5009found)
    {
        LDBGSTR ("COMM_Backup : 5009 is not found");
    }

    if (!B0001found)
    {
        LDBGSTR ("COMM_Backup : 0001 is not found");
    }

    if (!Enc5008found)
    {
        LDBGSTR ("COMM_Backup : 5008 is not found");
    }

    if (!Enc5077found)
    {
        LDBGSTR ("COMM_Backup : 5077 is not found");
    }

    return B5009found && B0001found && Enc5008found && Enc5077found;
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

BOOL COMM_Read (char *flashfile, BOOL Bbinary,
                UINT32 U32startaddr, UINT32 U32count)
{
    FILE                         *rfile;
    UINT32                        U32endaddr, i, U32left = U32count, U32toread;
    eFILE_TYPE                    file_type;
    UINT32                        U32mcustart;
    char                          startaddr[SYS_MAX_STRING_LEN];

    if ((UINT32) sizeof (AU8iobuffer) < MISC_FLASH_BLOCKSIZE)
    {
        LDBGSTR2 ("COMM_Read : internal buffer is small (%d/%d)",
                  sizeof (AU8iobuffer), MISC_FLASH_BLOCKSIZE);
        return FALSE;
    }

    MDBGSTR2 ("COMM_Read : reading from " UINT32_FORMAT " %d bytes",
              U32startaddr, U32count);
    if (Bbinary)
    {
        memset (startaddr, 0, sizeof (startaddr));
        sprintf (startaddr, " - " UINT32_FORMAT, U32startaddr);
        strcat (flashfile, startaddr);
        strcat (flashfile, "." FREIA_FLS_EXTENSION);
    }
    else
    {
        strcat (flashfile, "." FREIA_KSI_EXTENSION);
    }

    U32endaddr = U32startaddr + U32count;
    MISC_AdjustAddresses (&U32startaddr, &U32endaddr);
    U32count = U32endaddr - U32startaddr;
    if (!MISC_GetMCUStartAddress (&U32mcustart))
    {
        LDBGSTR ("COMM_Read : cannot find MCU start address");
        return FALSE;
    }

    if (U32startaddr == U32mcustart
        && U32mcustart + U32count >= FREIA_MCU_END_ADDRESS)
    {
        file_type = FILE_TYPE_FULL;
    }
    else
    {
        file_type = FILE_TYPE_PARTIAL;
    }

    if (flashfile)
    {
        for (i = 0; i < strlen (flashfile); i++)
        {
            flashfile[i] = toupper (flashfile[i]);
        }
    }
    else
    {
        freia_errno = KE_INVALID_FILENAME;
        return FALSE;
    }

    MISC_CheckFileName (flashfile);
    rfile = fopen (flashfile, "wb");
    if (!rfile)
    {
        freia_errno = KE_CANT_CREATE_FILE;
        return FALSE;
    }

    if (!Bbinary)
    {
        if (!MISC_WriteHeader (rfile, file_type, U32startaddr))
        {
            fclose (rfile);
            freia_errno = KE_CANT_WRITE_HEADER;
            return FALSE;
        }
    }

    MISC_GaugeInit (U32startaddr, U32endaddr);
    /*
     * read to mem 
     */
    for (i = U32startaddr; i < U32startaddr + U32count;
         i += MISC_FLASH_BLOCKSIZE, U32left -= MISC_FLASH_BLOCKSIZE)
    {
        U32toread =
            (U32left > MISC_FLASH_BLOCKSIZE ? MISC_FLASH_BLOCKSIZE : U32left);
        if (!COMM_ReadFlashBlock (i, U32toread))
        {
            fclose (rfile);
            MISC_GaugeDone ();
            return FALSE;
        }

        fwrite (AU8iobuffer, sizeof (UINT8), U32toread, rfile);
    }

    MISC_GaugeDone ();
    fclose (rfile);
    return TRUE;
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

static BOOL COMM_NormalWriteFlashBlock (FILE * rfile,
                                        UINT32
                                        U32startaddr, UINT32 * pU32length)
{
    UINT8                         U8writecmd = COMM_WRITE_REQ;
    UINT8                         U8checksum;
    UINT32                        i, U32blocklen;
    tCOMM_WRITERESPONSE           writeresponse;
    UINT32                        U32calcedaddr;
    UINT32                        U32originalreadtimeout =
        TTY_GetReadTimeout (U8comport_app);
    UINT16                        U16startseg;
    struct
    {
        UINT16                        U16chk;
        UINT16                        U16ack;
    }
    writeack;
    UINT16                        U16ack;
    BOOL                          Bsuccess;
    struct
    {
        UINT16                        U16flashbase;
        UINT16                        U16seg;
        UINT16                        U16ofs;
    }
    addresses;
    UINT16                        U16origchk, *pAU16;
    UINT8                         U8num_of_retries;
    BOOL                          Bwriteerror;
    UINT32                        U32mcustart;
    UINT32                        U32read;
    UINT32                        U32pos;

    if (!MISC_GetMCUStartAddress (&U32mcustart))
    {
        LDBGSTR ("COMM_NormalWriteFlashBlock : cannot find MCU start address");
        return FALSE;
    }

    if (U32startaddr < U32mcustart)
    {
        LDBGSTR2 ("COMM_NormalWriteFlashBlock : invalid write address ("
                  UINT32_FORMAT "<" UINT32_FORMAT, U32startaddr, U32mcustart);
        return FALSE;
    }

    U32pos = ftell (rfile);
    U8num_of_retries = COMM_DEFAULT_RETRIES;
    do
    {
        fseek (rfile, U32pos, SEEK_SET);

        MISC_GaugeUpdate (U32startaddr);
        if (!TTY_SetReadTimeout (U8comport_app, 6 * U32originalreadtimeout))
        {
            return FALSE;
        }

        if (!TTY_WriteComm (U8comport_app, &U8writecmd, sizeof (U8writecmd)))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            return FALSE;
        }

        U16startseg = SWAPWORD (((U32startaddr - U32mcustart) >> 12));  /* we start from a segment */
        if (!TTY_WriteComm
            (U8comport_app, (UINT8 *) & U16startseg, sizeof (U16startseg)))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            return FALSE;
        }

        memset (&writeresponse, 0, sizeof (writeresponse));
        Bsuccess = TTY_ReadComm
            (U8comport_app, (UINT8 *) & writeresponse,
             sizeof (writeresponse), sizeof (writeresponse));
        if (!Bsuccess || (Bsuccess && writeresponse.U16id != 0xFFFF))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR ("COMM_NormalWriteFlashBlock : write is not accepted");
            return FALSE;
        }

        U32calcedaddr =
            (UINT32) (writeresponse.U16seg << 14) + writeresponse.U16offset;
        HDBGSTR1 ("COMM_NormalWriteFlashBlock : received target address is "
                  UINT32_FORMAT, U32calcedaddr);

        U32blocklen = writeresponse.U16blocksize * (UINT32) 4096;
        HDBGSTR1 ("COMM_NormalWriteFlashBlock : received blocklength is %d",
                  U32blocklen);

        *pU32length = U32blocklen;

        U32read = fread (AU8iobuffer, sizeof (UINT8), U32blocklen, rfile);

        if (U32read != U32blocklen)
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR2
                ("COMM_NormalWriteFlashBlock : read error in file (%d/%d",
                 U32read, U32blocklen);
            return FALSE;
        }

        for (U16origchk = 0, i = 0, pAU16 = (UINT16 *) AU8iobuffer;
             i < U32blocklen / 2; i++, pAU16++)
        {
            U16origchk += *pAU16;
        }

        for (i = 0, U8checksum = 0; i < U32blocklen; i++)
        {
            U8checksum ^= AU8iobuffer[i];
        }

        MDBGSTR ("COMM_NormalWriteFlashBlock : sending block");
        if (!TTY_WriteComm (U8comport_app, AU8iobuffer, U32blocklen))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR ("COMM_NormalWriteFlashBlock : cannot write flash block");
            return FALSE;
        }

        if (!TTY_WriteComm (U8comport_app, &U8checksum, sizeof (U8checksum)))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR ("COMM_NormalWriteFlashBlock : cannot write flash block");
            return FALSE;
        }

        Bsuccess = TTY_ReadComm
            (U8comport_app, (UINT8 *) & addresses, sizeof (addresses),
             sizeof (addresses));
        if (!Bsuccess)
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1
                ("COMM_NormalWriteFlashBlock : erase response error at "
                 UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        if (addresses.U16flashbase == 0xFFFF)
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR ("COMM_NormalWriteFlashBlock : RAM error");
            return FALSE;
        }

        HDBGSTR3
            ("COMM_NormalWriteFlashBlock : flashbase is %04X, target address is %04X:%04X",
             addresses.U16flashbase, addresses.U16seg, addresses.U16ofs);

        MDBGSTR ("COMM_NormalWriteFlashBlock : erasing block");

        TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout * (10 * U32blocklen / 1024)); /* quite heuristic, but hey :))) */
        Bsuccess = TTY_ReadComm
            (U8comport_app, (UINT8 *) & U16ack, sizeof (U16ack),
             sizeof (U16ack));
        if (!Bsuccess || (Bsuccess && U16ack != COMM_ERASE_ACK))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1 ("COMM_NormalWriteFlashBlock : erase error at "
                      UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        Bsuccess = TTY_ReadComm
            (U8comport_app, (UINT8 *) & addresses, sizeof (addresses),
             sizeof (addresses));
        if (!Bsuccess)
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1
                ("COMM_NormalWriteFlashBlock : write response error at "
                 UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        HDBGSTR3
            ("COMM_NormalWriteFlashBlock : flashbase is %04X, target address is %04X:%04X",
             addresses.U16flashbase, addresses.U16seg, addresses.U16ofs);
        MDBGSTR ("COMM_NormalWriteFlashBlock : writing block");
        Bsuccess = TTY_ReadComm
            (U8comport_app, (UINT8 *) & U16ack, sizeof (U16ack),
             sizeof (U16ack));
        if (!Bsuccess || (Bsuccess && U16ack != COMM_WRITE_ACK))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1 ("COMM_NormalWriteFlashBlock : write error at "
                      UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        if (!TTY_ReadComm
            (U8comport_app, (UINT8 *) & writeack, sizeof (writeack),
             sizeof (writeack)))
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1 ("COMM_NormalWriteFlashBlock : write error at "
                      UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        if (writeack.U16ack != COMM_3RDBOOT_ACK)    /* ?OK */
        {
            TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
            LDBGSTR1 ("COMM_NormalWriteFlashBlock : write error at "
                      UINT32_FORMAT, U32startaddr);
            return FALSE;
        }

        Bwriteerror = FALSE;
        if (writeack.U16chk != U16origchk)  /* ?OK Chk */
        {
            LDBGSTR1 ("COMM_NormalWriteFlashBlock : checksum error at "
                      UINT32_FORMAT ", retrying", U32startaddr);
            Bwriteerror = TRUE;
            U8num_of_retries--;
            if (U8num_of_retries == 0)
            {
                TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
                LDBGSTR1 ("COMM_NormalWriteFlashBlock : write error at "
                          UINT32_FORMAT, U32startaddr);
                return FALSE;
            }
        }
    } while (Bwriteerror);

    TTY_SetReadTimeout (U8comport_app, U32originalreadtimeout);
    return TRUE;
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

static BOOL COMM_EmulatedWriteFlashBlock (FILE * rfile,
                                          UINT32
                                          U32startaddr, UINT32 * pU32length)
{
    FILE                         *sfile;
    BOOL                          Bsuccess;
    UINT32                        U32read;

    if (!Bbootisrunning)
    {
        LDBGSTR ("COMM_EmulatedWriteFlashBlock : boot is not running");
        return FALSE;
    }

    HDBGSTR1 ("COMM_EmulatedWriteFlashBlock : writing block to"
              UINT32_FORMAT, U32startaddr);
    if (!MISC_OpenEmulatedFlashFile (&sfile, "rb+", U32startaddr))
    {
        return FALSE;
    }

    MISC_GaugeUpdate (U32startaddr);

    *pU32length = COMM_EMULATED_WRITE_BLOCKSIZE;

    U32read =
        fread (AU8iobuffer, sizeof (UINT8), COMM_EMULATED_WRITE_BLOCKSIZE,
               rfile);

    if (U32read != COMM_EMULATED_WRITE_BLOCKSIZE)
    {
        LDBGSTR2
            ("COMM_EmulatedWriteFlashBlock : read error in file (%d/%d",
             U32read, COMM_EMULATED_WRITE_BLOCKSIZE);
        return FALSE;
    }

    Bsuccess =
        (UINT32) fwrite (AU8iobuffer, sizeof (UINT8),
                         COMM_EMULATED_WRITE_BLOCKSIZE,
                         sfile) == COMM_EMULATED_WRITE_BLOCKSIZE;
    fclose (sfile);
    return Bsuccess;
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

static BOOL COMM_WriteFlashBlock (FILE * rfile,
                                  UINT32 U32startaddr, UINT32 * pU32length)
{
    if (MISC_GetEmulation ())
    {
        return COMM_EmulatedWriteFlashBlock (rfile, U32startaddr, pU32length);
    }

    return COMM_NormalWriteFlashBlock (rfile, U32startaddr, pU32length);
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

BOOL COMM_Write (UINT32 U32extensionid,
                 char *flashfile,
                 UINT32 U32startfile,
                 UINT32 U32countfile, UINT32 U32givenstartaddr, BOOL Bbinary)
{
    FILE                         *rfile;
    UINT32                        i, U32towrite;
    UINT32                        U32left, U32len, U32end, U32startaddr,
        U32extid;
    eFILE_TYPE                    file_type;

    if ((UINT32) sizeof (AU8iobuffer) < (UINT32) MISC_FLASH_BLOCKSIZE)
    {
        LDBGSTR2 ("COMM_Write : internal buffer is small (%d/%d)",
                  sizeof (AU8iobuffer), MISC_FLASH_BLOCKSIZE);
        return FALSE;
    }

    rfile = fopen (flashfile, "rb");
    if (!rfile)
    {
        MDBGSTR1 ("COMM_Write : cannot open %s", flashfile);
        freia_errno = KE_FILE_NOT_FOUND;
        return FALSE;
    }

    for (i = 0; i < strlen (flashfile); i++)
    {
        flashfile[i] = toupper (flashfile[i]);
    }

    MDBGSTR1 ("COMM_Write : using %s", flashfile);

    if (!Bbinary)
    {
        if (!MISC_ReadHeader (rfile, &file_type, &U32startaddr))
        {
            MDBGSTR ("COMM_Write : invalid header");
            fclose (rfile);
            freia_errno = KE_CANT_READ_HEADER;
            return FALSE;
        }
    }
    else
    {
        U32startaddr = U32givenstartaddr;
    }

    if (U32startfile > 0)
    {
        fseek (rfile, U32startfile, SEEK_CUR);
    }

    if (U32countfile == 0)
    {
        U32len = MISC_GetFileSize (rfile) - (Bbinary ? 0 : ftell (rfile));  /* skip KNK header */
    }
    else
    {
        U32len = U32countfile;
    }

    U32end = U32startaddr + U32len;
    MDBGSTR2 ("COMM_Write : startaddress is " UINT32_FORMAT
              " end is " UINT32_FORMAT, U32startaddr, U32end);
    MISC_AdjustAddresses (&U32startaddr, &U32end);
    U32len = U32end - U32startaddr + 1;
    MDBGSTR2 ("COMM_Write : adjusted startaddress is " UINT32_FORMAT
              " end is " UINT32_FORMAT, U32startaddr, U32end);
    MISC_GaugeInit (U32startaddr, U32end);
    /*
     * read from file mem
     */
    U32left = U32len;
    for (i = U32startaddr; i < U32end; i += U32towrite, U32left -= U32towrite)
    {
        if (U32left > 0)
        {
            if (!COMM_WriteFlashBlock (rfile, i, &U32towrite))
            {
                fclose (rfile);
                MISC_GaugeDone ();
                return FALSE;
            }
        }
    }

    MISC_GaugeDone ();
    fclose (rfile);
    return TRUE;
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

BOOL COMM_BackupBattery (UINT8 * pB67)
{
    if (!COMM_SendCommand
        (TRUE, FREIA_67_LEN, COMM_DEFAULT_COMMAND_RETRY,
         COMM_BATTERY_REQ, COMM_BOOTCODE_CHECK_POS_REPLY, COMM_CMD_NAK, 1))
    {
        LDBGSTR ("COMM_BackupBattery : cannot get battery parameters");
        return FALSE;
    }

    memcpy (pB67, AU8iobuffer, FREIA_67_LEN);
    return TRUE;
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

static BOOL COMM_SendATCommand (char *cmd, char *desiredack)
{
    BOOL                          Bfinished;
    UINT32                        U32retry = COMM_DEFAULT_RETRIES;
    char                          defaultack[] = "\r\nOK\r\n";

    if (desiredack == NULL)
    {
        desiredack = defaultack;
    }

    do
    {
        if (!TTY_WriteComm (U8comport_app, (UINT8 *) cmd, strlen (cmd)))
        {
            return FALSE;
        }

        MISC_Sleep (1000);
        TTY_ReadComm (U8comport_app, AU8iobuffer, 0, sizeof (AU8iobuffer));
        Bfinished = strstr ((char *) AU8iobuffer, desiredack) != NULL;
    } while (!Bfinished && U32retry--);
    return Bfinished;
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

static BOOL COMM_ReceiveBlockWait (UINT8 * data, UINT8 * len)
{
    UINT8                         i = 0, c = *len;

    while (c > 0)
    {
        if (TTY_ReadComm
            (U8comport_app, data + i, sizeof (UINT8), sizeof (UINT8)))
        {
            c--;
            i++;
        }
    }

    *len = i;
    return i > 0;
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

static BOOL COMM_ReceiveCommand (UINT8 cmd, UINT8 * data, UINT8 * len)
{
    UINT8                         b[3], bLen;

    TTY_ReadComm (U8comport_app, b, 0, sizeof (b));
    if (TTY_GetNumberOfReceivedBytes (U8comport_app) == 3
        && b[2] == (b[0] ^ b[1]))
    {
        bLen = b[1];
        if (len)
        {
            *len = bLen;
        }
        else
        {
            len = &bLen;
        }

        return (COMM_ReceiveBlockWait (data, len) ? b[0] == cmd : FALSE);
    }

    return FALSE;
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

static BOOL COMM_SendBFBCommand (UINT8 cmd, UINT8 * data,
                                 UINT8 len, UINT8 * dst, UINT8 * len2)
{
    UINT8                         b[32], l = sizeof (b);
    UINT8                         AU8cmd[512];

    AU8cmd[0] = cmd;
    AU8cmd[1] = len;
    AU8cmd[2] = cmd ^ len;
    memcpy (&AU8cmd[3], data, len);
    if (!TTY_WriteComm (U8comport_app, AU8cmd, 3 + len))
    {
        return FALSE;
    }

    MISC_Sleep (500);
    if (COMM_ReceiveCommand (cmd, b, &l) && *data == b[0])
    {
        *len2 = l - 1;
        memcpy (dst, b + 1, l - 1);
        return TRUE;
    }

    return FALSE;
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

static BOOL COMM_ReadMemoryBlock (UINT32 offset, UINT32 len)
{
    UINT8                         cmd_readmem[] = {
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    UINT8                         l, numblocks = 0, *data = AU8iobuffer;

    while (len > 0)
    {
        MISC_GaugeUpdate (offset);
        l = len > 31 ? 31 : len;
        *(UINT32 *) & cmd_readmem[1] = offset;
        cmd_readmem[5] = l;
        if (((++numblocks) % 10) == 0)
        {
            numblocks = 0;
            LDBGSTR2 ("COMM_ReadMemory: reading from " UINT32_FORMAT
                      ", remaining %d bytes", offset, len);
        }

        if (!COMM_SendBFBCommand
            (0x02, cmd_readmem, sizeof (cmd_readmem), data, &l))
        {
            break;
        }

        data += l;
        offset += l;
        len -= l;
    }

    return (len == 0) ? 1 : 0;
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

static BOOL COMM_SetBFBMode (void)
{
    UINT8                         l;
    BOOL                          BFBMode = FALSE;

    if (!TTY_OpenCOMPort
        (MISC_GetAppCOMPort (), 19200, COMM_DEFAULT_BOOTUP_DELAY,
         MISC_BFB_BLOCKSIZE, FALSE, TRUE))
    {
        return FALSE;
    }

    LDBGSTR ("COMM_SetBFBMode: checking BFB mode");
    if (!TTY_SetSpeed (U8comport_app, COMM_BFB_SPEED))
    {
        return FALSE;
    }

    if (!COMM_SendBFBCommand (0x02, (UINT8 *) "\x14", 1, AU8iobuffer, &l))
    {
        if (COMM_SendBFBCommand (0x02, (UINT8 *) "\x14", 1, AU8iobuffer, &l))
        {
            BFBMode = TRUE;
        }
    }

    if (!BFBMode)
    {
        LDBGSTR ("COMM_SetBFBMode: setting up BFB mode");
        if (!TTY_SetSpeed (U8comport_app, 19200))
        {
            return FALSE;
        }

        if (!COMM_SendATCommand ("atz\r", NULL))
        {
            return FALSE;
        }

        if (!COMM_SendATCommand ("at+cgmi\r", "at+cgmi\r"))
        {
            return FALSE;
        }

        if (strstr ((char *) AU8iobuffer, "SIEMENS") == NULL)
        {
            return FALSE;
        }

        if (!COMM_SendATCommand ("at^sbfb=1\r", NULL))
        {
            return FALSE;
        }

        if (!TTY_SetSpeed (U8comport_app, COMM_BFB_SPEED))
        {
            return FALSE;
        }

        if (!COMM_SendBFBCommand (0x02, (UINT8 *) "\x14", 1, AU8iobuffer, &l))
        {
            if (!COMM_SendBFBCommand
                (0x02, (UINT8 *) "\x14", 1, AU8iobuffer, &l))
            {
                return FALSE;
            }
        }
    }

    return TRUE;
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

BOOL COMM_SendCode (BOOL BsetBFB)
{
    FILE                         *rfile;
    UINT16                        U16codelength;
    UINT8                         l, *data = AU8iobuffer;

    rfile = fopen ("code.bin", "rb");
    if (!rfile)
    {
        return FALSE;
    }

    U16codelength = fread (&AU8iobuffer[1], 1, sizeof (AU8iobuffer) - 1, rfile);

    fclose (rfile);

    if (U16codelength > 255)
    {
        return FALSE;
    }

    if (BsetBFB)
    {
        if (!COMM_SetBFBMode ())
        {
            return FALSE;
        }
    }

    AU8iobuffer[0] = 0x75;

    if (!COMM_SendBFBCommand (0x02, AU8iobuffer, U16codelength + 1, data, &l))
    {
        return FALSE;
    }

    return TRUE;
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

BOOL COMM_ReadMemory (char *flashfile,
                      BOOL Bbinary, UINT32 U32startaddr, UINT32 U32count)
{
    FILE                         *rfile;
    UINT32                        U32endaddr, i, U32left = U32count, U32toread;
    eFILE_TYPE                    file_type;
    UINT32                        U32mcustart;
    char                          startaddr[SYS_MAX_STRING_LEN];

    if ((UINT32) sizeof (AU8iobuffer) < MISC_BFB_BLOCKSIZE)
    {
        LDBGSTR2 ("COMM_ReadMemory : internal buffer is small (%d/%d)",
                  sizeof (AU8iobuffer), MISC_BFB_BLOCKSIZE);
        return FALSE;
    }

    if (!COMM_SetBFBMode ())
    {
        LDBGSTR ("COMM_ReadMemory : cannot set BFB");
        return FALSE;
    }

    MDBGSTR2 ("COMM_ReadMemory : reading from " UINT32_FORMAT " %d bytes",
              U32startaddr, U32count);
    if (Bbinary)
    {
        memset (startaddr, 0, sizeof (startaddr));
        sprintf (startaddr, " - " UINT32_FORMAT, U32startaddr);
        strcat (flashfile, startaddr);
        strcat (flashfile, "." FREIA_FLS_EXTENSION);
    }
    else
    {
        strcat (flashfile, "." FREIA_KSI_EXTENSION);
    }

    U32endaddr = U32startaddr + U32count;
    MISC_AdjustAddresses (&U32startaddr, &U32endaddr);
    U32count = U32endaddr - U32startaddr;
    if (!MISC_GetMCUStartAddress (&U32mcustart))
    {
        LDBGSTR ("COMM_ReadMemory : cannot find MCU start address");
        return FALSE;
    }

    if (U32startaddr == U32mcustart
        && U32mcustart + U32count >= FREIA_MCU_END_ADDRESS)
    {
        file_type = FILE_TYPE_FULL;
    }
    else
    {
        file_type = FILE_TYPE_PARTIAL;
    }

    if (flashfile)
    {
        for (i = 0; i < strlen (flashfile); i++)
        {
            flashfile[i] = toupper (flashfile[i]);
        }
    }
    else
    {
        freia_errno = KE_INVALID_FILENAME;
        return FALSE;
    }

    MISC_CheckFileName (flashfile);
    rfile = fopen (flashfile, "wb");
    if (!rfile)
    {
        freia_errno = KE_CANT_CREATE_FILE;
        return FALSE;
    }

    if (!Bbinary)
    {
        if (!MISC_WriteHeader (rfile, file_type, U32startaddr))
        {
            fclose (rfile);
            freia_errno = KE_CANT_WRITE_HEADER;
            return FALSE;
        }
    }

    MISC_GaugeInit (U32startaddr, U32endaddr);
    /*
     * read to mem 
     */
    for (i = U32startaddr; i < U32startaddr + U32count;
         i += MISC_BFB_BLOCKSIZE, U32left -= MISC_BFB_BLOCKSIZE)
    {
        U32toread =
            (U32left > MISC_BFB_BLOCKSIZE ? MISC_BFB_BLOCKSIZE : U32left);
        if (!COMM_ReadMemoryBlock (i, U32toread))
        {
            fclose (rfile);
            MISC_GaugeDone ();
            return FALSE;
        }

        fwrite (AU8iobuffer, sizeof (UINT8), U32toread, rfile);
    }

    MISC_GaugeDone ();
    fclose (rfile);
    return TRUE;
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

BOOL COMM_PatchBootcore (char *flashfile, BOOL Bbinary)
{
    FILE                         *rfile;
    UINT32                        U32extid, U32extensionid;
    UINT32                        i, U32left, U32toread;
    UINT32                        U32startaddr;
    eFILE_TYPE                    file_type;
    UINT32                        U32bootcorestart, U32bootcoresize;
    char                          tmpbootcore[SYS_MAX_PATHNAME_LEN] =
        "bootcore." FREIA_FLS_EXTENSION;

    if (!MISC_GetBootcoreAddress (&U32bootcorestart, &U32bootcoresize))
    {
        MDBGSTR ("COMM_PatchBootcore : cannot find bootcore data");
        return FALSE;
    }

    if (U32bootcoresize == 0)
    {
        MDBGSTR ("COMM_PatchBootcore : no bootcore for this phone");
        return FALSE;
    }

    /*
     * if no file, then read it from the phone 
     */

    if (!flashfile || (flashfile && strlen (flashfile) == 0))
    {
        Bbinary = TRUE;
        flashfile = tmpbootcore;

        rfile = fopen (flashfile, "wb");
        if (!rfile)
        {
            MDBGSTR
                ("COMM_PatchBootcore : cannot create temporary bootcore file");
            freia_errno = KE_CANT_CREATE_FILE;
            return FALSE;
        }

        MISC_GaugeInit (U32bootcorestart, U32bootcorestart + U32bootcoresize);

        for (i = U32bootcorestart, U32left = U32bootcoresize;
             i < U32bootcorestart + U32bootcoresize;
             i += MISC_FLASH_BLOCKSIZE, U32left -= MISC_FLASH_BLOCKSIZE)
        {
            U32toread =
                (U32left >
                 MISC_FLASH_BLOCKSIZE ? MISC_FLASH_BLOCKSIZE : U32left);

            if (!COMM_ReadFlashBlock (i, U32toread))
            {
                fclose (rfile);
                MISC_GaugeDone ();
                return FALSE;
            }

            fwrite (AU8iobuffer, sizeof (UINT8), U32toread, rfile);
        }
        fclose (rfile);

        MISC_GaugeDone ();
    }

    rfile = fopen (flashfile, "rb");
    if (!rfile)
    {
        MDBGSTR1 ("COMM_PatchBootcore : cannot open %s", flashfile);
        freia_errno = KE_FILE_NOT_FOUND;
        return FALSE;
    }

    for (i = 0; i < strlen (flashfile); i++)
    {
        flashfile[i] = toupper (flashfile[i]);
    }

    MDBGSTR1 ("COMM_PatchBootcore : using %s", flashfile);
    U32extensionid = (Bbinary ? FLS_EXTENSION_ID : KSI_EXTENSION_ID);

    memcpy (&U32extid,
            &flashfile[strlen (flashfile) - sizeof (U32extid)],
            sizeof (U32extid));
    if (U32extid != U32extensionid)
    {
        MDBGSTR ("COMM_PatchBootcore : invalid extension");
        freia_errno = KE_INVALID_EXTENSION;
        return FALSE;
    }

    if (!Bbinary)
    {
        if (!MISC_ReadHeader (rfile, &file_type, &U32startaddr))
        {
            MDBGSTR ("COMM_PatchBootcore : invalid header");
            fclose (rfile);
            freia_errno = KE_CANT_READ_HEADER;
            return FALSE;
        }

        if (U32startaddr != U32bootcorestart)
        {
            MDBGSTR ("COMM_PatchBootcore : invalid start address");
            fclose (rfile);
            freia_errno = KE_INVALID_FILE_FORMAT;
            return FALSE;
        }
    }

    if (fread (AU8iobuffer, 1, U32bootcoresize, rfile) != U32bootcoresize)
    {
        MDBGSTR ("COMM_PatchBootcore : file too short");
        fclose (rfile);
        freia_errno = KE_INVALID_FILE_FORMAT;
        return FALSE;
    }

    fclose (rfile);

    /*
     * AOK we have the file in the memory, let's patch it
     */
    if (strcmp ((char *) &AU8iobuffer[0x31c], "SIEMENS") != 0)
    {
        MDBGSTR ("COMM_PatchBootcore : invalid bootcore");
        freia_errno = KE_INVALID_FILE_FORMAT;
        return FALSE;
    }

    memcpy (&AU8iobuffer[0x300 + MISC_GetBootcorePasswordOffset ()],
            COMM_BOOTCORE_PWD, 16);

    MISC_CheckFileName (flashfile);
    rfile = fopen (flashfile, "wb");
    if (!rfile)
    {
        MDBGSTR ("COMM_PatchBootcore : cannot create temporary bootcore file");
        freia_errno = KE_CANT_CREATE_FILE;
        return FALSE;
    }

    fwrite (AU8iobuffer, sizeof (UINT8), U32bootcoresize, rfile);

    fclose (rfile);

    return TRUE;
}
#endif

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

void COMM_Construct (void)
{
    U8comport_app = MISC_GetAppCOMPort ();
}
