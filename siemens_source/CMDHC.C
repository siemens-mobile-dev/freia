
/********************************************************************
*                                                                   *
* FILE NAME: cmdhc.c                                                *
*                                                                   *
* PURPOSE: commandline interface to the DLL                         *
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
* EXTERNAL REFERENCES:                                              *
*                                                                   *
* Name:                                                             *
*                                                                   *
* Description:                                                      *
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
* Date: 21. May, 2002                                               *
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
#include "config.h"

#include <Windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dir.h>

#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "mappub.h"
#include "secpub.h"
#include "commpub.h"
#include "ttypub.h"
#include "dngpub.h"
#include "cmdpub.h"
#include "cmdloc.h"

eERROR                        freia_errno;
tFREIA_PHONEINFO              freia_phoneinfo;

static char                   errbuff[128];

static UINT8                  B5009[10], B0001[10];

static UINT8                  Enc5008[2048];    /* because of C30 */
static UINT8                  Enc5077[234];

static UINT8                  Dec5008[2048];    /* because of C30 */
static UINT8                  Dec5077[234];

static pLogCallout            mylog = NULL;

static BOOL                   Bcloseatonce = FALSE;


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_readarg                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the input charater argument                           *
*                                                       *
* the pointer to the buffer which will store the        *
* converted argument                                    *
*                                                       *
* a flag telling whether it's a number or a string      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* arg                                                   *
*                                                       *
* val                                                   *
*                                                       *
* Bnumber                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *                                                *
*                                                       *
* void *                                                *
*                                                       *
* BOOL                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* the function converts the input. Checks whether it's  *
* a hexdecimal number or not. If it's a number it       *
* converts it.                                          *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void cmdline_readarg (char *arg, void *val, BOOL Bnumber)
{
    UINT16                        U16start = *((UINT16 *) arg);

    if (Bnumber)
    {
        /*
         * 0x or 0X 
         */
        if (U16start == 0x7830 || U16start == 0x5830)
            *((long *) val) = MISC_HexToInt (arg);
        else
            *((long *) val) = atol (arg);
    }
    else
        strcpy ((char *) val, arg);
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_analyzearg                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the input argument                                    *
*                                                       *
* a pointer to the detected command structure           *
*                                                       *
* a pointer to the list of already detected command     *
* structures                                            *
*                                                       *
* a global list of available commands                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* arg                                                   *
*                                                       *
* actcmd                                                *
*                                                       *
* usedcommands                                          *
*                                                       *
* commcmd                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *                                                *
*                                                       *
* tCMDLINE_USED_COMMAND **                              *
*                                                       *
* tCMDLINE_USED_COMMANDS *                              *
*                                                       *
* tCMDLINE_CMDBLOCK[]                                   *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to identify the argument as a      *
* command or as an argument to a command.               *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the argument could be classified as a command *
* or as an arguemtn                                     *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_analyzearg (char *arg, tCMDLINE_USED_COMMAND ** actcmd,
                                tCMDLINE_USED_COMMANDS * usedcommands)
{
    UINT8                         i, *U8numcmds, *U8numargs;
    BOOL                          Bfound;
    char                          argcmd[CMDLINE_MAX_LONGCMD_LEN];

    if (arg[0] == '-' || arg[0] == '/' || arg[0] == '\\')   // it is a command
    {
        strcpy (argcmd, &arg[1]);
        for (i = 0; i < (UINT8) strlen (argcmd); i++)
            argcmd[i] = tolower (argcmd[i]);

        for (Bfound = FALSE, i = 0; i < U8num_of_commcmds && !Bfound; i++)
        {
            Bfound = strcmp (commcmd[i].longcmd_str, argcmd) == 0 ||
                strcmp (commcmd[i].shortcmd_str, argcmd) == 0;
            if (Bfound)
            {
                U8numcmds = &usedcommands->U8num_of_commands;

                if (*U8numcmds >= CMDLINE_MAX_COMMANDS)
                {
                    MISC_SetErrorCode (KE_TOO_MANY_COMMANDS);
                    return FALSE;
                }

                *actcmd = &usedcommands->command[*U8numcmds];

                strcpy ((*actcmd)->longcmd_str, commcmd[i].longcmd_str);
                (*actcmd)->U16cmdidx = i;
                (*U8numcmds)++;
            }
        }

        if (!Bfound)
        {
            MISC_SetErrorCode (KE_COMMAND_NOT_FOUND);
            return FALSE;
        }
    }
    else
    {
        if (!(*actcmd))         /* can't start with argument */
        {
            MISC_SetErrorCode (KE_NO_COMMAND_IS_SET);
            return FALSE;
        }

        U8numargs = &(*actcmd)->U8num_of_args;

        if (*U8numargs >= CMDLINE_MAX_CMD_ARGS)
        {
            MISC_SetErrorCode (KE_TOO_MANY_ARGUMENTS);
            return FALSE;
        }

        strcpy ((*actcmd)->arg[*U8numargs].value, arg);
        (*U8numargs)++;
    }

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_getnextnumberfromarg           *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a pointer to the current position of the current      *
* argument                                              *
*                                                       *
* a pointer to the next position of the current         *
* argument                                              *
*                                                       *
* a pointer to the converted number                     *
*                                                       *
* a pointer to a flag indicating whether it's the last  *
* number or not                                         *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* currargptr                                            *
*                                                       *
* nextargptr                                            *
*                                                       *
* U32num                                                *
*                                                       *
* Blast                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *                                                *
*                                                       *
* char **                                               *
*                                                       *
* UINT32 *                                              *
*                                                       *
* BOOL *                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This function checks the current argument for a       *
* number (since the arguments can hold multiple         *
* numbers, separated with , or ..).                     *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the conversion was done                       *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_getnextnumberfromarg (char *currargptr, char **nextargptr,
                                          UINT32 * U32num, BOOL * Blast)
{
    char                          tmpnum[CMDLINE_MAX_ARGLEN];
    UINT8                         i = 0;
    BOOL                          Bfinished, Bhexenabled;
    UINT16                        U16start = *((UINT16 *) currargptr);
    char                          chr;

    memset (tmpnum, 0, sizeof (tmpnum));

    /*
     * arg starts with 0x or 0X 
     */
    Bhexenabled = (U16start == 0x7830 || U16start == 0x5830);
    if (Bhexenabled)
        currargptr += 2;        /* skip "header" */

    *Blast = FALSE;
    *U32num = 0;
    *nextargptr = NULL;

    for (Bfinished = FALSE; !Bfinished; currargptr++)
    {
        chr = toupper (*currargptr);

        if (!Bhexenabled)
            Bfinished = (chr < '0' || chr > '9');
        else
        {
            Bfinished = !((chr >= '0' && chr <= '9') ||
                          (chr >= 'A' && chr <= 'F'));
        }

        /*
         * EOF? 
         */
        *Blast = chr == '\0';
        Bfinished = Bfinished || *Blast;

        if (!Bfinished)
            tmpnum[i++] = chr;
    }

    *nextargptr = currargptr - 1;

    if (strlen (tmpnum) == 0)
    {
        *Blast = TRUE;
        MISC_SetErrorCode (KE_INVALID_NUMBER);
        return FALSE;
    }

    /*
     * 0x or 0X 
     */
    if (Bhexenabled)
        *U32num = MISC_HexToInt (tmpnum);
    else
        *U32num = atol (tmpnum);

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_convertnumarg                  *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a pointer to the current argument                     *
*                                                       *
* a pointer to the structure of converted numbers       *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* arg                                                   *
*                                                       *
* runtimecmd                                            *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *                                                *
*                                                       *
* tCMDLINE_RUNTIME_COMMAND *                            *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This function converts series of numbers in the       *
* given argument. The argument can store multiple       *
* numbers spearated with , or .. Like: 1,5,6..9         *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the conversion went fine                      *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_convertnumarg (char *arg,
                                   tCMDLINE_RUNTIME_COMMAND * runtimecmd)
{
    char                         *nextargptr, *currargptr = arg;
    UINT32                        U32num, U32end;
    UINT32                        i = runtimecmd->U8num_of_numargs, j;
    BOOL                          Blast = FALSE, Bfinished;

    while (!Blast)
    {
        if (!cmdline_getnextnumberfromarg
            (currargptr, &nextargptr, &U32num, &Blast))
        {
            return FALSE;
        }

        if (i >= CMDLINE_MAX_EXPANDED_NUMBERS)
        {
            MISC_SetErrorCode (KE_TOO_MANY_NUMBERS);
            return FALSE;
        }

        runtimecmd->U32numarg[i] = U32num;
        i++;

        currargptr = nextargptr;
        if (*currargptr == ',')
            currargptr++;
        else if (*currargptr == '.' && *(currargptr + 1) == '.')
        {
            if (!cmdline_getnextnumberfromarg
                (currargptr + 2, &nextargptr, &U32end, &Blast))
            {
                return FALSE;
            }

            for (Bfinished = FALSE, j = U32num + 1; !Bfinished && j <= U32end;
                 i++, j++)
            {
                Bfinished = i > CMDLINE_MAX_EXPANDED_NUMBERS;
                if (!Bfinished)
                    runtimecmd->U32numarg[i] = j;
            }
            currargptr = nextargptr;
            if (*currargptr == ',')
                currargptr++;
        }
    }

    runtimecmd->U8num_of_numargs = i;

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_convertarg                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the given argument that is to be converted            *
*                                                       *
* the type of the argument                              *
*                                                       *
* a pointer to the structure of the converted argument  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* arg                                                   *
*                                                       *
* argtype                                               *
*                                                       *
* runtimecmd                                            *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *                                                *
*                                                       *
* eCMDLINE_COMMAND_ARGUMENT_TYPE                        *
*                                                       *
* tCMDLINE_RUNTIME_COMMAND *                            *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function converts the argument based upon the     *
* type of it.                                           *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the conversion went fine                      *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_convertarg (char *arg,
                                eCMDLINE_COMMAND_ARGUMENT_TYPE argtype,
                                tCMDLINE_RUNTIME_COMMAND * runtimecmd)
{
    char                         *posstart, *posend;
    UINT16                        U16strlen;
    UINT32                        i = runtimecmd->U8num_of_stringargs;

    switch (argtype)
    {
    case FCAT_STRING:
        posstart = strchr (arg, CMDLINE_STRING_SEP);
        if (posstart == NULL)
            posstart = arg;
        else
            posstart++;         /* skip start */

        posend = strchr (posstart, CMDLINE_STRING_SEP);
        if (posend == NULL)
            U16strlen = strlen (arg);
        else
            U16strlen = posend - posstart;

        if (i >= CMDLINE_MAX_STRINGS)
        {
            MISC_SetErrorCode (KE_TOO_MANY_STRINGS);
            return FALSE;
        }

        strncpy (runtimecmd->stringarg[i].value, posstart, U16strlen);
        runtimecmd->U8num_of_stringargs++;
        return TRUE;

    case FCAT_NUMBER:
        if (!cmdline_convertnumarg (arg, runtimecmd))
        {
            return FALSE;
        }
        return TRUE;

    default:
    case FCAT_NONE:
        break;
    }

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_convertcommandargs             *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the selected command                                  *
*                                                       *
* the arguments                                         *
*                                                       *
* the converted result command                          *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* cmd                                                   *
*                                                       *
* usedcommand                                           *
*                                                       *
* runtimecommands                                       *
*                                                       *
* TYPE:                                                 *
*                                                       *
* cmd                                                   *
*                                                       *
* usedcommand                                           *
*                                                       *
* runtimecommands                                       *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This function converts all the given commands'        *
* arguments                                             *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if everything went fine                          *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_convertcommandargs (const tCMDLINE_CMDBLOCK * cmd,
                                        tCMDLINE_USED_COMMAND * usedcommand,
                                        tCMDLINE_RUNTIME_COMMANDS *
                                        runtimecommands)
{
    UINT8                         i;
    UINT8                        *U8numofcommands =
        &runtimecommands->U8num_of_commands;
    tCMDLINE_RUNTIME_COMMAND     *runtimecmd;

    if (*U8numofcommands > CMDLINE_MAX_COMMANDS)
    {
        MISC_SetErrorCode (KE_TOO_MANY_COMMANDS);
        return FALSE;
    }

    runtimecmd = &runtimecommands->command[*U8numofcommands];
    runtimecmd->cmd = cmd->cmd;
    runtimecmd->Bboots = cmd->Bboots;

    strcpy (runtimecmd->longcmd_str, cmd->longcmd_str);

    (*U8numofcommands)++;

    if (cmd->U8num_of_args == 0)    /* no argument for the given command */
    {
        if (usedcommand->U8num_of_args > 0)
        {
            MISC_SetErrorCode (KE_TOO_MANY_ARGUMENTS_FOR_COMMAND);
            return FALSE;
        }

        return TRUE;
    }

    if (!cmd->Boptionalargs)
    {
        if (cmd->U8num_of_args != usedcommand->U8num_of_args)   /* less arguments */
        {
            MISC_SetErrorCode (KE_TOO_FEW_ARGUMENTS_FOR_COMMAND);
            return FALSE;
        }
    }
    else                        /* we have optional arguments */
    {
        if (usedcommand->U8num_of_args > cmd->U8num_of_args)    /* more arguments, less is accepted */
        {
            return FALSE;
        }
    }

    for (i = 0; i < usedcommand->U8num_of_args; i++)
    {
        if (!cmdline_convertarg
            (usedcommand->arg[i].value, cmd->argtype[i], runtimecmd))
        {
            return FALSE;
        }
    }

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmdline_analyzeargs                    *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* number of input arguments                             *
*                                                       *
* input argument array                                  *
*                                                       *
* output array of used commands                         *
*                                                       *
* output array of aggregated commands                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* argc                                                  *
*                                                       *
* argv                                                  *
*                                                       *
* usedcommands                                          *
*                                                       *
* runtimecommands                                       *
*                                                       *
* TYPE:                                                 *
*                                                       *
* int                                                   *
*                                                       *
* char *[]                                              *
*                                                       *
* tCMDLINE_USED_COMMANDS *                              *
*                                                       *
* tCMDLINE_RUNTIME_COMMANDS *                           *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function converts all the given arguements into   *
* the output arrays.                                    *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the conversion went fine                      *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmdline_analyzeargs (int argc, char *argv[],
                                 tCMDLINE_USED_COMMANDS * usedcommands,
                                 tCMDLINE_RUNTIME_COMMANDS * runtimecommands,
                                 char *model)
{
    UINT8                         i;
    const tCMDLINE_CMDBLOCK      *cmd;
    tCMDLINE_USED_COMMAND        *actcmd;

    memset (usedcommands, 0, sizeof (*usedcommands));
    memset (runtimecommands, 0, sizeof (*runtimecommands));

    if (argc < 2)
    {
        return FALSE;
    }

    /*
     * the first is always the phone model 
     */
    cmdline_readarg (argv[1], model, FALSE);

    actcmd = NULL;
    for (i = 2; i < argc; i++)
    {
        if (!cmdline_analyzearg (argv[i], &actcmd, usedcommands))
            return FALSE;
    }

    /*
     * We have commands and arguments 
     */
    for (i = 0; i < usedcommands->U8num_of_commands; i++)
    {
        actcmd = &usedcommands->command[i];
        cmd = &commcmd[actcmd->U16cmdidx];

        if (!cmdline_convertcommandargs (cmd, actcmd, runtimecommands)) /* bad arg */
        {
            return FALSE;
        }
    }

    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmd_help                               *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* global array of available commands                    *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* commcmd                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* tCMDLINE_CMDBLOCK[]                                   *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function lists all the available commands         *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void cmd_help (void)
{
    UINT8                         i, j;
    char                          onearg[SYS_MAX_STRING_LEN],
        arglist[SYS_MAX_STRING_LEN];
    UINT8                         argcount[CMDLINE_MAX_CMD_ARGS];
    BOOL                          Bwasarg;

    INFOSTR ("Usage:");
    INFOSTR (" freia C45 cmd1 arg1 cmd2 arg2 ...");
    for (i = 0; i < U8num_of_commcmds; i++)
    {
        for (j = 0; j < CMDLINE_MAX_CMD_ARGS; j++)
        {
            argcount[j] = 0;
        }

        strcpy (arglist, "");
        for (j = 0; j < commcmd[i].U8num_of_args; j++)
        {
            Bwasarg = FALSE;

            if (commcmd[i].argtype[j] == FCAT_NUMBER)
            {
                sprintf (onearg, "%s%d", "N", ++argcount[FCAT_NUMBER]);
                Bwasarg = TRUE;
            }
            else if (commcmd[i].argtype[j] == FCAT_STRING)
            {
                sprintf (onearg, "%s%d", "S", ++argcount[FCAT_STRING]);
                Bwasarg = TRUE;
            }

            if (Bwasarg)
            {
                strcat (arglist, onearg);
                if (j != commcmd[i].U8num_of_args - 1)
                {
                    strcat (arglist, " ");
                }
            }
        }

        if (strlen (arglist) > 0)
        {
            INFOSTR4 ("- %s(%s) %s : %s", commcmd[i].longcmd_str,
                      commcmd[i].shortcmd_str, arglist, commcmd[i].help);
        }
        else
        {
            INFOSTR3 ("- %s(%s) : %s", commcmd[i].longcmd_str,
                      commcmd[i].shortcmd_str, commcmd[i].help);
        }

    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmd_welcome                            *
*                                                       *
* ARGUMENTS: None                                       *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function lists a simple welcome info.             *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void cmd_welcome (void)
{
    INFOSTR2 ("FREIA - Engine build %d, Frontend build %d", MISC_GetBuild (),
              CMDLINE_BUILD);
    INFOSTR ("Thanks goes out for the following:");
    INFOSTR ("Maxim, Rolis, Sergey, PapaJoe, Lead,");
    INFOSTR ("TheWizard, Jozso (for hardware and cable");
    INFOSTR ("support and of course beta testing),");
    INFOSTR ("Nico, Victor, Pinker, Tom,");
    INFOSTR ("and all the others who supports ...");
}



/********************************************************
*                                                       *
* FUNCTION NAME: cmd_handlecommands                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the array of specified commands                       *
*                                                       *
* a flag showing whether a testcase has been specified  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* runtimecommands                                       *
*                                                       *
* Bruntestcase                                          *
*                                                       *
* TYPE:                                                 *
*                                                       *
* tCMDLINE_RUNTIME_COMMANDS *                           *
*                                                       *
* BOOL                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This function is to execute all commands. I.e. to     *
* call the appropriate function handler.                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if ALL commands has been executed                *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL cmd_handlecommands (tCMDLINE_RUNTIME_COMMANDS * runtimecommands,
                                tMISC_PHONE_MODEL * pmodelstruct, BOOL Bnomap,
                                BOOL Bnoupdate, UINT32 U32provider1,
                                UINT32 U32provider2, BOOL Bupdateflashimei)
{
    UINT8                         i;
    tCMDLINE_RUNTIME_COMMAND     *actcmd;
    UINT32                        AU32phoneid[2];
    BOOL                          Bbinary;
    char                          EEPROMIMEI[SYS_MAX_STRING_LEN],
        IMEI[SYS_MAX_STRING_LEN];
    BOOL                          Bfromlog;
    eMISC_MODEL_INDICES           uniquemodel, phonemodel;
    char                          modelname[SYS_MAX_STRING_LEN],
        filename[SYS_MAX_PATHNAME_LEN], srcpath[SYS_MAX_PATHNAME_LEN];
    char                          searchstring[SYS_MAX_PATHNAME_LEN];
    UINT8                         backupB5009[10];
    tFREIA_PHONEINFO             *pfreia_phoneinfo = MISC_GetPhoneInfoPtr ();
    UINT8                         backupB67[FREIA_67_LEN];
    struct ffblk                  ffblk;
    BOOL                          Bdone;
    BOOL                          Bempty, Boriginalimei;

    memset (IMEI, 0, sizeof (IMEI));

    for (i = 0; i < runtimecommands->U8num_of_commands; i++)
    {
        actcmd = &runtimecommands->command[i];

        switch (actcmd->cmd)
        {
        default:
        case CMD_NONE:
            break;

        case CMD_READ:
        case CMD_RAWREAD:
            memset (filename, 0, sizeof (filename));
            if (!MISC_C30Like (pmodelstruct->uniqueid))
            {
                sprintf (filename, "%s %s %s v%x",
                         pfreia_phoneinfo->manufacturer,
                         pfreia_phoneinfo->model,
                         pfreia_phoneinfo->languagepack,
                         pfreia_phoneinfo->U8version);
            }
            else
            {
                if (!MISC_GetPhoneModelName (modelname))
                {
                    return FALSE;
                }

                sprintf (filename, "Siemens %s", modelname);
            }

            if (!COMM_Read
                (filename, actcmd->cmd == CMD_RAWREAD, actcmd->U32numarg[0],
                 actcmd->U32numarg[1]))
            {
                return FALSE;
            }
            break;

        case CMD_WRITE:
        case CMD_RAWWRITE:
            Bbinary = actcmd->cmd == CMD_RAWWRITE;

            if (Bbinary)
            {
                if (!COMM_Write
                    (FLS_EXTENSION_ID, actcmd->stringarg[0].value,
                     actcmd->U32numarg[1], actcmd->U32numarg[2],
                     actcmd->U32numarg[0], Bbinary))
                {
                    return FALSE;
                }
            }
            else
            {
                if (!COMM_Write
                    (KSI_EXTENSION_ID, actcmd->stringarg[0].value,
                     actcmd->U32numarg[0], actcmd->U32numarg[1], 0, Bbinary))
                {
                    return FALSE;
                }
            }
            break;

        case CMD_BACKUP:
            if (COMM_Backup
                (pmodelstruct->modelid, IMEI, B5009, B0001, Enc5008, Enc5077))
            {
                if (!MAP_SaveMap
                    (pmodelstruct->modelid, MAPTYPE_BACKUP, NULL,
                     pfreia_phoneinfo->AU32phoneid, IMEI, B5009, B0001, Enc5008,
                     Enc5077))
                {
                    return FALSE;
                }
            }
            break;

        case CMD_BACKUPBATTERY:
            if (!COMM_BackupBattery (backupB67))
            {
                return FALSE;
            }

            if (!MAP_SaveBatteryMap
                (pmodelstruct->modelid, pfreia_phoneinfo->AU32phoneid,
                 pfreia_phoneinfo->IMEI, backupB67))
            {
                return FALSE;
            }
            break;

        case CMD_SETBATTERY:
            if (!MAP_LoadBatteryMap
                (pmodelstruct->modelid, actcmd->stringarg[0].value, AU32phoneid,
                 backupB67))
            {
                return FALSE;
            }

            if (!COMM_UpdateBatteryBlock (pmodelstruct->modelid, backupB67))
            {
                return FALSE;
            }
            break;

        case CMD_WRITEMAP:
            if (!MAP_LoadMap
                (pmodelstruct->modelid, actcmd->stringarg[0].value, AU32phoneid,
                 B5009, B0001, Enc5008, Enc5077))
            {
                INFOSTR1
                    ("cmd_handlecommands : invalid map file '%s'",
                     actcmd->stringarg[0].value);
                return FALSE;
            }

            if (!COMM_UpdateIMEIBlocks
                (pmodelstruct->modelid, B5009, B0001, Enc5008, Enc5077,
                 Bupdateflashimei))
            {
                return FALSE;
            }
            break;

        case CMD_UNLOCK:
        case CMD_UNLOCK_FROM_LOG:
            Bfromlog = actcmd->cmd == CMD_UNLOCK_FROM_LOG;

            if (!Bfromlog)
            {
                phonemodel = pmodelstruct->modelid;
                uniquemodel = pmodelstruct->uniqueid;

                AU32phoneid[0] = pfreia_phoneinfo->AU32phoneid[0];
                AU32phoneid[1] = pfreia_phoneinfo->AU32phoneid[1];

                memset (EEPROMIMEI, 0, sizeof (EEPROMIMEI));
                strcpy (EEPROMIMEI, pfreia_phoneinfo->IMEI);

                strcpy (IMEI, actcmd->stringarg[0].value);
                if (IMEI[0] == 'E')
                {
                    strcpy (IMEI, EEPROMIMEI);
                }
                else if (IMEI[0] == 'O')
                {
                    strcpy (IMEI, pfreia_phoneinfo->originalIMEI);
                }

                if (strlen (IMEI) < FREIA_IMEI_LEN)
                {
                    if (strlen (EEPROMIMEI) < FREIA_IMEI_LEN
                        && strlen (pfreia_phoneinfo->originalIMEI) <
                        FREIA_IMEI_LEN)
                    {
                        INFOSTR ("cmd_handlecommands : no valid IMEI found");
                        return FALSE;
                    }

                    if (strlen (EEPROMIMEI) >= FREIA_IMEI_LEN)
                    {
                        strcpy (IMEI, EEPROMIMEI);
                        INFOSTR1 ("cmd_handlecommands : using EEPROM IMEI (%s)",
                                  IMEI);
                    }
                    else
                    {
                        strcpy (IMEI, pfreia_phoneinfo->originalIMEI);
                        INFOSTR1
                            ("cmd_handlecommands : using original IMEI (%s)",
                             IMEI);
                    }
                }

                strcpy (actcmd->stringarg[0].value, IMEI);
            }
            else
            {
                if (!MAP_LoadLog
                    (actcmd->stringarg[0].value, &uniquemodel, &phonemodel,
                     IMEI, AU32phoneid))
                {
                    INFOSTR1
                        ("cmd_handlecommands : invalid log file '%s'",
                         actcmd->stringarg[0].value);
                    return FALSE;
                }

                strcpy (actcmd->stringarg[0].value, IMEI);
            }

            if (!MISC_C30Like (phonemodel))
            {
                SEC_CreateBlocks5009And76 (phonemodel,
                                           actcmd->stringarg[0].value, B5009,
                                           B0001);
            }

            if (!SEC_CreateIMEISpecificBlocks
                (U32provider1, U32provider2, uniquemodel, phonemodel,
                 actcmd->stringarg[0].value, AU32phoneid, Enc5008, Enc5077))
            {
                return FALSE;
            }

            if (!Bnomap || Bfromlog)
            {
                if (!MAP_SaveMap
                    (phonemodel, MAPTYPE_UNLOCK, NULL, AU32phoneid,
                     actcmd->stringarg[0].value, B5009, B0001, Enc5008,
                     Enc5077))
                {
                    return FALSE;
                }
            }

            if (!Bnoupdate && !Bfromlog)
            {
                if (!COMM_UpdateIMEIBlocks
                    (phonemodel, B5009, B0001, Enc5008, Enc5077,
                     Bupdateflashimei))
                {
                    return FALSE;
                }
            }
            break;

        case CMD_UNLOCK_FROM_MASS_OF_LOG:
            strcpy (srcpath, actcmd->stringarg[0].value);
            if (srcpath[strlen (srcpath) - 1] != '\\')
            {
                strcat (srcpath, "\\");
            }

            strcpy (searchstring, srcpath);
            strcat (searchstring, "*." FREIA_LOG_EXTENSION);
            Bdone = findfirst (searchstring, &ffblk, 0);
            while (!Bdone)
            {
                strcpy (filename, srcpath);
                strcat (filename, ffblk.ff_name);

                if (!MAP_LoadLog
                    (filename, &uniquemodel, &phonemodel, IMEI, AU32phoneid))
                {
                    Bdone = findnext (&ffblk);
                    continue;
                }


                if (!MISC_C30Like (phonemodel))
                {
                    SEC_CreateBlocks5009And76 (phonemodel, IMEI, B5009, B0001);
                }

                if (!SEC_CreateIMEISpecificBlocks
                    (U32provider1, U32provider2, uniquemodel, phonemodel,
                     IMEI, AU32phoneid, Enc5008, Enc5077))
                {
                    Bdone = findnext (&ffblk);
                    continue;
                }

                if (!MAP_SaveMap
                    (phonemodel, MAPTYPE_UNLOCK, srcpath, AU32phoneid,
                     IMEI, B5009, B0001, Enc5008, Enc5077))
                {
                    Bdone = findnext (&ffblk);
                    continue;
                }

                Bdone = findnext (&ffblk);
            }

            break;

        case CMD_DECRYPT:
        case CMD_DECRYPT_FROM_MAP:
            phonemodel = pmodelstruct->modelid;
            uniquemodel = pmodelstruct->uniqueid;

            if (actcmd->cmd == CMD_DECRYPT)
            {
                if (!COMM_Backup
                    (pmodelstruct->modelid, IMEI, B5009, B0001, Enc5008,
                     Enc5077))
                {
                    return FALSE;
                }

                AU32phoneid[0] = pfreia_phoneinfo->AU32phoneid[0];
                AU32phoneid[1] = pfreia_phoneinfo->AU32phoneid[1];
            }
            else
            {
                if (!MAP_LoadMap
                    (pmodelstruct->modelid, actcmd->stringarg[0].value,
                     AU32phoneid, B5009, B0001, Enc5008, Enc5077))
                {
                    INFOSTR1
                        ("cmd_handlecommands : invalid map file '%s'",
                         actcmd->stringarg[0].value);

                    return FALSE;
                }

                memcpy (backupB5009, B5009, sizeof (B5009));

                /*
                 * We need phone IDs for C30,S40 
                 */
                pfreia_phoneinfo->AU32phoneid[0] = AU32phoneid[0];
                pfreia_phoneinfo->AU32phoneid[1] = AU32phoneid[1];

                if (!SEC_RecreateIMEI
                    (pmodelstruct->modelid, B5009, IMEI, &Bempty))
                {
                    return FALSE;
                }
                memcpy (B5009, backupB5009, sizeof (B5009));
            }

            if (!MISC_C30Like (phonemodel))
            {
                SEC_RecreateBlocks5009And76 (phonemodel, B5009, B0001);
            }

            if (!SEC_RecreateIMEISpecificBlocks (uniquemodel,
                                                 phonemodel, IMEI,
                                                 AU32phoneid, Enc5008,
                                                 Enc5077, Dec5008, Dec5077))
            {
                return FALSE;
            }

            if (!MAP_SaveMap
                (phonemodel, MAPTYPE_DECRYPT, NULL, AU32phoneid,
                 IMEI, B5009, B0001, Dec5008, Dec5077))
            {
                return FALSE;
            }
            break;

        case CMD_FIX_CHECKSUMS:
        case CMD_CHECK_CHECKSUMS:
            if (!MISC_CRCCheck
                (actcmd->cmd == CMD_FIX_CHECKSUMS, actcmd->stringarg[0].value))
            {
                return FALSE;
            }
            break;

        case CMD_READMEMORY:
        case CMD_RAWREADMEMORY:
            if (!MISC_GetPhoneModelName (modelname))
            {
                return FALSE;
            }

            sprintf (filename, "Siemens %s", modelname);

            if (!COMM_ReadMemory
                (filename, actcmd->cmd == CMD_RAWREADMEMORY,
                 actcmd->U32numarg[0], actcmd->U32numarg[1]))
            {
                return FALSE;
            }
            break;

        case CMD_WRITELOG:
            Boriginalimei = pfreia_phoneinfo->BoriginalIMEI &&
                strcmp (pfreia_phoneinfo->originalIMEI,
                        pfreia_phoneinfo->IMEI) != 0;

            if (!MAP_SaveLog
                (pmodelstruct->uniqueid, pmodelstruct->modelid,
                 pfreia_phoneinfo->IMEI,
                 (Boriginalimei ? pfreia_phoneinfo->originalIMEI : NULL),
                 pfreia_phoneinfo->AU32phoneid))
            {
                return FALSE;
            }
            break;

        case CMD_PATCHBOOTCORE:
            Bbinary = actcmd->U8num_of_numargs > 0 && actcmd->U32numarg[0] == 1;

            if (!COMM_PatchBootcore (actcmd->stringarg[0].value, Bbinary))
            {
                return FALSE;
            }
            break;

        case CMD_GETDEVINFO:
            if (!DNG_GetDevInfo ())
            {
                return FALSE;
            }
            break;

        case CMD_SETDEVINFO:
            if (!DNG_SetDevInfo (actcmd->stringarg[0].value))
            {
                return FALSE;
            }
            break;

        case CMD_UPDATEDEV:
            if (!DNG_SendUpdate (actcmd->stringarg[0].value))
            {
                return FALSE;
            }
            break;

        case CMD_TESTDEV:
            if (!DNG_TestDongle ())
            {
                return FALSE;
            }
            break;

        }                       /* end switch */
    }                           /* end for */
    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: cmd_handleprecommands                  *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the array of specified commands                       *
*                                                       *
* the number of specified "real" commands               *
*                                                       *
* a pointer to the redirection setting                  *
*                                                       *
* a pointer to the debuglevel                           *
*                                                       *
* a pointer to a flag that tells whether there has      *
* been any testcase given                               *
*                                                       *
* the name of the specified testcase (if any)           *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* runtimecommands                                       *
*                                                       *
* numcommands                                           *
*                                                       *
* redirection                                           *
*                                                       *
* debuglevel                                            *
*                                                       *
* pBruntestcase                                         *
*                                                       *
* testcasename                                          *
*                                                       *
* TYPE:                                                 *
*                                                       *
* tCMDLINE_RUNTIME_COMMANDS *                           *
*                                                       *
* UINT8 *                                               *
*                                                       *
* eREDIRECTION_TYPE *                                   *
*                                                       *
* eDEBUG_LEVEL *                                        *
*                                                       *
* BOOL *                                                *
*                                                       *
* char *                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This function handles those commands that are only    *
* "preparing" the environment.                          *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if everything went fine                          *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static                        BOOL
cmd_handleprecommands (tCMDLINE_RUNTIME_COMMANDS *
                       runtimecommands, UINT8 * numcommands,
                       eREDIRECTION_TYPE * redirection,
                       eDEBUG_LEVEL * debuglevel,
                       UINT8 * comport_app, UINT8 * comport_dng,
                       BOOL * pBboots, BOOL * pBnomap,
                       BOOL * pBnoupdate, UINT32 * U32speed,
                       char *emulatefilename, BOOL * pBbinaryemulate,
                       UINT32 * U32provider1, UINT32 * U32provider2,
                       BOOL * pBupdateflashimei, MISC_BOOTTYPE * pboottype,
                       BOOL * pBcloseatonce, BOOL * pBcomport_app_dtr,
                       BOOL * pBcomport_app_rts)
{
    UINT8                         i;
    tCMDLINE_RUNTIME_COMMAND     *actcmd;

    *numcommands = 0;
    *debuglevel = DEBUG_LEVEL_MEDIUM;
    *redirection = REDIRECTION_TYPE_NONE;
    *comport_app = 1;
    *comport_dng = 2;
    *pBboots = FALSE;
    *pBnomap = FALSE;
    *pBnoupdate = FALSE;
    *U32speed = FREIA_DEFAULT_SPEED;
    *pBbinaryemulate = FALSE;
    *emulatefilename = '\0';
    *U32provider1 = 0;
    *U32provider2 = 0;
    *pBupdateflashimei = FALSE;
    *pboottype = MISC_BOOTTYPE_NORMAL;
    *pBcloseatonce = FALSE;
    *pBcomport_app_dtr = TRUE;
    *pBcomport_app_rts = TRUE;

    for (i = 0; i < runtimecommands->U8num_of_commands; i++)
    {
        actcmd = &runtimecommands->command[i];
        if (actcmd->Bboots)
            *pBboots = TRUE;
        switch (actcmd->cmd)
        {
        default:
            (*numcommands)++;
            break;
        case CMD_NONE:
            break;
        case CMD_COMAPP:
            *comport_app = actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_COMAPPTYPE:
            *pBcomport_app_dtr = actcmd->U32numarg[0];
            *pBcomport_app_rts = actcmd->U32numarg[1];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_COMDNG:
            *comport_dng = actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_SPEED:
            *U32speed = actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_SETREDIRECTION:
            *redirection = actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_SETDEBUGLEVEL:
            *debuglevel = actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_NOMAP:
            *pBnomap = TRUE;
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_NOUPDATE:
            *pBnoupdate = TRUE;
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_EMULATE:
        case CMD_RAWEMULATE:
            strcpy (emulatefilename, actcmd->stringarg[0].value);
            *pBbinaryemulate = actcmd->cmd == CMD_RAWEMULATE;
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_LOCKTOPROVIDER:
            *U32provider1 = actcmd->U32numarg[0];
            *U32provider2 = actcmd->U32numarg[1];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_UPDATEFLASHIMEI:
            *pBupdateflashimei = TRUE;
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_BOOTTYPE:
            *pboottype = (MISC_BOOTTYPE)actcmd->U32numarg[0];
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
        case CMD_PATCHBOOTCORE:
            (*numcommands)++;
            /*
             * we have not supplied a file, so shall boot 
             */
            if (!*pBboots)
            {
                *pBboots = actcmd->U8num_of_stringargs == 0;
            }
            break;
        case CMD_CLOSEATONCE:
            *pBcloseatonce = TRUE;
            actcmd->cmd = CMD_NONE; /* executed command */
            break;
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

static void Terminate (BOOL Bboots)
{
    DWORD                         numwritten;
    int                           c;

    if (Bboots)
    {
        COMM_Reboot ();
    }
    TTY_Destruct ();
    MISC_FinishRedirectionOfError ();

    if (MISC_IsConsoleAPP () && !Bcloseatonce)
    {
        printf ("Press Enter to close application...\n");
        scanf ("%c", &c);
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

void cmd_mylog (char *logstr, BOOL Biserror, BOOL Bshowaserror)
{
    printf ("%s\n", logstr);
    if (Biserror && Bshowaserror)
        printf ("ERRNO : " UINT32_FORMAT "\n", MISC_GetErrorCode ());
}

/********************************************************
*                                                       *
* FUNCTION NAME: cmd_main                               *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the number of input arguments                         *
*                                                       *
* the array of input arguments                          *
*                                                       *
* a pointer to the logger function                      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* argc                                                  *
*                                                       *
* argv                                                  *
*                                                       *
* logcallout                                            *
*                                                       *
* TYPE:                                                 *
*                                                       *
* int                                                   *
*                                                       *
* char *[]                                              *
*                                                       *
* pLogCallout                                           *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This is the main part of the commandline handler.     *
* It analyzes the input, loads the DLL and the executes *
* the given commands one by one.                        *
*                                                       *
* RETURNS:                                              *
*                                                       *
* 1 if everything went fine                             *
*                                                       *
* 0 otherwise                                           *
*                                                       *
*********************************************************/

int cmd_main (int argc, char *argv[],
              pLogCallout logcallout,
              pGaugeInit gaugeinit,
              pGaugeUpdate gaugeupdate, pGaugeDone gaugedone,
              pGUIUpdate guiupdate)
{
    eREDIRECTION_TYPE             redirectiontype = REDIRECTION_TYPE_NONE;
    eDEBUG_LEVEL                  debuglevel = DEBUG_LEVEL_HIGH;
    UINT8                         U8comport_app = FREIA_DEFAULT_COMPORT;
    UINT8                         U8comport_dng = FREIA_DEFAULT_COMPORT + 1;
    BOOL                          Bnomap = FALSE, Bnoupdate = FALSE;
    UINT32                        U32speed = FREIA_DEFAULT_SPEED;
    tCMDLINE_USED_COMMANDS        usedcommands;
    tCMDLINE_RUNTIME_COMMANDS     runtimecommands;
    UINT8                         U8numcommands = 0;
    BOOL                          Bboots = FALSE;
    char                          model[SYS_MAX_STRING_LEN];
    char                          emulatefilename[SYS_MAX_PATHNAME_LEN] = "";
    BOOL                          Bbinaryemulate;
    tMISC_PHONE_MODEL            *pmodelstruct;
    char                         *posstart, *posend;
    UINT32                        U32provider1 = 0, U32provider2 = 0;
    BOOL                          Bupdateflashimei = FALSE;
    MISC_BOOTTYPE                 boottype = MISC_BOOTTYPE_NORMAL;
    BOOL                          Bcom_dtr = TRUE;
    BOOL                          Bcom_rts = TRUE;

    TTY_Construct ();

    mylog = logcallout == NULL ? cmd_mylog : logcallout;
    cmd_welcome ();
    if (!cmdline_analyzeargs
        (argc, argv, &usedcommands, &runtimecommands, model))
    {
        cmd_help ();
        Terminate (FALSE);
        return FALSE;
    }

    posstart = strchr (model, CMDLINE_STRING_SEP);
    posstart = (posstart == NULL ? model : ++posstart);
    posend = strchr (posstart, CMDLINE_STRING_SEP);
    if (posend != NULL)
    {
        *posend = '\0';
    }

    if (!MISC_GetPhoneModel (posstart, &pmodelstruct))
    {
        INFOSTR1 ("not supported phone model (%s)", posstart);
        Terminate (FALSE);
        return FALSE;
    }

    if (!cmd_handleprecommands
        (&runtimecommands, &U8numcommands, &redirectiontype,
         &debuglevel, &U8comport_app, &U8comport_dng, &Bboots, &Bnomap,
         &Bnoupdate, &U32speed, emulatefilename, &Bbinaryemulate,
         &U32provider1, &U32provider2, &Bupdateflashimei, &boottype,
         &Bcloseatonce, &Bcom_dtr, &Bcom_rts))
    {
        Terminate (FALSE);
        return FALSE;
    }

    MISC_calibrate_delay ();
    MISC_SetBootType (boottype);
    MISC_SetCurrentPhonemodel (pmodelstruct->uniqueid);
    MISC_SetLogCallout (logcallout);
    MISC_SetGauge (gaugeinit, gaugeupdate, gaugedone);
    MISC_SetGUIUpdate (guiupdate);
    MISC_SetDebugLevel (debuglevel);
    MISC_SetRedirection (redirectiontype);

    if (!MISC_StartRedirectionOfError (FREIA_LOGFILE_NAME))
    {
        Terminate (FALSE);
        return FALSE;
    }

    if (U8numcommands == 0)
    {
        cmd_help ();
        Terminate (FALSE);
        return FALSE;
    }

    if (strlen (emulatefilename) > 0)
    {
        if (!MISC_SetEmulation (emulatefilename, Bbinaryemulate))
        {
            Terminate (FALSE);
            return FALSE;
        }
    }

    MISC_SetAppCOMPort (U8comport_app);
    MISC_SetDngCOMPort (U8comport_dng);
    MISC_SetAppCOMType (Bcom_dtr, Bcom_rts);

    COMM_Construct ();
    DNG_Construct ();

    if (Bboots)
    {
        if (!COMM_LoadBoots
            (U8comport_app, U32speed, pmodelstruct->flashid,
             pmodelstruct->modelid))
        {
            Terminate (FALSE);
            return FALSE;
        }
    }

    if (!cmd_handlecommands
        (&runtimecommands, pmodelstruct, Bnomap, Bnoupdate, U32provider1,
         U32provider2, Bupdateflashimei))
    {
        Terminate (Bboots);
        return FALSE;
    }

    Terminate (Bboots);
    INFOSTR ("O.K.");
    return TRUE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: main                                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the number of input arguments                         *
*                                                       *
* the array of input arguments                          *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* argc                                                  *
*                                                       *
* argv                                                  *
*                                                       *
* TYPE:                                                 *
*                                                       *
* int                                                   *
*                                                       *
* char *[]                                              *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* This is more or less a "dummy" function. It's only    *
* for making it possible to compile.                    *
*                                                       *
* RETURNS:                                              *
*                                                       *
* 1 if everything went fine                             *
*                                                       *
* 0 otherwise                                           *
*                                                       *
*********************************************************/
int main (int argc, char *argv[])
{
    return cmd_main (argc, argv, NULL, NULL, NULL, NULL, NULL);
}
