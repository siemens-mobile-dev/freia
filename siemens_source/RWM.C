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

    for (i = 0; i < FREIA_WATERMARK_NAME_LEN + 1; i++)
    {
        name[i] = ptr[i] ^ mask[i % FREIA_NAME_MASK_LEN];
    }
}

int main (int argc, char *argv[])
{
    int                           length, i, j, cnt;
    FILE                         *rfile;
    UINT8                         watermark_id[] = FREIA_WATERMARK_ID;
    UINT8                         watermark_id_len = FREIA_WATERMARK_ID_LEN;
    char                          filename[128], owner[32];

    if (argc < 2)
    {
        printf ("give exe name as argument\n");
        return -1;
    }

    rfile = fopen (argv[1], "rb");
    if (!rfile)
    {
        printf ("cannot open '%s'\n", argv[1]);
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
            printf ("reading %d watermarking at %08X\n", filebuffer[i + watermark_id_len], i);
            maskname (owner, &filebuffer[i + watermark_id_len + 1]);
            printf ("owner is '%s'\n", owner);
        }
    }

    return 0;
}
