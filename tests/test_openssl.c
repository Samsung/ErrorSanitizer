/*
    Copyright (c) 2018, Samsung Electronics Co., Ltd. All rights reserved.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; version 2 of the License.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA

    Author: Jakub Botwicz <j.botwicz@samsung.com>
*/
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <unistd.h>

#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  exit(-5);
}

void calculate_hash_correct(void)
{
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    char mess1[] = "Test Message\n";
    char mess2[] = "Hello World\n";
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    md = EVP_get_digestbyname("SHA1");
    if(!md) {
        printf("Unknown message digest %s\n", "SHA1");
        exit(1);
    }

    // mdctx = EVP_MD_CTX_new();
    mdctx = EVP_MD_CTX_create();
    if (NULL == mdctx) {
        printf("EVP_MD_CTX_create : failed allocation\n");
        return;
    }
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, mess1, strlen(mess1));
    EVP_DigestUpdate(mdctx, mess2, strlen(mess2));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    // EVP_MD_CTX_free(mdctx);
    EVP_MD_CTX_destroy(mdctx);

    printf("Digest is: ");
    for(i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("\n");
}

void calculate_hash_incorrect(void)
{
    OpenSSL_add_all_algorithms();

    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    char mess1[] = "Test Message\n";
    char mess2[] = "Hello World\n";
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    md = EVP_get_digestbyname("SHA1");
    if(!md) {
        printf("Unknown message digest %s\n", "SHA1");
        exit(1);
    }

    // mdctx = EVP_MD_CTX_new();
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, mess1, strlen(mess1));
    EVP_DigestUpdate(mdctx, mess2, strlen(mess2));
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    // EVP_MD_CTX_free(mdctx);
    EVP_MD_CTX_destroy(mdctx);

    printf("Digest is: ");
    for(i = 0; i < md_len; i++)
        printf("%02x", md_value[i]);
    printf("\n");
}

int encrypt_correct(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    handleErrors();
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
    handleErrors();
    ciphertext_len = len;

    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

int encrypt_incorrect(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;

    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext_len;
}

typedef int (*encrypt_func_ptr_t)(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext);

int test_aes(encrypt_func_ptr_t encrypt_func_ptr)
{
    unsigned char *key = (unsigned char *)"01234567890123456789012345678901";
    unsigned char *iv = (unsigned char *)"0123456789012345";
    unsigned char *plaintext =
                (unsigned char *)"The quick brown fox jumps over the lazy dog";
    unsigned char ciphertext[128];
    unsigned char decryptedtext[128];

    int decryptedtext_len, ciphertext_len;

    ciphertext_len = encrypt_func_ptr(plaintext, strlen ((char *)plaintext), key, iv,
                            ciphertext);

    printf("Ciphertext is:\n");
    BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);
    return 0;
}

int perform_testing(uint8_t* buffer_ptr, size_t buffer_size)
{
    OpenSSL_add_all_algorithms();

    // correct usage with error handling
    {
        test_aes(encrypt_correct);
        calculate_hash_correct();
    }
    printf("\nCorrect usage test finished!!!\n\n");

    // incorrect usage without error handling
    {
        test_aes(encrypt_incorrect);
        calculate_hash_incorrect();
    }
    printf("\nIncorrect usage test finished!!!\n\n");

    return 1;
}

