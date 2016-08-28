
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
#include "rc4loc.h"
#include "rc4pub.h"

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

static void SwapByte (UINT8 * a, UINT8 * b)
{
    UINT8                         swapByte;

    swapByte = *a;
    *a = *b;
    *b = swapByte;
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

static void RC4_Init (UINT8 * pKey, SINT16 S16keylen, tRC4Data * pData)
{
    UINT8                         index1;
    UINT8                         index2;
    UINT8                        *state;
    SINT16                        i;

    state = &pData->Key[0];
    for (i = 0; i < 256; i++)
    {
        state[i] = i;
    }

    pData->x = 0;
    pData->y = 0;

    for (index1 = 0, index2 = 0, i = 0; i < 256; i++)
    {
        index2 = (pKey[index1] + state[i] + index2) % 256;
        SwapByte (&state[i], &state[index2]);
        index1 = (index1 + 1) % S16keylen;
    }

    memcpy (pData->OrgKey, pData->Key, 256);
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

static void RC4_Crypt (UINT8 * pAU8src, SINT16 S16srclen, tRC4Data * pData,
                       UINT8 * pAU8dst)
{
    UINT8                         x;
    UINT8                         y;
    UINT8                        *state;
    UINT8                         xorIndex;
    SINT16                        i;

    x = pData->x;
    y = pData->y;

    state = &pData->Key[0];
    for (i = 0; i < S16srclen; i++)
    {
        x = (x + 1) % 256;
        y = (state[x] + y) % 256;
        SwapByte (&state[x], &state[y]);

        xorIndex = (state[x] + state[y]) % 256;

        pAU8dst[i] = pAU8src[i] ^ state[xorIndex];
    }

    pData->x = x;
    pData->y = y;
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

void RC4_Encrypt (UINT8 * Key, UINT16 U16Keylen, UINT8 * pSrc,
                  UINT16 U16len, UINT8 * pDst)
{
    tRC4Data                      Data;

    RC4_Init (Key, U16Keylen, &Data);
    RC4_Crypt (pSrc, U16len, &Data, pDst);
}

