
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
#include "commpub.h"
#include "secpub.h"
#include "ttypub.h"
#include "mappub.h"

eERROR                        freia_errno;
tFREIA_PHONEINFO              freia_phoneinfo;

static void Terminate (BOOL Bboots)
{
    if (Bboots)
    {
        COMM_Reboot ();
    }
    TTY_Destruct ();
    MISC_FinishRedirectionOfError ();
}

static void cmd_welcome (void)
{
    printf ("freialog\n");
    printf ("------------------------------------\n");
    printf ("Thanks goes out for the following:\n");
    printf ("Maxim, Rolis, Sergey, PapaJoe, Lead,\n");
    printf ("TheWizard, Jozso (for hardware and cable\n");
    printf ("support and of course beta testing),\n");
    printf ("Nico, Victor, Pinker, Tom,\n");
    printf ("and all the others who supports ...\n\n");
}

static BOOL cmd_getcom (UINT8 * pcom)
{
    char                          selection[32];
    BOOL                          Bfinished = FALSE;
    SINT16                        num_com;

    *pcom = 1;

    while (!Bfinished)
    {
        printf ("Select COMx (1/2/3/4/Q): ");
        gets (selection);
        if (selection[0] == 'Q' || selection[0] == 'q')
        {
            return FALSE;
        }
        num_com = atoi (selection);
        Bfinished = num_com >= 1 && num_com <= 4;
        if (!Bfinished)
        {
            printf ("illegal entry. select again\n");
        }
    };

    *pcom = num_com;
    return TRUE;
}

static BOOL cmd_getaction (BOOL * pBlog)
{
    char                          selection[32];
    BOOL                          Bfinished = FALSE;

    *pBlog = TRUE;

    while (!Bfinished)
    {
        printf ("(R)ead log/(W)rite map/(Q)uit: ");
        gets (selection);
        if (selection[0] == 'Q' || selection[0] == 'q')
        {
            return FALSE;
        }
        Bfinished = selection[0] == 'R' || selection[0] == 'r'
            || selection[0] == 'W' || selection[0] == 'w';

        if (!Bfinished)
        {
            printf ("illegal entry. select again\n");
        }
    };

    *pBlog = selection[0] == 'R' || selection[0] == 'r';
    return TRUE;
}

static BOOL cmd_getfilename (char *filename)
{
    FILE                         *rfile;
    BOOL                          Bfinished = FALSE;

    while (!Bfinished)
    {
        printf ("Enter MAP name ('Q' to exit): ");
        gets (filename);
        if (strlen (filename) == 1
            && (filename[0] == 'Q' || filename[0] == 'q'))
        {
            return FALSE;
        }

        rfile = fopen (filename, "rt");
        if (!rfile)
        {
            printf ("cannot open '%s'\n", filename);
        }
        else
        {
            fclose (rfile);
            Bfinished = TRUE;
        }
    }

    return TRUE;
}

static BOOL cmd_getphone (tMISC_PHONE_MODEL ** pmodelstruct)
{
    const tMISC_PHONE_MODEL      *pmodel1;
    const tMISC_PHONE_MODEL      *pmodel2;
    UINT8                         i;
    BOOL                          Bfinished = FALSE;
    char                          selection[32];
    SINT16                        num_selection;
    UINT16                        U16maxentry;

    *pmodelstruct = NULL;
    while (!Bfinished)
    {
        printf ("Select phone:\n");
        i = 0;
        do
        {
            pmodel1 = MISC_GetModel (i++);
            if (pmodel1 != NULL)
            {
                pmodel2 = MISC_GetModel (i++);
                if (pmodel2 == NULL)
                {
                    printf ("%d. '%s'\n", i - 1, pmodel1->modelname);
                }
                else
                {
                    printf ("%d. '%s'		%d. '%s'\n", i - 1,
                            pmodel1->modelname, i, pmodel2->modelname);
                }
            }
        }
        while (pmodel1);
        printf ("Q. Exit\n");
        U16maxentry = (pmodel2 == NULL ? i - 2 : i - 1);
        printf (": ");
        gets (selection);
        if (selection[0] == 'Q' || selection[0] == 'q')
        {
            return FALSE;
        }

        num_selection = atoi (selection);
        Bfinished = (num_selection >= 1 && num_selection <= U16maxentry);
        if (!Bfinished)
        {
            printf ("illegal entry. select again\n");
        }
    }

    *pmodelstruct = (tMISC_PHONE_MODEL *) MISC_GetModel (num_selection - 1);
    return TRUE;
}

int main (int argc, char *argv[])
{
    UINT8                         U8comport = 1;
    tMISC_PHONE_MODEL            *pmodelstruct;
    BOOL                          Blog;
    UINT32                        AU32phoneid[2];
    UINT8                         B5009[10], B0001[10];
    UINT8                         Enc5008[2048];    /* because of C30 */
    UINT8                         Enc5077[234];
    char                          filename[SYS_MAX_PATHNAME_LEN];
    BOOL                          Boriginalimei;
    UINT8                         U8boottype = 1;

    if (argc > 1)
    {
        U8boottype = argv[1][0] - '0';
        if (U8boottype > 3)
        {
            U8boottype = 1;
        }
    }

    cmd_welcome ();
    if (!cmd_getcom (&U8comport))
    {
        return -1;
    }

    if (!cmd_getphone (&pmodelstruct))
    {
        return -1;
    }

    if (!cmd_getaction (&Blog))
    {
        return -1;
    }

    if (!Blog)
    {
        if (!cmd_getfilename (filename))
        {
            return -1;
        }
    }

    printf ("using COM%d\n", U8comport);
    TTY_Construct ();
    MISC_SetCurrentPhonemodel (pmodelstruct->uniqueid);
    MISC_SetLogCallout (NULL);
    MISC_SetGauge (NULL, NULL, NULL);
    MISC_SetDebugLevel (DEBUG_LEVEL_NONE);
    MISC_SetRedirection (REDIRECTION_TYPE_NONE);
    MISC_SetAppCOMPort (U8comport);
    MISC_SetBootType (U8boottype);

    COMM_Construct ();
    if (!COMM_LoadBoots
        (U8comport, 115200, pmodelstruct->flashid, pmodelstruct->modelid))
    {
        Terminate (FALSE);
        return -1;
    }

    if (Blog)
    {
        Boriginalimei = freia_phoneinfo.BoriginalIMEI &&
            strcmp (freia_phoneinfo.originalIMEI, freia_phoneinfo.IMEI) != 0;

        if (!MAP_SaveLog
            (pmodelstruct->uniqueid, pmodelstruct->modelid,
             freia_phoneinfo.IMEI,
             (Boriginalimei ? freia_phoneinfo.originalIMEI : NULL),
             freia_phoneinfo.AU32phoneid))
        {
            return FALSE;
        }
    }
    else
    {
        printf ("Uploading %s (please be patient)\n", filename);
        if (!MAP_LoadMap
            (pmodelstruct->modelid, filename, AU32phoneid,
             B5009, B0001, Enc5008, Enc5077))
        {
            return FALSE;
        }

        if (!COMM_UpdateIMEIBlocks
            (pmodelstruct->modelid, B5009, B0001, Enc5008, Enc5077, FALSE))
        {
            return FALSE;
        }
    }

    Terminate (TRUE);
    return 0;
}
