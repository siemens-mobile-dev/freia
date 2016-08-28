
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

#ifndef __dng_pub_h
#define __dng_pub_h

#if defined(BUILD_DLL)
#define DLL_EXP __declspec(dllexport)
#else
#if defined(BUILD_APP)
#define DLL_EXP __declspec(dllimport)
#else
#define DLL_EXP
#endif
#endif

#ifdef __FROM_CPP__
#define __IMPORT_TYPE extern "C"
#else
#define __IMPORT_TYPE
#endif

#pragma pack(1)

typedef struct
{
    UINT16                        U16devidlo, U16devidhi;
    UINT16                        U16version;
    UINT8                         Bblocked;
    UINT16                        U16misusage_counter;
    UINT32                        U32update_counter;
}
tDNG_DEVINFO;

#ifndef MINIMAL_DNG
__IMPORT_TYPE DLL_EXP BOOL    DNG_Boot (BOOL Btoboot, UINT16 * pU16version);

__IMPORT_TYPE DLL_EXP void    DNG_GetMasterKey (UINT8 * pAU8key,
                                                BOOL Busedevcode);

__IMPORT_TYPE DLL_EXP BOOL    DNG_GetDeviceInfo (tDNG_DEVINFO * pdevinfo,
                                                 BOOL * pBencryptionerror);

__IMPORT_TYPE DLL_EXP BOOL    DNG_SetDeviceInfo (tDNG_DEVINFO * pdevinfo,
                                                 BOOL * pBbadinfo);

__IMPORT_TYPE DLL_EXP UINT16  DNG_GetMasterkeyLength (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_GetPage (UINT16 U16address, UINT8 * pDst,
                                           BOOL * pBencryptionerror);

__IMPORT_TYPE DLL_EXP BOOL    DNG_SetPage (UINT16 U16address, UINT8 * pSrc,
                                           BOOL * pBencryptionerror);

__IMPORT_TYPE DLL_EXP BOOL    DNG_TestEncryptionEngine (BOOL *
                                                        pBencryptionerror);

__IMPORT_TYPE DLL_EXP BOOL    DNG_TestUART (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_Calculate (eMISC_MODEL_INDICES phonetype,
                                             char *IMEI, UINT32 U32phoneid,
                                             void * pCodE2, BOOL * pBblocked);

__IMPORT_TYPE DLL_EXP BOOL    DNG_FinishBootstrap (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_GetStatus (BOOL * pBboot);

__IMPORT_TYPE DLL_EXP void    DNG_Construct (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_TestDongle (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_GetDevInfo (void);

__IMPORT_TYPE DLL_EXP BOOL    DNG_SetDevInfo (char *filename);

__IMPORT_TYPE DLL_EXP BOOL    DNG_GetLoaderDecryptionKey (UINT32 U32phoneid,
                                                          UINT8 * pDst);

__IMPORT_TYPE DLL_EXP UINT16  DNG_GetLoaderDecryptionKeyLen (void);

#endif

__IMPORT_TYPE DLL_EXP void    DNG_SetDeviceID (UINT16 U16lo, UINT16 U16hi);

#ifdef INCLUDE_RC4
__IMPORT_TYPE DLL_EXP BOOL    DNG_DecryptDevinfo (char *filename,
                                                  tDNG_DEVINFO * pdevinfo);

__IMPORT_TYPE DLL_EXP BOOL    DNG_EncryptDevinfo (tDNG_DEVINFO * pdevinfo);

__IMPORT_TYPE DLL_EXP BOOL    DNG_CreateUpdate (char *infilename);

#endif

__IMPORT_TYPE DLL_EXP BOOL    DNG_SendUpdate (char *infilename);

#endif
