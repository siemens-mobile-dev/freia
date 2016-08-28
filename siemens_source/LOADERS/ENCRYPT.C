#include <stdio.h>

#define UINT8 unsigned char
#define UINT16 unsigned short

#define DECRYPT_OFS (0xBC0-0x200)
#define DECRYPT_LEN 0x600
#define ENRYPTION_KEY_LEN 16

static UINT8                  AU8buffer[16384];

static UINT8            EncryptionKey[ENRYPTION_KEY_LEN] =
    { 0xAB, 0x04, 0x44, 0x15, 0x43, 0xFE, 0xCE, 0xFF,
    0xBA, 0x40, 0xEF, 0x51, 0x34, 0xEF, 0xEC, 0x10
};

void vigenere_encrypt (UINT8 * key, UINT16 U16keylen, UINT8 * text,
                       UINT16 U16textlen, UINT8 * chiffre)
{
    UINT16                        i, k = 0, j = 0;
    UINT16                        zp, kp;

    for (i = 0; i < U16textlen; i++)
    {
        zp = text[i];
        kp = key[j];
        chiffre[k++] = (zp + kp) % 256;
        j = (j + 1) % U16keylen;
    }
}

int main (int argc, char *argv[])
{
    UINT16                        U16len;
    FILE                         *rfile;

    if (argc < 2)
    {
        return -1;
    }

    rfile = fopen (argv[1], "rb");
    if (!rfile)
    {
        printf ("open error %s\n", argv[1]);
        return -1;
    }

    U16len = fread (AU8buffer, 1, sizeof (AU8buffer), rfile);
    fclose (rfile);

    if (U16len < (DECRYPT_OFS + DECRYPT_LEN))
    {
        printf ("%s is too small\n", argv[1]);
        return -1;
    }

    vigenere_encrypt (EncryptionKey, ENRYPTION_KEY_LEN, &AU8buffer[DECRYPT_OFS],
                      DECRYPT_LEN, &AU8buffer[DECRYPT_OFS]);

    rfile = fopen (argv[1], "wb");
    if (!rfile)
    {
        printf ("cannot create %s\n", argv[1]);
        return -1;
    }

    if ((UINT16)fwrite (AU8buffer, 1, U16len, rfile) != U16len)
    {
        printf ("write error to %s\n", argv[1]);
        fclose (rfile);
        return -1;
    }

    fclose (rfile);

    printf("%s encrypted successfully.\n",argv[1]);
    return 0;
}
