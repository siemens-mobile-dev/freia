#include <stdio.h>

#define UINT16 unsigned short
#define BOOL int
#define UINT8 unsigned char

#define TRUE 1
#define FALSE !TRUE

static BOOL Bin2Inc (char *filename, char *outname)
{
    FILE                         *rfile, *sfile;
    UINT16                        i, U16numread;
    UINT8                         AU8buffer[4096];

    rfile = fopen (filename, "rb");
    if (!rfile)
    {
        printf ("cannot open %s\n", filename);
        return FALSE;
    }
    sfile = fopen (outname, "wt");
    if (!sfile)
    {
        printf ("cannot create %s\n", outname);
        fclose (rfile);
        return FALSE;
    }

    fprintf(sfile, "{\n");
    do
    {
        U16numread = fread (AU8buffer, 1, sizeof(AU8buffer), rfile);
        if (U16numread>0)
        {
            for (i=0;i<U16numread;i++)
            {
               fprintf(sfile,"0x%02X, ",AU8buffer[i]);
               if ((i%8)==7)
               {
                  fprintf(sfile,"\n");
               }
            }
        }
    } while (U16numread>0);
    fprintf(sfile, "};\n");

    fclose (rfile);
    fclose (sfile);
    return TRUE;
}

void main (int argc, char *argv[])
{
    if (argc >= 2)
    {
        printf ("BIN2INC converter by Nutzo v0.0b\n");
        printf ("converting %s to %s\n", argv[1], argv[2]);
        Bin2Inc (argv[1], argv[2]);
    }
}
