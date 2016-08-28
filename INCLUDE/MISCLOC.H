
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

#ifndef __misc_loc_h
#define __misc_loc_h

#define xorpassword "\xFE\x45\xCC\x65\x11\xD7"

#define HZ 100

/* This is the number of bits of precision for the loops_per_jiffy.  Each
   bit takes on average 1.5/HZ seconds.  This (like the original) is a little
   better than 1% */

#define LPS_PREC 8

#define __delay(x) {\
                    UINT32 __U32now = MISC_rdtsc(); \
                    do {;} while (MISC_rdtsc()-__U32now<x); \
                   }

#define jiffies GetTickCount()

#define BASE 65521L             /* largest prime smaller than 65536 */
#define NMAX 5552               /* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);

#ifndef MINIMAL_MISC
typedef struct
{
    BOOL                          Bemulate_from_file;
    char                          emulatefile[SYS_MAX_PATHNAME_LEN];
    BOOL                          Bbinary_emulate;
    UINT32                        U32flashendaddr;
    eMISC_MODEL_INDICES           phonemodel;
}
tMISC_CONFIG;

#define FREIA_HEADER_ID 0x4549534B  /* FREIA */

#define MISC_MAX_NUM_OF_MODELS 27

static tMISC_PHONE_MODEL      misc_models[] = {
    {"C30", MISC_MODEL_INDEX_C30, MISC_MODEL_INDEX_C30, MISC_MODEL_INDEX_C30,
     0xF00000, {0, 0}, {{0xFF8000, 0x8000}, {0, 0}}},
    {"S40", MISC_MODEL_INDEX_S40, MISC_MODEL_INDEX_S40, MISC_MODEL_INDEX_S40,
     0xC00000, {0, 0}, {{0xFF8000, 0x8000}, {0, 0}}},
    {"C35", MISC_MODEL_INDEX_C35, MISC_MODEL_INDEX_C35, MISC_MODEL_INDEX_C35,
     0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"C35 NEW", MISC_MODEL_INDEX_C35_NEW, MISC_MODEL_INDEX_C35,
     MISC_MODEL_INDEX_C35, 0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"M35", MISC_MODEL_INDEX_M35, MISC_MODEL_INDEX_C35, MISC_MODEL_INDEX_C35,
     0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"M35 NEW", MISC_MODEL_INDEX_M35_NEW, MISC_MODEL_INDEX_C35,
     MISC_MODEL_INDEX_C35, 0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"S35", MISC_MODEL_INDEX_S35, MISC_MODEL_INDEX_C35, MISC_MODEL_INDEX_C35,
     0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"S35 NEW", MISC_MODEL_INDEX_S35_NEW, MISC_MODEL_INDEX_C35,
     MISC_MODEL_INDEX_C35, 0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"A35", MISC_MODEL_INDEX_A35, MISC_MODEL_INDEX_A35, MISC_MODEL_INDEX_A35,
     0xF00000, {0, 0}, {{0xFF8000, 0x8000}, {0, 0}}},
    {"A36", MISC_MODEL_INDEX_A36, MISC_MODEL_INDEX_A35, MISC_MODEL_INDEX_A35,
     0xF00000, {0, 0}, {{0xFF8000, 0x8000}, {0, 0}}},
    {"A40", MISC_MODEL_INDEX_A40, MISC_MODEL_INDEX_A35, MISC_MODEL_INDEX_A35,
     0xF00000, {0, 0}, {{0xFF8000, 0x8000}, {0, 0}}},
    {"A50", MISC_MODEL_INDEX_A50, MISC_MODEL_INDEX_A50, MISC_MODEL_INDEX_A50,
     0xC00000, {0xC00000, 0x10000}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"A55", MISC_MODEL_INDEX_A55, MISC_MODEL_INDEX_C55, MISC_MODEL_INDEX_A50,
     0x800000, {0x800000, 0x20000}, {{0xFA0000, 0x60000}, {0, 0}}},
    {"1168", MISC_MODEL_INDEX_1168, MISC_MODEL_INDEX_A50, MISC_MODEL_INDEX_A50,
     0xC00000, {0xC00000, 0x10000}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"C45", MISC_MODEL_INDEX_C45, MISC_MODEL_INDEX_C45, MISC_MODEL_INDEX_C45,
     0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"2118", MISC_MODEL_INDEX_2118, MISC_MODEL_INDEX_C45, MISC_MODEL_INDEX_C45,
     0xC00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"C55", MISC_MODEL_INDEX_C55, MISC_MODEL_INDEX_C55, MISC_MODEL_INDEX_A50,
     0x800000, {0x800000, 0x20000}, {{0xFA0000, 0x60000}, {0, 0}}},
    {"2128", MISC_MODEL_INDEX_2128, MISC_MODEL_INDEX_C55, MISC_MODEL_INDEX_A50,
     0x800000, {0x800000, 0x20000}, {{0xFA0000, 0x60000}, {0, 0}}},
    {"S45", MISC_MODEL_INDEX_S45, MISC_MODEL_INDEX_S45, MISC_MODEL_INDEX_S45,
     0xA00000, {0, 0}, {{0xBF0000, 0x10000}, {0xFF0000, 0x10000}}},
    {"S55", MISC_MODEL_INDEX_S55, MISC_MODEL_INDEX_S55, MISC_MODEL_INDEX_A50,
     0x400000, {0x800000, 0x10000}, {{0xFE0000, 0x20000}, {0, 0}}},
    {"ME45", MISC_MODEL_INDEX_ME45, MISC_MODEL_INDEX_S45, MISC_MODEL_INDEX_S45,
     0xA00000, {0, 0}, {{0xBF0000, 0x10000}, {0xFF0000, 0x10000}}},
    {"SL42", MISC_MODEL_INDEX_SL42, MISC_MODEL_INDEX_SL45,
     MISC_MODEL_INDEX_SL45, 0xA00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"SL45", MISC_MODEL_INDEX_SL45, MISC_MODEL_INDEX_SL45,
     MISC_MODEL_INDEX_SL45, 0xA00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"SL55", MISC_MODEL_INDEX_SL55, MISC_MODEL_INDEX_S55, MISC_MODEL_INDEX_A50,
     0x400000, {0x800000, 0x10000}, {{0xFE0000, 0x20000}, {0, 0}}},
    {"6688", MISC_MODEL_INDEX_6688, MISC_MODEL_INDEX_SL45,
     MISC_MODEL_INDEX_SL45, 0xA00000, {0, 0}, {{0xFF0000, 0x10000}, {0, 0}}},
    {"MT50", MISC_MODEL_INDEX_MT50, MISC_MODEL_INDEX_MT50, MISC_MODEL_INDEX_C45,
     0xA00000, {0, 0}, {{0xBF0000, 0x10000}, {0xFF0000, 0x10000}}},
    {"M50", MISC_MODEL_INDEX_M50, MISC_MODEL_INDEX_MT50, MISC_MODEL_INDEX_C45,
     0xA00000, {0, 0}, {{0xBF0000, 0x10000}, {0xFF0000, 0x10000}}},
    {"3118", MISC_MODEL_INDEX_3118, MISC_MODEL_INDEX_MT50, MISC_MODEL_INDEX_C45,
     0xA00000, {0, 0}, {{0xBF0000, 0x10000}, {0xFF0000, 0x10000}}},
};
#endif

#endif
