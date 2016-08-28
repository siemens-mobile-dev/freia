
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

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "ttyloc.h"
#include "ttypub.h"
#ifdef INCLUDE_RC4
#include "dngpub.h"
#include "rc4pub.h"
#endif

static const UINT16           U16baseport[] = { 0, 0x378, 0x278, 0x3bc };
extern eERROR                 freia_errno;
static tTTY_CONFIG            TTY_config[TTY_MAX_COM_PORTS + 1];    /* using 1,2,... as indices not 0,1,... */
static HANDLE                 hComm[TTY_MAX_COM_PORTS + 1] =
    { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static UINT32                 U32currentspeed[TTY_MAX_COM_PORTS + 1] =
    { -1, -1, -1, -1, -1, -1, -1, -1 };
#ifdef INCLUDE_RC4
static UINT8                  AU8encryptbuffer[256];
#endif


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

static BOOL TTY_SynchComm (UINT8 U8lptport, BOOL Bwaitforhigh)
{
    UINT32                        U32timeout = 20 * FREIA_USART_TIMEOUT;

    while (U32timeout--)
    {
        if (Bwaitforhigh)
        {
            if (MISC_inport (U16baseport[U8lptport], 1) & 0x08)
            {
                return TRUE;
            }
        }
        else
        {
            if (!(MISC_inport (U16baseport[U8lptport], 1) & 0x08))
            {
                return TRUE;
            }
        }

        MISC_delay (1);
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

static BOOL TTY_ReadLPTNibble (UINT8 U8lptport, UINT8 * pnibble)
{
    MISC_outport (U16baseport[U8lptport], 2, 0x21);

    if (!TTY_SynchComm (U8lptport, FALSE))
    {
        LDBGSTR ("TTY_ReadLPTNibble : read synch error");
        return FALSE;
    }

    *pnibble = MISC_inport (U16baseport[U8lptport], 0) & 0xF;

    MISC_outport (U16baseport[U8lptport], 2, 0x0);

    if (!TTY_SynchComm (U8lptport, TRUE))
    {
        LDBGSTR ("TTY_ReadLPTNibble : read synch error");
        return FALSE;
    }

    MISC_outport (U16baseport[U8lptport], 2, 0x01);

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

static BOOL TTY_ReadLPTByte (UINT8 U8lptport, UINT8 * pbyte)
{
    UINT8                         U8nibble;

    *pbyte = 0;

    if (!TTY_ReadLPTNibble (U8lptport, &U8nibble))
    {
        return FALSE;
    }

    *pbyte = U8nibble;

    if (!TTY_ReadLPTNibble (U8lptport, &U8nibble))
    {
        return FALSE;
    }

    *pbyte |= (U8nibble << 4);

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

static BOOL TTY_WriteLPTNibble (UINT8 U8lptport, UINT8 pnibble)
{
    MISC_outport (U16baseport[U8lptport], 2, 0x21);

    if (!TTY_SynchComm (U8lptport, TRUE))
    {
        HDBGSTR ("TTY_WriteLPTNibble : write synch error");
        return FALSE;
    }

    MISC_outport (U16baseport[U8lptport], 0, pnibble | 0xF0);

    MISC_outport (U16baseport[U8lptport], 2, 0x00);

    if (!TTY_SynchComm (U8lptport, FALSE))
    {
        HDBGSTR ("TTY_WriteLPTNibble : write synch error");
        return FALSE;
    }

    MISC_outport (U16baseport[U8lptport], 2, 0x01);

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

static BOOL TTY_WriteLPTByte (UINT8 U8lptport, UINT8 pbyte)
{
    if (!TTY_WriteLPTNibble (U8lptport, pbyte & 0x0F))
    {
        return FALSE;
    }

    if (!TTY_WriteLPTNibble (U8lptport, pbyte >> 4))
    {
        return FALSE;
    }

    if (!TTY_SynchComm (U8lptport, TRUE))
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

static BOOL TTY_ReadLPTFile (UINT8 U8comport, UINT8 * pDst,
                             UINT32 U32maxrequested, UINT32 * pRead,
                             void *dummy)
{
    UINT32                        i;
    UINT8                         U8lptport = U8comport - 4;    /* change from com5 to LPT1 */

    if (dummy)
    {
    }

    for (i = 0; i < U32maxrequested; i++)
    {
        if (!TTY_ReadLPTByte (U8lptport, &pDst[i]))
        {
            *pRead = i;
            return TRUE;
        }
    }

    *pRead = U32maxrequested;
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

static BOOL TTY_WriteLPTFile (UINT8 U8comport, UINT8 * pSrc,
                              UINT32 U32maxrequested, UINT32 * pWritten,
                              void *dummy)
{
    UINT32                        i;
    UINT8                         U8lptport = U8comport - 4;    /* change from com5 to LPT1 */

    if (dummy)
    {
    }

    for (i = 0; i < U32maxrequested; i++)
    {
        if (!TTY_WriteLPTByte (U8lptport, pSrc[i]))
        {
            *pWritten = i;
            return TRUE;
        }
    }

    *pWritten = U32maxrequested;
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

void TTY_Construct (void)
{
    UINT8                         i;

    for (i = 0; i < TTY_MAX_COM_PORTS + 1; i++)
    {
        hComm[i] = NULL;
        U32currentspeed[i] = -1;
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

BOOL TTY_ReadComm (UINT8 U8comport, UINT8 * pAU8buffer, UINT32 U32requested,
                   UINT32 U32limit)
{
    DWORD                         dwErrorFlags;
    COMSTAT                       ComStat;
    UINT32                        U32blocksize, U32readbytes;
    UINT32                        U32lastpendingbytes =
        (UINT32) - 1, U32idx, U32retry;
    BOOL                          Btocontinue;
    UINT8                         U32badblockretry = RETRY_TIMES / 2;

    if (U32requested > TTY_DEFAULT_READBLOCK_LEN || U32requested == 0)
    {
        U32blocksize = TTY_DEFAULT_READBLOCK_LEN;
    }
    else
    {
        U32blocksize = U32requested;
    }

    if (U32limit > 0)
    {
        memset (pAU8buffer, 0, U32limit);
    }

    TTY_config[U8comport].U32receivedbytes = 0;

    if (!TTY_config[U8comport].Binitialized)
    {
        return FALSE;
    }

    U32idx = 0;
    do
    {
        if (U32limit != 0)
        {
            if (U32idx + U32blocksize > U32limit)
            {
                U32blocksize = U32limit - U32idx;
                if (U32blocksize == 0)
                {
                    TTY_config[U8comport].U32receivedbytes = U32idx;
                    return TRUE;
                }
            }
        }

        if (U8comport < 5)
        {
            if (!ReadFile
                (hComm[U8comport], &pAU8buffer[U32idx], U32blocksize,
                 &U32readbytes, NULL))
            {
                MDBGSTR ("TTY_ReadComm : cannot read from COM port");
                /*
                 * not so keen on handling comm error
                 */
                ClearCommError (hComm[U8comport], &dwErrorFlags, &ComStat);
                return FALSE;
            }
        }
        else
        {
#ifndef LOGMODE
            if (!TTY_ReadLPTFile
                (U8comport, &pAU8buffer[U32idx], U32blocksize, &U32readbytes,
                 NULL))
            {
                MDBGSTR ("TTY_ReadComm : cannot read from LPT port");
                return FALSE;
            }
#endif
        }

        U32idx += U32readbytes;

        if (U32requested == 0)
        {
            Btocontinue = U32readbytes == TTY_DEFAULT_READBLOCK_LEN;
        }
        else
        {
            Btocontinue = U32idx < U32requested && U32readbytes > 0;

            if (U32idx + U32blocksize > U32requested)
            {
                U32blocksize = U32requested - U32idx;
                if (U32blocksize == 0)
                {
                    TTY_config[U8comport].U32receivedbytes = U32idx;
                    return TRUE;
                }
            }
        }

        if (!Btocontinue)       /* if we're to finish, check that there's nothing left */
        {
            if (U32requested == 0 || U32idx < U32requested)
            {
                U32retry = RETRY_TIMES;
                do
                {
                    if (U8comport < 5)
                    {
                        ClearCommError (hComm[U8comport], &dwErrorFlags,
                                        &ComStat);
                        Btocontinue = ComStat.cbInQue != 0;
                        if (!Btocontinue)
                        {
                            if (U32lastpendingbytes == 0
                                && ComStat.cbInQue == 0)
                            {
                                if (--U32badblockretry == 0 || U32idx >= U32requested)  /* We're really finished */
                                {
                                    Btocontinue = FALSE;
                                    break;
                                }
                            }
                            U32lastpendingbytes = ComStat.cbInQue;

                            MISC_Sleep (TTY_config[U8comport].U16comdelay);
                        }
                    }
                    else
                    {
                        Btocontinue = TRUE;
                        MISC_Sleep (TTY_config[U8comport].U16comdelay);
                    }
                } while (!Btocontinue && U32retry--);
            }
        }
    } while (Btocontinue);

    TTY_config[U8comport].U32receivedbytes = U32idx;
    return U32idx >= U32requested;
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

BOOL TTY_WriteComm (UINT8 U8comport, UINT8 * pAU8buffer, UINT32 U32desiredsize)
{
    DWORD                         dwErrorFlags;
    COMSTAT                       ComStat;
    UINT32                        U32blocksize;

    if (!TTY_config[U8comport].Binitialized)
    {
        return FALSE;
    }

    while (U32desiredsize > 0)
    {
        U32blocksize =
            (U32desiredsize >
             TTY_WRITE_BLOCKSIZE ? TTY_WRITE_BLOCKSIZE : U32desiredsize);


        if (U8comport < 5)
        {
            if (!WriteFile
                (hComm[U8comport], pAU8buffer, U32blocksize,
                 &TTY_config[U8comport].U32writtenbytes, NULL))
            {
                MDBGSTR ("TTY_WriteComm : cannot write to COM port");
                /*
                 * not so keen on handling comm error
                 */
                ClearCommError (hComm[U8comport], &dwErrorFlags, &ComStat);
                return FALSE;
            }
        }
        else
        {
#ifndef LOGMODE
            if (!TTY_WriteLPTFile
                (U8comport, pAU8buffer, U32blocksize,
                 &TTY_config[U8comport].U32writtenbytes, NULL))
            {
                MDBGSTR ("TTY_WriteComm : cannot write to LPT port");
                /*
                 * not so keen on handling comm error
                 */
                return FALSE;
            }
#endif
        }

        if (TTY_config[U8comport].U32writtenbytes != U32blocksize)
        {
            MDBGSTR
                ("TTY_WriteComm : invalid number of written bytes to COM port");
            return FALSE;
        }

        pAU8buffer += U32blocksize;
        U32desiredsize -= U32blocksize;
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
* Please note that in "real" life, you must receive     *
* "preencrypted" data plus the original checksum of     *
* the encrypted stream, since RC4 is not available      *
*                                                       *
* RETURNS:                                              *
*                                                       *
*********************************************************/

BOOL TTY_ReadStream (UINT8 U8comport, UINT8 * pAU8stream, UINT16 U16len,
                     BOOL Bencrypted, BOOL * pBencrypterror)
{
    UINT32                        U32crc, U32origcrc;
    UINT16                        i;

#ifdef INCLUDE_RC4
    UINT8                         U8byte;
    UINT32                        U32encryptcrc;
    UINT8                         AU8masterkey[64];
#endif

    *pBencrypterror = FALSE;

    if (!TTY_ReadComm (U8comport, (UINT8 *) & i, sizeof (UINT16), 0))
    {
        return (U16len == 0 ? TRUE : FALSE);
    }

    if (i != U16len)
    {
        return FALSE;
    }

    if (!TTY_ReadComm (U8comport, pAU8stream, U16len, 0))
    {
        return FALSE;
    }

    U32crc = MISC_Adler32 (pAU8stream, U16len);

    if (Bencrypted)
    {
#ifdef INCLUDE_RC4
        if (!TTY_ReadComm (U8comport, &U32origcrc, sizeof (UINT32), 0))
        {
            return FALSE;
        }

        /*
         * We send non-device dependant encryption 
         */
        DNG_GetMasterKey (AU8masterkey, FALSE);
        RC4_Encrypt (AU8masterkey, DNG_GetMasterkeyLength (), pAU8stream,
                     U16len, pAU8stream);

        U32encryptcrc = MISC_Adler32 (pAU8stream, U16len);

        if (U32encryptcrc != U32origcrc)
        {
            *pBencrypterror = TRUE;
            return FALSE;
        }
#else
        if (!TTY_ReadComm (U8comport, &pAU8stream[U16len], sizeof (UINT32), 0))
        {
            return FALSE;
        }
#endif
    }

    if (!TTY_ReadComm (U8comport, (UINT8 *) & U32origcrc, sizeof (UINT32), 0))
    {
        return FALSE;
    }

    return U32origcrc == U32crc;
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
* Please note that in "real" life, you must send        *
* "preencrypted" data plus the original checksum of     *
* the encrypted stream, since RC4 is not available      *
*                                                       *
* RETURNS:                                              *
*                                                       *
*********************************************************/

BOOL TTY_WriteStream (UINT8 U8comport, UINT8 * pSrc, UINT16 U16len,
                      BOOL Bencrypted)
{
    UINT32                        U32calcedcrc;
    UINT8                        *pAU8stream = pSrc;

#ifdef INCLUDE_RC4
    UINT32                        U32origcrc, U32receivedcrc;
    UINT8                         AU8masterkey[64];
#endif

    if (!TTY_WriteComm (U8comport, (UINT8 *) & U16len, sizeof (UINT16)))
    {
        return FALSE;
    }

#ifdef INCLUDE_RC4
    if (Bencrypted)
    {
        U32origcrc = MISC_Adler32 (pAU8stream, U16len);

        /*
         * We send device dependant encryption
         */

        DNG_GetMasterKey (AU8masterkey, TRUE);
        RC4_Encrypt (AU8masterkey, DNG_GetMasterkeyLength (), pSrc, U16len,
                     AU8encryptbuffer);

        pAU8stream = AU8encryptbuffer;
    }
#endif

    if (!TTY_WriteComm (U8comport, pAU8stream, U16len))
    {
        return FALSE;
    }

    if (Bencrypted)
    {
#ifdef INCLUDE_RC4
        if (!TTY_WriteComm (U8comport, &U32origcrc, sizeof (UINT32)))
        {
            return FALSE;
        }
#else
        if (!TTY_WriteComm (U8comport, &pAU8stream[U16len], sizeof (UINT32)))
        {
            return FALSE;
        }
#endif
    }

    U32calcedcrc = MISC_Adler32 (pAU8stream, U16len);

    if (!TTY_WriteComm (U8comport, (UINT8 *) & U32calcedcrc, sizeof (UINT32)))
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

BOOL TTY_SetSpeed (UINT8 U8comport, UINT32 U32newrate)
{
    DCB                           dcb;

    if (!TTY_config[U8comport].Binitialized)
    {
        return FALSE;
    }

    if (!GetCommState (hComm[U8comport], &dcb))
    {
        LDBGSTR ("TTY_SetSpeed : cannot get status of port");
        CloseHandle (hComm[U8comport]);
        hComm[U8comport] = 0;
        freia_errno = KE_INVALID_COM_PORT;
        return FALSE;
    }

    dcb.BaudRate = U32newrate;

    if (!SetCommState (hComm[U8comport], (DCB *) & dcb))
    {
        LDBGSTR ("TTY_SetSpeed : cannot set speed of port");
        return FALSE;
    }

    PurgeComm (hComm[U8comport],
               PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    U32currentspeed[U8comport] = U32newrate;

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

UINT32 TTY_GetSpeed (UINT8 U8comport)
{
    return U32currentspeed[U8comport];
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

BOOL TTY_OpenCOMPort (UINT8 U8comport, UINT32 U32speed,
                      UINT32 U32comdelay, UINT32 U32buffsize, BOOL Brts,
                      BOOL Bdtr)
{
    char                         *COMport = "COMx";
    DCB                           dcb;

#ifndef LOGMODE
    UINT8                         U8lptport = U8comport - 4;
#endif

    TTY_config[U8comport].Binitialized = FALSE;

#ifndef LOGMODE
    if (U8comport >= 5)
    {
        MDBGSTR1 ("TTY_OpenCOMPort : opening LPT%d", U8lptport);
        MISC_outport (U16baseport[U8lptport], 0, 0xFF);
        MISC_outport (U16baseport[U8lptport], 2, 0x01);
        hComm[U8comport] = (HANDLE) 0xFE;
        TTY_config[U8comport].Binitialized = TRUE;
        return TRUE;
    }
#endif

    MDBGSTR1 ("TTY_OpenCOMPort : opening COM%d", U8comport);

    if (U8comport < 1 || U8comport > 4)
    {
        freia_errno = KE_INVALID_COM_PORT;
        return FALSE;
    }

    if (hComm[U8comport])       /* opened? */
    {
        CloseHandle (hComm[U8comport]);
        hComm[U8comport] = 0;
    }

    memset (&dcb, 0, sizeof (dcb));

    if (!BuildCommDCB ("baud=1200 parity=N data=8 stop=1", &dcb))
    {
        dcb.DCBlength = sizeof (DCB);
        dcb.ByteSize = 8;
        dcb.StopBits = ONESTOPBIT;
        dcb.XonLim = 2048;
        dcb.XoffLim = 512;
        dcb.XonChar = 0x11;
        dcb.XoffChar = 0x13;
        dcb.ErrorChar = dcb.EofChar = dcb.EvtChar = 0;
        dcb.Parity = NOPARITY;
    }

    // set RTS
    dcb.fRtsControl = (Brts ? RTS_CONTROL_ENABLE : RTS_CONTROL_DISABLE);
    // set DTR
    dcb.fDtrControl = (Bdtr ? DTR_CONTROL_ENABLE : DTR_CONTROL_DISABLE);

    dcb.BaudRate = (U32speed > 115200 ? 115200 : U32speed);
    U32currentspeed[U8comport] = U32speed;

    HDBGSTR3 ("TTY_OpenCOMPort : baudrate is %d, DTR is %s, RTS is %s",
              U32currentspeed[U8comport],
              (dcb.fDtrControl == DTR_CONTROL_ENABLE ? "enabled" : "disabled"),
              (dcb.fRtsControl == RTS_CONTROL_ENABLE ? "enabled" : "disabled"));

    COMport[3] = U8comport + '0';
    hComm[U8comport] =
        CreateFile (COMport, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hComm[U8comport] == NULL)
    {
        LDBGSTR1 ("TTY_OpenCOMPort : cannot open COM%d", U8comport);
        freia_errno = KE_INVALID_COM_PORT;
        return FALSE;
    }

    if (!SetupComm (hComm[U8comport], U32buffsize, U32buffsize))
    {
        LDBGSTR1 ("TTY_OpenCOMPort : cannot setup COM%d", U8comport);
        freia_errno = KE_INVALID_COM_PORT;
        return FALSE;
    }

    if (!SetCommState (hComm[U8comport], &dcb))
    {
        CloseHandle (hComm[U8comport]);
        hComm[U8comport] = 0;
        LDBGSTR1 ("TTY_OpenCOMPort : cannot setup COM%d", U8comport);
        freia_errno = KE_INVALID_COM_PORT;
        return FALSE;
    }

    GetCommTimeouts (hComm[U8comport], &TTY_config[U8comport].newcommtimeouts);
    TTY_config[U8comport].oldcommtimeouts =
        TTY_config[U8comport].newcommtimeouts;
    TTY_config[U8comport].newcommtimeouts.ReadIntervalTimeout = -1;
    TTY_config[U8comport].newcommtimeouts.ReadTotalTimeoutConstant =
        TTY_config[U8comport].U16comdelay = U32comdelay;
    TTY_config[U8comport].newcommtimeouts.WriteTotalTimeoutMultiplier = 10;
    TTY_config[U8comport].newcommtimeouts.WriteTotalTimeoutConstant =
        U32comdelay;
    SetCommTimeouts (hComm[U8comport], &TTY_config[U8comport].newcommtimeouts);

    PurgeComm (hComm[U8comport],
               PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

    TTY_config[U8comport].Binitialized = TRUE;

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

void TTY_CloseCOMPort (UINT8 U8comport)
{
    DCB                           dcb;

#ifndef LOGMODE
    UINT8                         U8lptport = U8comport - 4;
#endif

    if (TTY_config[U8comport].Binitialized)
    {
#ifndef LOGMODE
        if (U8comport >= 5)
        {
            HDBGSTR1 ("TTY_CloseCOMPort : Closing LPT%d", U8lptport);
            MISC_outport (U16baseport[U8lptport], 0, 0x0);
            MISC_outport (U16baseport[U8lptport], 2, 0x0);
            return;
        }
#endif
        HDBGSTR1 ("TTY_CloseCOMPort : Closing COM%d", U8comport);

        GetCommState (hComm[U8comport], &dcb);

        /*
         * clear RTS 
         */
        dcb.fOutxCtsFlow = 0;
        dcb.fRtsControl = RTS_CONTROL_DISABLE;
        dcb.fDtrControl = DTR_CONTROL_DISABLE;

        SetCommState (hComm[U8comport], &dcb);

        SetCommTimeouts (hComm[U8comport],
                         &TTY_config[U8comport].oldcommtimeouts);
        CloseHandle (hComm[U8comport]);
        hComm[U8comport] = 0;
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

void TTY_Destruct (void)
{
    UINT8                         i;

    for (i = 0; i < TTY_MAX_COM_PORTS + 1; i++)
    {
        if (hComm[i] != NULL)
        {
            TTY_CloseCOMPort (i);
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

BOOL TTY_SetReadTimeout (UINT8 U8comport, UINT32 U32timeout)
{
    if (!TTY_config[U8comport].Binitialized)
    {
        return FALSE;
    }

    TTY_config[U8comport].newcommtimeouts.ReadTotalTimeoutConstant = U32timeout;
    SetCommTimeouts (hComm[U8comport], &TTY_config[U8comport].newcommtimeouts);
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

UINT32 TTY_GetReadTimeout (UINT8 U8comport)
{
    if (!TTY_config[U8comport].Binitialized)
    {
        return 0;
    }

    return TTY_config[U8comport].newcommtimeouts.ReadTotalTimeoutConstant;
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

UINT32 TTY_GetNumberOfReceivedBytes (UINT8 U8comport)
{
    if (!TTY_config[U8comport].Binitialized)
    {
        return 0;
    }

    return TTY_config[U8comport].U32receivedbytes;
}
