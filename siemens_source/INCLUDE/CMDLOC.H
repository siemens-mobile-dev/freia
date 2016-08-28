
/********************************************************************
* NAME: cmdhcloc.h                                                  *
*                                                                   *
* PURPOSE:                                                          *
*                                                                   *
* locally used type definitions, defines for the commandline        *
* handler                                                           *
*                                                                   *
* GLOBAL VARIABLES:                                                 *
*                                                                   *
* Variable:                                                         *
*                                                                   *
* Type:                                                             *
*                                                                   *
* Description:                                                      *
*                                                                   *
* DEVELOPMENT HISTORY:                                              *
*                                                                   *
* Date: 21. May, 2002                                               *
*                                                                   *
* Author: THI                                                       *
*                                                                   *
* Change Id:                                                        *
*                                                                   *
* Release:                                                          *
*                                                                   *
* Description of change:                                            *
*                                                                   *
*********************************************************************/

#ifndef __cmdhcloc_h__
#define __cmdhcloc_h__

typedef enum
{
    CMD_NONE = -1,
    CMD_SETREDIRECTION = 1,
    CMD_SETDEBUGLEVEL,
    CMD_COMAPP,
    CMD_COMAPPTYPE,
    CMD_COMDNG,
    CMD_SPEED,
    CMD_EMULATE,
    CMD_RAWEMULATE,
    CMD_NOMAP,
    CMD_NOUPDATE,
    CMD_BACKUP,
    CMD_UNLOCK,
    CMD_UNLOCK_FROM_LOG,
    CMD_WRITEMAP,
    CMD_READ,
    CMD_RAWREAD,
    CMD_WRITE,
    CMD_RAWWRITE,
    CMD_DECRYPT,
    CMD_DECRYPT_FROM_MAP,
    CMD_LOCKTOPROVIDER,
    CMD_CHECK_CHECKSUMS,
    CMD_FIX_CHECKSUMS,
    CMD_BACKUPBATTERY,
    CMD_SETBATTERY,
    CMD_READMEMORY,
    CMD_RAWREADMEMORY,
    CMD_WRITELOG,
    CMD_UPDATEFLASHIMEI,
    CMD_UNLOCK_FROM_MASS_OF_LOG,
    CMD_BOOTTYPE,
    CMD_PATCHBOOTCORE,
    CMD_GETDEVINFO,
    CMD_SETDEVINFO,
    CMD_UPDATEDEV,
    CMD_TESTDEV,
    CMD_CLOSEATONCE,
}
eCMDLINE_COMMAND;

/* always the last command */
#define CMDLINE_MAX_COMMANDS CMD_CLOSEATONCE

typedef enum
{
    FCAT_NONE = 0,
    FCAT_NUMBER,
    FCAT_STRING
}
eCMDLINE_COMMAND_ARGUMENT_TYPE;

#define CMDLINE_MAX_LONGCMD_LEN 32
#define CMDLINE_MAX_SHORTCMD_LEN 8
#define CMDLINE_MAX_CMD_ARGS 16
#define CMDLINE_MAX_HELP_LEN 64

typedef struct
{
    char                          longcmd_str[CMDLINE_MAX_LONGCMD_LEN];
    char                          shortcmd_str[CMDLINE_MAX_SHORTCMD_LEN];
    eCMDLINE_COMMAND              cmd;
    eCMDLINE_COMMAND_ARGUMENT_TYPE argtype[CMDLINE_MAX_CMD_ARGS];
    UINT8                         U8num_of_args;
    BOOL                          Boptionalargs;
    char                          help[CMDLINE_MAX_HELP_LEN];
    BOOL                          Bboots;
}
tCMDLINE_CMDBLOCK;

