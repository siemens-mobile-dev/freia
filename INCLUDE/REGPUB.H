
/********************************************************************
*                                                                   *
* NAME: rehcpub.h                                                   *
*                                                                   *
* PURPOSE: Public definition for registry handling                  *
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

#ifndef __regpub_h__
#define __regpub_h__

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
{ rdUnknown, rdString, rdExpandString, rdInteger, rdBinary }
eRegDataType;

__IMPORT_TYPE DLL_EXP BOOL    REG_Save (HKEY rootkey, const char *base,
                                        const char *ident, void *value,
                                        eRegDataType regtype, BOOL cancreate);

__IMPORT_TYPE DLL_EXP BOOL    REG_Load (HKEY rootkey, const char *base,
                                        const char *ident, void *value);

#endif
