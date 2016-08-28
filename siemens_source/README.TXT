---------------------------------------------------------------------------
The Revolutionary Service Tool
for Siemens Phones - Freia
---------------------------------------------------------------------------

***************************************************************************

                           D I S C L A I M E R

THIS SOFTWARE  AND ALL   THE ACCOMPANYING  FILES ARE   PROVIDED "AS IS" AND
WITHOUT  ANY   WARRANTIES   EXPRESSED   OR   IMPLIED  INCLUDING   BUT   NOT
LIMITED   TO  IMPLIED WARRANTIES  OF  MERCHANT ABILITY  AND  FITNESS FOR  A
PARTICULAR PURPOSE.

IN NO  EVENT  SHALL  I BE   LIABLE FOR  ANY  DAMAGES  WHATSOEVER (INCLUDING
WITHOUT  LIMITATION,  DAMAGES  FOR  LOSS  OF  BUSINESS  PROFITS,   BUSINESS
INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR  ANY OTHER PECUNIARY   LOSS)
ARISING OUT  OF THE USE  OR
INABILITY TO USE THIS PRODUCT.

***************************************************************************

---------------------
1. What is it anyway?
---------------------

It's a command line/GUI program, mostly for unlocking, 
flashing Siemens phones.

---------------
2. Installation
---------------

The package consists of the following files:

FREIA.EXE      *
FREIAMAP.EXE   *   The executables
FREIALOG.EXE   *

file_id.diz    *
readme.txt     *   Documentation
whatsnew.txt   *

---------------
2.1 Executables
---------------

These are the Freia executables. Nothing more nothing less.

-----------------
2.3 Documentation
-----------------

-----------------
2.3.1 file_id.diz
-----------------

The short description what is FREIA about.

----------------
2.3.2 readme.txt
----------------

The very file you're reading just now.

------------------
2.3.3 whatsnew.txt
------------------

The list of changes.


------------------
3 List of commands
------------------

Generally each and every command has a short and a long version. The 
subsections will contain both.
The usage of FREIA is as follows:

FREIA model cmd1 arg1a arg1b cmd2 arg2

Which means that you have to specify the phone model (C45, S45, etc.)
and after the commands you wish to run and right after the commands 
each and every argument the command has. For each and evry command 
there is the type of the argument that must be specified. There are 
two kinds of supported arugments:

- numerical (N)
- string (S)

Note that the string arguments (because of the long filenames) must be 
given between ". (i.e. "flash.fls")

**************************************************************************
Note: to start a command you must start it with a '-' or a '/' or a '\'
**************************************************************************

For example to read a raw flash (size of 65536) from a C45 starting from 
0xC00000 you should:

FREIA C45 -rr 0xC00000 65536

The headers of the subsections will have the name of the commands and the 
arguments it needs. The arguments starting with 'N' will mean a number, 
starting with an 'S' will mean a string.

**************************************************************************
Note: 'a' number can practically mean more than one number. You can specify 
'a' number in the following way:

1..4,7,9,11..15

Which will be used as if you specified 1,2,3,4,7,9,11,12,13,14,15

To specify a hexadecimal number you must prefix it with '0x' or '0X'.
**************************************************************************

Let's see the commands.

-------------------------
3.1 setredirection(sr) N1
-------------------------

The commands sets the type of redirection. If N1 is 0 no redirection is 
done, if N1 is 1 no logs will be displayed, instead of that logs will be 
copied to 'freialog.txt'. If N1 is 2 both 'freialog.txt' will be created 
and displaying will be done.

-------------------------
3.2 setdebuglevel (sd) N1
-------------------------

Only one argument which can start from 0 to 3.

