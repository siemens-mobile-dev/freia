
/********************************************************************
*                                                                   *
* FILE NAME: reghc.c                                                *
*                                                                   *
* PURPOSE: Registry handling                                        *
*                                                                   *
* FILE REFERENCES:                                                  *
*                                                                   *
* Name:                                                             *
*                                                                   *
* I/O:                                                              *
*                                                                   *
* Description:                                                      *
*                                                                   *
* EXTERNAL VARIABLES:                                               *
*                                                                   *
* Source:  <      >                                                 *
*                                                                   *
* Name:                                                             *
*                                                                   *
* Type:                                                             *
*                                                                   *
* I/O:                                                              *
*                                                                   *
* Description:                                                      *
*                                                                   *
* EXTERNAL REFERENCES:                                              *
*                                                                   *
* Name:                                                             *
*                                                                   *
* Description:                                                      *
*                                                                   *
* ABNORMAL TERMINATION CONDITIONS, ERROR AND WARNING MESSAGES:      *
*                                                                   *
* ASSUMPTIONS, CONSTRAINTS, RESTRICTIONS:                           *
*                                                                   *
* NOTES:                                                            *
*                                                                   *
* REQUIREMENTS/FUNCTIONAL SPECIFICATIONS REFERENCES:                *
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
********************************************************************/

#include "config.h"

#include <Windows.h>
#include <string.h>
#include <stdio.h>

#include "sysdef.h"
#include "freiapub.h"
#include "regpub.h"
#include "regloc.h"

extern eERROR                 freia_errno;

static HKEY                   FCurrentKey = 0;
static HKEY                   FRootKey = 0;
static char                   FCurrentPath[SYS_MAX_PATHNAME_LEN] = "";
static BOOL                   FCloseRootKey = TRUE;

static void                   REG_CloseKey (void);
static HKEY                   REG_GetKey (const char *Key);
static BOOL                   REG_GetKeyInfo (tRegKeyInfo * Value);
static void                   REG_CloseKey (void);
static BOOL                   REG_PutData (const char *Name,
                                           const UINT8 * Buffer,
                                           UINT32 BufSize,
                                           eRegDataType RegData);
static BOOL                   REG_GetData (const char *Name,
                                           UINT8 * Buffer, UINT32 BufSize,
                                           UINT32 * pOutBufSize,
                                           eRegDataType * RegData);


/********************************************************
*                                                       *
* FUNCTION NAME: IsRelative                             *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a registry path                                       *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function checks whether the argument is a         *
* relative registry path or not                         *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the path is relative                          *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL IsRelative (const char *Value)
{
    return !((strlen (Value) > 0) && (Value[1] == '\\'));
}


/********************************************************
*                                                       *
* FUNCTION NAME: RegDataToDataType                      *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* an enumaration type                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* eRegDataType                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function converts the argument to an internal     *
* Win32 registry type                                   *
*                                                       *
* RETURNS:                                              *
*                                                       *
* the converted registry type                           *
*                                                       *
*********************************************************/

