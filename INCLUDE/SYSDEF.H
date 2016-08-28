
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


#ifndef __sysdef_h
#define __sysdef_h

#define SYS_MAX_LOGSTRING_LEN 256
#define SYS_MAX_PATHNAME_LEN 256
#define SYS_MAX_STRING_LEN 128
#define SYS_MAX_FILENAME_LEN 128
#define SYS_MAX_EXTENSION_LEN 8
#define SYS_GENERAL_HEADER_LEN 256

#define UINT32_FORMAT "%08X"

#define UINT16 unsigned short
#define SINT16 signed short
#define UINT8 unsigned char
#define SINT8 signed char
#define UINT32 unsigned long
#define SINT32 signed long
#define BOOL unsigned int
#ifndef __targetwindows__
#define TRUE 1
#define FALSE !TRUE
#endif

typedef enum
{
    DEBUG_LEVEL_NONE = 0,
    DEBUG_LEVEL_LOW,
    DEBUG_LEVEL_MEDIUM,
    DEBUG_LEVEL_HIGH,
}
eDEBUG_LEVEL;

#ifdef DEBUG_PRT
extern eDEBUG_LEVEL           debug_level;
extern char                   logbuffer[SYS_MAX_LOGSTRING_LEN];

#define DBGSTR(l,x) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR1(l,x,p1) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR2(l,x,p1,p2) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR3(l,x,p1,p2,p3) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR4(l,x,p1,p2,p3,p4) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR5(l,x,p1,p2,p3,p4,p5) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5); MISC_HandleLogString(FALSE,FALSE);}
#define DBGSTR6(l,x,p1,p2,p3,p4,p5,p6) if (debug_level>=l) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5,p6); MISC_HandleLogString(FALSE,FALSE);}

#define ERROR(x) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR1(x,p1) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR2(x,p1,p2) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR3(x,p1,p2,p3) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR4(x,p1,p2,p3,p4) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR5(x,p1,p2,p3,p4,p5) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5); MISC_HandleLogString(TRUE,TRUE);}
#define ERROR6(x,p1,p2,p3,p4,p5,p6) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5,p6); MISC_HandleLogString(TRUE,TRUE);}

#define INFO(x) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x); MISC_HandleLogString(TRUE,FALSE);}
#define INFO1(x,p1) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1); MISC_HandleLogString(TRUE,FALSE);}
#define INFO2(x,p1,p2) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2); MISC_HandleLogString(TRUE,FALSE);}
#define INFO3(x,p1,p2,p3) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3); MISC_HandleLogString(TRUE,FALSE);}
#define INFO4(x,p1,p2,p3,p4) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4); MISC_HandleLogString(TRUE,FALSE);}
#define INFO5(x,p1,p2,p3,p4,p5) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5); MISC_HandleLogString(TRUE,FALSE);}
#define INFO6(x,p1,p2,p3,p4,p5,p6) if (debug_level>=DEBUG_LEVEL_LOW) {memset(logbuffer,0,sizeof(logbuffer)); sprintf(logbuffer,x,p1,p2,p3,p4,p5,p6); MISC_HandleLogString(TRUE,FALSE);}

#define LDBGSTR(x) DBGSTR(DEBUG_LEVEL_LOW,x)
#define LDBGSTR1(x,p1) DBGSTR1(DEBUG_LEVEL_LOW,x,p1)
#define LDBGSTR2(x,p1,p2) DBGSTR2(DEBUG_LEVEL_LOW,x,p1,p2)
#define LDBGSTR3(x,p1,p2,p3) DBGSTR3(DEBUG_LEVEL_LOW,x,p1,p2,p3)
#define LDBGSTR4(x,p1,p2,p3,p4) DBGSTR4(DEBUG_LEVEL_LOW,x,p1,p2,p3,p4)
#define LDBGSTR5(x,p1,p2,p3,p4,p5) DBGSTR5(DEBUG_LEVEL_LOW,x,p1,p2,p3,p4,p5)
#define LDBGSTR6(x,p1,p2,p3,p4,p5,p6) DBGSTR6(DEBUG_LEVEL_LOW,x,p1,p2,p3,p4,p5,p6)

