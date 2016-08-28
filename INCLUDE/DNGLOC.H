
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

#ifndef __dng_loc_h
#define __dng_loc_h

#ifndef MINIMAL_DNG

#define DNG_BOOT_TO_BOOT 0x55
#define DNG_BOOT_TO_APP 0x56

#define DNG_BOOT_BOOT_ACK 0xA5
#define DNG_BOOT_APP_ACK 0xA0
#define DNG_CMD_ACK 0xA6
#define DNG_BLOCKED_NAK 0xAB
#define DNG_DECRYPT_NAK 0xAA
#define DNG_INVALID_DEVINFO_NAK 0xAC

#define DNG_APPLICATION_BOOTUP 'B'
#define DNG_APPLICATION_CALCULATION 'C'

#define DNG_GET_DEVINFO 'G'
#define DNG_SET_DEVINFO 'S'
#define DNG_READ_PAGE 'R'
#define DNG_DUMMY 'D'
#define DNG_UART_TEST 'U'
#define DNG_WRITE_PAGE 'W'
#define DNG_END_OF_SESSION 'E'
#define DNG_GET_STATUS '?'
#define DNG_GET_LOADER_DECRYPTION_KEY 'K'

#define DNG_BCDIMEI_LEN 8

#define DNG_SPEED 19200

#define DNG_LOADER_DECRYPTION_KEY_LEN 16

BOOL                          DNG_GetStatus (BOOL * pBboot);
#endif

#define DNG_PAGE_SIZE 128
#define DNG_NUM_PAGES (0x7000/DNG_PAGE_SIZE)

/* Because of Adler32 */
#define DNG_CHECKSUM_SIZE sizeof(UINT32)

#ifdef INCLUDE_RC4
#define DNG_MASTERKEY "KSIEBYNUTZO2003"
#define DNG_MASTERKEY_STRLEN 15
#endif

#define DNG_TIMEOUT 200

#endif