static UINT32 RegDataToDataType (eRegDataType Value)
{
    switch (Value)
    {
    case rdString:
        return REG_SZ;

    case rdExpandString:
        return REG_EXPAND_SZ;

    case rdInteger:
        return REG_DWORD;

    case rdBinary:
        return REG_BINARY;

    default:
        return REG_NONE;
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: DataTypeToRegData                      *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* an internal Win32 registry value                      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* UINT32                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function converts the argument to an enumeration  *
* type                                                  *
*                                                       *
* RETURNS:                                              *
*                                                       *
* the converted enumeration type                        *
*                                                       *
*********************************************************/

static eRegDataType DataTypeToRegData (UINT32 Value)
{
    switch (Value)
    {
    case REG_SZ:
        return rdString;

    case REG_EXPAND_SZ:
        return rdExpandString;

    case REG_DWORD:
        return rdInteger;

    case REG_BINARY:
        return rdBinary;

    default:
        return rdUnknown;
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_Create                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a global variable storing the actual root key         *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* FRootKey                                              *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function is the "constructor" of the library      *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_Create (void)
{
    FRootKey = HKEY_CURRENT_USER;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_Destroy                       *
*                                                       *
* ARGUMENTS: None                                       *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function is the "destructor"  of the library      *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_Destroy (void)
{
    REG_CloseKey ();
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_Destroy                       *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a global variable storing the currently opened key    *
*                                                       *
* a global variable storing the currently used path     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* FCurrentPath                                          *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* char[]                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function closes the current key and clears the    *
* internal variables                                    *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_CloseKey (void)
{
    if (FCurrentKey != 0)
    {
        RegCloseKey (FCurrentKey);
        FCurrentKey = 0;
        FCurrentPath[0] = '\0';
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_SetRootKey                    *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired new root key                              *
*                                                       *
* a global variable storing the currently used root key *
*                                                       *
* a global flag telling whether to close the root key   *
* or not                                                *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* FRootKey                                              *
*                                                       *
* FCloseRootKey                                         *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* BOOL                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function changes the root key                     *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_SetRootKey (HKEY Value)
{
    if (FRootKey != Value)
    {
        if (FCloseRootKey)
        {
            RegCloseKey (FRootKey);
            FCloseRootKey = FALSE;
        }
        FRootKey = Value;
        REG_CloseKey ();
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_ChangeKey                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the new root key                                      *
*                                                       *
* the new path                                          *
*                                                       *
* a global variable storing the currently opened key    *
*                                                       *
* a global variable storing the currently used path     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* Path                                                  *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* FCurrentPath                                          *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* char *                                                *
*                                                       *
* HKEY                                                  *
*                                                       *
* char[]                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the currently used key           *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_ChangeKey (HKEY Value, const char *Path)
{
    REG_CloseKey ();
    FCurrentKey = Value;
    strcpy (FCurrentPath, Path);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_ChangeKey                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a flag whether to return a relative or absolute       *
* base                                                  *
*                                                       *
* a global variable storing the currently opened key    *
*                                                       *
* a global variable storing the currently used root key *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Relative                                              *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* FRootKey                                              *
*                                                       *
* TYPE:                                                 *
*                                                       *
* BOOL                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the currently used "base" key       *
*                                                       *
* RETURNS:                                              *
*                                                       *
* The base key                                          *
*                                                       *
*********************************************************/

static HKEY REG_GetBaseKey (BOOL Relative)
{
    if (FCurrentKey == 0 || !Relative)
    {
        return FRootKey;
    }

    return FCurrentKey;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_SetCurrentKey                 *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired new key                                   *
*                                                       *
* a global variable storing the currently opened key    *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function sets the currently used key              *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_SetCurrentKey (HKEY Value)
{
    FCurrentKey = Value;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_CreateKey                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired key                                       *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Key                                                   *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to create the given key            *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_CreateKey (const char *Key)
{
    HKEY                          TempKey;
    char                          S[SYS_MAX_STRING_LEN];
    char                         *pS = S;
    UINT32                        Disposition;
    BOOL                          Relative;
    BOOL                          Bresult;

    TempKey = 0;
    strcpy (S, Key);

    Relative = IsRelative (S);

    if (!Relative)
    {
        pS++;                   /* skip leading '\' */
    }

    Bresult =
        RegCreateKeyEx (REG_GetBaseKey (Relative), pS, 0, NULL,
                        REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &TempKey,
                        &Disposition) == ERROR_SUCCESS;

    if (Bresult)
    {
        RegCloseKey (TempKey);
    }
    else
    {
        freia_errno = KE_CANNOT_CREATE_KEY;
    }

    return Bresult;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_OpenKey                       *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired key                                       *
*                                                       *
* a flag whether the key can be created if cannot be    *
* opened                                                *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Key                                                   *
*                                                       *
* CanCreate                                             *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* BOOL                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to open the given key. If it       *
* fails, it tries to create it (if the flag allows it)  *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_OpenKey (const char *Key, BOOL CanCreate)
{
    HKEY                          TempKey;
    char                          S[SYS_MAX_STRING_LEN],
        buffer[SYS_MAX_STRING_LEN];
    char                         *pS = S;
    UINT32                        Disposition;
    BOOL                          Relative;
    BOOL                          Bresult;

    strcpy (S, Key);
    Relative = IsRelative (S);

    if (!Relative)
    {
        pS++;
    }

    TempKey = 0;

    if (!CanCreate || strlen (pS) == 0)
    {
        Bresult =
            RegOpenKeyEx (REG_GetBaseKey (Relative), pS, 0, KEY_ALL_ACCESS,
                          &TempKey) == ERROR_SUCCESS;
    }
    else
    {
        Bresult =
            RegCreateKeyEx (REG_GetBaseKey (Relative), pS, 0, NULL,
                            REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
                            &TempKey, &Disposition) == ERROR_SUCCESS;
    }

    if (Bresult)
    {
        if (FCurrentKey != 0 && Relative)
        {
            strcpy (buffer, FCurrentPath);
            strcat (buffer, "\\");
            strcat (buffer, S);
            strcpy (S, buffer);
        }
        REG_ChangeKey (TempKey, S);
    }
    else
    {
        freia_errno = KE_CANNOT_OPEN_KEY;
    }

    return Bresult;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_DeleteKey                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired key                                       *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Key                                                   *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* HKEY                                                  *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to delete the given key            *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_DeleteKey (const char *Key)
{
    UINT32                        i, Len;
    BOOL                          Relative;
    char                          S[SYS_MAX_STRING_LEN], *pS =
        S, KeyName[SYS_MAX_STRING_LEN];
    HKEY                          OldKey, DeleteKey;
    tRegKeyInfo                   Info;
    BOOL                          Bsuccess;

    strcpy (S, Key);
    Relative = IsRelative (S);

    if (!Relative)
    {
        pS++;
    }

    OldKey = FCurrentKey;
    DeleteKey = REG_GetKey (Key);
    if (DeleteKey != 0)
    {
        REG_SetCurrentKey (DeleteKey);
        if (REG_GetKeyInfo (&Info))
        {
            memset (KeyName, 0, Info.MaxSubKeyLen);
            for (i = 0; i < Info.NumSubKeys; i++)
            {
                Len = Info.MaxSubKeyLen + 1;
                if (RegEnumKeyEx
                    (DeleteKey, i, KeyName, &Len, NULL, NULL, NULL,
                     NULL) == ERROR_SUCCESS)
                {
                    REG_DeleteKey (KeyName);
                }
            }
        }
    }

    REG_SetCurrentKey (OldKey);
    RegCloseKey (DeleteKey);

    Bsuccess = RegDeleteKey (REG_GetBaseKey (Relative), pS) == ERROR_SUCCESS;

    if (!Bsuccess)
    {
        freia_errno = KE_CANNOT_DELETE_KEY;
    }
    return Bsuccess;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_DeleteValue                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the desired name                                      *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to delete the value corresponding  *
* to the actual key                                     *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_DeleteValue (const char *Name)
{
    BOOL                          Bsuccess;

    Bsuccess = RegDeleteValue (FCurrentKey, Name) == ERROR_SUCCESS;
    if (!Bsuccess)
    {
        freia_errno = KE_CANNOT_DELETE_VALUE;
    }

    return Bsuccess;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetKeyInfo                    *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a pointer to the info structure                       *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Value                                                 *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* tRegKeyInfo *                                         *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the information about the currently *
* used key                                              *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_GetKeyInfo (tRegKeyInfo * Value)
{
    memset (Value, 0, sizeof (tRegKeyInfo));

    return RegQueryInfoKey (FCurrentKey, NULL, NULL, NULL,
                            &Value->NumSubKeys, &Value->MaxSubKeyLen, NULL,
                            &Value->NumValues, &Value->MaxValueLen,
                            &Value->MaxDataLen, NULL,
                            &Value->FileTime) == ERROR_SUCCESS;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetKeyNames                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a stringarray                                         *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* strings                                               *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *[]                                              *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the name of the keys corresponding  *
* with the currently used key                           *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_GetKeyNames (char *strings[])
{
    UINT32                        i, Len;
    tRegKeyInfo                   Info;
    char                          S[SYS_MAX_STRING_LEN];

    strings[0] = "\0";
    if (REG_GetKeyInfo (&Info))
    {
        memset (S, 0, Info.MaxSubKeyLen + 1);
        for (i = 0; i < Info.NumSubKeys; i++)
        {
            Len = Info.MaxSubKeyLen + 1;
            RegEnumKeyEx (FCurrentKey, i, S, &Len, NULL, NULL, NULL, NULL);
            strcpy (strings[i], S);
        }
        strings[i] = "\0";
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetValueNames                 *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a stringarray                                         *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* strings                                               *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* char *[]                                              *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the name of the values              *
* corresponding with the currently used key             *
*                                                       *
* RETURNS: Nothing                                      *
*                                                       *
*********************************************************/

static void REG_GetValueNames (char *strings[])
{
    UINT32                        i, Len;
    tRegKeyInfo                   Info;
    char                          S[SYS_MAX_STRING_LEN];

    strings[0] = "\0";
    if (REG_GetKeyInfo (&Info))
    {
        memset (S, 0, Info.MaxValueLen + 1);
        for (i = 0; i < Info.NumValues; i++)
        {
            Len = Info.MaxValueLen + 1;
            RegEnumValue (FCurrentKey, i, S, &Len, NULL, NULL, NULL, NULL);
            strcpy (strings[i], S);
        }
        strings[i] = "\0";
    }
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetDataInfo                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* a stringarray                                         *
*                                                       *
* a global variable storing the currently used key      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* ValueName                                             *
*                                                       *
* Value                                                 *
*                                                       *
* FCurrentKey                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* tRegDataInfo *                                        *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the information of the value        *
* corresponding with the currently used key             *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_GetDataInfo (const char *ValueName, tRegDataInfo * Value)
{
    UINT32                        DataType;
    BOOL                          Bresult;

    memset (Value, 0, sizeof (tRegDataInfo));

    Bresult =
        RegQueryValueEx (FCurrentKey, ValueName, NULL, &DataType, NULL,
                         &Value->DataSize) == ERROR_SUCCESS;

    Value->RegData = DataTypeToRegData (DataType);

    if (!Bresult)
    {
        freia_errno = KE_VALUE_NOT_EXIST;
    }

    return Bresult;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetDataSize                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the value                                 *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* ValueName                                             *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the size of the given value         *
*                                                       *
* RETURNS:                                              *
*                                                       *
* the size if the operation went just fine              *
*                                                       *
* -1 otherwise                                          *
*                                                       *
*********************************************************/

static SINT32 REG_GetDataSize (const char *ValueName)
{
    tRegDataInfo                  Info;

    if (REG_GetDataInfo (ValueName, &Info))
    {
        return Info.DataSize;
    }

    return -1;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetDataType                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the value                                 *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* ValueName                                             *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the type of the keys corresponding  *
* with the currently used key                           *
*                                                       *
* RETURNS:                                              *
*                                                       *
* the enumeration type if everything went just fine     *
*                                                       *
* rdUnknown otherwise                                   *
*                                                       *
*********************************************************/

static eRegDataType REG_GetDataType (const char *ValueName)
{
    tRegDataInfo                  Info;

    if (REG_GetDataInfo (ValueName, &Info))
    {
        return Info.RegData;
    }

    freia_errno = KE_INVALID_VALUE_TYPE;
    return rdUnknown;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteString                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired value                                     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteString (const char *Name, const char *Value)
{
    return REG_PutData (Name, (const UINT8 *)Value, strlen (Value), rdString);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteExpandedString           *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired value                                     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteExpandString (const char *Name, const char *Value)
{
    return REG_PutData (Name, (const UINT8 *)Value, strlen (Value), rdExpandString);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_ReadString                    *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the value of the key                                  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Result                                                *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* char *                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ReadString (const char *Name, char *Result)
{
    UINT32                        Len;
    eRegDataType                  RegData;

    Result[0] = '\0';

    Len = REG_GetDataSize (Name);
    if (Len > 0)
    {
        memset (Result, 0, Len);
        if (!REG_GetData (Name, (UINT8 *)Result, Len, NULL, &RegData))
        {
            return FALSE;
        }

        if (RegData != rdString && RegData != rdExpandString)
        {
            return FALSE;
        }
        return TRUE;
    }
    return FALSE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteInteger                  *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired value                                     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT32                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteInteger (const char *Name, UINT32 Value)
{
    return REG_PutData (Name, (UINT8 *) & Value, sizeof (UINT32), rdInteger);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_ReadInteger                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the value of the key                                  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* pU32result                                            *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT32 *                                              *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ReadInteger (const char *Name, UINT32 * pU32result)
{
    eRegDataType                  RegData;

    if (!REG_GetData
        (Name, (UINT8 *) pU32result, sizeof (UINT32), NULL, &RegData))
    {
        return FALSE;
    }

    return (RegData == rdInteger);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteBool                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired value                                     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* BOOL                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteBool (const char *Name, BOOL Value)
{
    return REG_WriteInteger (Name, (UINT32) Value);
}



/********************************************************
*                                                       *
* FUNCTION NAME: REG_ReadBool                      *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the value of the key                                  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* pBresult                                              *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* BOOL *                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ReadBool (const char *Name, BOOL * pBresult)
{
    BOOL                          Bsuccess;
    UINT32                        U32result;

    Bsuccess = REG_ReadInteger (Name, &U32result);
    if (Bsuccess)
    {
        *pBresult = U32result == TRUE;
    }

    return Bsuccess;
}



/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteFloat                    *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired value                                     *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* double                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteFloat (const char *Name, const double Value)
{
    return REG_PutData (Name, (const UINT8 *) & Value, sizeof (double), rdBinary);
}



/********************************************************
*                                                       *
* FUNCTION NAME: REG_ReadFloat                     *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the value of the key                                  *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* pdResult                                              *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* double *                                              *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ReadFloat (const char *Name, double *pdResult)
{
    UINT32                        Len;
    eRegDataType                  RegData;

    if (!REG_GetData
        (Name, (UINT8 *) pdResult, sizeof (double), &Len, &RegData))
    {
        return FALSE;
    }

    return (RegData == rdBinary) && (Len == sizeof (double));
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_WriteBinaryData               *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the desired binary stream                             *
*                                                       *
* length of the binary stream                           *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Buffer                                                *
*                                                       *
* BufSize                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT8 *                                               *
*                                                       *
* UINT32                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_WriteBinaryData (const char *Name, UINT8 * Buffer,
                                 UINT32 BufSize)
{
    return REG_PutData (Name, Buffer, BufSize, rdBinary);
}



/********************************************************
*                                                       *
* FUNCTION NAME: REG_ReadBinaryData                *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the output binary stream                              *
*                                                       *
* the maximum length of the output stream               *
*                                                       *
* the retrieved length of the stream                    *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Buffer                                                *
*                                                       *
* BufSize                                               *
*                                                       *
* pOutBufSize                                           *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT8 *                                               *
*                                                       *
* UINT32                                                *
*                                                       *
* UINT32 *                                              *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ReadBinaryData (const char *Name, UINT8 * Buffer,
                                UINT32 BufSize, UINT32 * pOutBufSize)
{
    eRegDataType                  RegData;
    tRegDataInfo                  Info;

    if (REG_GetDataInfo (Name, &Info))
    {
        *pOutBufSize = Info.DataSize;
        RegData = Info.RegData;
        if (RegData != rdBinary || *pOutBufSize >= BufSize)
        {
            return FALSE;
        }

        return REG_GetData (Name, Buffer, *pOutBufSize, NULL, &RegData);
    }

    return FALSE;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_PutData                       *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the input binary stream                               *
*                                                       *
* the length of the binary stream                       *
*                                                       *
* the desired type                                      *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Buffer                                                *
*                                                       *
* BufSize                                               *
*                                                       *
* RegData                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT8 *                                               *
*                                                       *
* UINT32                                                *
*                                                       *
* eRegDataType                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function updates the given key with the given     *
* value                                                 *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_PutData (const char *Name, const UINT8 * Buffer,
                         UINT32 BufSize, eRegDataType RegData)
{
    UINT32                        DataType;
    BOOL                          Bsuccess;

    DataType = RegDataToDataType (RegData);
    Bsuccess = RegSetValueEx (FCurrentKey, Name, 0, DataType, Buffer,
                              BufSize) == ERROR_SUCCESS;
    if (!Bsuccess)
    {
        freia_errno = KE_CANNOT_PUT_DATA;
    }

    return Bsuccess;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetData                       *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* the output binary stream                              *
*                                                       *
* the maximum length of the output stream               *
*                                                       *
* the retrieved length of the stream                    *
*                                                       *
* the type of the key                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* Buffer                                                *
*                                                       *
* BufSize                                               *
*                                                       *
* pOutBufSize                                           *
*                                                       *
* RegData                                               *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* UINT8 *                                               *
*                                                       *
* UINT32                                                *
*                                                       *
* UINT32 *                                              *
*                                                       *
* eRegDataType *                                        *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function gets the value of the key                *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_GetData (const char *Name, UINT8 * Buffer, UINT32 BufSize,
                         UINT32 * pOutBufSize, eRegDataType * RegData)
{
    UINT32                        DataType;
    BOOL                          Bsuccess;

    DataType = REG_NONE;
    Bsuccess =
        RegQueryValueEx (FCurrentKey, Name, NULL, &DataType, Buffer,
                         &BufSize) == ERROR_SUCCESS;
    *RegData = DataTypeToRegData (DataType);

    if (pOutBufSize)
    {
        *pOutBufSize = BufSize;
    }

    if (!Bsuccess)
    {
        freia_errno = KE_CANNOT_GET_DATA;
    }

    return Bsuccess;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_HasSubKeys                    *
*                                                       *
* ARGUMENTS: Nothing                                    *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* TYPE:                                                 *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function returns whether the currently used       *
* keys has subkeys                                      *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the key has subkeys                           *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_HasSubKeys (void)
{
    tRegKeyInfo                   Info;

    return REG_GetKeyInfo (&Info) && (Info.NumSubKeys > 0);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_ValueExists                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Name                                                  *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function checks whether the given key exists or   *
* not                                                   *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the key exists                                *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_ValueExists (const char *Name)
{
    tRegDataInfo                  Info;

    return REG_GetDataInfo (Name, &Info);
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_GetKey                        *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the key                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Keys                                                  *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function returns the key corresponding to the     *
* argument                                              *
*                                                       *
* RETURNS:                                              *
*                                                       *
* the corresponding key                                 *
*                                                       *
* 0 otherwise                                           *
*                                                       *
*********************************************************/

static HKEY REG_GetKey (const char *Key)
{
    char                          S[SYS_MAX_STRING_LEN], *pS = S;
    BOOL                          Relative;
    HKEY                          Result = 0;

    strcpy (S, Key);

    Relative = IsRelative (S);

    if (!Relative)
    {
        pS++;
    }

    RegOpenKeyEx (REG_GetBaseKey (Relative), pS, 0, KEY_ALL_ACCESS, &Result);
    return Result;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_KeyExists s                   *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* the name of the Key                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* Key                                                   *
*                                                       *
* TYPE:                                                 *
*                                                       *
* const char *                                          *
*                                                       *
* HKEY                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function checks whether the key exists or not     *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the key exists                                *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

static BOOL REG_KeyExists (const char *Key)
{
    HKEY                          TempKey;
    BOOL                          Bresult = FALSE;

    TempKey = REG_GetKey (Key);

    if (TempKey != 0)
    {
        RegCloseKey (TempKey);
        Bresult = TRUE;
    }

    return Bresult;
}


/********************************************************
*                                                       *
* FUNCTION NAME: REG_Load                          *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* name of the root                                      *
*                                                       *
* name of the key                                       *
*                                                       *
* name of the subkey                                    *
*                                                       *
* value of the subkey                                   *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* rootkey                                               *
*                                                       *
* base                                                  *
*                                                       *
* ident                                                 *
*                                                       *
* value                                                 *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* const char *                                          *
*                                                       *
* const char *                                          *
*                                                       *
* void *                                                *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function reads the given key                      *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

BOOL REG_Load (HKEY rootkey, const char *base, const char *ident, void *value)
{
    eRegDataType                  regtype;
    char                          tmpstr[SYS_MAX_STRING_LEN];
    BOOL                          Bsuccess;

    FRootKey = rootkey;

    memset (tmpstr, 0, sizeof (tmpstr));
    sprintf (tmpstr, "SOFTWARE\\%s", base);

    if (!REG_OpenKey (tmpstr, FALSE))
    {
        return FALSE;
    }

    if (!REG_ValueExists (ident))
    {
        REG_CloseKey ();
        return FALSE;
    }

    regtype = REG_GetDataType (ident);

    if (regtype == rdUnknown)
    {
        REG_CloseKey ();
        return FALSE;
    }

    if (regtype == rdString || regtype == rdExpandString)
    {
        Bsuccess = REG_ReadString (ident, value);
    }
    else if (regtype == rdInteger)
    {
        Bsuccess = REG_ReadInteger (ident, value);
    }

    REG_CloseKey ();
    return Bsuccess;
}



/********************************************************
*                                                       *
* FUNCTION NAME: REG_Save                          *
*                                                       *
* ARGUMENTS:                                            *
*                                                       *
* name of the root                                      *
*                                                       *
* name of the key                                       *
*                                                       *
* name of the subkey                                    *
*                                                       *
* value of the subkey                                   *
*                                                       *
* type of the subkey                                    *
*                                                       *
* whether it can be created                             *
*                                                       *
* ARGUMENT NAME:                                        *
*                                                       *
* rootkey                                               *
*                                                       *
* base                                                  *
*                                                       *
* ident                                                 *
*                                                       *
* value                                                 *
*                                                       *
* regtype                                               *
*                                                       *
* cancreate                                             *
*                                                       *
* TYPE:                                                 *
*                                                       *
* HKEY                                                  *
*                                                       *
* const char *                                          *
*                                                       *
* const char *                                          *
*                                                       *
* void *                                                *
*                                                       *
* eRegDataType                                          *
*                                                       *
* BOOL                                                  *
*                                                       *
* I/O:                                                  *
*                                                       *
* DESCRIPTION                                           *
*                                                       *
* The function tries to update (or create) the given    *
* key                                                   *
*                                                       *
* RETURNS:                                              *
*                                                       *
* TRUE if the operation went just fine                  *
*                                                       *
* FALSE otherwise                                       *
*                                                       *
*********************************************************/

BOOL REG_Save (HKEY rootkey, const char *base, const char *ident,
               void *value, eRegDataType regtype, BOOL cancreate)
{
    char                          tmpstr[SYS_MAX_STRING_LEN];
    BOOL                          Bsuccess;

    FRootKey = rootkey;

    memset (tmpstr, 0, sizeof (tmpstr));
    sprintf (tmpstr, "SOFTWARE\\%s", base);

    if (!REG_OpenKey (tmpstr, cancreate))
    {
        return FALSE;
    }

    if (regtype == rdString || regtype == rdExpandString)
    {
        Bsuccess = REG_WriteString (ident, (const char *) value);
    }
    else if (regtype == rdInteger)
    {
        Bsuccess = REG_WriteInteger (ident, *((UINT32 *) value));
    }

    REG_CloseKey ();
    return Bsuccess;
}
