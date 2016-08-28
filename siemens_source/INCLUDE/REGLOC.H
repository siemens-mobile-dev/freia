
/********************************************************************
*                                                                   *
* NAME: reghcloc.h                                                  *
*                                                                   *
* PURPOSE: Local definition for registry handling                   *
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
* Date: 16. May, 2002                                               *
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

#ifndef __regloc_h__
#define __regloc_h__

typedef struct
{
    UINT32                        NumSubKeys;
    UINT32                        MaxSubKeyLen;
    UINT32                        NumValues;
    UINT32                        MaxValueLen;
    UINT32                        MaxDataLen;
    FILETIME                      FileTime;
}
tRegKeyInfo;

typedef struct
{
    eRegDataType                  RegData;
    UINT32                        DataSize;
}
tRegDataInfo;

#endif
