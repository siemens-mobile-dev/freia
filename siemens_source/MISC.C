
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
#include <fcntl.h>
#include <io.h>

#include "config.h"
#include "sysdef.h"
#include "freiapub.h"
#include "miscpub.h"
#include "miscloc.h"

static const UINT8 watermark[] = FREIA_WATERMARK_ID "3\x13\xfe\xe5\x07\x55\x10\x83\x47";

#ifndef MINIMAL_MISC
extern eERROR                 freia_errno;
extern tFREIA_PHONEINFO       freia_phoneinfo;
#endif

char                          logbuffer[SYS_MAX_LOGSTRING_LEN];
eDEBUG_LEVEL                  debug_level = DEBUG_LEVEL_HIGH;
static pLogCallout            logcallout = NULL;
static eREDIRECTION_TYPE      eRedirection = REDIRECTION_TYPE_NONE;
static FILE                  *logfile = NULL;
static BOOL                   foundseriallogger = FALSE;

/* this should be approx 2 BogoMips to start (note initial shift), and will
   still work even if initially too large, it will just take slightly longer */
UINT32                        U32loops_per_jiffy = (1 << 12);

#ifndef MINIMAL_MISC
static tMISC_CONFIG           misc_config = { FALSE, "", FALSE, 0xFFFFFF };

static pGaugeInit             GaugeInit = NULL;
static pGaugeDone             GaugeDone = NULL;
static pGaugeUpdate           GaugeUpdate = NULL;
static pGUIUpdate             GUIUpdate = NULL;
static UINT8                  U8comport_app = 0;
static UINT8                  U8comport_dng = 0;
static MISC_BOOTTYPE          boottype = MISC_BOOTTYPE_NORMAL;
static BOOL                   Bcomport_app_dtr = TRUE;
static BOOL                   Bcomport_app_rts = TRUE;
#endif

static BOOL                   Bconsoleapp = FALSE;


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

BOOL MISC_RedirectIOToConsole(void)
{
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    int hCrt;
    FILE *rf;

    FreeConsole();

    /* allocate a console for this app */
    if (!AllocConsole())
    {
        return FALSE;
    }

    /* set the screen buffer to be big enough to let us scroll text */
    if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo))
    {
        FreeConsole();
        return FALSE;
    }
    coninfo.dwSize.Y = 500;

    if (!SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize))
    {
        FreeConsole();
        return FALSE;
    }

    hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    if (hCrt==-1)
    {
        FreeConsole();
        return FALSE;
    }

    rf = ::_fdopen(hCrt, "w");
    if (rf==NULL)
    {
        FreeConsole();
        return FALSE;
    }

    if (::setvbuf(rf, NULL, _IONBF, 0)!=0)
    {
        FreeConsole();
        return FALSE;
    }
    *stdout = *rf;

    rf = ::_fdopen(hCrt, "w");
    if (rf==NULL)
    {
        FreeConsole();
        return FALSE;
    }

    if (::setvbuf(rf, NULL, _IONBF, 0)!=0)
    {
        FreeConsole();
        return FALSE;
    }
    *stderr = *rf;

    hCrt = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
    if (hCrt==-1)
    {
        FreeConsole();
        return FALSE;
    }

    rf = ::_fdopen(hCrt, "r");
    if (rf==NULL)
    {
        FreeConsole();
        return FALSE;
    }

    if (::setvbuf(rf, NULL, _IONBF, 0)!=0)
    {
        FreeConsole();
        return FALSE;
    }
    *stdin = *rf;

    Bconsoleapp = TRUE;
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

