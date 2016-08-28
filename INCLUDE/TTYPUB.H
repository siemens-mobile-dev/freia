
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

#ifndef __tty_pub_h
#define __tty_pub_h

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

__IMPORT_TYPE DLL_EXP BOOL    TTY_ReadComm (UINT8 U8comport, UINT8 * pAU8buffer,
                                            UINT32 U32requested,
                                            UINT32 U32limit);

__IMPORT_TYPE DLL_EXP BOOL    TTY_ReadStream (UINT8 U8comport,
                                              UINT8 * pAU8stream, UINT16 U16len,
                                              BOOL Bencrypted,
                                              BOOL * pBencrypterror);

__IMPORT_TYPE DLL_EXP BOOL    TTY_WriteComm (UINT8 U8comport,
                                             UINT8 * pAU8buffer,
                                             UINT32 U32desiredsize);

__IMPORT_TYPE DLL_EXP BOOL    TTY_WriteStream (UINT8 U8comport, UINT8 * pSrc,
                                               UINT16 U16len, BOOL Bencrypted);

__IMPORT_TYPE DLL_EXP BOOL    TTY_SetSpeed (UINT8 U8comport, UINT32 U32newrate);

__IMPORT_TYPE DLL_EXP BOOL    TTY_OpenCOMPort (UINT8 U8comport, UINT32 U32speed,
                                               UINT32 U32comdelay,
                                               UINT32 U32buffsize, BOOL Brts,
                                               BOOL Bdtr);

__IMPORT_TYPE DLL_EXP void    TTY_CloseCOMPort (UINT8 U8comport);

__IMPORT_TYPE DLL_EXP BOOL    TTY_SetReadTimeout (UINT8 U8comport,
                                                  UINT32 U32timeout);

__IMPORT_TYPE DLL_EXP UINT32  TTY_GetNumberOfReceivedBytes (UINT8 U8comport);

__IMPORT_TYPE DLL_EXP UINT32  TTY_GetReadTimeout (UINT8 U8comport);

__IMPORT_TYPE DLL_EXP UINT32  TTY_GetSpeed (UINT8 U8comport);

__IMPORT_TYPE DLL_EXP void    TTY_Construct (void);

__IMPORT_TYPE DLL_EXP void    TTY_Destruct (void);

#endif