static const UINT8            U8num_of_commcmds = CMDLINE_MAX_COMMANDS;
static const tCMDLINE_CMDBLOCK commcmd[] = {
    {
     "setredirection",
     "sr",
     CMD_SETREDIRECTION,
     {FCAT_NUMBER},
     1,
     FALSE,
     "sets redirection",
     FALSE},
    {
     "setdebuglevel",
     "sd",
     CMD_SETDEBUGLEVEL,
     {FCAT_NUMBER},
     1,
     FALSE,
     "sets debuglevel",
     FALSE},
    {
     "comapp",
     "ca",
     CMD_COMAPP,
     {FCAT_NUMBER},
     1,
     FALSE,
     "sets comport of cable",
     FALSE},
    {
     "comapptype",
     "cat",
     CMD_COMAPPTYPE,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "sets type of comport of cable",
     FALSE},
    {
     "comdng",
     "cd",
     CMD_COMDNG,
     {FCAT_NUMBER},
     1,
     FALSE,
     "sets comport of dongle",
     FALSE},
    {
     "speed",
     "s",
     CMD_SPEED,
     {FCAT_NUMBER},
     1,
     FALSE,
     "overrides default speed of comport",
     FALSE},
    {
     "emulate",
     "e",
     CMD_EMULATE,
     {FCAT_STRING},
     1,
     FALSE,
     "turns on emulation from KSI file",
     FALSE},
    {
     "rawemulate",
     "re",
     CMD_RAWEMULATE,
     {FCAT_STRING},
     1,
     FALSE,
     "turns on emulation from FLS file",
     FALSE},
    {
     "nomap",
     "nm",
     CMD_NOMAP,
     {FCAT_NONE},
     0,
     FALSE,
     "disables map saving during unlock",
     FALSE},
    {
     "noupdate",
     "nu",
     CMD_NOUPDATE,
     {FCAT_NONE},
     0,
     FALSE,
     "disables eeprom update during unlock",
     FALSE},
    {
     "backup",
     "b",
     CMD_BACKUP,
     {FCAT_NONE},
     0,
     FALSE,
     "backups maps of phone",
     TRUE},
    {
     "unlock",
     "u",
     CMD_UNLOCK,
     {FCAT_STRING},
     1,
     FALSE,
     "unlocks phone",
     TRUE},
    {
     "unlockfromlog",
     "ul",
     CMD_UNLOCK_FROM_LOG,
     {FCAT_STRING},
     1,
     FALSE,
     "unlocks phone from a log file",
     FALSE},
    {
     "writemap",
     "wm",
     CMD_WRITEMAP,
     {FCAT_STRING},
     1,
     FALSE,
     "writes map to phone",
     TRUE},
    {
     "read",
     "r",
     CMD_READ,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "reads flash and saves it as KSI",
     TRUE},
    {
     "rawread",
     "rr",
     CMD_RAWREAD,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "reads flash and saves it as FLS",
     TRUE},
    {
     "write",
     "w",
     CMD_WRITE,
     {FCAT_STRING, FCAT_NUMBER, FCAT_NUMBER},
     3,
     FALSE,
     "writes to phone from KSI file",
     TRUE},
    {
     "rawwrite",
     "rw",
     CMD_RAWWRITE,
     {FCAT_STRING, FCAT_NUMBER, FCAT_NUMBER, FCAT_NUMBER},
     4,
     FALSE,
     "writes to phone from FLS file",
     TRUE},
    {
     "decrypt",
     "d",
     CMD_DECRYPT,
     {FCAT_NONE},
     0,
     FALSE,
     "decrypt EEPROM blocks",
     TRUE},
    {
     "decryptfrommap",
     "dm",
     CMD_DECRYPT_FROM_MAP,
     {FCAT_STRING},
     1,
     FALSE,
     "decrypts EEPROM blocks from a map file",
     FALSE},
    {
     "locktoprovider",
     "ltp",
     CMD_LOCKTOPROVIDER,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "sets provider's code",
     FALSE},
    {
     "checkchecksums",
     "cc",
     CMD_CHECK_CHECKSUMS,
     {FCAT_STRING},
     1,
     FALSE,
     "checks checksums",
     FALSE},
    {
     "fixchecksums",
     "fc",
     CMD_FIX_CHECKSUMS,
     {FCAT_STRING},
     1,
     FALSE,
     "fixes checksums",
     FALSE},
    {
     "backupbattery",
     "bb",
     CMD_BACKUPBATTERY,
     {FCAT_NONE},
     0,
     FALSE,
     "backups phone's battery parameters",
     TRUE},
    {
     "setbattery",
     "sb",
     CMD_SETBATTERY,
     {FCAT_STRING},
     1,
     FALSE,
     "restores phone's battery parameters",
     TRUE},
    {
     "read",
     "rm",
     CMD_READMEMORY,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "reads memory and saves it as KSI",
     FALSE},                    /* doesn't need boot, since it uses BFB mode */
    {
     "rawread",
     "rrm",
     CMD_RAWREADMEMORY,
     {FCAT_NUMBER, FCAT_NUMBER},
     2,
     FALSE,
     "reads memory and saves it as FLS",
     FALSE},                    /* doesn't need boot, since it uses BFB mode */
    {
     "writelog",
     "wl",
     CMD_WRITELOG,
     {FCAT_NONE},
     0,
     FALSE,
     "writes log of phone",
     TRUE},
    {
     "updateflashimei",
     "ufi",
     CMD_UPDATEFLASHIMEI,
     {FCAT_NONE},
     0,
     FALSE,
     "updates original IMEI during unlock (if possible)",
     FALSE},
    {
     "unlockfrommassoflog",
     "uml",
     CMD_UNLOCK_FROM_MASS_OF_LOG,
     {FCAT_STRING},
     1,
     FALSE,
     "generates mass of unlock maps",
     FALSE},
    {
     "boottype",
     "bt",
     CMD_BOOTTYPE,
     {FCAT_NUMBER},
     1,
     FALSE,
     "sets booting type",
     FALSE},
    {
     "patchbootcore",
     "pb",
     CMD_PATCHBOOTCORE,
     {FCAT_STRING, FCAT_NUMBER},
     2,
     TRUE,
     "patches bootcore",
     FALSE},
    {
     "getdevinfo",
     "gdi",
     CMD_GETDEVINFO,
     {FCAT_NONE},
     0,
     FALSE,
     "gets dongle info",
     FALSE},
    {
     "setdevinfo",
     "sdi",
     CMD_SETDEVINFO,
     {FCAT_STRING},
     1,
     FALSE,
     "sets dongle info",
     FALSE},
    {
     "updatedev",
     "ud",
     CMD_UPDATEDEV,
     {FCAT_STRING},
     1,
     FALSE,
     "updates dongle",
     FALSE},
    {
     "testdev",
     "td",
     CMD_TESTDEV,
     {FCAT_NONE},
     0,
     FALSE,
     "tests dongle",
     FALSE},
    {
     "closeatonce",
     "cao",
     CMD_CLOSEATONCE,
     {FCAT_NONE},
     0,
     FALSE,
     "closes console application at once",
     FALSE},
};

