
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

#ifndef __sec_pub_h
#define __sec_pub_h

#if defined(BUILD_DLL)
#define DLL_EXP __declspec(dllexport)
#else
#if defined(BUILD_APP)
#define DLL_EXP __declspec(dllimport)
#else
#define DLL_EXP
#endif
#endif

#ifndef LOGMODE
DLL_EXP BOOL                  SEC_CreateIMEISpecificBlocks (UINT32 U32provider1,
                                                            UINT32 U32provider2,
                                                            eMISC_MODEL_INDICES
                                                            uniquemodel,
                                                            eMISC_MODEL_INDICES
                                                            phonemodel,
                                                            char *IMEI,
                                                            UINT32 *
                                                            pU32phoneid,
                                                            UINT8 * pEnc5008,
                                                            UINT8 * pEnc5077);

DLL_EXP void                  SEC_CreateBlocks5009And76 (eMISC_MODEL_INDICES
                                                         phonemodel, char *IMEI,
                                                         UINT8 * pB5009,
                                                         UINT8 * pB0001);

#ifndef MAPMODE
DLL_EXP                       BOOL
SEC_RecreateIMEISpecificBlocks (eMISC_MODEL_INDICES uniquemodel,
                                eMISC_MODEL_INDICES phonemodel, char *IMEI,
                                UINT32 * pU32phoneid, UINT8 * pEnc5008,
                                UINT8 * pEnc5077, UINT8 * pDec5008,
                                UINT8 * pDec5077);

DLL_EXP void                  SEC_RecreateBlocks5009And76 (eMISC_MODEL_INDICES
                                                           phonemodel,
                                                           UINT8 * pB5009,
                                                           UINT8 * pB76);
#endif

DLL_EXP void                  SEC_NormalIMEIToBCDIMEI (char *myIMEI,
                                                       UINT8 * myBCD);

#endif

DLL_EXP BOOL                  SEC_BCDIMEIToNormalIMEI (UINT8 * pB5009,
                                                       char *IMEI,
                                                       BOOL * pBempty);

DLL_EXP UINT8                 SEC_CreateCRCDigit (UINT8 * pB5009);

DLL_EXP void                  SEC_GetBlockLen (eMISC_MODEL_INDICES phonemodel,
                                               UINT16 * pU16B5008,
                                               UINT16 * pU16B5077);

DLL_EXP BOOL                  SEC_RecreateIMEI (eMISC_MODEL_INDICES phonemodel,
                                                UINT8 * pB5009, char *IMEI,
                                                BOOL * pBempty);

DLL_EXP BOOL                  SEC_AdjustOriginalIMEI (char *desirednewIMEI);

#endif
