
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
#include "mappub.h"
#include "maploc.h"

static BOOL                   ishexdigit (char chr);

static const UINT8 watermark[] = FREIA_WATERMARK_ID "2\x13\xfe\xe5\x07\x55\x10\x83\x47";

#ifndef LOGMODE
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

BOOL MAP_LoadBatteryMap (eMISC_MODEL_INDICES phonemodel, char *filename,
                         UINT32 * pU32phoneid, UINT8 * pB67)
{
    FILE                         *rfile;
    char                          num[32], line[SYS_MAX_STRING_LEN], *pblockid;
    UINT16                        i;
    UINT8                         U8blockid;
    BOOL                          Bfinished;
    UINT16                        U16blocklen;
    UINT8                         U8parsed_block = 0;

    if (MISC_C30Like (phonemodel))
    {
        LDBGSTR ("MAP_LoadBatteryMap : not supported for this phone");
        return FALSE;
    }

    rfile = fopen (filename, "rt");
    if (!rfile)
    {
        return FALSE;
    }

    Bfinished = fgets (line, sizeof (line), rfile) == NULL;
    while (!Bfinished)
    {
        pblockid = strstr (line, MAP_ID_BLOCK_ID_STR);
        if (pblockid != NULL)   /* yahoo! we have a block */
        {
            pblockid += strlen (MAP_ID_BLOCK_ID_STR);
            i = 0;
            while (i < sizeof (num) && pblockid != '\0'
                   && pblockid < line + SYS_MAX_STRING_LEN
                   && ishexdigit (pblockid[i]))
            {
                num[i] = pblockid[i];
                i++;
            }
            num[i] = 0;

            *pU32phoneid = MISC_SwapDword (MISC_HexToInt (num));
        }

        pblockid = strstr (line, MAP_IMEI_BLOCK_ID_STR);

        if (pblockid != NULL)   /* yahoo! we have a block */
        {
            pblockid += strlen (MAP_IMEI_BLOCK_ID_STR);
            i = 0;
            while (i < sizeof (num) && pblockid != '\0'
                   && pblockid < line + SYS_MAX_STRING_LEN
                   && isdigit (pblockid[i]))
            {
                num[i] = pblockid[i];
                i++;
            }
            num[i] = 0;
            U8blockid = atoi (num);

            if (U8blockid != 67)
            {
                LDBGSTR1 ("MAP_LoadBatteryMap : invalid blockid '%d'",
                          U8blockid);
                fclose (rfile);
                return FALSE;
            }

            U16blocklen = FREIA_67_LEN;
            if (!MISC_ReadHexBlock (rfile, &U16blocklen, pB67))
            {
                fclose (rfile);
                return FALSE;
            }

            U8parsed_block++;
        }

        Bfinished = (fgets (line, sizeof (line), rfile) == NULL)
            || (U8parsed_block > 0);
    }
    fclose (rfile);
    return U8parsed_block > 0;
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

static void MAP_WriteHexBlock (FILE * rfile, UINT16 U16len, UINT8 * pAU8buffer)
{
    UINT16                        i;

    for (i = 0; i < U16len; i++)
    {
        fprintf (rfile, "0x%02X ", pAU8buffer[i]);
        if (((i % 10) == 9) | (i == U16len - 1))
        {
            fprintf (rfile, "\n");
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
#ifdef SAVE_BINARY_MAP
static void MAP_WriteBlock (UINT8 U8idx, UINT16 U16len, UINT8 * pAU8buffer)
{
    char                          name[SYS_MAX_PATHNAME_LEN];
    FILE                         *sfile;

    memset (name, 0, sizeof (name));
    sprintf (name, "block%d.bin", U8idx);
    sfile = fopen (name, "wb");
    if (sfile)
    {
        fwrite (pAU8buffer, 1, U16len, sfile);
        fclose (sfile);
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

BOOL MAP_SaveMap (eMISC_MODEL_INDICES phonemodel,
                  tMAP_MAPTYPE maptype,
                  char *srcpath,
                  UINT32 * pU32phoneid, char *IMEI,
                  UINT8 * pB5009, UINT8 * pB0001,
                  UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    FILE                         *rfile;
    char                          modelname[SYS_MAX_STRING_LEN],
        filename[SYS_MAX_PATHNAME_LEN];
    UINT16                        U16B5008Len, U16B5077Len;

    SEC_GetBlockLen (phonemodel, &U16B5008Len, &U16B5077Len);
    if (!MISC_GetPhoneModelName (modelname))
    {
#ifndef MAPMODE
        LDBGSTR1 ("MAP_SaveMap : invalid model id %d", phonemodel);
#endif
        return FALSE;
    }

    if (srcpath != NULL && strlen (srcpath) != 0)
    {
        sprintf (filename, "%sSiemens %s - %s", srcpath, modelname, IMEI);
    }
    else
    {
        sprintf (filename, "Siemens %s - %s", modelname, IMEI);
    }

    if (maptype == MAPTYPE_BACKUP)
    {
        strcat (filename, " backup." FREIA_MAP_EXTENSION);
    }
    else if (maptype == MAPTYPE_UNLOCK)
    {
        strcat (filename, " unlock." FREIA_MAP_EXTENSION);
    }
    else if (maptype == MAPTYPE_DECRYPT)
    {
        strcat (filename, " decrypt." FREIA_MAP_EXTENSION);
    }
    else
    {
        strcat (filename, "." FREIA_MAP_EXTENSION);
    }

    MISC_CheckFileName (filename);
    rfile = fopen (filename, "wt");
    if (!rfile)
    {
#ifndef MAPMODE
        LDBGSTR1 ("MAP_SaveMap : cannot save '%s'", filename);
#else
        printf ("cannot save '%s'\n", filename);
#endif
        return FALSE;
    }

    fprintf (rfile, "******************** Section ********************\n");
    if (!MISC_C30Like (phonemodel))
    {
        fprintf (rfile,
                 "***************** ID: " UINT32_FORMAT
                 " ******************\n", MISC_SwapDword (*pU32phoneid));
    }
    else
    {
        fprintf (rfile,
                 "************* ID: " UINT32_FORMAT UINT32_FORMAT
                 " **************\n",
                 MISC_SwapDword (pU32phoneid[0]),
                 MISC_SwapDword (pU32phoneid[1]));
    }

    fprintf (rfile, "********** Old IMEI = XXXXXX-XX-XXXXXX **********\n");
    fprintf (rfile,
             "********** New IMEI = %.6s-%.2s-%.6s **********\n",
             IMEI, &IMEI[6], &IMEI[8]);
    fprintf (rfile, "*************************************************\n\n");
    if (!MISC_C30Like (phonemodel))
    {
        fprintf (rfile, "%s00 [Siemens %s]\n",
                 MAP_IMEI_BLOCK_ID_STR, modelname);
        MAP_WriteHexBlock (rfile, FREIA_5009_AND_0001_LEN, pB5009);
#ifdef SAVE_BINARY_MAP
        MAP_WriteBlock (0, FREIA_5009_AND_0001_LEN, pB5009);
#endif
        fprintf (rfile, "\n");
        fprintf (rfile, "%s01\n", MAP_IMEI_BLOCK_ID_STR);
        MAP_WriteHexBlock (rfile, FREIA_5009_AND_0001_LEN, pB0001);
#ifdef SAVE_BINARY_MAP
        MAP_WriteBlock (1, FREIA_5009_AND_0001_LEN, pB0001);
#endif
        fprintf (rfile, "\n");
        fprintf (rfile, "%s02\n", MAP_IMEI_BLOCK_ID_STR);
        MAP_WriteHexBlock (rfile, U16B5008Len, pEnc5008);
#ifdef SAVE_BINARY_MAP
        MAP_WriteBlock (2, U16B5008Len, pEnc5008);
#endif
        fprintf (rfile, "\n");
        fprintf (rfile, "%s03\n", MAP_IMEI_BLOCK_ID_STR);
        MAP_WriteHexBlock (rfile, U16B5077Len, pEnc5077);
#ifdef SAVE_BINARY_MAP
        MAP_WriteBlock (3, U16B5077Len, pEnc5077);
#endif
    }
    else
    {
        fprintf (rfile, "%s %s[Siemens %s]\n", modelname,
                 MAP_C30_IMEI_BLOCK_ID_STR, modelname);
        MAP_WriteHexBlock (rfile, U16B5008Len, pEnc5008);
    }

    fprintf (rfile, "*************************************************\n\n");
    fclose (rfile);

#ifdef MAPMODE
    printf("map is saved to '%s'\n", filename);
#endif

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

BOOL MAP_SaveBatteryMap (eMISC_MODEL_INDICES phonemodel,
                         UINT32 * pU32phoneid, char *IMEI, UINT8 * pB67)
{
    FILE                         *rfile;
    char                          modelname[SYS_MAX_STRING_LEN],
        filename[SYS_MAX_PATHNAME_LEN];

    if (!MISC_GetPhoneModelName (modelname))
    {
        LDBGSTR1 ("MAP_SaveMap : invalid model id %d", phonemodel);
        return FALSE;
    }

    sprintf (filename, "Siemens %s - %s", modelname, IMEI);
    strcat (filename, " battery." FREIA_MAP_EXTENSION);
    MISC_CheckFileName (filename);
    rfile = fopen (filename, "wt");
    if (!rfile)
    {
        LDBGSTR1 ("MAP_SaveMap : cannot save '%s'", filename);
        return FALSE;
    }

    fprintf (rfile, "******************** Section ********************\n");
    fprintf (rfile,
             "***************** ID: " UINT32_FORMAT
             " ******************\n", MISC_SwapDword (*pU32phoneid));
    fprintf (rfile, "********** Old IMEI = XXXXXX-XX-XXXXXX **********\n");
    fprintf (rfile,
             "********** New IMEI = %.6s-%.2s-%.6s **********\n",
             IMEI, &IMEI[6], &IMEI[8]);
    fprintf (rfile, "*************************************************\n\n");
    fprintf (rfile, "%s67 [Siemens %s]\n", MAP_IMEI_BLOCK_ID_STR, modelname);
    MAP_WriteHexBlock (rfile, FREIA_67_LEN, pB67);
    fprintf (rfile, "\n");
    fprintf (rfile, "*************************************************\n\n");
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

static BOOL MAP_GetBlock (char *line, char *blockid,
                          char *value, UINT16 U16valuelen, BOOL * Bfound)
{
    char                         *pidstart, *pidend;

    memset (value, 0, U16valuelen);
    pidstart = strstr (line, blockid);
    if (pidstart)
    {
        pidend = strchr (pidstart, MAP_BLOCK_END_CHR);
        if (pidend == NULL)
        {
#ifndef MAPMODE
            LDBGSTR2
                ("MAP_GetBlock : syntax error (missing '%c') in '%s'",
                 MAP_BLOCK_END_CHR, line);
#endif
            return FALSE;
        }

        if ((UINT16) (pidend - pidstart) > U16valuelen)
        {
#ifndef MAPMODE
            LDBGSTR2
                ("MAP_GetBlock : syntax error (line too long (>%d)) in '%s'",
                 U16valuelen, line);
#endif
            return FALSE;
        }

        pidstart += strlen (blockid);
        strncpy (value, pidstart, pidend - pidstart);
        *Bfound = TRUE;
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

BOOL MAP_LoadLog (char *logname,
                  eMISC_MODEL_INDICES * puniquemodel,
                  eMISC_MODEL_INDICES * pphonemodel,
                  char *IMEI, UINT32 * pU32phone_id)
{
    UINT16                        i;
    FILE                         *rfile;
    char                         *pimei, *pid, tmpstr[SYS_MAX_STRING_LEN],
        line[SYS_MAX_STRING_LEN];
    BOOL                          Bfinished = FALSE, Bmodelfound =
        FALSE, BIMEIfound = FALSE, Bphoneidfound = FALSE;
    tMISC_PHONE_MODEL            *pmodelstruct;

    rfile = fopen (logname, "rt");
    if (!rfile)
    {
#ifdef MAPMODE
        printf ("cannot open '%s'\n", logname);
#else
        LDBGSTR1 ("MAP_LoadLog : cannot open '%s'", logname);
#endif
        return FALSE;
    }

    while (!Bfinished && !Bmodelfound && !BIMEIfound && !Bphoneidfound)
    {
        Bfinished = fgets (line, sizeof (line), rfile) == NULL;
        if (!Bfinished)
        {
            if (!Bmodelfound)
            {
                if (!MAP_GetBlock
                    (line, MAP_MODEL_ID, tmpstr, sizeof (tmpstr), &Bmodelfound))
                {
                    fclose (rfile);
                    return FALSE;
                }

                if (Bmodelfound)
                {
                    pid = strstr (tmpstr, MAP_MANUFACTURER_ID);
                    if (pid == NULL)
                    {
#ifndef MAPMODE
                        LDBGSTR1 ("MAP_LoadLog : invalid phone model '%s'",
                                  tmpstr);
#else
                        printf ("invalid phone model '%s'\n",
                                  tmpstr);
#endif
                        fclose (rfile);
                        return FALSE;
                    }

                    pid += strlen (MAP_MANUFACTURER_ID);
                    while (*pid <= 32 && pid != '\0')
                    {
                        pid++;
                    }

                    if (!MISC_GetPhoneModel (pid, &pmodelstruct))
                    {
#ifndef MAPMODE
                        LDBGSTR1 ("MAP_LoadLog : not supported model '%s'",
                                  tmpstr);
#else
                        printf ("not supported model '%s'\n",
                                  tmpstr);
#endif
                        fclose (rfile);
                        return FALSE;
                    }

                    *pphonemodel = pmodelstruct->modelid;
                    *puniquemodel = pmodelstruct->uniqueid;
                }
            }

            if (!BIMEIfound)
            {
                if (!MAP_GetBlock
                    (line, MAP_IMEI_ID, tmpstr, sizeof (tmpstr), &BIMEIfound))
                {
                    fclose (rfile);
                    return FALSE;
                }

                if (BIMEIfound)
                {
                    pimei = strchr(tmpstr,'/');
                    if (pimei)
                    {
                       pimei++;
                    }
                    else
                    {
                       pimei = tmpstr;
                    }

                    for (i = 0; i < (UINT16) strlen (pimei); i++)
                    {
                        if (pimei[i] != '-')
                        {
                            *(IMEI++) = pimei[i];
                        }
                    }
                    *IMEI = '\0';
                }
            }

            if (!Bphoneidfound)
            {
                if (!MAP_GetBlock
                    (line, MAP_PHONE_ID, tmpstr, sizeof (tmpstr),
                     &Bphoneidfound))
                {
                    fclose (rfile);
                    return FALSE;
                }

                if (Bphoneidfound)
                {
                    if (!MISC_C30Like (*pphonemodel))
                    {
                        *pU32phone_id = MISC_SwapDword (MISC_HexToInt (tmpstr));
                    }
                    else
                    {
                        pU32phone_id[1] =
                            MISC_SwapDword (MISC_HexToInt (&tmpstr[8]));
                        tmpstr[8] = '\0';
                        pU32phone_id[0] =
                            MISC_SwapDword (MISC_HexToInt (tmpstr));
                    }
                }
            }
        }
    }

    fclose (rfile);
    if (Bmodelfound && BIMEIfound && Bphoneidfound)
    {
        MISC_SetCurrentPhonemodel (*puniquemodel);
    }

#ifdef MAPMODE
    if (!(Bmodelfound && BIMEIfound && Bphoneidfound))
    {
        printf("invalid logfile '%s'\n", logname);
    }
#endif

    return Bmodelfound && BIMEIfound && Bphoneidfound;
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

BOOL MAP_SaveLog (eMISC_MODEL_INDICES uniquemodel,
                  eMISC_MODEL_INDICES phonemodel, char *IMEI,
                  char *originalIMEI, UINT32 * pU32phoneid)
{
    FILE                         *rfile;
    char                          modelname[SYS_MAX_STRING_LEN],
        filename[SYS_MAX_PATHNAME_LEN];

    if (!MISC_GetPhoneModelName (modelname))
    {
        LDBGSTR1 ("MAP_SaveLog : invalid model id %d", phonemodel);
        return FALSE;
    }

    sprintf (filename, "Siemens %s - %s", modelname, IMEI);
    strcat (filename, "." FREIA_LOG_EXTENSION);
    MISC_CheckFileName (filename);
    rfile = fopen (filename, "wt");
    if (!rfile)
    {
#ifndef LOGMODE
        LDBGSTR1 ("MAP_SaveLog : cannot save '%s'", filename);
#else
        printf ("cannot create '%s'\n", filename);
#endif
        return FALSE;
    }

    fprintf (rfile, "LOG File.\nThis log file should be sent to /dev/null\n\n");

    if (MISC_C30Like (phonemodel))
    {
        if (originalIMEI)
        {
            fprintf (rfile,
                     "[Model: Siemens %s][PhoneID: " UINT32_FORMAT UINT32_FORMAT
                     "][Desired IMEI: %.6s-%.2s-%.6s/%.6s-%.2s-%.6s]\n",
                     modelname, MISC_SwapDword (pU32phoneid[0]),
                     MISC_SwapDword (pU32phoneid[1]), IMEI, &IMEI[6], &IMEI[8],
                     originalIMEI, &originalIMEI[6], &originalIMEI[8]);
        }
        else
        {
            fprintf (rfile,
                     "[Model: Siemens %s][PhoneID: " UINT32_FORMAT UINT32_FORMAT
                     "][Desired IMEI: %.6s-%.2s-%.6s]\n", modelname,
                     MISC_SwapDword (pU32phoneid[0]),
                     MISC_SwapDword (pU32phoneid[1]), IMEI, &IMEI[6], &IMEI[8]);
        }
    }
    else
    {
        if (originalIMEI)
        {
            fprintf (rfile,
                     "[Model: Siemens %s][PhoneID: " UINT32_FORMAT
                     "][Desired IMEI: %.6s-%.2s-%.6s/%.6s-%.2s-%.6s]\n",
                     modelname, MISC_SwapDword (pU32phoneid[0]), IMEI, &IMEI[6],
                     &IMEI[8], originalIMEI, &originalIMEI[6],
                     &originalIMEI[8]);
        }
        else
        {
            fprintf (rfile,
                     "[Model: Siemens %s][PhoneID: " UINT32_FORMAT
                     "][Desired IMEI: %.6s-%.2s-%.6s]\n", modelname,
                     MISC_SwapDword (pU32phoneid[0]), IMEI, &IMEI[6], &IMEI[8]);
        }
    }

    fclose (rfile);

#ifdef LOGMODE
    printf ("log is saved to '%s'\n", filename);
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

static BOOL ishexdigit (char chr)
{
    return isdigit (chr) ||
        (chr >= 'A' && chr <= 'F') || (chr >= 'a' && chr <= 'f');
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

BOOL MAP_LoadMap (eMISC_MODEL_INDICES phonemodel, char *filename,
                  UINT32 * pU32phoneid, UINT8 * pB5009, UINT8 * pB0001,
                  UINT8 * pEnc5008, UINT8 * pEnc5077)
{
    FILE                         *rfile;
    char                          num[32], line[SYS_MAX_STRING_LEN], *pblockid;
    UINT16                        i;
    UINT8                         U8upcoming_block = 0;
    UINT8                         U8blockid;
    BOOL                          Bfinished;
    BOOL                          Bgood;
    UINT16                        U16blocklen;
    UINT16                        U16B5008Len, U16B5077Len;
    UINT8                        *pIMEIblocks[4];
    UINT8                         U8map_last_imei_block =
        (MISC_C30Like (phonemodel) ? MAP_LAST_C30_IMEI_BLOCK :
         MAP_LAST_IMEI_BLOCK);

    if (!MISC_C30Like (phonemodel))
    {
        pIMEIblocks[0] = pB5009;
        pIMEIblocks[1] = pB0001;
        pIMEIblocks[2] = pEnc5008;
        pIMEIblocks[3] = pEnc5077;
    }
    else
    {
        pIMEIblocks[0] = pEnc5008;
        pIMEIblocks[1] = NULL;
        pIMEIblocks[2] = NULL;
        pIMEIblocks[3] = NULL;
    }

    rfile = fopen (filename, "rt");
    if (!rfile)
    {
#ifdef LOGMODE
        printf ("cannot open %s\n", filename);
#endif
        return FALSE;
    }

    SEC_GetBlockLen (phonemodel, &U16B5008Len, &U16B5077Len);

    Bfinished = fgets (line, sizeof (line), rfile) == NULL;
    while (!Bfinished)
    {
        pblockid = strstr (line, MAP_ID_BLOCK_ID_STR);
        if (pblockid != NULL)   /* yahoo! we have a block */
        {
            pblockid += strlen (MAP_ID_BLOCK_ID_STR);
            i = 0;
            while (i < sizeof (num) && pblockid != '\0'
                   && pblockid < line + SYS_MAX_STRING_LEN
                   && ishexdigit (pblockid[i]))
            {
                num[i] = pblockid[i];
                i++;
            }
            num[i] = 0;

            if (!MISC_C30Like (phonemodel))
            {
                *pU32phoneid = MISC_SwapDword (MISC_HexToInt (num));
            }
            else
            {
                pU32phoneid[1] = MISC_SwapDword (MISC_HexToInt (&num[8]));
                num[8] = '\0';
                pU32phoneid[0] = MISC_SwapDword (MISC_HexToInt (num));
            }
        }

        pblockid = strstr (line, MAP_IMEI_BLOCK_ID_STR);
        if (pblockid == NULL)
        {
            pblockid = strstr (line, MAP_C30_IMEI_BLOCK_ID_STR);
        }

        if (pblockid != NULL)   /* yahoo! we have a block */
        {
            if (!MISC_C30Like (phonemodel))
            {
                pblockid += strlen (MAP_IMEI_BLOCK_ID_STR);
                i = 0;
                while (i < sizeof (num) && pblockid != '\0'
                       && pblockid < line + SYS_MAX_STRING_LEN
                       && isdigit (pblockid[i]))
                {
                    num[i] = pblockid[i];
                    i++;
                }
                num[i] = 0;
                U8blockid = atoi (num);
            }
            else
            {
                U8blockid = 0;
            }

            if (U8blockid > U8upcoming_block)
            {
                fclose (rfile);
                return FALSE;
            }

            if (!MISC_C30Like (phonemodel))
            {
                switch (U8blockid)
                {
                case 0:
                case 1:
                    U16blocklen = FREIA_5009_AND_0001_LEN;
                    break;

                case 2:
                    U16blocklen = U16B5008Len;
                    break;

                case 3:
                    U16blocklen = U16B5077Len;
                    break;

                default:
                    fclose (rfile);
                    return FALSE;
                }
            }
            else
            {
                switch (U8blockid)
                {
                case 0:
                    U16blocklen = U16B5008Len;
                    break;

                default:
                    fclose (rfile);
                    return FALSE;
                }
            }

            if (!MISC_ReadHexBlock
                (rfile, &U16blocklen, pIMEIblocks[U8blockid]))
            {
                fclose (rfile);
#ifdef LOGMODE
                printf ("invalid MAP format\n");
#endif
                return FALSE;
            }

            U8upcoming_block++;
        }

        Bfinished = (fgets (line, sizeof (line), rfile) == NULL)
            || (U8upcoming_block > U8map_last_imei_block);
    }
    fclose (rfile);

    Bgood = U8upcoming_block > U8map_last_imei_block;
#ifdef LOGMODE
    if (!Bgood)
    {
        printf ("invalid MAP format\n");
    }
#endif
    return Bgood;
}
