
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

#ifndef __tty_loc_h
#define __tty_loc_h

#define TTY_MAX_COM_PORTS 7 /* include LPT as well */

#define TTY_DEFAULT_READBLOCK_LEN 16

#define RETRY_TIMES 10

typedef struct
{
#ifdef _Windows
    COMMTIMEOUTS                  newcommtimeouts, oldcommtimeouts;
#endif
    UINT32                        U32writtenbytes, U32receivedbytes;
    UINT8                         U8comport;
    UINT8                         U8phoneid;
    BOOL                          Binitialized;
    UINT16                        U16comdelay;
}
tTTY_CONFIG;

#define TTY_WRITE_BLOCKSIZE 32768

#define FREIA_USART_TIMEOUT 60000l

#endif
