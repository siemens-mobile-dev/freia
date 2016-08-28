#include <stdio.h>
#include <string.h>

#define UINT8 unsigned char

#define FREIA_WATERMARK_NAME_LEN 7

#define FREIA_WATERMARK_ID "\x32\x24\x31\x20\x37"

#define FREIA_WATERMARK_ID_LEN 5

#define FREIA_NAME_MASK "\xAA\x45\xFE\xDD\xE0\x77\x17\x21\xC0"

#define FREIA_NAME_MASK_LEN 9

static UINT8                  filebuffer[5 * 1024 * 1024];  /* 5 megs is cool */

void maskname (char *name, UINT8 * ptr)
{
    UINT8                         i;
    UINT8                         mask[] = FREIA_NAME_MASK;
    UINT8                         mask_len = FREIA_NAME_MASK_LEN;
    UINT8                         name_len =
        (strlen (name) >
         FREIA_WATERMARK_NAME_LEN ? FREIA_WATERMARK_NAME_LEN : strlen (name));
    UINT8                         namebuff[FREIA_WATERMARK_NAME_LEN + 1];

    memset (namebuff, 0, sizeof (namebuff));
    strncpy (namebuff, name, name_len);

    for (i = 0; i < name_len + 1; i++)
    {
        ptr[i] = namebuff[i] ^ mask[i % mask_len];
    }
}

int main (int argc, char *argv[])
{
    int                           length, i, j, cnt;
    FILE                         *rfile;
    UINT8                         watermark_id[] = FREIA_WATERMARK_ID;
    UINT8                         watermark_id_len = FREIA_WATERMARK_ID_LEN;
    char                          filename[128];
    int                           num_watermarks = 0;

    if (argc < 2)
    {
        printf ("give who to watermark to\n");
        return -1;
    }

    rfile = fopen ("freia_st.exe", "rb");
    if (!rfile)
    {
        printf ("cannot open 'freia_st.exe'\n");
        return -1;
    }

    length = fread (filebuffer, 1, sizeof (filebuffer), rfile);
    fclose (rfile);

    for (i = 0; i < length - 5; i++)
    {
        for (j = 0, cnt = 0; j < watermark_id_len; j++)
        {
            cnt += (filebuffer[i + j] == watermark_id[j]);
        }

        if (cnt == watermark_id_len)
        {
            printf ("watermarking at %08X\n", i);
            maskname (argv[1], &filebuffer[i + watermark_id_len + 1]);
            num_watermarks++;
        }
    }

    if (num_watermarks > 0)
    {
        strcpy (filename, "freia_st_");
        strcat (filename, argv[1]);
        strcat (filename, ".exe");

        rfile = fopen (filename, "wb");
        if (rfile)
        {
            fwrite (filebuffer, 1, length, rfile);
            fclose (rfile);
            printf ("watermarked exe is in '%s'\n", filename);
        }
    }
    return 0;
}