BOOL MISC_IsConsoleAPP(void)
{
    return Bconsoleapp;
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

void MISC_DefaultLogCallout (char *logstr, BOOL Biserror, BOOL Bshowaserror)
{
    printf ("%s\n", logstr);
    if (Biserror && Bshowaserror)
    {
#ifndef MINIMAL_MISC
        printf ("ERRNO : " UINT32_FORMAT "\n", freia_errno);
#endif
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

void MISC_SetLogCallout (pLogCallout desiredlogcallout)
{
    logcallout =
        (desiredlogcallout ? desiredlogcallout : MISC_DefaultLogCallout);
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

BOOL MISC_StartRedirectionOfError (char *filename)
{
    if (eRedirection != REDIRECTION_TYPE_NONE)
    {
        logfile = fopen (filename, "wt");

        if (logfile == NULL)
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

void MISC_FinishRedirectionOfError (void)
{
    if (eRedirection != REDIRECTION_TYPE_NONE && logfile != NULL)
        fclose (logfile);
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

void MISC_Sleep (UINT32 U32count)
{
    Sleep (U32count);
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

void MISC_HandleLogString (BOOL Biserror, BOOL Bshowaserror)
{
    if (logfile != NULL
        && (eRedirection == REDIRECTION_TYPE_SAVE_TO_FILE
            || eRedirection == REDIRECTION_TYPE_REDIRECT_TO_FILE))
    {
        fprintf (logfile, "%s\n", logbuffer);
    }

    if (eRedirection == REDIRECTION_TYPE_SAVE_TO_FILE
        || eRedirection == REDIRECTION_TYPE_NONE)
    {
        if (logcallout)
            logcallout (logbuffer, Biserror, Bshowaserror);
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

UINT8 MISC_HexCharToByte (char hex)
{
    if (hex >= 'A' && hex <= 'F')
        return (UINT8) hex - 55;
    else if (hex >= 'a' && hex <= 'f')
        return (UINT8) hex - 87;
    else if (hex >= '0' && hex <= '9')
        return (UINT8) hex - 48;

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

UINT32 MISC_HexToInt (char *hex)
{
    UINT32                        U32ret = 0;
    UINT16                        U16hexlen = strlen (hex);
    SINT16                        i;
    UINT32                        U32exp = 1;

    for (i = U16hexlen - 1; i >= 0; i--, U32exp *= 16)
        U32ret += (U32exp * MISC_HexCharToByte (hex[i]));

    return U32ret;
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

BOOL MISC_GetNextToken (char **str, char delim, char *token)
{
    char                         *start = *str;
    UINT8                         U8sub;

    if (*str == NULL || **str == '\0')
    {
        return FALSE;
    }

    *str = strchr (*str, delim);

    if (*str)
    {
        while (**str == delim && **str != '\0')
        {
            (*str)++;
        }
        U8sub = 1;
    }
    else
    {
        *str = start + strlen (start);
        U8sub = 0;
    }

    strncpy (token, start, *str - start - U8sub);
    token[*str - start - U8sub] = 0;

    return strlen (token) > 0;
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

UINT32 MISC_GetFileSize (FILE * rfile)
{
    UINT32                        U32length, U32origpos;

    U32origpos = ftell (rfile);
    fseek (rfile, 0L, SEEK_END);
    U32length = ftell (rfile);
    HDBGSTR1 ("misc_getfilesize : length of file is %d", U32length);
    fseek (rfile, U32origpos, SEEK_SET);

    return U32length;
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

UINT32 MISC_Adler32 (UINT8 * buf, UINT16 len)
{
    UINT32                        s1 = LOWORD (1);
    UINT32                        s2 = HIWORD (1);
    UINT16                        k;

    while (len > 0)
    {
        k = len < NMAX ? len : NMAX;
        len -= k;

        while (k >= 16)
        {
            DO16 (buf);
            buf += 16;
            k -= 16;
        }

        if (k != 0)
        {
            do
            {
                s1 += *buf++;
                s2 += s1;
            } while (--k);
        }

        s1 %= BASE;
        s2 %= BASE;
    }

    return (s2 << 16) | s1;
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

int __stdcall EnumWindowsProc (HWND hwnd, LPARAM lp)
{
    char                          classname[512];

    GetClassName (hwnd, classname, sizeof (classname));

    if (strstr (classname, (char *) lp))
    {
        foundseriallogger = TRUE;
        return FALSE;           // stop enumerating
    }

    GetWindowText (hwnd, classname, sizeof (classname));

    if (strstr (classname, (char *) lp))
    {
        foundseriallogger = TRUE;
        return FALSE;           // stop enumerating
    }

    return TRUE;                // keep enumerating
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

static BOOL FindApplications (char *tofind)
{
    foundseriallogger = FALSE;

    EnumWindows (EnumWindowsProc, (LPARAM) tofind);

    return foundseriallogger;
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

static BOOL SearchApplication (UINT8 * appclass, UINT16 appclasslen)
{
    HWND                          h;
    UINT16                        i;

    for (i = 0; i < appclasslen; i++)
    {
        appclass[i] ^= xorpassword[i % 6];
    }

    return FindApplications (appclass);
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

BOOL MISC_SerialLoggerIsRunning (void)
{
    return SearchApplication ("\xAE\x2A\xBE\x11\x7C\xB8\x90", 7)
        || SearchApplication ("\xBD\x2A\xA1\x29\x78\xA3\x9B\x76\xFE", 9);
}

#ifndef MINIMAL_MISC

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

void MISC_GUIUpdate (void)
{
    if (GUIUpdate)
    {
        GUIUpdate ();
    }
}

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

static UINT32 MISC_mul64 (UINT32 U32src_hi, UINT32 U32src_lo)
{
    asm
    {
        mov edx,[U32src_hi]
        mov eax,[U32src_lo] 
        mul edx 
        mov eax, edx
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

static UINT32 MISC_rdtsc (void)
{
    asm
    {
        db 0Fh;
        db 31h;
    }                           // in rdtsc edx:eax the value, we only need the low dword
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

void MISC_calibrate_delay (void)
{
    UINT32                        U32ticks, U32loopbit;
    SINT32                        S32lps_precision = LPS_PREC;

    U32loops_per_jiffy = (1 << 12);

    while (U32loops_per_jiffy <<= 1)
    {
        /*
         * wait for "start of" clock tick 
         */
        U32ticks = jiffies;
        while (U32ticks == jiffies)
            /*
             * nothing 
             */ ;

        /*
         * Go .. 
         */
        U32ticks = jiffies;
        __delay (U32loops_per_jiffy);
        U32ticks = jiffies - U32ticks;
        if (U32ticks)
        {
            break;
        }
    }

    /*
     * Do a binary approximation to get loops_per_jiffy set to equal one clock
     * (up to lps_precision bits) 
     */

    U32loops_per_jiffy >>= 1;
    U32loopbit = U32loops_per_jiffy;
    while (S32lps_precision-- && (U32loopbit >>= 1))
    {
        U32loops_per_jiffy |= U32loopbit;
        U32ticks = jiffies;
        while (U32ticks == jiffies)
            /*
             * nothing 
             */ ;

        U32ticks = jiffies;
        __delay (U32loops_per_jiffy);
        if (jiffies != U32ticks)    /* longer than 1 tick */
        {
            U32loops_per_jiffy &= ~U32loopbit;
        }
    }

    MDBGSTR2 ("MISC_calibrate_delay : %lu.%02lu BogoMIPS",
              U32loops_per_jiffy / (500000 / HZ),
              (U32loops_per_jiffy / (5000 / HZ)) % 100);
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

void MISC_outport (UINT16 U16port, SINT16 S16shift, UINT8 U8byte)
{
    asm
    {
        mov dx, U16port
        add dx, S16shift 
        mov al, U8byte
        out dx, al
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

UINT8 MISC_inport (UINT16 U16port, SINT16 S16shift)
{
    UINT8                         U8ret = 0;

    asm
    {
        mov dx, U16port
        add dx, S16shift 
        in al, dx 
        mov U8ret, al
    }

    return U8ret;
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

void MISC_delay (UINT32 x)
{
    UINT32                        __U32t0 =
        MISC_mul64 (U32loops_per_jiffy, x * 0x000010c6);
    __delay (__U32t0 * HZ);
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

UINT8 MISC_Calculate8BitChecksum (UINT8 * pAU8buffer, UINT16 U16length)
{
    UINT16                        i;
    UINT8                         U8chk;

    for (i = 0, U8chk = 0; i < U16length; i++, pAU8buffer++)
    {
        U8chk += (*pAU8buffer);
    }

    return (0xff - U8chk);
}
#endif
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

void MISC_CheckFileName (char *filename)
{
    char                          ext[16], timeanddate[64],
        newname[SYS_MAX_PATHNAME_LEN];
    SYSTEMTIME                    systime;
    char                         *pext;
    FILE                         *rfile;
    UINT16                        i;

    for (i = 0; i < (UINT16) strlen (filename); i++)
    {
        if (filename[i] < ' ' || filename[i] > 'z')
        {
            filename[i] = '0';
        }
    }

    rfile = fopen (filename, "rb");
    if (rfile == NULL)
    {
        return;
    }

    fclose (rfile);

    strcpy (newname, filename);
    GetLocalTime (&systime);
    sprintf (timeanddate, "-%04d.%02d.%02d-%02d.%02d.%02d", systime.wYear,
             systime.wMonth, systime.wDay, systime.wHour, systime.wMinute,
             systime.wSecond);

    if (strlen (newname) > 0)
    {
        pext = &newname[strlen (newname) - 1];
        while (*pext != '.' && pext > newname)
        {
            pext--;
        }

        if (*pext == '.')
        {
            strcpy (ext, pext);
            *pext = 0;
            strcat (newname, timeanddate);
            strcat (newname, ext);
        }
        else
        {
            strcat (newname, timeanddate);
        }
    }

    LDBGSTR2 ("MISC_CheckFileName : '%s' already exists, new filename is '%s'",
              filename, newname);
    strcpy (filename, newname);
}


#ifndef MINIMAL_MISC
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

BOOL MISC_ReadHeader (FILE * rfile, eFILE_TYPE * file_type,
                      UINT32 * U32startaddr)
{
    tFILE_HEADER                  header;
    UINT8                         U8read;
    UINT8                         U8checksum;

    memset (&header, 0, sizeof (header));

    fseek (rfile, 0L, SEEK_SET);
    U8read = fread (&header, sizeof (tFILE_HEADER), 1, rfile);
    if (U8read != 1)
    {
        HDBGSTR ("MISC_ReadHeader : invalid size of header");
        return FALSE;
    }

    if (header.U32id != FREIA_HEADER_ID)
    {
        HDBGSTR ("MISC_ReadHeader : invalid header id in header");
        return FALSE;
    }

    U8checksum =
        MISC_Calculate8BitChecksum ((UINT8 *) & header,
                                    sizeof (tFILE_HEADER) -
                                    sizeof (header.U32checksum));
    HDBGSTR2 ("MISC_ReadHeader : calculated checksum is %lX, original is %lX",
              U8checksum, header.U32checksum);
    if (U8checksum != (UINT8) header.U32checksum)
    {
        HDBGSTR ("MISC_ReadHeader : checksum is bad");
        return FALSE;
    }

    *U32startaddr = header.U32startaddr;
    *file_type = header.filetype;

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

BOOL MISC_WriteHeader (FILE * rfile, eFILE_TYPE file_type, UINT32 U32startaddr)
{
    tFILE_HEADER                  header;
    UINT8                         U8written;

    memset (&header, 0, sizeof (header));

    header.U32startaddr = U32startaddr;
    header.U32id = FREIA_HEADER_ID;
    header.filetype = file_type;
    /*
     * without checksum 
     */
    header.U32checksum =
        MISC_Calculate8BitChecksum ((UINT8 *) & header,
                                    sizeof (tFILE_HEADER) -
                                    sizeof (header.U32checksum));
    U8written = fwrite (&header, sizeof (tFILE_HEADER), 1, rfile);
    if (U8written != 1)
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

eMISC_MODEL_INDICES MISC_GetCurrentPhonemodel (void)
{
    return misc_config.phonemodel;
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

BOOL MISC_SetEmulation (char *filename, BOOL Bbinaryemulate)
{
    UINT8                         j;
    UINT32                        U32extid, U32len;
    FILE                         *rfile;
    UINT32                        U32storedstartaddr;
    eFILE_TYPE                    file_type;
    BOOL                          Bsuccess;
    UINT32                        U32mcustart;

    if (filename == NULL || strlen (filename) == 0)
    {
        misc_config.Bemulate_from_file = FALSE;
        return TRUE;
    }

    HDBGSTR2 ("MISC_SetEmulation : trying to set emulation for %s as %s",
              filename, (Bbinaryemulate ? "FLS" : "KSI"));

    for (j = 0; j < (UINT8) strlen (filename); j++)
        filename[j] = toupper (filename[j]);

    if (!Bbinaryemulate)
    {
        memcpy (&U32extid, &filename[strlen (filename) - sizeof (U32extid)],
                sizeof (U32extid));
        if (U32extid != KSI_EXTENSION_ID)
        {
            freia_errno = KE_INVALID_FILE_FORMAT;
            return FALSE;
        }
    }

    MDBGSTR1 ("MISC_SetEmulation : setting emulation from %s", filename);

    strncpy (misc_config.emulatefile, filename, SYS_MAX_PATHNAME_LEN);
    rfile = fopen (filename, "rb");
    if (!rfile)
    {
        MDBGSTR1 ("MISC_SetEmulation : cannot open %s", filename);
        freia_errno = KE_FILE_NOT_FOUND;
        return FALSE;
    }

    if (!MISC_GetMCUStartAddress (&U32mcustart))
    {
        LDBGSTR ("MISC_SetEmulation : cannot find MCU start address");
        return FALSE;
    }

    if (!Bbinaryemulate)
    {
        Bsuccess = MISC_ReadHeader (rfile, &file_type, &U32storedstartaddr);

        if (!Bsuccess || file_type != FILE_TYPE_FULL
            || U32storedstartaddr != U32mcustart)
        {
            fclose (rfile);
            freia_errno = KE_INVALID_FILEHEADER;
            return FALSE;
        }
    }

    U32len = MISC_GetFileSize (rfile);

    if (!Bbinaryemulate)
    {
        U32len -= sizeof (tFILE_HEADER);
    }

    misc_config.U32flashendaddr = U32mcustart + U32len;
    HDBGSTR1 ("MISC_SetEmulation : end of flash is set to " UINT32_FORMAT,
              misc_config.U32flashendaddr);
    fclose (rfile);

    misc_config.Bemulate_from_file = TRUE;
    misc_config.Bbinary_emulate = Bbinaryemulate;

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

BOOL MISC_OpenEmulatedFlashFile (FILE ** rfile, char *mode, UINT32 U32startaddr)
{
    fpos_t                        filepos;
    UINT32                        U32seekaddr = U32startaddr;
    UINT32                        U32mcustart;

    if (!MISC_GetMCUStartAddress (&U32mcustart))
    {
        LDBGSTR ("MISC_OpenEmulatedFlashFile : cannot find MCU start address");
        return FALSE;
    }

    U32seekaddr -= U32mcustart;

    (*rfile) = fopen (misc_config.emulatefile, mode);
    if (!(*rfile))
    {
        HDBGSTR1 ("MISC_OpenEmulatedFlashFile : cannot open emulation file %s",
                  misc_config.emulatefile);
        return FALSE;
    }

    HDBGSTR1 ("MISC_OpenEmulatedFlashFile :  binary emulation is %s",
              (misc_config.Bbinary_emulate ? "on" : "off"));

    /*
     * must seek from current (after header) 
     */
    if (!misc_config.Bbinary_emulate)
        U32seekaddr += sizeof (tFILE_HEADER);

    HDBGSTR1 ("MISC_OpenEmulatedFlashFile : seek address is %d", U32seekaddr);
    fseek (*rfile, U32seekaddr, SEEK_CUR);

    /*
     * save the file pointer position 
     */
    fgetpos (*rfile, &filepos);

    if ((UINT32) filepos != U32seekaddr)
    {
        HDBGSTR ("MISC_OpenEmulatedFlashFile : seek error in emulation file");
        freia_errno = KE_SEEK_ERROR;
        fclose (*rfile);
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

void MISC_AdjustAddresses (UINT32 * U32startaddr, UINT32 * U32endaddr)
{
    if (*U32startaddr > misc_config.U32flashendaddr)
        *U32startaddr = misc_config.U32flashendaddr;

    if (*U32endaddr > misc_config.U32flashendaddr)
        *U32endaddr = misc_config.U32flashendaddr;
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

void MISC_GaugeInit (UINT32 U32start, UINT32 U32end)
{
    if (GaugeInit)
    {
        GaugeInit (U32start, U32end);
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

void MISC_GaugeUpdate (UINT32 U32addr)
{
    if (GaugeUpdate)
    {
        GaugeUpdate (U32addr);
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

void MISC_GaugeDone (void)
{
    if (GaugeDone)
    {
        GaugeDone ();
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

void MISC_SetErrorCode (eERROR errcode)
{
    freia_errno = errcode;
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

eERROR MISC_GetErrorCode (void)
{
    return freia_errno;
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

UINT32 MISC_GetBuild (void)
{
    return FREIA_BUILD;
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

BOOL MISC_GetEEPROMAddress (UINT8 U8eepromidx,
                            UINT32 * pU32eepstart, UINT32 * pU32eepsize)
{
    UINT16                        i;

    if (U8eepromidx > MISC_MAX_EEPROM_BLOCKS)
    {
        return FALSE;
    }

    for (i = 0; i < MISC_MAX_NUM_OF_MODELS; i++)
    {
        if (misc_models[i].uniqueid == misc_config.phonemodel)
        {
            *pU32eepstart = misc_models[i].eeprom[U8eepromidx].U32start;
            *pU32eepsize = misc_models[i].eeprom[U8eepromidx].U32size;
            return TRUE;
        }
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

UINT32 MISC_GetBootcorePasswordOffset (void)
{
    switch (misc_config.phonemodel)
    {
    default:
        return 0x30;

    case MISC_MODEL_INDEX_A50:
    case MISC_MODEL_INDEX_1168:
        return 0x2E;
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

BOOL MISC_GetMCUStartAddress (UINT32 * pU32mcustart)
{
    UINT16                        i;

    for (i = 0; i < MISC_MAX_NUM_OF_MODELS; i++)
    {
        if (misc_models[i].uniqueid == misc_config.phonemodel)
        {
            *pU32mcustart = misc_models[i].U32mcustartaddress;
            return TRUE;
        }
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

tFREIA_PHONEINFO             *MISC_GetPhoneInfoPtr (void)
{
    return &freia_phoneinfo;
}


static UINT8                  BigBuffer[16384]; // DPP all, UINT16's

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

static UINT32 FileSize (FILE * stream)
{
    UINT32                        curpos, length;

    curpos = ftell (stream);
    fseek (stream, 0L, SEEK_END);
    length = ftell (stream);
    fseek (stream, curpos, SEEK_SET);
    return length;
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

static void GotoDataAddr (BOOL B6megs, UINT16 DPP, UINT16 Offs, FILE * fflash)
{
    UINT32                        offset;

    Offs = Offs & 0x3FFF;
    offset = (UINT32) (DPP) * (UINT32) (16384) + (UINT32) (Offs);

    if (B6megs)
    {
        if (offset >= 0xA00000)
        {
            offset -= 0xA00000; // Flash Image Mapping!
        }
    }
    else
    {
        if (offset >= 0xC00000)
        {
            offset -= 0xC00000; // Flash Image Mapping!
        }
        else
        {
            if (offset >= 0x800000)
            {
                offset -= 0x800000; // Flash Image Mapping!
            }
            else
            {
                if (offset >= 0x400000)
                {
                    offset -= 0x400000; // Flash Image Mapping!
                }
            }
        }
    }

    fseek (fflash, offset, SEEK_SET);
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

static UINT16 GetWordAtAddr (BOOL B6megs, UINT16 DPP, UINT16 Offs,
                             FILE * fflash)
{
    UINT16                        buffer = 0;

    GotoDataAddr (B6megs, DPP, Offs, fflash);
    fread (&buffer, 1, 2, fflash);
    return buffer;
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

static UINT8 GetByteAtAddr (BOOL B6megs, UINT16 DPP, UINT16 Offs, FILE * fflash)
{
    UINT8                         buffer = 0;

    GotoDataAddr (B6megs, DPP, Offs, fflash);
    fread (&buffer, 1, 1, fflash);
    return buffer;
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

static void PutWordAtAddr (BOOL B6megs, UINT16 DPP, UINT16 Offs, UINT16 Data,
                           FILE * fflash)
{
    GotoDataAddr (B6megs, DPP, Offs, fflash);
    fwrite (&Data, 1, 2, fflash);
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

static void BigAtAddr (BOOL B6megs, UINT16 DPP, FILE * fflash)
{
    GotoDataAddr (B6megs, DPP, 0, fflash);
    fread (&BigBuffer, 16384, 1, fflash);
}

static UINT16                 R1 = 0, R2 = 0, R4 = 0, R5 = 0;
static UINT16                 R6 = 0, R7 = 0, R8 = 0, R9 = 0, R10 = 0;
static UINT16                 R12 = 0, R13 = 0, R14 = 0, R15 = 0;


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

static void Xorka (void)
{
    UINT16                        T;

    while (R1 != R10)
    {
        T = *(UINT16 *) (&BigBuffer[R1]);   // DPP0 = R13 - all in BigBuffer
        T = T & 0xFFFF;
        R4 = R4 ^ T;
        R5 += T;
        R4 = R4 & 0xFFFF;
        R5 = R5 & 0xFFFF;
        R1 += 2;
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

static void CRCCalculator (BOOL B6megs, FILE * fflash)
{
    R4 = 0;
    R5 = 0;
    R1 = R12;

    while (R15 > R13)
    {                           // Check Addr R13[R12] -> R15[R14]
        R10 = 0x4000;

        BigAtAddr (B6megs, R13, fflash);
        Xorka ();

        R1 = 0;
        R13 += 1;
    }

    BigAtAddr (B6megs, R13, fflash);

    R10 = R14;
    Xorka ();
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

static void CRCOther4Bytes (BOOL B6megs, FILE * fflash)
{
    UINT16                        T;

    R2 = R12;
    R4 = R14;
    R5 = R15;
    T = GetWordAtAddr (B6megs, R13, R2, fflash);
    R4 = R4 ^ T;
    R5 -= T;
    R2 += 2;

    R4 = R4 & 0xFFFF;
    R5 = R5 & 0xFFFF;

    T = GetWordAtAddr (B6megs, R13, R2, fflash);
    R4 = R4 ^ T;
    R5 -= T;
    R2 += 2;
    R5 -= 2;

    R4 = R4 & 0xFFFF;
    R5 = R5 & 0xFFFF;
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

BOOL MISC_CRCCheck (BOOL Bfix, char *flashfile)
{
    UINT16                        U16crclo, U16crchi;
    UINT8                         T;
    FILE                         *fflash;
    UINT32                        U32originalflashsize;
    UINT32                        U32MCUstartaddress;
    BOOL                          B6megs;
    UINT32                        U32flashsize;

    if (misc_config.phonemodel != MISC_MODEL_INDEX_C35 &&
        misc_config.phonemodel != MISC_MODEL_INDEX_C35_NEW &&
        misc_config.phonemodel != MISC_MODEL_INDEX_MT50 &&
        misc_config.phonemodel != MISC_MODEL_INDEX_C45 &&
        misc_config.phonemodel != MISC_MODEL_INDEX_S45 &&
        misc_config.phonemodel != MISC_MODEL_INDEX_SL45)
    {
        LDBGSTR ("MISC_CRCCheck : the selected model is not supported");
        return FALSE;
    }

    if (!MISC_GetMCUStartAddress (&U32MCUstartaddress))
    {
        LDBGSTR ("MISC_CRCCheck : cannot determine size of software");
        return FALSE;
    }

    U32flashsize = 0x1000000 - U32MCUstartaddress;

    B6megs = U32flashsize == 6 * 1024 * 1024;

    if ((fflash = fopen (flashfile, "rb+")) == NULL)
    {
        LDBGSTR1 ("MISC_CRCCheck : cannot open '%s'", flashfile);
        return FALSE;
    }

    U32originalflashsize = FileSize (fflash);
    if (U32originalflashsize != U32flashsize)
    {
        LDBGSTR2 ("MISC_CRCCheck : invalid filesize (%d/%d)",
                  U32originalflashsize, U32flashsize);
        fclose (fflash);
        return FALSE;
    }

    if (B6megs)
    {
        R8 = 0;
        R9 = 0;
        R13 = 0x300;
        R12 = 0x204;
        R15 = 0x3FB;
        R14 = 0x3FFE;
        CRCCalculator (B6megs, fflash);
        R8 = R8 ^ R4;
        R9 += R5;
        R8 = R8 & 0xFFFF;
        R9 = R9 & 0xFFFF;
        R4 = ROL (R4, 2);
        R5 = ROR (R5, 3);
        R4 = R4 & 0xFFFF;
        R5 = R5 & 0xFFFF;
        U16crchi = GetWordAtAddr (B6megs, 0x300, 0x200, fflash);
        U16crclo = GetWordAtAddr (B6megs, 0x300, 0x202, fflash);
        if (R4 != U16crchi || R5 != U16crclo)
        {
            LDBGSTR4 ("MISC_CRCCheck : CRC differs (%04X%04X/%04X%04X)", R4,
                      R5, U16crclo, U16crchi);
            if (Bfix)
            {
                LDBGSTR ("MISC_CRCCheck : fixing invalid CRC");
                PutWordAtAddr (B6megs, 0x300, 0x200, R4, fflash);
                PutWordAtAddr (B6megs, 0x300, 0x202, R5, fflash);
            }
        }
    }
    else
    {
        R7 = 0x21F;
        R6 = 0x3E80;
        R8 = 0;
        R9 = 0;
        T = GetByteAtAddr (B6megs, 0x1F, 0x3F50, fflash);
        if (T < 0x20)
        {
            fclose (fflash);
            return FALSE;
        }

        while (TRUE)
        {                       // CRCLoop
            R12 = GetWordAtAddr (B6megs, R7, R6, fflash);
            R6 += 2;
            R13 = GetWordAtAddr (B6megs, R7, R6, fflash);
            R6 += 2;
            R14 = GetWordAtAddr (B6megs, R7, R6, fflash);
            R6 += 2;
            R15 = GetWordAtAddr (B6megs, R7, R6, fflash);
            R6 += 2;
            if ((R12 == 0xFFFF) && (R13 == 0xFFFF) && (R14 == 0xFFFF)
                && (R15 == 0xFFFF))
                break;          //EOT
            R2 = R12;
            R13 = R13 << 2;
            R12 = R12 & 0x3FFF;
            R2 = R2 >> 14;
            R13 = R13 | R2;
            R2 = R14;
            R15 = R15 << 2;
            R14 = R14 & 0x3FFF;
            R2 = R2 >> 14;
            R15 = R15 | R2;
            R13 += 0x300;
            R15 += 0x300;
            CRCCalculator (B6megs, fflash);
            R8 = R8 ^ R4;
            R9 += R5;
            R8 = R8 & 0xFFFF;
            R9 = R9 & 0xFFFF;
        }

        R12 = 0x3FFC;
        R13 = 0x21F;
        R14 = R8;
        R15 = R9;
        CRCOther4Bytes (B6megs, fflash);

        R12 = 0x3FFC;
        R13 = 0x3DB;
        R14 = R4;
        R15 = R5;
        CRCOther4Bytes (B6megs, fflash);

        R4 = R4 & 0xFFFF;
        R5 = R5 & 0xFFFF;
        R4 = ROL (R4, 2);
        R5 = ROR (R5, 3);
        R4 = R4 & 0xFFFF;
        R5 = R5 & 0xFFFF;

        U16crchi = GetWordAtAddr (B6megs, 0x3DB, 0x3FFC, fflash);
        U16crclo = GetWordAtAddr (B6megs, 0x3DB, 0x3FFE, fflash);
        if (R4 != U16crchi || R5 != U16crclo)
        {
            LDBGSTR4 ("MISC_CRCCheck : CRC differs (%04X%04X/%04X%04X)", R4,
                      R5, U16crclo, U16crchi);
            if (Bfix)
            {
                LDBGSTR ("MISC_CRCCheck : fixing invalid CRC");
                PutWordAtAddr (B6megs, 0x3DB, 0x3FFC, R4, fflash);
                PutWordAtAddr (B6megs, 0x3DB, 0x3FFE, R5, fflash);
            }
        }
    }

    fclose (fflash);
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

void MISC_SetDngCOMPort (UINT8 U8newcomport)
{
    U8comport_dng = U8newcomport;
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

UINT8 MISC_GetDngCOMPort (void)
{
    return U8comport_dng;
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

void MISC_GetOriginalIMEI (UINT8 * pAU8dst)
{
    memcpy (pAU8dst, freia_phoneinfo.originalB5009, FREIA_IMEI_LEN / 2 + 1);
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

static UINT16 MISC_IntToHex (UINT16 U16src)
{
    UINT16                        U16lo, U16hi;

    U16lo = U16src % 10;
    U16hi = U16src / 10;
    return U16hi * 16 + U16lo;
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

void MISC_ConvertProvider (UINT32 U32provider, char *provider)
{
    provider[0] = SWAPBYTE (MISC_IntToHex (U32provider / 1000));
    provider[1] = (U32provider % 1000) / 100;
    provider[2] = SWAPBYTE (MISC_IntToHex (U32provider % 100));

    provider[1] |= 0xF0;
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

BOOL MISC_PossibleToUpdateOriginalIMEI (void)
{
    /*
     * 0th bit is factory lock status, 
     * 1st bit is user lock status 
     */

    return (freia_phoneinfo.U16lockstatus & 0x02);
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

void MISC_SetBootType (MISC_BOOTTYPE value)
{
    boottype = value;
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

MISC_BOOTTYPE MISC_GetBootType (void)
{
    UINT32                        U32corestart, U32coresize;

    if (!MISC_GetBootcoreAddress (&U32corestart, &U32coresize))
    {
        return MISC_BOOTTYPE_NORMAL;
    }

    if (U32coresize == 0)
    {
        return MISC_BOOTTYPE_NORMAL;
    }

    return boottype;
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

BOOL MISC_GetBootcoreAddress (UINT32 * pU32corestart, UINT32 * pU32coresize)
{
    UINT16                        i;

    for (i = 0; i < MISC_MAX_NUM_OF_MODELS; i++)
    {
        if (misc_models[i].uniqueid == misc_config.phonemodel)
        {
            *pU32corestart = misc_models[i].bootcore.U32start;
            *pU32coresize = misc_models[i].bootcore.U32size;
            return TRUE;
        }
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

BOOL MISC_C30Like (eMISC_MODEL_INDICES model)
{
    return (model == MISC_MODEL_INDEX_C30 || model == MISC_MODEL_INDEX_S40);
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

BOOL MISC_GetPhoneModelName (char *modelname)
{
    UINT16                        i;

    for (i = 0; i < MISC_MAX_NUM_OF_MODELS; i++)
    {
        if (misc_models[i].uniqueid == misc_config.phonemodel)
        {
            strcpy (modelname, misc_models[i].modelname);
            return TRUE;
        }
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

BOOL MISC_GetEmulation (void)
{
    return misc_config.Bemulate_from_file;
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

BOOL MISC_GetAppCOMDTR (void)
{
    return Bcomport_app_dtr;
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

BOOL MISC_GetAppCOMRTS (void)
{
    return Bcomport_app_rts;
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

UINT8 MISC_GetAppCOMPort (void)
{
    return U8comport_app;
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

BOOL MISC_GetPhoneModel (char *model, tMISC_PHONE_MODEL ** pmodel)
{
    UINT8                         i;

    *pmodel = NULL;

    if (model == NULL)
    {
        return FALSE;
    }

    if (strlen (model) == 0)
    {
        return FALSE;
    }

    for (i = 0; i < (UINT8) strlen (model); i++)
    {
        model[i] = toupper (model[i]);
    }

    for (i = 0; i < MISC_MAX_NUM_OF_MODELS; i++)
    {
        if (strcmp (model, misc_models[i].modelname) == 0)
        {
            *pmodel = &misc_models[i];
            return TRUE;

        }
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

void MISC_SetCurrentPhonemodel (eMISC_MODEL_INDICES phonemodel)
{
    misc_config.phonemodel = phonemodel;
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

void MISC_SetGauge (pGaugeInit Init, pGaugeUpdate Update, pGaugeDone Done)
{
    GaugeInit = Init;
    GaugeUpdate = Update;
    GaugeDone = Done;
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

void MISC_SetGUIUpdate (pGUIUpdate guiupdate)
{
    GUIUpdate = guiupdate;
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

void MISC_SetDebugLevel (eDEBUG_LEVEL level)
{
    if (level > DEBUG_LEVEL_HIGH)
        level = DEBUG_LEVEL_HIGH;

    if (level < DEBUG_LEVEL_NONE)
        level = DEBUG_LEVEL_NONE;

    debug_level = level;
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

void MISC_SetRedirection (eREDIRECTION_TYPE redirecttype)
{
    eRedirection = redirecttype;
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

void MISC_SetAppCOMPort (UINT8 U8newcomport)
{
    U8comport_app = U8newcomport;
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

void MISC_SetAppCOMType (BOOL Bdtr, BOOL Brts)
{
    Bcomport_app_dtr = Bdtr;
    Bcomport_app_rts = Brts;
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

UINT32 MISC_SwapDword (UINT32 U32dword)
{
    UINT16                        U16hiword, U16loword;

    U16hiword = SWAPWORD (HIWORD (U32dword));
    U16loword = SWAPWORD (LOWORD (U32dword));

    return (((UINT32) (U16loword)) << 16) | U16hiword;
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

const tMISC_PHONE_MODEL      *MISC_GetModel (UINT8 U8idx)
{
    if (U8idx >= MISC_MAX_NUM_OF_MODELS)
    {
        return NULL;
    }

    return &misc_models[U8idx];
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

BOOL MISC_ReadHexBlock (FILE * rfile, UINT16 * pU16len, UINT8 * pAU8buffer)
{
    UINT16                        i;
    char                          line[SYS_MAX_STRING_LEN], *pline = line;
    char                          token[8];
    char                         *pchr1, *pchr2;
    BOOL                          Bnextline, Bfinished;

    if (*pU16len > 0)
    {
        memset (pAU8buffer, 0, *pU16len);
    }

    if (fgets (line, sizeof (line), rfile) == NULL)
    {
        return FALSE;
    }

    for (Bfinished = FALSE, i = 0; !Bfinished; i++)
    {
        if (!MISC_GetNextToken (&pline, ' ', token))
        {
            Bnextline = TRUE;
        }
        else
        {
            Bnextline = (token[0] == '\n');
        }

        if (Bnextline)
        {
            if (fgets (line, sizeof (line), rfile) == NULL)
            {
                /*
                 * eof 
                 */
                if (*pU16len == 0)
                {
                    *pU16len = i;
                    return TRUE;
                }

                return FALSE;
            }

            pline = line;

            if (!MISC_GetNextToken (&pline, ' ', token))
            {
                return FALSE;
            }
        }

        pchr1 = strstr (token, "0X");
        pchr2 = strstr (token, "0x");

        if (pchr1 == NULL && pchr2 == NULL)
        {
            return FALSE;
        }

        if (pchr1 == NULL)
        {
            pchr1 = pchr2;
        }

        pchr1[4] = '\0';

        pAU8buffer[i] = MISC_HexToInt (pchr1 + 2);

        Bfinished = (*pU16len > 0 ? i >= (*pU16len - 1) : FALSE);
    }

    return TRUE;
}

#endif
