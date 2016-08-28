
/********************************************************************
* NAME:                                                             *
*                                                                   *
* PURPOSE:                                                          *
*                                                                   *
* GLOBAL VARIABLES:                                                 *
*                                                                   *
* Variable:                                                         *
*                                                                   *
* Type:                                                             *
*                                                                   *
* Description:                                                      *
*                                                                   *
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
*********************************************************************/

#ifndef __misc_pub_h
#define __misc_pub_h

#ifdef __FROM_CPP__
#define __IMPORT_TYPE extern "C"
#else
#define __IMPORT_TYPE
#endif

#if defined(BUILD_DLL)
#define DLL_EXP __declspec(dllexport)
#else
#if defined(BUILD_APP)
#define DLL_EXP __declspec(dllimport)
#else
#define DLL_EXP
#endif
#endif

typedef enum
{
  MISC_BOOTTYPE_NORMAL = 1,
  MISC_BOOTTYPE_VIA_PATCHED_BOOTCORE,
  MISC_BOOTTYPE_VIA_BOOTCORE_BUG,
} MISC_BOOTTYPE;

typedef enum
{
    MISC_MODEL_INDEX_C35 = 0,
    MISC_MODEL_INDEX_A35,
    MISC_MODEL_INDEX_C45,
    MISC_MODEL_INDEX_S45,
    MISC_MODEL_INDEX_SL45,
    MISC_MODEL_INDEX_C30,
    MISC_MODEL_INDEX_S40,
    MISC_MODEL_INDEX_A50,
    MISC_MODEL_INDEX_C55,
    MISC_MODEL_INDEX_S55,
    MISC_MODEL_INDEX_MT50,

/* until the previous entry, there are the "physical" entries */
    MISC_MODEL_INDEX_M50,
    MISC_MODEL_INDEX_C35_NEW,
    MISC_MODEL_INDEX_S35,
    MISC_MODEL_INDEX_M35,
    MISC_MODEL_INDEX_A36,
    MISC_MODEL_INDEX_A40,
    MISC_MODEL_INDEX_A55,
    MISC_MODEL_INDEX_ME45,
    MISC_MODEL_INDEX_SL42,
    MISC_MODEL_INDEX_SL55,
    MISC_MODEL_INDEX_S35_NEW,
    MISC_MODEL_INDEX_M35_NEW,
    MISC_MODEL_INDEX_1168,
    MISC_MODEL_INDEX_2118,
    MISC_MODEL_INDEX_2128,
    MISC_MODEL_INDEX_6688,
    MISC_MODEL_INDEX_3118
}
eMISC_MODEL_INDICES;

#ifndef MINIMAL_MISC

#define MISC_FLASH_BLOCKSIZE 0x10000l

#define MISC_BFB_BLOCKSIZE 1024l

typedef struct
{
    UINT32                        U32start, U32size;
}
tMISC_EEPROM_ADDRESS;

#define MISC_MAX_EEPROM_BLOCKS 2

typedef struct
{
    char                          modelname[SYS_MAX_STRING_LEN];
    eMISC_MODEL_INDICES           uniqueid;
    eMISC_MODEL_INDICES           flashid;
    eMISC_MODEL_INDICES           modelid;
    UINT32                        U32mcustartaddress;
    tMISC_EEPROM_ADDRESS          bootcore;
    tMISC_EEPROM_ADDRESS          eeprom[MISC_MAX_EEPROM_BLOCKS];
}
tMISC_PHONE_MODEL;

#endif

__IMPORT_TYPE DLL_EXP BOOL    MISC_RedirectIOToConsole (void);

DLL_EXP BOOL                  MISC_IsConsoleAPP (void);

DLL_EXP BOOL                  MISC_SerialLoggerIsRunning (void);

DLL_EXP void                  MISC_calibrate_delay (void);

DLL_EXP UINT8                 MISC_inport (UINT16 U16port, SINT16 S16shift);

DLL_EXP void                  MISC_outport (UINT16 U16port, SINT16 S16shift,
                                            UINT8 U8byte);

DLL_EXP void                  MISC_delay (UINT32 x);

DLL_EXP void                  MISC_DefaultLogCallout (char *logstr,
                                                      BOOL Biserror,
                                                      BOOL Bshowaserror);

__IMPORT_TYPE DLL_EXP void    MISC_SetLogCallout (pLogCallout
                                                  desiredlogcallout);

DLL_EXP BOOL                  MISC_StartRedirectionOfError (char *filename);

DLL_EXP void                  MISC_FinishRedirectionOfError (void);

DLL_EXP void                  MISC_Sleep (UINT32 U32count);

DLL_EXP void                  MISC_HandleLogString (BOOL Biserror,
                                                    BOOL Bshowaserror);

DLL_EXP UINT8                 MISC_HexCharToByte (char hex);

__IMPORT_TYPE DLL_EXP UINT32  MISC_HexToInt (char *hex);

DLL_EXP BOOL                  MISC_GetNextToken (char **str, char delim,
                                                 char *token);

DLL_EXP UINT32                MISC_GetFileSize (FILE * rfile);

DLL_EXP UINT32                MISC_Adler32 (UINT8 * buf, UINT16 len);

