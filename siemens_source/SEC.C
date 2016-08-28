
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
#include "secpub.h"
#include "secloc.h"
#include "dngpub.h"

extern tFREIA_PHONEINFO       freia_phoneinfo;

static const UINT8 watermark[] = FREIA_WATERMARK_ID "4\x13\xfe\xe5\x07\x55\x10\x83\x47";

static void                   InvDoSomeTrickyCalculations (eMISC_MODEL_INDICES
                                                           phonemodel,
                                                           UINT8 Vls,
                                                           UINT8 * Arr);

static UINT8                  GetCodeTable (eMISC_MODEL_INDICES phonemodel,
                                            UINT8 U8idxhi, UINT8 U8idxlo);

static UINT32                 ConvertIDIntoDWord (UINT32 * pU32phoneid,
                                                  UINT16 XKey, UINT8 * pEnc00,
                                                  UINT16 * pKeys);

static void                   Gosub4FD7EC (UINT16 Key, UINT16 Ln, Arr512 Dec00);

static void                   DecryptC30HiddenBlocks (UINT32 * pU32phoneid,
                                                      UINT16 * pKeys, UINT16 Ln,
                                                      UINT16 XKey,
                                                      UINT8 * pDecod, UINT16 Ad,
                                                      UINT16 * pIdx,
                                                      UINT8 * pEncod);

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

