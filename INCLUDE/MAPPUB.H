
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

#ifndef __map_pub_h
#define __map_pub_h

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
    MAPTYPE_UNLOCK = 1,
    MAPTYPE_BACKUP,
    MAPTYPE_DECRYPT,
}
tMAP_MAPTYPE;

#ifndef LOGMODE

DLL_EXP BOOL                  MAP_SaveBatteryMap (eMISC_MODEL_INDICES
                                                  phonemodel,
                                                  UINT32 * pU32phoneid,
                                                  char *IMEI, UINT8 * pB67);

DLL_EXP BOOL                  MAP_LoadBatteryMap (eMISC_MODEL_INDICES
                                                  phonemodel, char *filename,
                                                  UINT32 * pU32phoneid,
                                                  UINT8 * pB67);

DLL_EXP BOOL                  MAP_SaveMap (eMISC_MODEL_INDICES phonemodel,
                                           tMAP_MAPTYPE maptype,
                                           char *srcpath,
                                           UINT32 * pU32phoneid, char *IMEI,
                                           UINT8 * pB5009,
                                           UINT8 * pB0001, UINT8 * pEnc5008,
                                           UINT8 * pEnc5077);

DLL_EXP BOOL                  MAP_LoadLog (char *logname,
                                           eMISC_MODEL_INDICES * puniquemodel,
                                           eMISC_MODEL_INDICES * pphonemodel,
                                           char *IMEI, UINT32 * pU32phone_id);
#endif

DLL_EXP BOOL                  MAP_SaveLog (eMISC_MODEL_INDICES uniquemodel,
                                           eMISC_MODEL_INDICES phonemodel,
                                           char *IMEI, char *originalIMEI,
                                           UINT32 * pU32phoneid);

DLL_EXP BOOL                  MAP_LoadMap (eMISC_MODEL_INDICES phonemodel,
                                           char *filename, UINT32 * pU32phoneid,
                                           UINT8 * pB5009, UINT8 * pB0001,
                                           UINT8 * pEnc5008, UINT8 * pEnc5077);

#endif