0 : no debug info to be displayed
1 : low debug infos (mostly errors)
2 : medium debug infos (low infos + some useful information)
3 : high debug infos (all kind of information, can be used to
    trace the commnucation

--------------
3.3 comapp(ca) N1
--------------

The command specifies which COMx (1..4) serial port to use for
communication towards the phone. The default is COM1.

-------------------------
3.4 comapptype(cat) N1 N2
-------------------------

The command specifies whether to use DTR (N1 is 1) or not
(N1 is 0) and whether to use RTS (N2 is 1) or not (N2 is 0)
for communication. The default is to use both. Only modify
the setup in case you have boot-up problem.

-----------------
3.5 comdng(cd) N1
-----------------

The command specifies which COMx (1..4) serial port to use for
communication towards the dongle. The default is COM2.

---------------
3.6 speed(s) N1
---------------

The command overrides the default speed (56k) of the serial 
communication. Valid values are 9600, 19200, 38400, 57600,
115200, 230400 and 460800. Note that speed above 115k needs
(on the PC side) special drivers (like SHSMOD).

-----------------
3.7 emulate(e) S1
-----------------

The command selects emulation mode with the full flash file specified by 
S1. S1 must be a 'ksi' flash file. Selecting this option means that all 
operations (read, write, backup and erase) will be performed on the file
and not the phone itself. With this option you can create backup maps
based upon your saved flash content.

---------------------
3.8 rawemulate(re) S1
---------------------

The command selects emulation mode with the full flash file specified by 
S1. S1 must be a 'raw' flash file. Selecting this option means that 
all operations (read, write and erase) will be performed on the file 
and not the phone itself. With this option you can create backup maps
based upon your saved flash content.

-------------
3.9 nomap(nm)
-------------

The command must be used together with 'unlock'. If this option is
selected no unlock map file will be saved during unlocking.

-----------------
3.10 noupdate(nu)
-----------------

The command must be used together with 'unlock'. If this option is
selected no unlocking will be done just the map file will be saved.

--------------
3.11 backup(b)
--------------

The command creates a MAP file based upon the phone's current EEPROM
content. I.e. if the phone is locked you can use the saved MAP to
relock your phone later.

-----------------
3.12 unlock(u) S1
-----------------

This command unlocks the phone. By default an unlock MAP will be saved
AND the phone will be unlocked (i.e. the EEPROM will be updated).
So to make sure please backup your phone with 'backup'.

-------------------------
3.13 unlockfromlog(ul) S1
-------------------------

This command creates an unlock MAP from the log file (given by S1).

--------------------
3.14 writemap(wm) S1
--------------------

The command uploads the specified (by S1) MAP to the phone.

----------------------
3.15 rawread(rr) N1 N2
----------------------

To support 'raw' flash format (that's why it's called raw format 
- since it only has the bytes from the flash one by one) another command is 
implemented which behaves the same way the previous one. The only 
difference is the lack of the header and the extension which is 'fls'.

-----------------------------
3.16 rawwrite(rw) S1 N1 N2 N3
-----------------------------

To write a plain 'FLS' file to the phone you must specify the name of the 
flash file and the starting address as well. Note that NO checking is done 
regarding the starting address, so you can easily fuck up your phone. If 
you wish to write only fraction of the input file, you can set N2 to the 
offset you wish to start write from (0 is the very first of the file) and 
N3 to the length (in bytes) you wish to write (note that you must set N2 
and N3 both to zero to achieve "normal" behaviour, i.e. write the whole 
file).

---------------
3.17 decrypt(d)
---------------

This option is for decrypting the current content of the phone's
EEPROM blocks. This only supports AXX, S45, ME45 and the SLXX series.
With the decrypted blocks you can analyze the setup of the phone.

--------------------------
3.18 decryptfrommap(dm) S1
--------------------------

This option is for decrypting the content of the given (by S1) map.
This only supports AXX, S45, ME45 and the SLXX series.
With the decrypted blocks you can analyze the setup of the phone.

------------------------------
3.19 locktoprovider(ltp) N1 N2
------------------------------

This option is only to be used together with the unlocking and
map generation routines. This way you can create a locked MAP
or you can lock the phone to a specific provider. Set N1 as
21650 for example. To 'multiple lock' the phone, set N2 to other
than zero. To 'autolock to network' your phone, set N1 to 1.

----------------------
3.20 backupbattery(bb)
----------------------

This command backups block #67 AKA the battery parameters of the
phone. It saves it as a map file.

----------------------
3.21 setbattery(sb) S1
----------------------

This command restores block #67 AKA the battery parameters of the
phone based upon the values found in the map file given by S1.

-----------------------------
3.22 rawreadmemory(rrm) N1 N2
-----------------------------

This command behaves the very same way (regarding output) like command
'rawread', but it does the reading in BFB mode. It means that you have to
switch ON your phone. Please note that to be able to read "special"
addresses, you need to have your firmware patched.

-----------------
3.23 writelog(wl)
-----------------

This command creates a log from the phone. (i.e. saves phone id and
IMEI).

-------------------------
3.24 updateflashimei(ufi)
-------------------------

From A50/C55 on there's another IMEI in the OTP protection registers
of the flash chip. Theis register is locked to prevent further updates.
Apart from this, you might want to set this IMEI as well when - for
example - replacing the flash chip.
Please note that because of the OTP behaviour of the protection
register, it means that you can only write (theoretically) once the
contetn of the register. To be exact it means that you might write the
register as many times as many times you wish (if it's not locked),
but you can only make 1 to 0 transitions in the IMEI mask.
So it might turn out that you end-up with a different IMEI (different
from the one you requested to set).
This command is only a flag indicating that the flash IMEI shall be
updated after a successful unlocking. Please note that you might
"reunlock" with the "adjusted" flash IMEI (because of 1 to 0 transitions
only).

--------------------------------
3.25 unlockfrommassoflog(uml) S1
--------------------------------

With this command you might generate mass of unlock MAPs for mass of
logs. S1 indicates the directory of the LOGs.

--------------------
3.26 boottype(bt) N1
--------------------

This option sets the boottype. 1 is the "default", 2 is via patched
bootcore, 3 is via bootcore bug. (Note that you only have to change
this if you have x5x phone with 'new boot concept'. To be able to use
'2' you must patch the bootcore first with 3.27)

--------------------------------
3.27 patchbootcore(pb) [S1] [N1]
--------------------------------

This option patches the bootcore. If no option is used, it will first
read the bootcore and store the patched one in another file. Otherwise
it'll read the bootcore file given by S1. If N1 is 1 the bootcore file
must be binary (FLS) otherwise it has to be a KSI file.

--------------------
3.28 getdevinfo(gdi)
--------------------

This commands reads the dongle info.

--------------------
3.29 setdevinfo(sdi)
--------------------

This commands writes the dongle info.

------------------
3.30 updatedev(ud)
------------------

This commands updates the dongle.

------------------
3.31 testdev(td)
------------------

This commands tests the dongle.

---------------------
3.32 closeatonce(cao)
---------------------

Closes console without waiting for Enter.
