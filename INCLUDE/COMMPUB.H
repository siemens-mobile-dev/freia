
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

#ifndef __comm_pub_h
#define __comm_pub_h

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

__IMPORT_TYPE DLL_EXP BOOL    COMM_LoadBoots (UINT8 U8comport, UINT32 U32speed,
                                              eMISC_MODEL_INDICES phonemodel,
                                              eMISC_MODEL_INDICES
                                              securitymodel);

__IMPORT_TYPE DLL_EXP BOOL    COMM_UpdateIMEIBlocks (eMISC_MODEL_INDICES
                                                     phonemodel, UINT8 * pB5009,
                                                     UINT8 * pB0001,
                                                     UINT8 * pEnc5008,
                                                     UINT8 * pEnc5077,
                                                     BOOL Bupdateflashimei);

DLL_EXP void                  COMM_Reboot (void);

#ifndef LOGMODE
DLL_EXP BOOL                  COMM_UpdateBatteryBlock (eMISC_MODEL_INDICES
                                                       phonemodel,
                                                       UINT8 * pB67);


DLL_EXP BOOL                  COMM_Backup (eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, UINT8 * pB5009,
                                           UINT8 * pB0001, UINT8 * pEnc5008,
                                           UINT8 * pEnc5077);

DLL_EXP BOOL                  COMM_Read (char *flashfile, BOOL Bbinary,
                                         UINT32 U32startaddr, UINT32 U32count);

DLL_EXP BOOL                  COMM_Write (UINT32 U32extensionid,
                                          char *flashfile, UINT32 U32startfile,
                                          UINT32 U32countfile,
                                          UINT32 U32givenstartaddr,
                                          BOOL Bbinary);

DLL_EXP BOOL                  COMM_BackupBattery (UINT8 * pB67);

DLL_EXP BOOL                  COMM_ReadMemory (char *flashfile, BOOL Bbinary,
                                               UINT32 U32startaddr,
                                               UINT32 U32count);

DLL_EXP BOOL                  COMM_UpdateOriginalIMEI (char *IMEI);

DLL_EXP BOOL                  COMM_PatchBootcore (char *flashfile,
                                                  BOOL Bbinary);

#endif

DLL_EXP void                  COMM_Construct (void);

#endif
