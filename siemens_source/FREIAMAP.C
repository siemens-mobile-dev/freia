
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "secpub.h"
#include "mappub.h"

eERROR                        freia_errno;
tFREIA_PHONEINFO              freia_phoneinfo;

int main (int argc, char *argv[])
{
    eMISC_MODEL_INDICES           uniquemodel, phonemodel;
    UINT32                        AU32phoneid[2];
    UINT8                         B5009[10], B0001[10];
    UINT8                         Enc5008[2048];    /* because of C30 */
    UINT8                         Enc5077[234];
    char                          IMEI[16];

    if (argc < 2)
    {
        printf ("Give a log file as an argument\n");
        return -1;
    }

    if (!MAP_LoadLog (argv[1], &uniquemodel, &phonemodel, IMEI, AU32phoneid))
    {
        return -1;
    }

    if (!MISC_C30Like (phonemodel))
    {
        SEC_CreateBlocks5009And76 (phonemodel, IMEI, B5009, B0001);
    }

    if (!SEC_CreateIMEISpecificBlocks (0, 0, uniquemodel, phonemodel,
                                       IMEI, AU32phoneid, Enc5008, Enc5077))
    {
        return -1;
    }

    if (!MAP_SaveMap
        (phonemodel, MAPTYPE_UNLOCK, NULL, AU32phoneid,
         IMEI, B5009, B0001, Enc5008, Enc5077))
    {
        return -1;
    }

    return 0;
}
