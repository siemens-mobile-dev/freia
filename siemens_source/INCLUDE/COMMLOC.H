
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

#ifndef __comm_loc_h
#define __comm_loc_h

#define DESLOCK_BUFSIZE 0x2000

typedef struct
{
    UINT32                        U32length;
    UINT32                        U32ofs;
}
tENTRY;

#define COMM_MAX_NUM_OF_BOOTS_PER_MODEL 4

#define COMM_DEFAULT_COMMAND_RETRY 64

#define COMM_UPDATE_COMMAND_RETRY 512

#define COMM_DEFAULT_DELAY 100

#define COMM_MEDIUM_DELAY 10

#define COMM_SMALL_DELAY 1

#define COMM_CMD_NOACKNAK 0x00

#define COMM_CMD_NAK 0x5A

#define COMM_CMD_ACK 0xA5

#define COMM_1STBOOT_RAM_ACK '>'

#define COMM_1STBOOT_RAM_NAK '!'

#define COMM_2NDBOOT_ACK 0xA6

#define COMM_3RDBOOT_ACK 0x4B4F

#define COMM_UPDATE_REQ 0x57

#define COMM_FLASHIMEI_REQ 'P'

#define COMM_WRITE_REQ 'F'

#define COMM_INFO_REQ 0x49

#define COMM_DEFAULT_BOOTUP_RETRY 128

#define COMM_DEFAULT_BOOTUP_DELAY 50

#define COMM_DEFAULT_BFB_DELAY 100

#define COMM_BOOTUP_REQ 0x55

#define COMM_BOOTUP_ACK 0xA0

#define COMM_BOOTCODE_CHECK_REQ 0x41

#define COMM_BOOTCODE_CHECK_POS_REPLY 0x52

#define COMM_VERSION_REQ 0x56

#define COMM_PHONEINFO_REQ 0x49

#define COMM_BATTERY_REQ 'B'

#define COMM_SETBATTERY_REQ 'S'

#define COMM_REBOOT_REQ 0x51

#define COMM_CHANGESPEED_REQ 'H'

#define COMM_DECRYPT_REQ 'D'

#define COMM_CHANGESPEED_PARAM_ACK 'h'

#define COMM_CHANGESPEED_CONFIRM 'A'

#define COMM_CHANGESPEED_RETRY 6

#define COMM_DECRYPT_OFS (0xBC0-0x200)

#define COMM_DECRYPT_LEN 0x600

typedef struct
{
    UINT16                        U16startid;
    UINT16                        U16blocklength;
    UINT16                        U16blockoffs;
    UINT16                        U16blockseg;
    UINT16                        U16blockid;
    UINT16                        U16endid;
}
tCOMM_BLOCKHEADER;

#define COMM_EEPROM_BLOCKHEADER_END_ID1 0xFC00

#define COMM_EEPROM_BLOCKHEADER_END_ID2 0xF800

#define COMM_EEPROM_BLOCKHEADER_END_ID3 0xF000

#define COMM_FLASHLOADER_ACK "Ready"

#define COMM_DEFAULT_PHONEBLOCK_SEG 0xFF

#define COMM_DEFAULT_PHONEBLOCK_ID 0xFF

#define COMM_MES45_PHONEBLOCK_SEG 0xBF

#define COMM_EEPROM_BLOCKNAME_EELITE "EELITE"

#define COMM_EEPROM_BLOCKNAME_EELITE_LENGTH 6

#define COMM_EEPROM_BLOCKNAME_EEFULL "EEFULL"

#define COMM_EEPROM_BLOCK_ID_OFS 0x12

#define COMM_EEPROM_EELITE_LEN 0x4000

#define COMM_EEPROM_EEFULL_LEN 0x2000

#define COMM_EEPROM_PHONEINFO_FLASHID_OFS 0x18

#define COMM_EEPROM_PHONEINFO_PHONEMODEL_OFS 0x170

#define COMM_EEPROM_PHONEINFO_MANUFACTURER_OFS 0x180

#define COMM_PHONEINFO_SIZE 512

#define COMM_MAX_EEPROM_SIZE 0x60000l   /* Oh my goodness, it's the C55 */

#pragma pack(1)

typedef struct
{
#ifndef LOGMODE
    UINT8                         AU8phoneinfo[0x200];
#endif
    UINT32                        U32phoneid1;  /* note that for C30, phoneid1 and phoneid2 is the only one phone id */
    UINT32                        U32phoneid2;
    UINT32                        U32flashhc1;
    UINT32                        U32flashhc2;
    UINT16                        U16flashhcram;
    UINT16                        BC35_Type;
    UINT8                         B5009[FREIA_5009_AND_0001_LEN];
    UINT16                        U16lockstatus;
    UINT16                        factory_register[4];
    UINT8                         originalB5009[8];

/* 512 + 16 + 4 + 10 + 2 + 8 + 8 = 560 bytes */
}
tCOMM_PHONEINFO;

#define COMM_VERSION_OFFSET 0x150
#define COMM_LANGUAGEPACK_OFFSET 0x160
#define COMM_MANUFACTURER_OFFSET 0x180
#define COMM_MODEL_OFFSET 0x170

#define COMM_UNKNOWN_FLASH_NAK 0xCCCC

typedef struct
{
    UINT16                        U16flashbase;
    UINT16                        U16offset;
    UINT16                        U16seg;
    UINT16                        U16dummy;
    UINT16                        U16flashend;
    UINT16                        U16blocksize;
    UINT16                        U16id;
}
tCOMM_WRITERESPONSE;

#define COMM_ERASE_ACK 0x0202

#define COMM_WRITE_ACK 0x0303

#define COMM_DEFAULT_RETRIES 5

#define COMM_BFB_SPEED 57600

#define COMM_EMULATED_WRITE_BLOCKSIZE 16384

#define COMM_BOOTCORE_PWD "\xd4\x86\x21\xac\x48\x74\xbf\x02\x7c\x2c\x52\x04\x3e\x1b\xb4\x56"

#define COMM_FLASHBLOCK_CHUNK_SIZE 4096

#define COMM_SPECIALBOOT_LENGTH 36

typedef struct
{
    UINT8                         U8id1;
    UINT8                         U8id2;
    UINT16                        U16id;
}
tCOMM_X30BLOCK;

#define COMM_SIZE_OF_MODIFIED_PREBOOT 529

#endif