void SEC_NormalIMEIToBCDIMEI (char *myIMEI, UINT8 * myBCD)
{
    UINT16                        i;

    for (i = 0; i < FREIA_IMEI_LEN / 2; i++)
    {
        myBCD[i] = (UINT8) (myIMEI[2 * i] - '0');
        myBCD[i] |= ((UINT8) (myIMEI[2 * i + 1] - '0') << 4);
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

static void ConvertToBCD (char *myIMEI, Arr08 myBCD)
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

static void ConvertToC30BCD (char *myIMEI, Arr08 myBCD)
{
    UINT16                        i = 0;

    while (i < 7)
    {
        myBCD[i] = (UINT8) (*(myIMEI++) - '0');
        myBCD[i] |= (UINT8) ((*(myIMEI++) - '0') << 4);
        i++;
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

static void ConvertIMEIToWords (char *IMEI, UINT16 * pWords00)
{
    UINT16                        i;

    for (i = 0; i < 14; i++)
    {
        pWords00[i] = IMEI[i] - '0';
    }

    pWords00[14] = 0x0000;
    pWords00[15] = 0xFFFF;
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

static void Operation01WithWords01 (UINT16 * pWords00, UINT16 * pWords01)
{
    UINT16                        i, j;
    UINT16                        BP06;

    i = 0;
    j = 0;
    while (pWords00[i] != 0xFFFF)
    {
        if (!(i & 1))
        {
            pWords01[j] = pWords00[i];
        }
        else
        {
            BP06 = (pWords00[i] << 1);
            if (BP06 <= 9)
            {
                pWords01[j] = BP06;
            }
            else
            {
                pWords01[j] = BP06 / 10;
                pWords01[j + 1] = BP06 % 10;
                j++;
            }
        }
        i++;
        j++;
    }
    pWords01[j] = 0xFFFF;
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

static UINT16 GetCRCOfWords01 (UINT16 * pWords01)
{
    UINT16                        i, Result;

    for (i = 0, Result = 0; pWords01[i] != 0xFFFF; Result += pWords01[i], i++);

    return Result;
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

static void Cod00XORWithKey (Arr08 Key, Arr08 srcCod, Arr08 newCod)
{
    UINT8                         i;

    for (i = 0; i < 8; i++)
    {
        newCod[i] = srcCod[i] ^ Key[i];
    }
}

#ifndef USE_DONGLE

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

static void ModifyinputArray1 (Arrdw6 Arr64, Arrdw16 Arr4c)
{
    UINT16                        j, i;
    UINT32                        Arr64_0, Arr64_1, Arr64_2, Arr64_3;
    UINT8                         ReorderIndices[4] = { 0, 2, 1, 3 };

    Arr64_0 = Arr64[0];
    Arr64_1 = Arr64[1];
    Arr64_2 = Arr64[2];
    Arr64_3 = Arr64[3];

    for (i = 0; i < 4; i++)
    {
        Arr64_0 +=
            (Arr4c[(i << 2)] + (((Arr64_2 ^ Arr64_3) & Arr64_1) ^ Arr64_3));
        Arr64_0 = (Arr64_0 << 3) | (Arr64_0 >> 29);

        Arr64_3 +=
            (Arr4c[(i << 2) + 1] + (((Arr64_1 ^ Arr64_2) & Arr64_0) ^ Arr64_2));
        Arr64_3 = (Arr64_3 << 7) | (Arr64_3 >> 25);

        Arr64_2 +=
            (Arr4c[(i << 2) + 2] + (((Arr64_0 ^ Arr64_1) & Arr64_3) ^ Arr64_1));
        Arr64_2 = (Arr64_2 << 11) | (Arr64_2 >> 21);

        Arr64_1 +=
            (Arr4c[(i << 2) + 3] + (((Arr64_3 ^ Arr64_0) & Arr64_2) ^ Arr64_0));
        Arr64_1 = (Arr64_1 << 19) | (Arr64_1 >> 13);
    }

    for (i = 0; i < 4; i++)
    {
        Arr64_0 +=
            ((((Arr64_1 & Arr64_2) | (Arr64_1 & Arr64_3)) | (Arr64_2 & Arr64_3))
             + Arr4c[i] + 0x5A827999);
        Arr64_0 = (Arr64_0 << 3) | (Arr64_0 >> 29);

        Arr64_3 +=
            ((((Arr64_0 & Arr64_1) | (Arr64_0 & Arr64_2)) | (Arr64_1 & Arr64_2))
             + Arr4c[i + 4] + 0x5A827999);
        Arr64_3 = (Arr64_3 << 5) | (Arr64_3 >> 27);

        Arr64_2 +=
            ((((Arr64_3 & Arr64_0) | (Arr64_3 & Arr64_1)) | (Arr64_0 & Arr64_1))
             + Arr4c[i + 8] + 0x5A827999);
        Arr64_2 = (Arr64_2 << 9) | (Arr64_2 >> 23);

        Arr64_1 +=
            ((((Arr64_2 & Arr64_3) | (Arr64_2 & Arr64_0)) | (Arr64_3 & Arr64_0))
             + Arr4c[i + 12] + 0x5A827999);
        Arr64_1 = (Arr64_1 << 13) | (Arr64_1 >> 19);
    }

    for (i = 0; i < 4; i++)
    {
        j = ReorderIndices[i];

        Arr64_0 += (Arr4c[j] + (Arr64_1 ^ Arr64_2 ^ Arr64_3) + 0x6Ed9EBA1);
        Arr64_0 = (Arr64_0 << 3) | (Arr64_0 >> 29);

        Arr64_3 += (Arr4c[j + 8] + (Arr64_0 ^ Arr64_1 ^ Arr64_2) + 0x6Ed9EBA1);
        Arr64_3 = (Arr64_3 << 9) | (Arr64_3 >> 23);

        Arr64_2 += (Arr4c[j + 4] + (Arr64_3 ^ Arr64_0 ^ Arr64_1) + 0x6Ed9EBA1);
        Arr64_2 = (Arr64_2 << 11) | (Arr64_2 >> 21);

        Arr64_1 += (Arr4c[j + 12] + (Arr64_2 ^ Arr64_3 ^ Arr64_0) + 0x6Ed9EBA1);
        Arr64_1 = (Arr64_1 << 15) | (Arr64_1 >> 17);
    }

    Arr64[0] += Arr64_0;
    Arr64[1] += Arr64_1;
    Arr64[2] += Arr64_2;
    Arr64[3] += Arr64_3;
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

static void ModifyinputArray2 (Arrdw6 Arr64, UINT32 * pCodE2)
{
    Arrdw16                       arr50;
    UINT16                        i;

    arr50[0] = 0x80;
    for (i = 1; i < 14; i++)
    {
        arr50[i] = 0x0;
    }

    arr50[14] = Arr64[4];
    arr50[15] = Arr64[5];

    ModifyinputArray1 (Arr64, arr50);

    for (i = 0; i < 4; i++)
    {
        pCodE2[i] = Arr64[i];
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

static void CreateCodE2 (eMISC_MODEL_INDICES phonemodel, UINT32 * pCodE2)
{
    UINT16                        i;
    Arrdw16                       Arr4c;
    Arrdw6                        Arr64;

    for (i = 0; i < 16; i++)
    {
        Arr4c[i] = UnknownKey1[phonemodel][i];
    }

    for (i = 0; i < 6; i++)
    {
        Arr64[i] = UnknownKey2[i];
    }

    ModifyinputArray1 (Arr64, Arr4c);
    ModifyinputArray2 (Arr64, pCodE2);
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

static void DoSomeTrickyCalculations (eMISC_MODEL_INDICES phonemodel, UINT8 Vls,
                                      UINT8 * Arr)
{
    UINT16                        i;
    UINT8                         U8byte, U8idxlo, U8idxhi;
    UINT8                         Bytes05[FREIA_5009_AND_0001_LEN];

    memcpy (Bytes05, Arr, sizeof (Bytes05));

    for (i = 0; i < FREIA_5009_AND_0001_LEN / 2; i++)
    {
        U8idxhi = (Arr[i] >> 4) & 0x0F;
        U8idxlo = Arr[i] & 0x0F;

        U8byte = GetCodeTable (phonemodel, U8idxhi, U8idxlo);

        if (!(i & 1))
        {
            Arr[i] = (U8byte ^ Vls) ^ Bytes05[i + FREIA_5009_AND_0001_LEN / 2];
        }
        else
        {
            Arr[i] = (~U8byte ^ Vls) ^ Bytes05[i + FREIA_5009_AND_0001_LEN / 2];
        }

        Arr[i + FREIA_5009_AND_0001_LEN / 2] = Bytes05[i];
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

void SEC_RecreateBlocks5009And76 (eMISC_MODEL_INDICES phonemodel,
                                  UINT8 * pB5009, UINT8 * pB76)
{
    SINT8                         i;

    for (i = 7; i >= 0; i--)
    {
        if (!(i & 1))
        {
            InvDoSomeTrickyCalculations (phonemodel, Pars[phonemodel][i],
                                         pB5009);
        }
        else
        {
            InvDoSomeTrickyCalculations (phonemodel, Pars[phonemodel][i], pB76);
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

static UINT16 DoLikeProcedure0323 (char *IMEI)
{
    UINT16                        CRC;
    UINT16                        Words01[64];
    UINT16                        Words00[64];

    ConvertIMEIToWords (IMEI, Words00);
    Operation01WithWords01 (Words00, Words01);
    CRC = GetCRCOfWords01 (Words01);

    if ((CRC % 10) == 0)
    {
        return 0;
    }

    return (CRC / 10 + 1) * 10 - CRC;
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

void SEC_CreateBlocks5009And76 (eMISC_MODEL_INDICES phonemodel, char *IMEI,
                                UINT8 * pB5009, UINT8 * pB0001)
{
    UINT8                         U8digit, BP0A, BP0B, BP0C;
    UINT16                        i, j, Si;
    UINT16                        BP04;
    UINT8                         Bytes01[8], Bytes02[8];

    for (i = 0; i < 8; i++)
    {
        Bytes01[i] = 0;
        Bytes02[i] = 0;
    }

    j = 0;
    Si = 0;
    BP04 = DoLikeProcedure0323 (IMEI) & 0xFF;

    Bytes01[0] = 10;
    i = 0;
    while (i < 14)
    {
        U8digit = IMEI[i] - '0';
        if (!(i & 1))
        {
            Bytes01[j] |= (U8digit << 4);
            j++;
            i++;
            Bytes02[Si] = U8digit;
        }
        else
        {
            Bytes01[j] = U8digit;
            Bytes02[Si] |= (U8digit << 4);
            Si++;
            i++;
        }
    }
    Bytes02[Si] |= (BP04 << 4);

    for (i = 0, BP0A = 0, BP0B = 0; i < 8; i++)
    {
        if (!(i & 1))
        {
            BP0B ^= Bytes02[i];
        }
        else
        {
            BP0A ^= Bytes02[i];
        }
    }

    BP0C = ~(BP0A ^ BP0B);

    for (i = 0; i < 8; i++)
    {
        pB5009[i] = Bytes02[i];
        pB0001[i] = Bytes02[i];
    }

    pB5009[8] = BP0A;
    pB0001[8] = BP0B;
    pB5009[9] = BP0C;
    pB0001[9] = BP0C;

    for (i = 0; i < 8; i++)
    {
        if (!(i & 1))
        {
            DoSomeTrickyCalculations (phonemodel, Pars[phonemodel][i], pB5009);
        }
        else
        {
            DoSomeTrickyCalculations (phonemodel, Pars[phonemodel][i], pB0001);
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

static void GosubC7D524 (UINT8 * Cods08, UINT32 * CodsE2, UINT8 Num)
{
    UINT16                        i;
    UINT8                         Nc01, Nc02;
    UINT8                         Cd01[16];

    Nc01 = 0;
    Num >>= 3;

    Cods08[0] = 0;
    Cods08[1] = 0;
    Cods08[2] = 0;

    for (i = 0; i < 256; i++)
    {
        Cods08[i + 3] = i;
    }

    for (i = 0; i < 4; i++)
    {
        Cd01[i * 4 + 0] = CodsE2[i];
        Cd01[i * 4 + 1] = CodsE2[i] >> 8;
        Cd01[i * 4 + 2] = CodsE2[i] >> 16;
        Cd01[i * 4 + 3] = CodsE2[i] >> 24;
    }

    for (i = 0; i < 256; i++)
    {
        Nc01 += (Cd01[i % Num] + Cods08[i + 3]);
        Nc02 = Cods08[i + 3];
        Cods08[i + 3] = Cods08[Nc01 + 3];
        Cods08[Nc01 + 3] = Nc02;
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

static void GosubC7D5CE (BOOL Bdecrypt, UINT8 * Cods08, UINT8 * Encod,
                         UINT16 idx, UINT8 * Cods00, UINT8 Num)
{
    UINT8                         Rl6, Rl3, Rl1, Rl7, i;

    Rl6 = Cods08[0];
    Rl3 = Cods08[1];
    Rl1 = Cods08[2];
    for (i = 0; i < Num; i++)
    {
        Rl6++;
        Rl1 += Cods08[Rl6 + 3];
        Rl3 += Rl1;
        Rl7 = Rl1 + Cods08[Rl3 + 3];
        Cods08[Rl6 + 3] = Cods08[Rl3 + 3];
        Cods08[Rl3 + 3] = Rl1;
        if (Bdecrypt)
        {
            Rl1 = Cods00[i];
            Encod[i + idx] = Rl1 ^ Cods08[Rl7 + 3];
        }
        else
        {
            Rl1 = Cods00[i] ^ Cods08[Rl7 + 3];
            Encod[i + idx] = Rl1;
        }

    }
    Cods08[0] = Rl6;
    Cods08[1] = Rl3;
    Cods08[2] = Rl1;
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

static void CRCBuffer (UINT8 * pAU8buffer, UINT16 U16len)
{
    UINT16                        i;
    UINT8                         Bt01, Bt02;

    Bt01 = 0;
    Bt02 = 0;
    for (i = 0; i < U16len; i++)
    {
        Bt01 += pAU8buffer[i];
        Bt02 ^= pAU8buffer[i];
    }
    pAU8buffer[U16len] = Bt01;
    pAU8buffer[U16len + 1] = Bt02;
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

static BOOL CreateIMEISpecificBlocksC45 (eMISC_MODEL_INDICES uniquemodel,
                                         eMISC_MODEL_INDICES phonemodel,
                                         char *IMEI, UINT32 * pU32phoneid,
                                         UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    UINT16                        RNDNumberA, RNDNumberB;
    UINT8                         Cod08[512];
    UINT32                        CodE2[4];
    Arr08                         Cod00, BCD;
    BOOL                          Bblocked;

    if (uniquemodel)
    {
    }

    ConvertToBCD (IMEI, BCD);

#ifndef USE_DONGLE
    memcpy (&UnknownKey1[phonemodel][12], pU32phoneid, 4);
    memcpy (&UnknownKey1[phonemodel][14], BCD, 8);
    CreateCodE2 (phonemodel, CodE2);
#else
    if (!DNG_Calculate (phonemodel, IMEI, *pU32phoneid, CodE2, &Bblocked))
    {
        return FALSE;
    }
#endif

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;
    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (FALSE, Cod08, pEnc5008, 0x00, Cod00, 0x08);
    GosubC7D5CE (FALSE, Cod08, pEnc5008, 0x08, StrangeHeaderCopy,
                 sizeof (StrangeHeaderCopy));

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;
    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (FALSE, Cod08, pEnc5008, sizeof (StrangeHeaderCopy) + 8, Cod00,
                 0x08);
    GosubC7D5CE (FALSE, Cod08, pEnc5008, sizeof (StrangeHeaderCopy) + 16,
                 MainBody5008Copy,
                 B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 16));

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;
    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (FALSE, Cod08, pEnc5077, 0x00, Cod00, 0x08);
    GosubC7D5CE (FALSE, Cod08, pEnc5077, 0x08, MainBody5077,
                 B77Sz[phonemodel] - 8);

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

static void CreateMask (Arr512 Mask)
{
    UINT16                        i, j;

    memset (Mask, 0, sizeof (Mask));

    for (j = 1, i = 0; i < 256; i++)
    {
        Mask[i] = j & 0xff;
        Mask[(j & 0xFF) + 256] = i;
        j = (j * 45) % 0x101;
    }
}

#ifndef USE_DONGLE

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

static void CreateKeyTable (Arr512 Mask, Arr08 pAU8src, Arr204 Key, UINT8 Nb01,
                            UINT8 Nb0C, Arr08 pAU8key)
{
    UINT8                         Cd00[18];
    UINT8                         U8idx, U8idx2, i, j, k;

    Key[0] = Nb0C;

    Cd00[8] = Cd00[0x11] = 0;

    i = 1;                      /* starting index */

    for (j = 0; j < 8; j++, i++)
    {
        Cd00[j] = (pAU8src[j] << 5) | (pAU8src[j] >> 3);
        Cd00[8] ^= Cd00[j];

        Key[i] = pAU8key[j];
        Cd00[j + 9] = pAU8key[j];
        Cd00[0x11] ^= pAU8key[j];
    }

    if (Nb0C == 0)
    {
        return;
    }

    for (k = 1; k <= Nb0C; k++)
    {
        for (j = 0; j < 9; j++)
        {
            Cd00[j] = (Cd00[j] << 6) | (Cd00[j] >> 2);
            Cd00[j + 9] = (Cd00[j + 9] << 6) | (Cd00[j + 9] >> 2);
        }

        for (j = 0; j < 8; j++)
        {
            U8idx = Mask[1 + j + k * 2 * 9];
            U8idx = Mask[U8idx];

            if (Nb01 != 0)
            {
                U8idx2 = (j + 2 * k - 1) % 9;
                U8idx += Cd00[U8idx2];
            }
            else
            {
                U8idx += Cd00[j];
            }

            Key[i++] = U8idx;
        }

        for (j = 0; j < 8; j++)
        {
            U8idx = Mask[10 + j + k * 2 * 9];
            U8idx = Mask[U8idx];

            if (Nb01 != 0)
            {
                U8idx2 = (j + 2 * k) % 9;
                U8idx += Cd00[U8idx2 + 9];
            }
            else
            {
                U8idx += Cd00[j + 9];
            }

            Key[i++] = U8idx;
        }
    }
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

static void GosubE5419C (Arr512 Mask, Arr08 AU8src, Arr204 Key, Arr08 AU8dst)
{
    Arr08                         AU8src_copy;
    UINT16                        i, j;
    UINT8                         U8byte;

    memcpy (AU8src_copy, AU8src, sizeof (Arr08));

    i = 0;
    if (Key[0] >= 1)
    {
        for (j = Key[0]; j > 0; j--)
        {
            AU8src_copy[0] ^= Key[++i];
            AU8src_copy[1] += Key[++i];
            AU8src_copy[2] += Key[++i];
            AU8src_copy[3] ^= Key[++i];
            AU8src_copy[4] ^= Key[++i];
            AU8src_copy[5] += Key[++i];
            AU8src_copy[6] += Key[++i];
            AU8src_copy[7] ^= Key[++i];

            AU8src_copy[0] = (Mask[AU8src_copy[0]] + Key[++i]);
            AU8src_copy[1] = (Mask[0x100 + AU8src_copy[1]] ^ Key[++i]);
            AU8src_copy[2] = (Mask[0x100 + AU8src_copy[2]] ^ Key[++i]);
            AU8src_copy[3] = (Mask[AU8src_copy[3]] + Key[++i]);
            AU8src_copy[4] = (Mask[AU8src_copy[4]] + Key[++i]);
            AU8src_copy[5] = (Mask[0x100 + AU8src_copy[5]] ^ Key[++i]);
            AU8src_copy[6] = (Mask[0x100 + AU8src_copy[6]] ^ Key[++i]);
            AU8src_copy[7] = (Mask[AU8src_copy[7]] + Key[++i]);

            AU8src_copy[1] += AU8src_copy[0];
            AU8src_copy[0] += AU8src_copy[1];

            AU8src_copy[3] += AU8src_copy[2];
            AU8src_copy[2] += AU8src_copy[3];

            AU8src_copy[5] += AU8src_copy[4];
            AU8src_copy[4] += AU8src_copy[5];

            AU8src_copy[7] += AU8src_copy[6];
            AU8src_copy[6] += AU8src_copy[7];

            AU8src_copy[2] += AU8src_copy[0];
            AU8src_copy[0] += AU8src_copy[2];

            AU8src_copy[6] += AU8src_copy[4];
            AU8src_copy[4] += AU8src_copy[6];

            AU8src_copy[3] += AU8src_copy[1];
            AU8src_copy[1] += AU8src_copy[3];

            AU8src_copy[7] += AU8src_copy[5];
            AU8src_copy[5] += AU8src_copy[7];

            AU8src_copy[4] += AU8src_copy[0];
            AU8src_copy[0] += AU8src_copy[4];

            AU8src_copy[5] += AU8src_copy[1];
            AU8src_copy[1] += AU8src_copy[5];

            AU8src_copy[6] += AU8src_copy[2];
            AU8src_copy[2] += AU8src_copy[6];

            AU8src_copy[7] += AU8src_copy[3];
            AU8src_copy[3] += AU8src_copy[7];

            U8byte = AU8src_copy[1];
            AU8src_copy[1] = AU8src_copy[4];
            AU8src_copy[4] = AU8src_copy[2];
            AU8src_copy[2] = U8byte;
            U8byte = AU8src_copy[3];
            AU8src_copy[3] = AU8src_copy[5];
            AU8src_copy[5] = AU8src_copy[6];
            AU8src_copy[6] = U8byte;
        }
    }

    AU8src_copy[0] ^= Key[++i];
    AU8src_copy[1] += Key[++i];
    AU8src_copy[2] += Key[++i];
    AU8src_copy[3] ^= Key[++i];
    AU8src_copy[4] ^= Key[++i];
    AU8src_copy[5] += Key[++i];
    AU8src_copy[6] += Key[++i];
    AU8src_copy[7] ^= Key[++i];

    memcpy (AU8dst, AU8src_copy, sizeof (Arr08));
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

static void GosubE5443E (Arr512 Mask, Arr08 AU8src, Arr204 Key, Arr08 AU8dst)
{
    Arr08                         AU8src_copy;
    UINT16                        St, i, j;
    UINT8                         U8byte;

    memcpy (AU8src_copy, AU8src, sizeof (Arr08));

    St = Key[0];

    i = St * 16 + 8;
    AU8src_copy[7] ^= Key[i];
    AU8src_copy[6] -= Key[--i];
    AU8src_copy[5] -= Key[--i];
    AU8src_copy[4] ^= Key[--i];
    AU8src_copy[3] ^= Key[--i];
    AU8src_copy[2] -= Key[--i];
    AU8src_copy[1] -= Key[--i];
    AU8src_copy[0] ^= Key[--i];

    for (j = St; j > 0; j--)
    {
        U8byte = AU8src_copy[4];
        AU8src_copy[4] = AU8src_copy[1];
        AU8src_copy[1] = AU8src_copy[2];
        AU8src_copy[2] = U8byte;
        U8byte = AU8src_copy[5];
        AU8src_copy[5] = AU8src_copy[3];
        AU8src_copy[3] = AU8src_copy[6];

        AU8src_copy[0] -= AU8src_copy[4];
        AU8src_copy[4] -= AU8src_copy[0];
        AU8src_copy[1] -= AU8src_copy[5];
        AU8src_copy[5] -= AU8src_copy[1];
        AU8src_copy[2] -= U8byte;
        AU8src_copy[6] = U8byte - AU8src_copy[2];

        AU8src_copy[3] -= AU8src_copy[7];
        AU8src_copy[7] -= AU8src_copy[3];

        AU8src_copy[0] -= AU8src_copy[2];
        AU8src_copy[2] -= AU8src_copy[0];

        AU8src_copy[4] -= AU8src_copy[6];
        AU8src_copy[6] -= AU8src_copy[4];

        AU8src_copy[1] -= AU8src_copy[3];
        AU8src_copy[3] -= AU8src_copy[1];

        AU8src_copy[5] -= AU8src_copy[7];
        AU8src_copy[7] -= AU8src_copy[5];

        AU8src_copy[0] -= AU8src_copy[1];
        AU8src_copy[1] -= AU8src_copy[0];

        AU8src_copy[2] -= AU8src_copy[3];
        AU8src_copy[3] -= AU8src_copy[2];

        AU8src_copy[4] -= AU8src_copy[5];
        AU8src_copy[5] -= AU8src_copy[4];

        AU8src_copy[6] -= AU8src_copy[7];
        AU8src_copy[7] -= AU8src_copy[6];

        AU8src_copy[7] -= Key[--i];
        AU8src_copy[6] ^= Key[--i];
        AU8src_copy[5] ^= Key[--i];
        AU8src_copy[4] -= Key[--i];
        AU8src_copy[3] -= Key[--i];
        AU8src_copy[2] ^= Key[--i];
        AU8src_copy[1] ^= Key[--i];
        AU8src_copy[0] -= Key[--i];

        AU8src_copy[7] = Mask[AU8src_copy[7] + 0x100] ^ Key[--i];
        AU8src_copy[6] = Mask[AU8src_copy[6] + 0x000] - Key[--i];
        AU8src_copy[5] = Mask[AU8src_copy[5] + 0x000] - Key[--i];
        AU8src_copy[4] = Mask[AU8src_copy[4] + 0x100] ^ Key[--i];
        AU8src_copy[3] = Mask[AU8src_copy[3] + 0x100] ^ Key[--i];
        AU8src_copy[2] = Mask[AU8src_copy[2] + 0x000] - Key[--i];
        AU8src_copy[1] = Mask[AU8src_copy[1] + 0x000] - Key[--i];
        AU8src_copy[0] = Mask[AU8src_copy[0] + 0x100] ^ Key[--i];
    }

    memcpy (AU8dst, AU8src_copy, sizeof (Arr08));
}


#ifndef USE_DONGLE

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

static void CreateKey (eMISC_MODEL_INDICES phonemodel, Arr512 Mask, Arr204 Key)
{
    Arr08                         A35Key1Copy, A35Key2Copy;
    Arr08                         tmpKey, tmpArr, tmpArr2;
    UINT8                         i, j;
    Arr204                        LocalKey;
    Arrdw16                       UnknownKey1Copy;

    for (i = 0; i < 8; i++)
    {
        A35Key1Copy[i] = EncryptKey1[phonemodel][i];
        A35Key2Copy[i] = EncryptKey2[phonemodel][i];
    }

    memcpy (UnknownKey1Copy, UnknownKey1[phonemodel], sizeof (UnknownKey1Copy));
    ((UINT8 *) UnknownKey1Copy)[0x37] = 8;

    for (i = 0; i < 8; i++)
    {
        CreateKeyTable (Mask, A35Key2Copy, LocalKey, 1, 0x0c,
                        (UINT8 *) & UnknownKey1Copy[2 * i]);
        GosubE5419C (Mask, A35Key1Copy, LocalKey, tmpArr);

        CreateKeyTable (Mask, (UINT8 *) & UnknownKey1Copy[2 * i], LocalKey,
                        1, 0x0c, tmpArr);
        GosubE5419C (Mask, A35Key2Copy, LocalKey, tmpArr2);

        Cod00XORWithKey (A35Key2Copy, tmpArr2, tmpArr2);
        Cod00XORWithKey (tmpArr, A35Key1Copy, tmpKey);

        for (j = 0; j < 8; j++)
        {
            A35Key1Copy[j] = tmpKey[j];
            A35Key2Copy[j] = tmpArr2[j];
        }
    }

    CreateKeyTable (Mask, tmpKey, Key, 1, 0x0c, tmpArr2);
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

static void EncryptA35 (Arr512 Mask, Arr08 Cod00, Arr204 Key, UINT8 * pAU8src,
                        UINT8 * pAU8dst, UINT32 U32size)
{
    Arr08                         Cd01, Cd02;
    UINT8                         i, j;

    for (i = 0; i < 8; i++)
    {
        pAU8dst[i] = Cod00[i];
    }

    j = 0;
    U32size -= 8;
    while (U32size != 0)
    {
        for (i = 0; i < 8; i++)
        {
            Cd01[i] = pAU8dst[i + j];
        }

        j += 8;
        Cod00XORWithKey (Cd01, &pAU8src[j - 8], Cd02);
        GosubE5419C (Mask, Cd02, Key, &pAU8dst[j]);
        U32size -= 8;
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

static void DecryptA35 (Arr512 Mask, Arr204 Key, UINT8 * pAU8src,
                        UINT8 * pAU8dst, UINT32 U32size)
{
    Arr08                         Cd01, Cd02;
    UINT32                        i, j;

    j = 0;

    while (U32size != 0)
    {
        for (i = 0; i < 8; i++)
        {
            Cd01[i] = pAU8src[j + i];
        }

        j += 8;

        if (U32size <= 8)
        {
            break;
        }
        else
        {
            GosubE5443E (Mask, &pAU8src[j], Key, Cd02);
            Cod00XORWithKey (Cd02, Cd01, &pAU8dst[j - 8]);
        }
        U32size -= 8;
    }

    Cod00XORWithKey (Cd02, Cd01, Cd02);

    for (i = 0; i < U32size; i++)
    {
        pAU8dst[j - 8 + i] = Cd02[i];
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

static BOOL CreateIMEISpecificBlocksA35 (eMISC_MODEL_INDICES uniquemodel,
                                         eMISC_MODEL_INDICES phonemodel,
                                         char *IMEI, UINT32 * pU32phoneid,
                                         UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    UINT16                        RNDNumberA, RNDNumberB;
    Arr08                         Cod00, BCD;
    Arr512                        Mask;
    Arr204                        Key;
    BOOL                          Bblocked;

    if (uniquemodel)
    {
    }

    ConvertToBCD (IMEI, BCD);

    CreateMask (Mask);

#ifndef USE_DONGLE
    memcpy (&UnknownKey1[phonemodel][12], pU32phoneid, 4);
    memcpy (&UnknownKey1[phonemodel][14], BCD, 8);
    CreateKey (phonemodel, Mask, Key);
#else
    if (!DNG_Calculate (phonemodel, IMEI, *pU32phoneid, Key, &Bblocked))
    {
        return FALSE;
    }
#endif

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;

    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    EncryptA35 (Mask, Cod00, Key, StrangeHeaderCopy, pEnc5008,
                sizeof (StrangeHeaderCopy) + 8);

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;
    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    EncryptA35 (Mask, Cod00, Key, MainBody5008Copy,
                &pEnc5008[sizeof (StrangeHeaderCopy) + 8],
                B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 8));

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;
    Cod00XORWithKey (Cod00XORKey[phonemodel], Cod00, Cod00);
    EncryptA35 (Mask, Cod00, Key, MainBody5077, pEnc5077, B77Sz[phonemodel]);

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

static void Encrypt (UINT16 * RNDCode, UINT8 * Mask, UINT8 * Target,
                     UINT16 ImSn, UINT16 Size, UINT16 Off, UINT16 Sbs)
{
    UINT32                        My;
    UINT16                        Bt, i, Res;

    for (i = 0; i < Size; i += 2)
    {
        Bt = ((UINT16) Mask[i + 1] << 8) | Mask[i];
        My = ((UINT32) (*RNDCode) << 16) + Bt;
        *RNDCode = My % ImSn;
        Res = My / ImSn;
        Target[(Size - i) - 02 + Off - Sbs] = Res;
        Target[(Size - i) - 01 + Off - Sbs] = Res >> 8;
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

static void GosubC750B8 (Arr08 Cod10, UINT8 * Cod18, UINT8 * Cod0C, Arr10 Cod00)
{
    UINT16                        i, j;
    UINT8                         RL3, RL4, RL5, RL6;
    UINT16                        R3, R4;

    for (i = 0; i < 4; i++)
    {
        Cod0C[i] = Cod00[i + 2];
    }

    for (i = 0; i < 4; i++)
    {
        RL6 = Cod18[i];

        for (j = 0; j < 4; j++)
        {
            Cod10[j] = Cod0C[j];
        }

        for (j = 2; j < 4; j++)
        {
            R3 = (Cod0C[j] >> 04) & 0x0F;
            R4 = Cod0C[j] & 0x0F;
            RL4 = CodeTable00[R3];
            RL5 = CodeTable01[R4];
            RL4 = (RL4 << 04);
            Cod10[5 + j - 3] = RL4 | RL5;
        }

        for (j = 0; j < 2; j++)
        {
            if (j != 0)
            {
                Cod10[j + 6] = (~Cod10[j + 4]) ^ RL6;
            }
            else
            {
                Cod10[j + 6] = Cod10[j + 4] ^ RL6;
            }
        }

        for (j = 0; j < 2; j++)
        {
            Cod0C[j] = Cod10[j + 2];
            Cod0C[j + 2] = Cod10[j + 6] ^ Cod10[j];
        }
    }

    for (j = 0; j < 4; j++)
    {
        RL3 = Cod0C[j];
        RL4 = Cod00[j + 2];
        Cod0C[j] = RL3 ^ RL4;
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

static void GosubC75038 (Arr08 Cod10, Arr10 Cod00, UINT8 * Cod0C)
{
    UINT32                        Addrs;
    UINT32                        i, j, Ad;
    UINT8                         RL3, RL4, RL5, RL6;
    UINT16                        R3, R4;

    Addrs = 0x04065A;
    while (Addrs < 0x04066E)
    {
        Ad = Addrs - 0x04065A;

        for (i = 0; i < 4; i++)
        {
            Cod00[i] = CodROM00[i + Ad];
        }

        for (i = 0; i < 4; i++)
        {
            RL6 = Cod0C[i];

            for (j = 0; j < 4; j++)
            {
                Cod10[j] = Cod00[j];
            }

            for (j = 2; j < 4; j++)
            {
                R3 = (Cod00[j] >> 4) & 0x0F;
                R4 = Cod00[j] & 0x0F;
                RL4 = CodeTable00[R3];
                RL5 = CodeTable01[R4];
                RL4 = (RL4 << 4);
                Cod10[5 + j - 3] = RL4 | RL5;
            }

            for (j = 0; j < 2; j++)
            {
                if (j != 0)
                {
                    Cod10[j + 6] = (~Cod10[j + 4]) ^ RL6;
                }
                else
                {
                    Cod10[j + 6] = Cod10[j + 4] ^ RL6;
                }
            }

            for (j = 0; j < 2; j++)
            {
                Cod00[j] = Cod10[j + 2];
                Cod00[j + 2] = Cod10[j + 6] ^ Cod10[j];
            }
        }

        for (j = 0; j < 4; j++)
        {
            RL3 = Cod00[j];
            RL4 = CodROM00[j + Ad];
            Cod00[j] = RL3 ^ RL4;
        }

        for (j = 0; j < 4; j++)
        {
            Cod0C[j] = Cod00[j];
        }

        Addrs += 8;
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

static void GosubC75038Next (Arr08 Cod10, Arr10 Cod00, UINT8 * Cod0C)
{
    UINT32                        Addrs;
    UINT32                        i, j, Ad;
    UINT8                         RL3, RL4, RL5, RL6;
    UINT16                        R3, R4;

    Addrs = 0x031E002C;
    while (Addrs < 0x031E004C)
    {
        Ad = Addrs - 0x031E002C;

        for (i = 0; i < 4; i++)
        {
            Cod00[i] = CodROM01[i + Ad];
        }

        for (i = 0; i < 4; i++)
        {
            RL6 = Cod0C[i];

            for (j = 0; j < 4; j++)
            {
                Cod10[j] = Cod00[j];
            }

            for (j = 2; j < 4; j++)
            {
                R3 = (Cod00[j] >> 4) & 0x0F;
                R4 = Cod00[j] & 0x0F;
                RL4 = CodeTable00[R3];
                RL5 = CodeTable01[R4];
                RL4 = (RL4 << 4);
                Cod10[5 + j - 3] = RL4 | RL5;
            }

            for (j = 0; j < 2; j++)
            {
                if (j != 0)
                {
                    Cod10[j + 6] = (~Cod10[j + 4]) ^ RL6;
                }
                else
                {
                    Cod10[j + 6] = Cod10[j + 4] ^ RL6;
                }
            }

            for (j = 0; j < 2; j++)
            {
                Cod00[j] = Cod10[j + 2];
                Cod00[j + 2] = Cod10[j + 6] ^ Cod10[j];
            }
        }

        for (j = 0; j < 4; j++)
        {
            RL3 = Cod00[j];
            RL4 = CodROM01[j + Ad];
            Cod00[j] = RL3 ^ RL4;
        }

        for (j = 0; j < 4; j++)
        {
            Cod0C[j] = Cod00[j];
        }

        Addrs += 8;
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

static void GetR4R5 (eMISC_MODEL_INDICES uniquemodel, UINT32 U32phoneid,
                     char *IMEI, UINT8 Dn, UINT8 * Cod0C)
{
    Arr10                         Cod00;
    Arr08                         Cod10;
    UINT8                         Cod18[4];
    UINT16                        i;
    UINT8                         RL6, RL1;
    Arr08                         BCD;

    ConvertToBCD (IMEI, BCD);
    memcpy (Cod18, &U32phoneid, sizeof (U32phoneid));

    if (uniquemodel == MISC_MODEL_INDEX_C35_NEW ||
        uniquemodel == MISC_MODEL_INDEX_M35_NEW ||
        uniquemodel == MISC_MODEL_INDEX_S35_NEW)
    {
        for (i = 0; i < sizeof (Cod18MajicNumbers); i++)
        {
            Cod18[i] ^= Cod18MajicNumbers[i];
        }
    }

    Cod00[1] = 8;
    for (i = 0; i < 8; i++)
    {
        Cod00[i + 2] = BCD[i];
    }

    for (i = 0; i < 4; i++)
    {
        if (Dn == 0)
        {
            RL6 = Cod00[i + 6];
            RL1 = Cod00[i + 2] & 0x55;
            Cod00[i + 2] = RL1 ^ RL6;
        }
        else
        {
            RL6 = Cod00[i + 6];
            RL1 = Cod00[i + 2] & 0xAA;
            Cod00[i + 2] = RL1 ^ RL6;
        }
    }

    GosubC750B8 (Cod10, Cod18, Cod0C, Cod00);
    GosubC75038 (Cod10, Cod00, Cod0C);
    GosubC75038Next (Cod10, Cod00, Cod0C);
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

static BOOL CreateIMEISpecificBlocksC35 (eMISC_MODEL_INDICES uniquemodel,
                                         eMISC_MODEL_INDICES phonemodel,
                                         char *IMEI, UINT32 * pU32phoneid,
                                         UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    UINT16                        Const10, Const11;
    UINT16                        RNDCode;
    UINT8                         Cod0C[4];
    UINT8                         SMask00[512], SMask01[512], SMask02[512];

    if (phonemodel)
    {
    }

    GetR4R5 (uniquemodel, *pU32phoneid, IMEI, 0, Cod0C);

    Const10 = Cod0C[0] | (Cod0C[1] << 8);
    Const11 = Cod0C[2] | (Cod0C[3] << 8);

    RNDCode = 0;
    Encrypt (&RNDCode, StrangeHeaderCopy, SMask01, Const10,
             sizeof (StrangeHeaderCopy), 02, 00);
    SMask01[0] = RNDCode;
    SMask01[1] = RNDCode >> 8;

    RNDCode = 0;
    Encrypt (&RNDCode, SMask01, pEnc5008, Const11,
             sizeof (StrangeHeaderCopy) + 2, 02, 00);
    pEnc5008[0] = RNDCode;
    pEnc5008[1] = RNDCode >> 8;

    RNDCode = 0;
    Encrypt (&RNDCode, MainBody5008Copy, SMask00, Const10,
             B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 8), 0x02, 00);
    SMask00[0] = RNDCode;
    SMask00[1] = RNDCode >> 8;

    RNDCode = 0;
    Encrypt (&RNDCode, SMask00, pEnc5008, Const11,
             B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 8) + 2, 0x1E,
             00);
    pEnc5008[28] = RNDCode;
    pEnc5008[29] = RNDCode >> 8;

    RNDCode = 0;
    Encrypt (&RNDCode, MainBody5077, SMask02, Const10,
             B77Sz[phonemodel] - 4, 0x02, 00);
    SMask02[0] = RNDCode;
    SMask02[1] = RNDCode >> 8;

    RNDCode = 0;
    Encrypt (&RNDCode, SMask02, pEnc5077, Const11, B77Sz[phonemodel] - 4 + 2,
             0x02, 00);
    pEnc5077[0] = RNDCode;
    pEnc5077[1] = RNDCode >> 8;

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

static void EncryptC30HiddenBlocks (UINT32 * pU32phoneid, UINT16 * pKeys,
                                    UINT16 Ln, UINT16 XKey, UINT8 * pDecod,
                                    UINT16 Ad, UINT16 * pIdx, UINT8 * pEncod)
{
    UINT16                        Nb;
    Arr512                        Dec00;
    UINT32                        DWordID;

    memset (Dec00, 0, sizeof (Dec00));

    pEncod[*pIdx + Ln + 02] = 00;
    pEncod[*pIdx + 00] = 0x00;
    pEncod[*pIdx + 01] = 0x00;

    for (Nb = 0; Nb < Ln; Nb++)
    {
        Dec00[Nb] = pDecod[Nb];
    }

    DWordID = ConvertIDIntoDWord (pU32phoneid, XKey, &pEncod[*pIdx], pKeys);

    for (Nb = 0; Nb < Ln; Nb++)
    {
        pEncod[*pIdx + Ln + 02] ^= Dec00[Nb];
    }

    Gosub4FD7EC (*pKeys, Ln, Dec00);

    for (Nb = 0; Nb < Ln; Nb++)
    {
        DWordID = (DWordID * 0x2455 + 0xC091) % 0x38F40;
        pEncod[*pIdx + Nb + 02] = Dec00[Nb] ^ (DWordID * 0xFFFF / 0x038F40);
    }

    *pIdx += (Ln + Ad + 4);

    pEncod[*pIdx - 01] = 0x00;
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

static BOOL CreateIMEISpecificBlocksC30 (eMISC_MODEL_INDICES uniquemodel,
                                         eMISC_MODEL_INDICES phonemodel,
                                         char *IMEI, UINT32 * pU32phoneid,
                                         UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    UINT16                        Keys, Idx = 0;
    UINT8                         Def00[1024];

    if (uniquemodel || phonemodel || pEnc5077)
    {
    }

    memset (Def00, 0, sizeof (Def00));

    ConvertToC30BCD (IMEI, Def00);

    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x000A, Def00, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x09E4, Def0C, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x1C, 0x0F40, Def22, 02, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x3C, 0x0F62, Def40, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x0C, 0x0FA2, Def10, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x28, 0x0FB2, Def2E, 02, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x40, 0x0FE0, Def44, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0A40, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0B40, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0C40, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0D40, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0E40, DefFF, 00, &Idx,
                            pEnc5008);

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

static BOOL CreateIMEISpecificBlocksS40 (eMISC_MODEL_INDICES uniquemodel,
                                         eMISC_MODEL_INDICES phonemodel,
                                         char *IMEI, UINT32 * pU32phoneid,
                                         UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    UINT16                        Keys, Idx = 0;
    UINT8                         Def00[1024];

    if (uniquemodel || phonemodel || pEnc5077)
    {
    }

    memset (Def00, 0, sizeof (Def00));

    ConvertToC30BCD (IMEI, Def00);

    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x000A, Def00, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x06E6, Def0C, 0x18, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x1C, 0x0CFE, Def22, 02, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x3C, 0x0D20, Def40, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x0C, 0x0D60, Def10, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x28, 0x0D70, Def2E, 02, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0x40, 0x0D9E, Def44, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x07FE, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x08FE, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x09FE, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0AFE, DefFF, 00, &Idx,
                            pEnc5008);
    EncryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0BFE, DefFF, 00, &Idx,
                            pEnc5008);

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

BOOL SEC_CreateIMEISpecificBlocks (UINT32 U32provider1, UINT32 U32provider2,
                                   eMISC_MODEL_INDICES uniquemodel,
                                   eMISC_MODEL_INDICES phonemodel, char *IMEI,
                                   UINT32 * pU32phoneid, UINT8 * pEnc5008,
                                   UINT8 * pEnc5077)
{
    memset (MainBody5008Copy, 0, sizeof (MainBody5008Copy));
    memset (StrangeHeaderCopy, 0, sizeof (StrangeHeaderCopy));

    memcpy (MainBody5008Copy, MainBody5008, sizeof (MainBody5008));
    memcpy (StrangeHeaderCopy, StrangeHeader, sizeof (StrangeHeader));

    if (U32provider1 > 0)
    {
        if (U32provider1 != 1)
        {
            StrangeHeaderCopy[2] = 0x1;
            StrangeHeaderCopy[18] = 0xFE;
            MISC_ConvertProvider (U32provider1, (char *) &MainBody5008Copy[3]);
            if (U32provider2 > 0)
            {
                MISC_ConvertProvider (U32provider2,
                                      (char *) &MainBody5008Copy[9]);
            }
        }
        else                    /* autolock to network */
        {
            StrangeHeaderCopy[2] = 0x2;
            StrangeHeaderCopy[18] = 0;
        }
    }

    CRCBuffer (StrangeHeaderCopy, sizeof (StrangeHeaderCopy) - 2);
    CRCBuffer (MainBody5008Copy, SEC_MAINBODY5008_CRC_LEN);
    CRCBuffer (MainBody5077, SEC_MAINBODY5077_CRC_LEN);
    switch (phonemodel)
    {
    case MISC_MODEL_INDEX_C30:
        return CreateIMEISpecificBlocksC30 (uniquemodel, phonemodel, IMEI,
                                            pU32phoneid, pEnc5008, pEnc5077);

    case MISC_MODEL_INDEX_S40:
        return CreateIMEISpecificBlocksS40 (uniquemodel, phonemodel, IMEI,
                                            pU32phoneid, pEnc5008, pEnc5077);

    case MISC_MODEL_INDEX_C35:
        return CreateIMEISpecificBlocksC35 (uniquemodel, phonemodel, IMEI,
                                            pU32phoneid, pEnc5008, pEnc5077);

    case MISC_MODEL_INDEX_C45:
    case MISC_MODEL_INDEX_A50:
    case MISC_MODEL_INDEX_C55:
        return CreateIMEISpecificBlocksC45 (uniquemodel, phonemodel, IMEI,
                                            pU32phoneid, pEnc5008, pEnc5077);

    case MISC_MODEL_INDEX_A35:
    case MISC_MODEL_INDEX_S45:
    case MISC_MODEL_INDEX_SL45:
        return CreateIMEISpecificBlocksA35 (uniquemodel, phonemodel, IMEI,
                                            pU32phoneid, pEnc5008, pEnc5077);
    }

    return TRUE;
}


#ifndef MAPMODE
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

static BOOL RecreateIMEISpecificBlocksC30 (eMISC_MODEL_INDICES uniquemodel,
                                           eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, UINT32 * pU32phoneid,
                                           UINT8 * pEnc5008,
                                           UINT8 * pEnc5077,
                                           UINT8 * pDec5008, UINT8 * pDec5077)
{
    UINT16                        Keys, Idx = 0;

    if (uniquemodel || phonemodel || pEnc5077 || IMEI || pDec5077)
    {
    }

    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x000A, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x09E4, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x1C, 0x0F40, pEnc5008, 02,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x3C, 0x0F62, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x0C, 0x0FA2, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x28, 0x0FB2, pEnc5008, 02,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x40, 0x0FE0, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0A40, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0B40, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0C40, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0D40, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0E40, pEnc5008, 00,
                            &Idx, pDec5008);

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

static BOOL RecreateIMEISpecificBlocksS40 (eMISC_MODEL_INDICES uniquemodel,
                                           eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, UINT32 * pU32phoneid,
                                           UINT8 * pEnc5008,
                                           UINT8 * pEnc5077,
                                           UINT8 * pDec5008, UINT8 * pDec5077)
{
    UINT16                        Keys, Idx = 0;
    UINT8                         Def00[1024];

    if (uniquemodel || phonemodel || pEnc5077 || IMEI || pDec5077)
    {
    }

    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x000A, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x08, 0x06E6, pEnc5008, 0x18,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x1C, 0x0CFE, pEnc5008, 02,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x3C, 0x0D20, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x0C, 0x0D60, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x28, 0x0D70, pEnc5008, 02,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0x40, 0x0D9E, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x07FE, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x08FE, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x09FE, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0AFE, pEnc5008, 00,
                            &Idx, pDec5008);
    DecryptC30HiddenBlocks (pU32phoneid, &Keys, 0xFC, 0x0BFE, pEnc5008, 00,
                            &Idx, pDec5008);

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

static BOOL RecreateIMEISpecificBlocksA35 (eMISC_MODEL_INDICES uniquemodel,
                                           eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, UINT32 * pU32phoneid,
                                           UINT8 * pEnc5008,
                                           UINT8 * pEnc5077,
                                           UINT8 * pDec5008, UINT8 * pDec5077)
{
    UINT16                        RNDNumberA, RNDNumberB;
    Arr08                         Cod00, BCD;
    Arr512                        Mask;
    Arr204                        Key;
    BOOL                          Bblocked;

    if (uniquemodel)
    {
    }

    ConvertToBCD (IMEI, BCD);
    CreateMask (Mask);

#ifndef USE_DONGLE
    memcpy (&UnknownKey1[phonemodel][12], pU32phoneid, 4);
    memcpy (&UnknownKey1[phonemodel][14], BCD, 8);
    CreateKey (phonemodel, Mask, Key);
#else
    if (!DNG_Calculate (phonemodel, IMEI, *pU32phoneid, Key, &Bblocked))
    {
        return FALSE;
    }
#endif

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;

    DecryptA35 (Mask, Key, pEnc5008, pDec5008, sizeof (StrangeHeaderCopy) + 8);

    DecryptA35 (Mask, Key, &pEnc5008[sizeof (StrangeHeaderCopy) + 8],
                &pDec5008[sizeof (StrangeHeaderCopy) + 8],
                B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 8));

    DecryptA35 (Mask, Key, pEnc5077, pDec5077, B77Sz[phonemodel]);

    return TRUE;
}


static BOOL RecreateIMEISpecificBlocksC45 (eMISC_MODEL_INDICES uniquemodel,
                                           eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, UINT32 * pU32phoneid,
                                           UINT8 * pEnc5008,
                                           UINT8 * pEnc5077,
                                           UINT8 * pDec5008, UINT8 * pDec5077)
{
    UINT16                        RNDNumberA, RNDNumberB;
    Arr08                         Cod00, BCD;
    Arr512                        Mask;
    UINT32                        CodE2[4];
    UINT8                         Cod08[512];
    BOOL                          Bblocked;

    if (uniquemodel)
    {
    }

    CreateMask (Mask);
    ConvertToBCD (IMEI, BCD);

#ifndef USE_DONGLE
    memcpy (&UnknownKey1[phonemodel][12], pU32phoneid, 4);
    memcpy (&UnknownKey1[phonemodel][14], BCD, 8);
    CreateCodE2 (phonemodel, CodE2);
#else
    if (!DNG_Calculate (phonemodel, IMEI, *pU32phoneid, CodE2, &Bblocked))
    {
        return FALSE;
    }
#endif

    RNDNumberA = RNDNumberB = 0;
    memset (Cod00, 0, sizeof (Cod00));
    Cod00[0] = RNDNumberA;
    Cod00[1] = RNDNumberA >> 8;
    Cod00[2] = RNDNumberB;
    Cod00[3] = RNDNumberB >> 8;

    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (TRUE, Cod08, pDec5008, 0x00, pEnc5008, 0x08);
    GosubC7D5CE (TRUE, Cod08, pDec5008, 0x08, pEnc5008 + 8,
                 sizeof (StrangeHeaderCopy));

    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (TRUE, Cod08, pDec5008, sizeof (StrangeHeaderCopy) + 8,
                 pEnc5008 + sizeof (StrangeHeaderCopy) + 8, 0x08);
    GosubC7D5CE (TRUE, Cod08, pDec5008, sizeof (StrangeHeaderCopy) + 16,
                 pEnc5008 + sizeof (StrangeHeaderCopy) + 16,
                 B08Sz[phonemodel] - (sizeof (StrangeHeaderCopy) + 16));

    GosubC7D524 (Cod08, CodE2, 0x80);
    GosubC7D5CE (TRUE, Cod08, pDec5077, 0x00, pEnc5077, 0x08);
    GosubC7D5CE (TRUE, Cod08, pDec5077, 0x08, pEnc5077 + 8,
                 B77Sz[phonemodel] - 8);

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

BOOL SEC_RecreateIMEISpecificBlocks (eMISC_MODEL_INDICES uniquemodel,
                                     eMISC_MODEL_INDICES phonemodel,
                                     char *IMEI, UINT32 * pU32phoneid,
                                     UINT8 * pEnc5008, UINT8 * pEnc5077,
                                     UINT8 * pDec5008, UINT8 * pDec5077)
{
    switch (phonemodel)
    {
    case MISC_MODEL_INDEX_S40:
        return RecreateIMEISpecificBlocksS40 (uniquemodel,
                                              phonemodel,
                                              IMEI, pU32phoneid, pEnc5008,
                                              pEnc5077, pDec5008, pDec5077);

    case MISC_MODEL_INDEX_C30:
        return RecreateIMEISpecificBlocksC30 (uniquemodel,
                                              phonemodel,
                                              IMEI, pU32phoneid, pEnc5008,
                                              pEnc5077, pDec5008, pDec5077);

    case MISC_MODEL_INDEX_C35:
        LDBGSTR ("SEC_RecreateIMEISpecificBlocks : not supported yet");
        return FALSE;

    case MISC_MODEL_INDEX_A35:
    case MISC_MODEL_INDEX_S45:
    case MISC_MODEL_INDEX_SL45:
        return RecreateIMEISpecificBlocksA35 (uniquemodel,
                                              phonemodel,
                                              IMEI, pU32phoneid, pEnc5008,
                                              pEnc5077, pDec5008, pDec5077);

    case MISC_MODEL_INDEX_C45:
    case MISC_MODEL_INDEX_A50:
    case MISC_MODEL_INDEX_C55:
        return RecreateIMEISpecificBlocksC45 (uniquemodel,
                                              phonemodel,
                                              IMEI, pU32phoneid, pEnc5008,
                                              pEnc5077, pDec5008, pDec5077);
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

static UINT8 SEC_AdjustIMEIByte (UINT8 src, UINT8 dst)
{
    UINT8                         i, U8mask;
    UINT8                         U8srcbit, U8dstbit;

    if (src == dst)
    {
        return dst;
    }

    for (i = 0, U8mask = 1; i < 8; i++, U8mask <<= 1)
    {
        U8srcbit = (src & U8mask) >> i;
        U8dstbit = (dst & U8mask) >> i;

        /*
         * can't change from 0 to 1
         */
        if (U8dstbit == 1 && U8srcbit == 0)
        {
            U8dstbit = 0;
        }

        dst &= (UINT8) ~ U8mask;
        if (U8dstbit)
        {
            dst |= U8mask;
        }
    }

    return dst;
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

BOOL SEC_AdjustOriginalIMEI (char *desirednewIMEI)
{
    char                          newIMEI[FREIA_IMEI_LEN + 1];
    UINT8                         desirednewBCDIMEI[FREIA_IMEI_LEN / 2 + 1];
    UINT8                         originalflashBCDIMEI[FREIA_IMEI_LEN / 2 + 1];
    UINT8                         newBCDIMEI[FREIA_IMEI_LEN / 2 + 1];
    UINT8                         U8lendst, i;
    BOOL                          Bempty;

    if (desirednewIMEI == NULL)
    {
        /*
         * invalid IMEI 
         */

        LDBGSTR1 ("SEC_AdjustOriginalIMEI : invalid IMEI '%s'", desirednewIMEI);
        return FALSE;
    }

    U8lendst = strlen (desirednewIMEI);
    if (U8lendst < FREIA_IMEI_LEN)
    {
        /*
         * invalid IMEI 
         */
        LDBGSTR1 ("SEC_AdjustOriginalIMEI : invalid IMEI '%s'", desirednewIMEI);
        return FALSE;
    }

    MISC_GetOriginalIMEI (originalflashBCDIMEI);

    SEC_NormalIMEIToBCDIMEI (desirednewIMEI, desirednewBCDIMEI);

    for (i = 0; i < FREIA_IMEI_LEN / 2 + 1; i++)
    {
        newBCDIMEI[i] =
            SEC_AdjustIMEIByte (originalflashBCDIMEI[i], desirednewBCDIMEI[i]);
    }

    if (!SEC_BCDIMEIToNormalIMEI (newBCDIMEI, newIMEI, &Bempty))
    {
        LDBGSTR1 ("SEC_AdjustOriginalIMEI : invalid new IMEI '%s'", newIMEI);
        return FALSE;
    }

    if (Bempty)
    {
        LDBGSTR ("SEC_AdjustOriginalIMEI : new IMEI is empty");
        return FALSE;
    }

    if (strcmp (newIMEI, desirednewIMEI) != 0)
    {
        LDBGSTR2 ("SEC_AdjustOriginalIMEI : changed IMEI from '%s' to '%s'",
                  desirednewIMEI, newIMEI);
        strcpy (desirednewIMEI, newIMEI);
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

UINT8 SEC_CreateCRCDigit (UINT8 * pB5009)
{
    UINT8                         U8chk = 0, i;
    UINT8                         AU8oddevenbytes[] =
        { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };

    for (i = 0; i < 7; i++)
    {
        U8chk += pB5009[i] & 0x0F;
        if ((pB5009[i] >> 4) >= sizeof (AU8oddevenbytes))
        {
            return 0;
        }

        U8chk += AU8oddevenbytes[pB5009[i] >> 4];
    }

    U8chk = (U8chk % 10);
    if (U8chk != 0)
    {
        U8chk = 10 - U8chk;
        U8chk <<= 4;
    }

    return U8chk;
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

void SEC_GetBlockLen (eMISC_MODEL_INDICES phonemodel, UINT16 * pU16B5008,
                      UINT16 * pU16B5077)
{
    *pU16B5008 = B08Sz[phonemodel];
    *pU16B5077 = B77Sz[phonemodel];
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

BOOL SEC_BCDIMEIToNormalIMEI (UINT8 * pB5009, char *IMEI, BOOL * pBempty)
{
    UINT8                         i, U8digit;
    UINT16                        U16sum;

    *IMEI = 0;
    *pBempty = FALSE;

    for (i = 0, U16sum = 0; i < 7; i++)
    {
        U16sum += pB5009[i];
    }

    if (U16sum == 7 * 255)
    {
        *pBempty = TRUE;
        return TRUE;
    }

    for (i = 0; i < 7; i++)
    {
        U8digit = (pB5009[i] & 0x0f);
        if (U8digit > 9)
        {
            LDBGSTR ("SEC_BCDIMEIToNormalIMEI : invalid decoded IMEI");
            return FALSE;
        }
        IMEI[2 * i] = U8digit + '0';

        U8digit = ((pB5009[i] >> 4) & 0x0f);
        if (U8digit > 9)
        {
            LDBGSTR ("SEC_BCDIMEIToNormalIMEI : invalid decoded IMEI");
            return FALSE;
        }
        IMEI[2 * i + 1] = U8digit + '0';
    }

    IMEI[FREIA_IMEI_LEN] = '\0';
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

static UINT8 GetCodeTable (eMISC_MODEL_INDICES phonemodel, UINT8 U8idxhi,
                           UINT8 U8idxlo)
{
    switch (phonemodel)
    {
    case MISC_MODEL_INDEX_C35:
    case MISC_MODEL_INDEX_A35:
    case MISC_MODEL_INDEX_S45:
    case MISC_MODEL_INDEX_SL45:
        return (CodeTable00[U8idxhi] << 4) | CodeTable01[U8idxlo];

    case MISC_MODEL_INDEX_C45:
        return (CodeTable02[U8idxhi] << 4) | CodeTable03[U8idxlo];

    case MISC_MODEL_INDEX_A50:
        return (CodeTable04[U8idxhi] << 4) | CodeTable05[U8idxlo];
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

static void InvDoSomeTrickyCalculations (eMISC_MODEL_INDICES phonemodel,
                                         UINT8 Vls, UINT8 * Arr)
{
    UINT8                         i;
    UINT8                         U8byte, U8idxlo, U8idxhi;
    UINT8                         Bytes05[FREIA_5009_AND_0001_LEN];

    memcpy (Bytes05, Arr, sizeof (Bytes05));

    for (i = 0; i < FREIA_5009_AND_0001_LEN / 2; i++)
    {
        U8idxhi = (Arr[i + FREIA_5009_AND_0001_LEN / 2] >> 4) & 0x0f;
        U8idxlo = Arr[i + FREIA_5009_AND_0001_LEN / 2] & 0x0f;

        U8byte = GetCodeTable (phonemodel, U8idxhi, U8idxlo);

        if (!(i & 1))
        {
            Arr[i + FREIA_5009_AND_0001_LEN / 2] = U8byte ^ Vls ^ Bytes05[i];
        }
        else
        {
            Arr[i + FREIA_5009_AND_0001_LEN / 2] = ~U8byte ^ Vls ^ Bytes05[i];
        }

        Arr[i] = Bytes05[i + FREIA_5009_AND_0001_LEN / 2];
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

static UINT32 ConvertIDIntoDWord (UINT32 * pU32phoneid, UINT16 XKey,
                                  UINT8 * pEnc00, UINT16 * pKeys)
{
    UINT16                        Nc08, Nc03;
    UINT8                         PhID[8];

    memcpy (PhID, pU32phoneid, sizeof (PhID));

    Nc08 = PhID[1] ^ PhID[3] ^ PhID[4] ^ PhID[6];
    Nc03 = PhID[0] ^ PhID[2] ^ PhID[5] ^ PhID[7];
    *pKeys = (pEnc00[1] << 8) | pEnc00[0];
    Nc08 = Nc08 - Nc03 + (Nc03 << 8);
    return Nc08 ^ XKey ^ *pKeys;
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

static void Gosub4FD7EC (UINT16 Key, UINT16 Ln, Arr512 Dec00)
{
    UINT16                        Nb;
    UINT8                         Bt, Bp;

    Ln &= 0xFFFE;

    if (Key & 0x2000)
    {
        for (Nb = 0; Nb < Ln; Nb++)
        {
            Bt = Dec00[Nb * 2 + 0] & 0xF0;
            Bt |= (Dec00[Nb * 2 + 1] >> 0x04);
            Bp = Dec00[Nb * 2 + 1] & 0x0F;
            Bp |= (Dec00[Nb * 2 + 0] << 0x04);
            Dec00[Nb * 2 + 0] = Bt;
            Dec00[Nb * 2 + 1] = Bp;
        }
    }
    else
    {
        for (Nb = 0; Nb < Ln; Nb++)
        {
            Bp = Dec00[Nb * 2 + 0] & 0x0F;
            Bp |= (Dec00[Nb * 2 + 1] << 0x04);
            Bt = Dec00[Nb * 2 + 1] & 0xF0;
            Bt |= (Dec00[Nb * 2 + 0] >> 0x04);
            Dec00[Nb * 2 + 0] = Bp;
            Dec00[Nb * 2 + 1] = Bt;
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

static void DecryptC30HiddenBlocks (UINT32 * pU32phoneid, UINT16 * pKeys,
                                    UINT16 Ln, UINT16 XKey, UINT8 * pDecod,
                                    UINT16 Ad, UINT16 * pIdx, UINT8 * pEncod)
{
    UINT16                        Nb;
    Arr512                        Dec00;
    UINT32                        DWordID;

    memset (Dec00, 0, sizeof (Dec00));

    for (Nb = 0; Nb < Ln; Nb++)
    {
        Dec00[Nb] = pDecod[*pIdx + Nb + 2];
    }

    DWordID = ConvertIDIntoDWord (pU32phoneid, XKey, &pEncod[*pIdx], pKeys);

    for (Nb = 0; Nb < Ln; Nb++)
    {
        DWordID = (DWordID * 0x2455 + 0xC091) % 0x38F40;
        pEncod[*pIdx + Nb + 02] = Dec00[Nb] ^ (DWordID * 0xFFFF / 0x038F40);
    }

    Gosub4FD7EC (*pKeys, Ln, &pEncod[*pIdx + 2]);

    *pIdx += (Ln + Ad + 4);
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

BOOL SEC_RecreateIMEI (eMISC_MODEL_INDICES phonemodel, UINT8 * pB5009,
                       char *IMEI, BOOL * pBempty)
{
    SINT8                         i;
    UINT16                        Keys, Idx = 0;
    UINT32                        U32phoneid[2];
    UINT8                        *pIMEI;

    IMEI[0] = '\0';

    if (MISC_C30Like (phonemodel))
    {
        pIMEI = &pB5009[2];

        DecryptC30HiddenBlocks (freia_phoneinfo.AU32phoneid, &Keys, 0x08,
                                0x000A, pB5009, 00, &Idx, pB5009);
    }
    else
    {
        pIMEI = pB5009;

        for (i = 7; i >= 0; i--)
        {
            if (!(i & 1))
            {
                InvDoSomeTrickyCalculations (phonemodel, Pars[phonemodel][i],
                                             pB5009);
            }
        }
    }

    return SEC_BCDIMEIToNormalIMEI (pIMEI, IMEI, pBempty);
}
