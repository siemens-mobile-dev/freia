#include <stdio.h>

#define UINT16 unsigned short
#define BOOL int
#define UINT8 unsigned char

#define TRUE 1
#define FALSE !TRUE

static BOOL Hex2Bin (char *filename, char *outname)
{
    FILE                         *rfile, *sfile;

    UINT16                        U16address;
    UINT16                        U16written, U16reclen, U16type;
    UINT16                        i, U16numread;
    BOOL                          Bfinished;
    UINT8                         AU8buffer[4096];
    UINT16                        U16line = 1;

    rfile = fopen (filename, "rt");
    if (!rfile)
    {
        printf ("cannot open %s\n", filename);
        return FALSE;
    }
    sfile = fopen (outname, "wb");
    if (!sfile)
    {
        printf ("cannot create %s\n", outname);
        fclose (rfile);
        return FALSE;
    }

    do
    {
        U16numread =
            fscanf (rfile, ":%2x%4x%2x", &U16reclen, &U16address, &U16type);
        Bfinished = U16numread != 3;

        if (!Bfinished)
        {
            for (i = 0; i < U16reclen + 1; i++) /* the checksum as well */
            {
                if (i == U16reclen)
                {
                    U16numread = fscanf (rfile, "%2x\n", &AU8buffer[i]);
                }
                else
                {
                    U16numread = fscanf (rfile, "%2x", &AU8buffer[i]);
                }

                if (U16numread != 1)
                {
                    printf ("missing data in line %d\n", U16line);
                    fclose (rfile);
                    fclose (sfile);
                    return FALSE;
                }
            }

            if (U16type == 0)   /* convert only datatypes */
            {
                U16written = fwrite (AU8buffer, 1, U16reclen, sfile);
                if (U16written != U16reclen)
                {
                    printf ("write error in %s\n", outname);
                    fclose (rfile);
                    fclose (sfile);
                    return FALSE;
                }
            }
        }
        U16line++;
    }
    while (!Bfinished);

    fclose (rfile);
    fclose (sfile);
    return TRUE;
}

void main (int argc, char *argv[])
{
    if (argc >= 2)
    {
        printf ("HEX2BIN converter by Nutzo v0.0b\n");
        printf ("converting %s to %s\n", argv[1], argv[2]);
        Hex2Bin (argv[1], argv[2]);
    }
}