#define MDBGSTR(x) DBGSTR(DEBUG_LEVEL_MEDIUM,x)
#define MDBGSTR1(x,p1) DBGSTR1(DEBUG_LEVEL_MEDIUM,x,p1)
#define MDBGSTR2(x,p1,p2) DBGSTR2(DEBUG_LEVEL_MEDIUM,x,p1,p2)
#define MDBGSTR3(x,p1,p2,p3) DBGSTR3(DEBUG_LEVEL_MEDIUM,x,p1,p2,p3)
#define MDBGSTR4(x,p1,p2,p3,p4) DBGSTR4(DEBUG_LEVEL_MEDIUM,x,p1,p2,p3,p4)
#define MDBGSTR5(x,p1,p2,p3,p4,p5) DBGSTR5(DEBUG_LEVEL_MEDIUM,x,p1,p2,p3,p4,p5)
#define MDBGSTR6(x,p1,p2,p3,p4,p5,p6) DBGSTR6(DEBUG_LEVEL_MEDIUM,x,p1,p2,p3,p4,p5,p6)

#define HDBGSTR(x) DBGSTR(DEBUG_LEVEL_HIGH,x)
#define HDBGSTR1(x,p1) DBGSTR1(DEBUG_LEVEL_HIGH,x,p1)
#define HDBGSTR2(x,p1,p2) DBGSTR2(DEBUG_LEVEL_HIGH,x,p1,p2)
#define HDBGSTR3(x,p1,p2,p3) DBGSTR3(DEBUG_LEVEL_HIGH,x,p1,p2,p3)
#define HDBGSTR4(x,p1,p2,p3,p4) DBGSTR4(DEBUG_LEVEL_HIGH,x,p1,p2,p3,p4)
#define HDBGSTR5(x,p1,p2,p3,p4,p5) DBGSTR5(DEBUG_LEVEL_HIGH,x,p1,p2,p3,p4,p5)
#define HDBGSTR6(x,p1,p2,p3,p4,p5,p6) DBGSTR6(DEBUG_LEVEL_HIGH,x,p1,p2,p3,p4,p5,p6)
#else
#define ERROR(x)
#define ERROR1(x,p1)
#define ERROR2(x,p1,p2)
#define ERROR3(x,p1,p2,p3)
#define ERROR4(x,p1,p2,p3,p4)
#define ERROR5(x,p1,p2,p3,p4,p5)
#define ERROR6(x,p1,p2,p3,p4,p5,p6)

#define INFO(x)
#define INFO1(x,p1)
#define INFO2(x,p1,p2)
#define INFO3(x,p1,p2,p3)
#define INFO4(x,p1,p2,p3,p4)
#define INFO5(x,p1,p2,p3,p4,p5)
#define INFO6(x,p1,p2,p3,p4,p5,p6)

#define LDBGSTR(x)
#define LDBGSTR1(x,p1)
#define LDBGSTR2(x,p1,p2)
#define LDBGSTR3(x,p1,p2,p3)
#define LDBGSTR4(x,p1,p2,p3,p4)
#define LDBGSTR5(x,p1,p2,p3,p4,p5)
#define LDBGSTR6(x,p1,p2,p3,p4,p5,p6)

#define MDBGSTR(x)
#define MDBGSTR1(x,p1)
#define MDBGSTR2(x,p1,p2)
#define MDBGSTR3(x,p1,p2,p3)
#define MDBGSTR4(x,p1,p2,p3,p4)
#define MDBGSTR5(x,p1,p2,p3,p4,p5)
#define MDBGSTR6(x,p1,p2,p3,p4,p5,p6)

#define HDBGSTR(x)
#define HDBGSTR1(x,p1)
#define HDBGSTR2(x,p1,p2)
#define HDBGSTR3(x,p1,p2,p3)
#define HDBGSTR4(x,p1,p2,p3,p4)
#define HDBGSTR5(x,p1,p2,p3,p4,p5)
#define HDBGSTR6(x,p1,p2,p3,p4,p5,p6)
#endif

#define LOBYTE(x) (UINT8)((UINT16)x & 0xFF)
#define HIBYTE(x) (UINT8)((UINT16)(x >> 8) & 0xFF)

#define LOWORD(x) (UINT16)((UINT32)x & 0xFFFF)
#define HIWORD(x) (UINT16)((UINT32)(x >> 16) & 0xFFFF)

#define XCHGWORDS(x) (((UINT32)LOWORD(x) << 16) | HIWORD(x))    /* it's a ror 16 ;* */
#define SWAPWORD(x) (((UINT16)LOBYTE(x) << 8) | HIBYTE(x))
#define SWAPBYTE(x) ((((x) & 0xF0) >> 4) | (((x) & 0x0F) << 4))

#define ROL(x,skew) x=(x<<(skew))|(x>>(16-(skew)))
#define ROR(x,skew) x=(x>>(skew))|(x<<(16-(skew)))

#define wordatbyteptr(x) (*((UINT16 *)(x)))

#endif
