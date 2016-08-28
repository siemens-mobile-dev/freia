
/********************************************************************
* NAME: cmdhcpub.h                                                  *
*                                                                   *
* PURPOSE:                                                          *
*                                                                   *
* globally used prototypes and defines for the commandline          *
* handler                                                           *
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
* Date: 21. May, 2002                                               *
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

#ifndef __cmdhcpub_h__
#define __cmdhcpub_h__

#ifdef __FROM_CPP__
#define __IMPORT_TYPE__ extern "C"
#else
#define __IMPORT_TYPE__
#endif

#define CMDLINE_BUILD 3

#define CMDLINE_STRING_SEP 39   /* an apostrophe setting the start and end of a string */
#define CMDLINE_STRING_SEP_STR "'"  /* an apostrophe setting the start and end of a string */

__IMPORT_TYPE__ int           cmd_main (int argc, char *argv[],
                                        pLogCallout logcallout,
                                        pGaugeInit gaugeinit,
                                        pGaugeUpdate gaugeupdate,
                                        pGaugeDone gaugedone,
                                        pGUIUpdate guiupdate);

#endif
