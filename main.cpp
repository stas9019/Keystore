#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include "getch.h"

#define ADD_KEY 1
#define GET_KEY 2
#define ENCRYPT_FILE 3
#define DECRYPT_FILE 4


void handleErrors(void);
int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext);
int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext);


int checkPassword()
{
    char secretPassword[20] = "qwerty";
    unsigned char *password = (unsigned char *) malloc(80);

    char c;
    int i =0;

    printf ("Enter keystore password: ");

    c = getch();
    while (c != '\n')
    {
        password[i] = c;
        c = getch();

        i++;
    }
    printf ("\n");

    if(strcmp(secretPassword, (char *)password) == 0)
        return 1;
    else
        return 0;

}


int main (int argc, char **argv)
{
    /* Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, dependant on the
     * algorithm and mode
     */
    unsigned char ciphertext[1024];

    /* Buffer for the decrypted text */
    unsigned char decryptedtext[1024];

    int decryptedtext_len, ciphertext_len;

    /* Initialise the library
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);*/


    FILE  *fIN;
    char path[80] = "/home/stas/ClionProjects/Keystore/";
    char outpath[80] = "/home/stas/ClionProjects/Keystore/";



    /* A 256 bit key */
    unsigned char *key = (unsigned char *)"01234567890123456789012345678901";

    /* A 128 bit IV */
    unsigned char *iv = (unsigned char *)"01234567890123456";


    int mode;
    char keyID[10];
    unsigned char plaintext[1024];

    /* Message to be encrypted
    unsigned char *plaintext =
            (unsigned char *)"The quick brown fox jumps over the lazy dog";*/

    if(argc < 1)
    {
        printf ("Usage: \n");
        printf ("To add new Key: [NewKeyID] [KEY] \n");
        printf ("To get key with known ID: [KeyID] \n");
        printf ("To encrypt text file with your key: encrypt/decrypt [filePath] [your Key]  \n");
    }


    /*Part for encrypting files*/
    if(argc == 4)
    {
        char command[10];
        char filePath[80];
        char userKey[260];

        sscanf(argv[2], "%s", filePath );
        sscanf(argv[3], "%s", userKey );


        sscanf(argv[1], "%s", command );
        if(strcmp(command, "encrypt") == 0)
            mode = ENCRYPT_FILE;
        else
            mode = DECRYPT_FILE;


        if(mode == ENCRYPT_FILE)
        {

            /* Decrypt the ciphertext*/
            fIN = fopen(filePath, "rb");
            //Get file size
            fseek(fIN, 0L, SEEK_END);
            int fsize = ftell(fIN);
            //set back to normal
            fseek(fIN, 0L, SEEK_SET);

            /*unsigned char *plaintext =
                    (unsigned char *)"The quick brown fox jumps over the lazy dog";*/

            int len = fread(plaintext, sizeof(unsigned char), fsize, fIN);


            fclose(fIN);


            ciphertext_len = encrypt (plaintext, fsize,
                                      (unsigned char *)userKey, iv, ciphertext);

            //ciphertext[ciphertext_len] = '\0';

            fIN = fopen(filePath, "wb");
            fwrite(ciphertext, sizeof(char) ,ciphertext_len, fIN);
            fclose(fIN);

            //printf("File encrypted\n %s\n", plaintext);

        }
        else if(mode == DECRYPT_FILE)
        {

            /* Decrypt the ciphertext*/
            fIN = fopen(filePath, "rb");
            //Get file size
            fseek(fIN, 0L, SEEK_END);
            int fsize = ftell(fIN);
            //set back to normal
            fseek(fIN, 0L, SEEK_SET);

            fread(ciphertext, sizeof(char), fsize, fIN);


            fclose(fIN);

            //ciphertext[fsize] = '\0';



            decryptedtext_len = decrypt (ciphertext, fsize,
                                      (unsigned char *)userKey, iv, decryptedtext);


            decryptedtext[decryptedtext_len] = '\0';

            /*unsigned char *plaintext =
                    (unsigned char *)"The quick brown fox jumps over the lazy dog";

            decryptedtext_len = strlen((char*)plaintext);*/

            fIN = fopen(filePath, "wb");
            fwrite(decryptedtext, sizeof(char) ,decryptedtext_len, fIN);
            fclose(fIN);

            //printf("File decrypted\n %s\n", decryptedtext);

        }
        else
        {
            printf("Wrong command \n");
        }


        return 0;
    }


    if(!checkPassword())          //TODO turn on
    {
        printf ("Wrong password \n");
        return 0;
    }

    sscanf(argv[1], "%s", keyID );
    //printf("keyID %s\n", keyID);

    if(argc == 3)
    {
        mode = ADD_KEY;//add key mode

        sscanf(argv[2], "%s", plaintext );
        //printf("plaintext %s\n", plaintext);

    }
    else if(argc == 2)
        mode = GET_KEY;




    if(mode == ADD_KEY)
    {
        /* Encrypt the plaintext */

        ciphertext_len = encrypt (plaintext, strlen ((char *)plaintext), key, iv, ciphertext);

        fIN = fopen(strcat(path,keyID), "wb");
        fwrite(ciphertext, sizeof(char) ,ciphertext_len, fIN);
        fclose(fIN);

        printf("Key added %s\n", plaintext);
        /*
        printf("ciphertext_len = %d", ciphertext_len);

         //Do something useful with the ciphertext here
        printf("Ciphertext is:\n");
        BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);*/


    }
    else if(mode == GET_KEY)
    {
        /* Decrypt the ciphertext*/
        fIN = fopen(strcat(path,keyID), "rb");
        //Get file size
        fseek(fIN, 0L, SEEK_END);
        int fsize = ftell(fIN);
        //set back to normal
        fseek(fIN, 0L, SEEK_SET);

        //printf("fsize = %d", fsize);

        fread(ciphertext, sizeof(unsigned char), fsize, fIN);
        fclose(fIN);

        decryptedtext_len = decrypt(ciphertext, fsize, key, iv, decryptedtext);


        /* Add a NULL terminator. We are expecting printable text */
        decryptedtext[decryptedtext_len] = '\0';

        FILE  *fOUT;
        fOUT = fopen(strcat(outpath,"temp"), "wb");
        fwrite(decryptedtext, sizeof(char) ,decryptedtext_len, fOUT);
        fclose(fOUT);

        /* Show the decrypted text */
        printf("Key extracted : ");
        printf("%s\n", decryptedtext);


    }




    /* Clean up */
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}



int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int ciphertext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /* Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;

    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}



int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;

    int len;

    int plaintext_len;

    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
        handleErrors();

    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */

    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;

    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
    plaintext_len += len;

    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}