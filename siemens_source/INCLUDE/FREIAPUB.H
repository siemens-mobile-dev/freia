
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

#ifndef __freia_pub_h
#define __freia_pub_h

#ifdef __FROM_CPP__
#define __IMPORT_TYPE__ extern "C"
#else
#define __IMPORT_TYPE__
#endif

#define FREIA_BUILD 6

typedef void                  (*pGaugeInit) (UINT32 U32startaddr,
                                             UINT32 U32endaddr);

typedef void                  (*pGaugeDone) (void);

typedef void                  (*pGUIUpdate) (void);

typedef void                  (*pGaugeUpdate) (UINT32 U32addr);

typedef void                  (*pLogCallout) (char *logstr, BOOL Biserror,
                                              BOOL Bshowaserror);

#define FREIA_WATERMARK_ID "\x32\x24\x31\x20\x37"

#define FREIA_5009_AND_0001_LEN 10

#define FREIA_67_LEN 20

#define FREIA_LOGFILE_NAME "freialog.txt"

typedef enum
{
    REDIRECTION_TYPE_NONE = 0,
    REDIRECTION_TYPE_REDIRECT_TO_FILE = 1,
    REDIRECTION_TYPE_SAVE_TO_FILE = 2,
}
eREDIRECTION_TYPE;

typedef enum
{
    KE_OK = 0,
    KE_INVALID_COM_PORT,
    KE_CANNOT_OPEN_BOOTPACKAGE,
    KE_SEEK_ERROR,
    KE_INVALID_FILEHEADER,
    KE_FILE_NOT_FOUND,
    KE_INVALID_FILE_FORMAT,
    KE_INVALID_FILENAME,
    KE_CANT_CREATE_FILE,
    KE_CANT_WRITE_HEADER,
    KE_COMMAND_NOT_FOUND,
    KE_NO_COMMAND_IS_SET,
    KE_TOO_MANY_ARGUMENTS,
    KE_INVALID_NUMBER,
    KE_TOO_MANY_NUMBERS,
    KE_TOO_MANY_STRINGS,
    KE_TOO_MANY_COMMANDS,
    KE_TOO_MANY_ARGUMENTS_FOR_COMMAND,
    KE_TOO_FEW_ARGUMENTS_FOR_COMMAND,
    KE_CANNOT_CREATE_KEY,
    KE_CANNOT_OPEN_KEY,
    KE_CANNOT_DELETE_KEY,
    KE_CANNOT_DELETE_VALUE,
    KE_VALUE_NOT_EXIST,
    KE_INVALID_VALUE_TYPE,
    KE_CANNOT_PUT_DATA,
    KE_CANNOT_GET_DATA,
    KE_INVALID_EXTENSION,
    KE_CANT_READ_HEADER,
}
eERROR;

#define FREIA_DEFAULT_SPEED 115200

#define FREIA_DEFAULT_COMPORT 1

#define FREIA_ENGINE_BUILD 1

typedef enum
{
    FILE_TYPE_DONT_CARE = 0,
    FILE_TYPE_PARTIAL = 1,
    FILE_TYPE_FULL
}
eFILE_TYPE;

typedef struct
{
    UINT32                        U32id;
    UINT32                        U32startaddr;
    UINT32                        filetype;
    UINT32                        U32checksum;
}
tFILE_HEADER;

#define FREIA_DI_EXTENSION "DI"
#define FREIA_DU_EXTENSION "DU"
#define FREIA_FLS_EXTENSION "FLS"
#define FREIA_KSI_EXTENSION "KSI"
#define FREIA_MAP_EXTENSION "MAP"
#define FREIA_LOG_EXTENSION "LOG"

#define KSI_EXTENSION_ID 0x45534B2E /* .KSI */
#define FLS_EXTENSION_ID 0x534C462E /* .FLS */

#define FREIA_MCU_START_ADDRESS 0xC00000

#define FREIA_MCU_START_ADDRESS_AXX 0xE00000

#define FREIA_MCU_START_ADDRESS_MT50 0xA00000

#define FREIA_MCU_END_ADDRESS 0xFFFFFF

#define FREIA_IMEI_LEN 14

#define FREIA_MAX_TAG_LEN 16

typedef struct
{
    UINT16                        U16loaderversion;
    UINT8                         U8version;
    char                          languagepack[FREIA_MAX_TAG_LEN];
    char                          manufacturer[FREIA_MAX_TAG_LEN];
    char                          model[FREIA_MAX_TAG_LEN];
    UINT32                        AU32phoneid[2]; /* C30 has 8 byte phone id ;o */
    UINT32                        U32flashid1;
    UINT32                        U32flashid2;
    UINT16                        U16flashid;
    char                          IMEI[FREIA_IMEI_LEN+1];
    char                          originalIMEI[FREIA_IMEI_LEN+1];
    UINT8                         originalB5009[FREIA_IMEI_LEN/2+1];
    UINT16                        U16lockstatus;
    BOOL                          BoriginalIMEI; /* whether original(flash) IMEI has been found */
}
tFREIA_PHONEINFO;

#endif
