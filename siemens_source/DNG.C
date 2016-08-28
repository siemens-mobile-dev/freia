
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

#include <string.h>
#include <stdio.h>

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "dngloc.h"
#include "dngpub.h"
#include "ttypub.h"
#ifdef INCLUDE_RC4
#include "rc4pub.h"
#endif
static UINT16                 U16devidlo, U16devidhi;

#ifndef MINIMAL_DNG
static UINT8                  U8comport_dng = 0;

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

void DNG_Construct (void)
{
    U8comport_dng = MISC_GetDngCOMPort ();
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

BOOL DNG_Boot (BOOL Btoboot, UINT16 * pU16version)
{
    UINT8                         U8byte, U8response;

    *pU16version = 0;

    U8byte = (Btoboot ? DNG_BOOT_TO_BOOT : DNG_BOOT_TO_APP);    /* boot to application at once */
    TTY_WriteComm (U8comport_dng, &U8byte, sizeof (UINT8));

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (Btoboot)
    {
        if (U8response != DNG_BOOT_BOOT_ACK)
        {
            return FALSE;
        }
    }
    else
    {
        if (U8response != DNG_BOOT_APP_ACK)
        {
            return FALSE;
        }
    }

    if (!TTY_ReadComm
        (U8comport_dng, (UINT8 *) pU16version, sizeof (UINT16), 0))
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

BOOL DNG_SetupCommand (UINT8 U8cmd)
{
    UINT8                         U8response;

    TTY_WriteComm (U8comport_dng, &U8cmd, sizeof (UINT8));

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    return U8cmd == U8response;
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

static BOOL DNG_ApplicationBootOrCalculation (BOOL Btoboot)
{
    UINT8                         U8byte =
        (Btoboot ? DNG_APPLICATION_BOOTUP : DNG_APPLICATION_CALCULATION);
    TTY_WriteComm (U8comport_dng, &U8byte, sizeof (UINT8));
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

void DNG_SetDeviceID (UINT16 U16lo, UINT16 U16hi)
{
    U16devidlo = U16lo;
    U16devidhi = U16hi;
}

#ifdef INCLUDE_RC4

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

void DNG_GetMasterKey (UINT8 * pAU8key, BOOL Busedevcode)
{
    UINT8                         i;
    UINT8                         AU8devcode[4];

    memcpy (pAU8key, DNG_MASTERKEY, DNG_MASTERKEY_STRLEN);

    if (Busedevcode)
    {
        wordatbyteptr (AU8devcode) = U16devidlo;
        wordatbyteptr (&AU8devcode[2]) = U16devidhi;
        for (i = 0; i < DNG_MASTERKEY_STRLEN; i++)
        {
            pAU8key[i] ^= AU8devcode[i % 4];
        }
    }
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

UINT16 DNG_GetMasterkeyLength (void)
{
    return DNG_MASTERKEY_STRLEN;
}
#endif

#ifndef MINIMAL_DNG

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

BOOL DNG_GetDeviceInfo (tDNG_DEVINFO * pdevinfo, BOOL * pBencryptionerror)
{
    UINT8                         U8response;
    BOOL                          Batboot;

    memset (pdevinfo, 0, sizeof (*pdevinfo));

    if (!DNG_GetStatus (&Batboot))
    {
        return FALSE;
    }

    if (!Batboot)
    {
        if (!DNG_ApplicationBootOrCalculation (TRUE))
        {
            return FALSE;
        }
    }

    if (!DNG_SetupCommand (DNG_GET_DEVINFO))
    {
        return FALSE;
    }

    if (!TTY_ReadStream
        (U8comport_dng, (UINT8 *) pdevinfo, sizeof (*pdevinfo), TRUE,
         pBencryptionerror))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
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

BOOL DNG_SetDeviceInfo (tDNG_DEVINFO * pdevinfo, BOOL * pBbadinfo)
{
    UINT8                         U8response;

    *pBbadinfo = FALSE;

    if (!DNG_ApplicationBootOrCalculation (TRUE))
    {
        return FALSE;
    }

    if (!DNG_SetupCommand (DNG_SET_DEVINFO))
    {
        return FALSE;
    }

    if (!TTY_WriteStream
        (U8comport_dng, (UINT8 *) pdevinfo, sizeof (*pdevinfo), TRUE))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        *pBbadinfo = U8response == DNG_INVALID_DEVINFO_NAK;
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

BOOL DNG_GetPage (UINT16 U16address, UINT8 * pDst, BOOL * pBencryptionerror)
{
    UINT8                         U8response;

    memset (pDst, 0, DNG_PAGE_SIZE);

    if (!DNG_ApplicationBootOrCalculation (TRUE))
    {
        return FALSE;
    }

    if (!DNG_SetupCommand (DNG_READ_PAGE))
    {
        return FALSE;
    }

    TTY_WriteComm (U8comport_dng, (UINT8 *) & U16address, sizeof (UINT16));

    if (!TTY_ReadStream
        (U8comport_dng, pDst, DNG_PAGE_SIZE, TRUE, pBencryptionerror))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
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

BOOL DNG_SetPage (UINT16 U16address, UINT8 * pSrc, BOOL * pBencryptionerror)
{
    UINT8                         U8response;

    *pBencryptionerror = FALSE;

    if (!DNG_SetupCommand (DNG_WRITE_PAGE))
    {
        return FALSE;
    }

    TTY_WriteComm (U8comport_dng, (UINT8 *) & U16address, sizeof (UINT16));

    if (!TTY_WriteStream (U8comport_dng, pSrc, DNG_PAGE_SIZE, TRUE))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        *pBencryptionerror = DNG_DECRYPT_NAK;
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

BOOL DNG_TestEncryptionEngine (BOOL * pBencryptionerror)
{
    UINT8                         U8response;
    UINT8                         AU8dummy[6 + DNG_CHECKSUM_SIZE];
    BOOL                          Bgood;

    memset (AU8dummy, 0, sizeof (AU8dummy));

    if (!DNG_ApplicationBootOrCalculation (TRUE))
    {
        return FALSE;
    }

    if (!DNG_SetupCommand (DNG_DUMMY))
    {
        return FALSE;
    }

    if (!TTY_ReadStream (U8comport_dng, AU8dummy, 5, TRUE, pBencryptionerror))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        return FALSE;
    }

#ifdef INCLUDE_RC4
    Bgood = strncmp ((char *) AU8dummy, "DUMMY", 5) == 0;
#else
    Bgood = memcmp (AU8dummy, "\xC4\x94\xB4\x6A\x15", 5) == 0;
#endif

    return Bgood;
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

BOOL DNG_TestUART (void)
{
    UINT8                         U8response;
    UINT16                        i;

    if (!DNG_ApplicationBootOrCalculation (TRUE))
    {
        return FALSE;
    }

    if (!DNG_SetupCommand (DNG_UART_TEST))
    {
        return FALSE;
    }

    for (i = 0; i <= 0xFF; i++)
    {
        if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
        {
            return FALSE;
        }

        if (U8response != i)
        {
            return FALSE;
        }
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
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

static void DNG_ConvertToBCD (char *myIMEI, UINT8 * myBCD)
{
    UINT16                        i = 0;

    myBCD[i] = 10;

    while (i < 7)
    {
        myBCD[i] |= ((UINT8) (*(myIMEI++) - '0') << 4);
        i++;
        myBCD[i] = (UINT8) (*(myIMEI++) - '0');
    }
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

BOOL DNG_Calculate (eMISC_MODEL_INDICES phonetype, char *IMEI,
                    UINT32 U32phoneid, void *pCodE2, BOOL * pBblocked)
{
    UINT8                         U8response;
    UINT8                         BCD[DNG_BCDIMEI_LEN];
    UINT32                        U32crc, U32response;
    UINT16                        U16version, U16len;
    UINT8                        *ptr;

    *pBblocked = FALSE;

    DNG_ConvertToBCD (IMEI, BCD);

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (FALSE, &U16version)) /* Boot to app */
    {
        LDBGSTR ("DNG_Calculate : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_Calculate : dongle version %04X", U16version);

    if (!DNG_ApplicationBootOrCalculation (FALSE))
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        *pBblocked = U8response == DNG_BLOCKED_NAK;
        LDBGSTR1 ("DNG_Calculate : %s",
                  (*pBblocked ? "dongle is blocked" :
                   "dongle communication error"));
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (!DNG_SetupCommand (phonetype))
    {
        return FALSE;
    }

    TTY_WriteComm (U8comport_dng, (UINT8 *) & U32phoneid, sizeof (UINT32));
    TTY_WriteComm (U8comport_dng, BCD, sizeof (BCD));

    /* Let some time for the dongle */
    MISC_Sleep(500);

    switch (phonetype)
    {
    default:
    case MISC_MODEL_INDEX_C45:
    case MISC_MODEL_INDEX_A50:
    case MISC_MODEL_INDEX_C55:
        if (!TTY_ReadComm
            (U8comport_dng, (UINT8 *) pCodE2, 4 * sizeof (UINT32), 0))
        {
            LDBGSTR ("DNG_Calculate : dongle communication error");
            TTY_CloseCOMPort (U8comport_dng);
            return FALSE;
        }
        ptr = (UINT8 *) pCodE2;
        U16len = 16;
        break;

    case MISC_MODEL_INDEX_A35:
    case MISC_MODEL_INDEX_S45:
    case MISC_MODEL_INDEX_SL45:
        if (!TTY_ReadComm (U8comport_dng, (UINT8 *) pCodE2, 204, 0))
        {
            LDBGSTR ("DNG_Calculate : dongle communication error");
            TTY_CloseCOMPort (U8comport_dng);
            return FALSE;
        }
        ptr = (UINT8 *) pCodE2;
        U16len = 204;
        break;
    }

    U32crc = MISC_Adler32 (ptr, U16len);

    if (!TTY_ReadComm
        (U8comport_dng, (UINT8 *) & U32response, sizeof (UINT32), 0))
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (U32crc != U32response)
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        LDBGSTR ("DNG_Calculate : dongle communication error");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    TTY_CloseCOMPort (U8comport_dng);
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

BOOL DNG_FinishBootstrap (void)
{
    UINT8                         U8byte = DNG_END_OF_SESSION;
    UINT8                         U8response;

    TTY_WriteComm (U8comport_dng, &U8byte, sizeof (UINT8));

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
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

BOOL DNG_GetStatus (BOOL * pBboot)
{
    UINT8                         U8byte = DNG_GET_STATUS;
    UINT8                         U8response;

    TTY_WriteComm (U8comport_dng, &U8byte, sizeof (UINT8));

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    *pBboot = U8response == DNG_BOOT_BOOT_ACK;

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

BOOL DNG_TestDongle (void)
{
    BOOL                          Bencryptionerror;
    UINT16                        U16version;

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (FALSE, &U16version)) /* Boot to app */
    {
        LDBGSTR ("DNG_TestDongle : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_TestDongle : dongle version %04X", U16version);

    if (!DNG_TestUART ())
    {
        LDBGSTR ("DNG_TestDongle : UART test failed");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }
    LDBGSTR ("DNG_TestDongle : UART test OK");

    if (!DNG_TestEncryptionEngine (&Bencryptionerror))
    {
        LDBGSTR ("DNG_TestDongle : encryption test failed");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    LDBGSTR ("DNG_TestDongle : encryption test OK");
    TTY_CloseCOMPort (U8comport_dng);
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

BOOL DNG_GetDevInfo (void)
{
    FILE                         *rfile;
    char                          filename[SYS_MAX_PATHNAME_LEN] =
        "devinfo.in." FREIA_DI_EXTENSION;
    BOOL                          Bencryptionerror;
    UINT16                        U16version;
    UINT8                         devinfo[sizeof (tDNG_DEVINFO) +
                                          DNG_CHECKSUM_SIZE];

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (FALSE, &U16version)) /* Boot to app */
    {
        LDBGSTR ("DNG_GetDevInfo : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_GetDevInfo : dongle version %04X", U16version);

    if (!DNG_GetDeviceInfo ((tDNG_DEVINFO *) devinfo, &Bencryptionerror))
    {
        LDBGSTR ("DNG_GetDevInfo : cannot read dongle information");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }
    TTY_CloseCOMPort (U8comport_dng);

    MISC_CheckFileName (filename);
    rfile = fopen (filename, "wb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_GetDevInfo : cannot save dongle inforation into %s",
                  filename);
        return FALSE;
    }

    if (fwrite (&devinfo, sizeof (devinfo), 1, rfile) != 1)
    {
        LDBGSTR1 ("DNG_GetDevInfo : write error in %s", filename);
        fclose (rfile);
        return FALSE;
    }
    fclose (rfile);

    MDBGSTR1 ("DNG_GetDevInfo : dongle info is written to %s", filename);
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

BOOL DNG_SetDevInfo (char *filename)
{
    FILE                         *rfile;
    BOOL                          Bbadinfo;
    UINT16                        U16version;
    UINT8                         devinfo[sizeof (tDNG_DEVINFO) +
                                          DNG_CHECKSUM_SIZE];

    rfile = fopen (filename, "rb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_SetDevInfo : cannot open dongle information from %s",
                  filename);
        return FALSE;
    }

    if (fread (&devinfo, sizeof (devinfo), 1, rfile) != 1)
    {
        LDBGSTR1 ("DNG_SetDevInfo : read error in %s", filename);
        fclose (rfile);
        return FALSE;
    }
    fclose (rfile);

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (FALSE, &U16version)) /* Boot to app */
    {
        LDBGSTR ("DNG_SetDevInfo : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_SetDevInfo : dongle version %04X", U16version);

    if (!DNG_SetDeviceInfo ((tDNG_DEVINFO *) devinfo, &Bbadinfo))
    {
        LDBGSTR1 ("DNG_SetDevInfo : %s",
                  (Bbadinfo ? "device info is bad" :
                   "cannot set dongle information"));
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }
    TTY_CloseCOMPort (U8comport_dng);

    MDBGSTR ("DNG_SetDevInfo : dongle info is updated");
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

BOOL DNG_SendUpdate (char *infilename)
{
    FILE                         *rfile;
    UINT16                        i, U16address;
    UINT8                         AU8buffer[DNG_PAGE_SIZE + DNG_CHECKSUM_SIZE];
    BOOL                          Bencryptionerror;
    UINT16                        U16version;

    rfile = fopen (infilename, "rb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_SendUpdate : cannot open %s", infilename);
        return FALSE;
    }

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (TRUE, &U16version))  /* Boot to app */
    {
        LDBGSTR ("DNG_SendUpdate : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_SendUpdate : dongle version %04X", U16version);

    MISC_GaugeInit (0, DNG_PAGE_SIZE * DNG_NUM_PAGES);

    for (i = 0, U16address = 0; i < DNG_NUM_PAGES;
         i++, U16address += DNG_PAGE_SIZE)
    {
        MISC_GaugeUpdate (U16address);

        if (fread (AU8buffer, DNG_PAGE_SIZE + DNG_CHECKSUM_SIZE, 1, rfile) != 1)
        {
            LDBGSTR1 ("DNG_SendUpdate : read error in %s", infilename);
            MISC_GaugeDone ();
            fclose (rfile);
            return FALSE;
        }

        if (!DNG_SetPage (U16address, AU8buffer, &Bencryptionerror))
        {
            LDBGSTR1 ("DNG_SendUpdate : update error at %04X", U16address);
            MISC_GaugeDone ();
            fclose (rfile);
            return FALSE;
        }
    }
    MISC_GaugeDone ();
    fclose (rfile);
    MDBGSTR ("DNG_SendUpdate : dongle is updated");
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

BOOL DNG_GetLoaderDecryptionKey (UINT32 U32phoneid, UINT8 * pDst)
{
    UINT16                        U16version;
    UINT8                         U8response;

    if (!TTY_OpenCOMPort
        (U8comport_dng, DNG_SPEED, DNG_TIMEOUT, 4096, TRUE, TRUE))
    {
        return FALSE;
    }

    if (!DNG_Boot (FALSE, &U16version)) /* Boot to app */
    {
        LDBGSTR ("DNG_GetLoaderDecryptionKey : dongle not found");
        TTY_CloseCOMPort (U8comport_dng);
        return FALSE;
    }

    MDBGSTR1 ("DNG_GetLoaderDecryptionKey : dongle version %04X", U16version);

    if (!DNG_ApplicationBootOrCalculation (TRUE))
    {
        return FALSE;
    }

    if (!DNG_SetupCommand (DNG_GET_LOADER_DECRYPTION_KEY))
    {
        return FALSE;
    }

    if (!TTY_WriteComm
        (U8comport_dng, (UINT8 *) & U32phoneid, sizeof (U32phoneid)))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, pDst, DNG_LOADER_DECRYPTION_KEY_LEN, 0))
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport_dng, &U8response, sizeof (UINT8), 0))
    {
        return FALSE;
    }

    if (U8response != DNG_CMD_ACK)
    {
        return FALSE;
    }


    TTY_CloseCOMPort (U8comport_dng);
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

UINT16 DNG_GetLoaderDecryptionKeyLen (void)
{
    return DNG_LOADER_DECRYPTION_KEY_LEN;
}

#endif

#ifdef INCLUDE_RC4

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

BOOL DNG_DecryptDevinfo (char *filename, tDNG_DEVINFO * pdevinfo)
{
    UINT8                         devinfo[sizeof (tDNG_DEVINFO) +
                                          DNG_CHECKSUM_SIZE];
    FILE                         *rfile;
    UINT8                         AU8masterkey[64];
    UINT16                        i;
    UINT32                        U32crc;

    rfile = fopen (filename, "rb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_DecryptDevinfo : cannot open %s", filename);
        return FALSE;
    }

    if (fread (devinfo, sizeof (devinfo), 1, rfile) != 1)
    {
        LDBGSTR1 ("DNG_DecryptDevinfo : read error in %s", filename);
        fclose (rfile);
        return FALSE;
    }
    fclose (rfile);

    DNG_GetMasterKey (AU8masterkey, FALSE);

    RC4_Encrypt (AU8masterkey, DNG_GetMasterkeyLength (), devinfo,
                 sizeof (devinfo) - DNG_CHECKSUM_SIZE, devinfo);


    U32crc = MISC_Adler32 (devinfo, sizeof (tDNG_DEVINFO));

    if (U32crc != *((UINT32 *) & devinfo[sizeof (tDNG_DEVINFO)]))
    {
        LDBGSTR1 ("DNG_DecryptDevinfo : invalid device info in %s", filename);
        return FALSE;
    }

    LDBGSTR1 ("DNG_DecryptDevinfo : loaded device info %s", filename);
    memcpy (pdevinfo, devinfo, sizeof (*pdevinfo));
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

BOOL DNG_EncryptDevinfo (tDNG_DEVINFO * pdevinfo)
{
    UINT8                         devinfo[sizeof (tDNG_DEVINFO) +
                                          DNG_CHECKSUM_SIZE];
    FILE                         *rfile;
    UINT8                         AU8masterkey[64];
    char                          filename[SYS_MAX_PATHNAME_LEN] =
        "devinfo.out." FREIA_DI_EXTENSION;
    UINT16                        i;
    UINT32                        U32crc;

    memcpy (devinfo, pdevinfo, sizeof (*pdevinfo));
    U32crc = MISC_Adler32 (devinfo, sizeof (tDNG_DEVINFO));
    *((UINT32 *) & devinfo[sizeof (tDNG_DEVINFO)]) = U32crc;

    DNG_GetMasterKey (AU8masterkey, TRUE);
    RC4_Encrypt (AU8masterkey, DNG_GetMasterkeyLength (), (UINT8 *) pdevinfo,
                 sizeof (*pdevinfo), devinfo);

    MISC_CheckFileName (filename);
    rfile = fopen (filename, "wb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_EncryptDevinfo : cannot create %s", filename);
        return FALSE;
    }

    if (!fwrite (devinfo, sizeof (devinfo), 1, rfile))
    {
        LDBGSTR1 ("DNG_EncryptDevinfo : write error in %s", filename);
        fclose (rfile);
        return FALSE;
    }
    fclose (rfile);

    LDBGSTR1 ("DNG_EncryptDevinfo : encrypted deviceinfo is written to %s",
              filename);
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

void DNG_EncryptStream (UINT8 * pSrc, UINT16 U16len, UINT8 * pDst)
{
    UINT8                         AU8masterkey[64];
    UINT16                        i;
    UINT32                        U32crc;

    U32crc = MISC_Adler32 (pSrc, U16len);

    DNG_GetMasterKey (AU8masterkey, TRUE);
    RC4_Encrypt (AU8masterkey, DNG_GetMasterkeyLength (), pSrc, U16len, pDst);
    *((UINT32 *) & pDst[U16len]) = U32crc;
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

BOOL DNG_CreateUpdate (char *infilename)
{
    FILE                         *rfile, *sfile;
    char                          outfilename[SYS_MAX_PATHNAME_LEN];
    UINT16                        i;
    UINT8                         AU8buffer[DNG_PAGE_SIZE + DNG_CHECKSUM_SIZE];

    rfile = fopen (infilename, "rb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_CreateUpdate : cannot open %s", infilename);
        return FALSE;
    }

    strcpy (outfilename, infilename);
    strcat (outfilename, "." FREIA_DU_EXTENSION);

    MISC_CheckFileName (outfilename);
    sfile = fopen (outfilename, "wb");
    if (!rfile)
    {
        LDBGSTR1 ("DNG_CreateUpdate : cannot create %s", outfilename);
        fclose (rfile);
        return FALSE;
    }

    for (i = 0; i < DNG_NUM_PAGES; i++)
    {
        if (fread (AU8buffer, DNG_PAGE_SIZE, 1, rfile) != 1)
        {
            LDBGSTR1 ("DNG_CreateUpdate : read error in %s", infilename);
            fclose (rfile);
            fclose (sfile);
            remove (outfilename);
            return FALSE;
        }

        DNG_EncryptStream (AU8buffer, DNG_PAGE_SIZE, AU8buffer);
        if (fwrite (AU8buffer, sizeof (AU8buffer), 1, sfile) != 1)
        {
            LDBGSTR1 ("DNG_CreateUpdate : write error in %s", outfilename);
            fclose (rfile);
            fclose (sfile);
            remove (outfilename);
            return FALSE;
        }
    }
    fclose (rfile);
    fclose (sfile);
    LDBGSTR1 ("DNG_CreateUpdate : update file %s is created", outfilename);
    return TRUE;
}

#endif