#define CMDLINE_MAX_ARGLEN SYS_MAX_PATHNAME_LEN

typedef struct
{
    char                          value[CMDLINE_MAX_ARGLEN];
}
tCMDLINE_STRINGARG;

typedef struct
{
    UINT8                         U8num_of_args;
    UINT16                        U16cmdidx;
    char                          longcmd_str[CMDLINE_MAX_LONGCMD_LEN];
    tCMDLINE_STRINGARG            arg[CMDLINE_MAX_CMD_ARGS];
}
tCMDLINE_USED_COMMAND;

typedef struct
{
    UINT8                         U8num_of_commands;
    tCMDLINE_USED_COMMAND         command[CMDLINE_MAX_COMMANDS];
}
tCMDLINE_USED_COMMANDS;

#define CMDLINE_MAX_EXPANDED_NUMBERS 32

/* If i give an arg like,
  0..6,9..10,12 it will mean

  0,1,2,3,4,5,6,9,10,12
  so the expanded numbers mean that
*/

#define CMDLINE_MAX_STRINGS 3

typedef struct
{
    eCMDLINE_COMMAND              cmd;
    char                          longcmd_str[CMDLINE_MAX_LONGCMD_LEN];
    UINT8                         U8num_of_stringargs;
    tCMDLINE_STRINGARG            stringarg[CMDLINE_MAX_STRINGS];   /* if the arg is a string */
    UINT8                         U8num_of_numargs;
    UINT32                        U32numarg[CMDLINE_MAX_EXPANDED_NUMBERS];
    BOOL                          Bboots;
}
tCMDLINE_RUNTIME_COMMAND;

typedef struct
{
    UINT8                         U8num_of_commands;
    tCMDLINE_RUNTIME_COMMAND      command[CMDLINE_MAX_COMMANDS];
}
tCMDLINE_RUNTIME_COMMANDS;


#define INFOSTR(x) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x); mylog(errbuff,FALSE,FALSE);}
#define INFOSTR1(x,p1) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1); mylog(errbuff,FALSE,FALSE);}
#define INFOSTR2(x,p1,p2) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2); mylog(errbuff,FALSE,FALSE);}
#define INFOSTR3(x,p1,p2,p3) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2,p3); mylog(errbuff,FALSE,FALSE);}
#define INFOSTR4(x,p1,p2,p3,p4) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2,p3,p4); mylog(errbuff,FALSE,FALSE);}

#define ERRORSTR(x) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x); mylog(errbuff,TRUE,TRUE);}
#define ERRORSTR1(x,p1) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1); mylog(errbuff,TRUE,TRUE);}
#define ERRORSTR2(x,p1,p2) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2); mylog(errbuff,TRUE,TRUE);}
#define ERRORSTR3(x,p1,p2,p3) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2,p3); mylog(errbuff,TRUE,TRUE);}
#define ERRORSTR4(x,p1,p2,p3,p4) {memset(errbuff,0,sizeof(errbuff)); sprintf(errbuff,x,p1,p2,p3,p4); mylog(errbuff,TRUE,TRUE);}

#endif