DLL_EXP void                  MISC_CheckFileName (char *filename);

#ifndef MINIMAL_MISC

DLL_EXP void                  MISC_GUIUpdate (void);

#ifndef LOGMODE
#ifndef MAPMODE

DLL_EXP UINT8                 MISC_Calculate8BitChecksum (UINT8 * pAU8buffer,
                                                          UINT16 U16length);

DLL_EXP BOOL                  MISC_ReadHeader (FILE * rfile,
                                               eFILE_TYPE * file_type,
                                               UINT32 * U32startaddr);

DLL_EXP BOOL                  MISC_WriteHeader (FILE * rfile,
                                                eFILE_TYPE file_type,
                                                UINT32 U32startaddr);

DLL_EXP BOOL                  MISC_SetEmulation (char *filename,
                                                 BOOL Bbinaryemulate);

DLL_EXP BOOL                  MISC_OpenEmulatedFlashFile (FILE ** rfile,
                                                          char *mode,
                                                          UINT32 U32seekaddr);

DLL_EXP UINT8                 MISC_Calculate8BitChecksum (UINT8 * pAU8buffer,
                                                          UINT16 U16length);

DLL_EXP void                  MISC_AdjustAddresses (UINT32 * U32startaddr,
                                                    UINT32 * U32endaddr);

DLL_EXP void                  MISC_GaugeInit (UINT32 U32start, UINT32 U32end);

DLL_EXP void                  MISC_GaugeUpdate (UINT32 U32addr);

DLL_EXP void                  MISC_GaugeDone (void);

DLL_EXP void                  MISC_SetErrorCode (eERROR errcode);

DLL_EXP eERROR                MISC_GetErrorCode (void);

__IMPORT_TYPE DLL_EXP UINT32  MISC_GetBuild (void);

DLL_EXP eMISC_MODEL_INDICES   MISC_GetCurrentPhonemodel (void);

__IMPORT_TYPE DLL_EXP BOOL    MISC_GetEEPROMAddress (UINT8 U8eepromidx,
                                                     UINT32 * pU32eepstart,
                                                     UINT32 * pU32eepsize);

DLL_EXP UINT32                MISC_GetBootcorePasswordOffset (void);

DLL_EXP tFREIA_PHONEINFO     *MISC_GetPhoneInfoPtr (void);

DLL_EXP BOOL                  MISC_CRCCheck (BOOL Bfix, char *flashfile);

DLL_EXP void                  MISC_SetDngCOMPort (UINT8 U8newcomport);

DLL_EXP UINT8                 MISC_GetDngCOMPort (void);

DLL_EXP void                  MISC_GetOriginalIMEI (UINT8 * pAU8dst);

#endif
#endif

DLL_EXP void                  MISC_ConvertProvider (UINT32 U32provider,
                                                    char *provider);

DLL_EXP BOOL                  MISC_PossibleToUpdateOriginalIMEI (void);

DLL_EXP void                  MISC_SetRedirection (eREDIRECTION_TYPE
                                                   redirecttype);

__IMPORT_TYPE DLL_EXP void    MISC_SetCurrentPhonemodel (eMISC_MODEL_INDICES
                                                         phonemodel);

DLL_EXP void                  MISC_SetGauge (pGaugeInit Init,
                                             pGaugeUpdate Update,
                                             pGaugeDone Done);

DLL_EXP void                  MISC_SetGUIUpdate (pGUIUpdate guiupdate);

DLL_EXP void                  MISC_SetDebugLevel (eDEBUG_LEVEL level);

__IMPORT_TYPE DLL_EXP BOOL    MISC_GetPhoneModel (char *model,
                                                  tMISC_PHONE_MODEL ** pmodel);

DLL_EXP void                  MISC_SetAppCOMPort (UINT8 U8newcomport);

DLL_EXP void                  MISC_SetAppCOMType (BOOL Bdtr, BOOL Brts);

__IMPORT_TYPE DLL_EXP BOOL    MISC_GetBootcoreAddress (UINT32 * pU32corestart,
                                                       UINT32 * pU32coresize);

DLL_EXP void                  MISC_SetBootType (MISC_BOOTTYPE value);

DLL_EXP MISC_BOOTTYPE         MISC_GetBootType (void);

DLL_EXP BOOL                  MISC_C30Like (eMISC_MODEL_INDICES model);

DLL_EXP UINT8                 MISC_GetAppCOMPort (void);

DLL_EXP BOOL                  MISC_GetAppCOMRTS (void);

DLL_EXP BOOL                  MISC_GetAppCOMDTR (void);

DLL_EXP BOOL                  MISC_GetPhoneModelName (char *modelname);

DLL_EXP BOOL                  MISC_GetEmulation (void);

DLL_EXP UINT32                MISC_SwapDword (UINT32 U32dword);

__IMPORT_TYPE DLL_EXP BOOL    MISC_GetMCUStartAddress (UINT32 * pU32mcustart);

DLL_EXP const tMISC_PHONE_MODEL *MISC_GetModel (UINT8 U8idx);

DLL_EXP BOOL                  MISC_ReadHexBlock (FILE * rfile, UINT16 * pU16len,
                                                 UINT8 * pAU8buffer);

#endif

#endif
