#include "code.h"

// 加密函数
zend_result encrypt_file_code(zend_string *encrypt_file)
{

    FILE *fp = fopen(encrypt_file->val, "r");

    if (!fp)
    {
        err_msg("Fail open ", encrypt_file->val);
        return FAILURE;
    }

    int key_len = strlen(YAOLING_ENCRYPT_LIB);

    struct stat statbuf;
    fstat(fileno(fp), &statbuf);
    int bodylen = statbuf.st_size;
    char *buffer = malloc(sizeof(char) * (bodylen + key_len) * 7);

    int ch;

    // key header
    for (int i = 0; i < key_len; i++)
    {
        short temp = (short)YAOLING_ENCRYPT_LIB[i];
        temp <<= 4;

        temp |= 0x8000;

        temp += rand() % 16;

        char str[7];

        sprintf(str, "%hd", temp);

        strcat(buffer, str);
    }

    // body
    while ((ch = fgetc(fp)) != EOF)
    {

        short temp = (short)ch;

        temp <<= 4;

        temp |= 0x8000;

        temp += rand() % 16;
        char str[7];

        sprintf(str, "%hd", temp);

        strcat(buffer, str);
    }

    fclose(fp);

    fp = fopen(encrypt_file->val, "wb");
    if (fp == NULL)
    {
        err_msg("Can not create encrypt file (%s)", encrypt_file->val);
        return FAILURE;
    }

    fwrite(buffer, 1, strlen(buffer), fp);

    free(buffer);

    fclose(fp);

    alert_msg("success encrypt file ", encrypt_file->val);
    return SUCCESS;
}

// 解密函数
zend_result decrypt_file_code(zend_string *decrypt_file, zend_string *decrypt_key)
{
    FILE *fp = fopen(decrypt_file->val, "r");
    zend_string *result = NULL;

    if (!fp)
    {
        err_msg("Fail open ", decrypt_file->val);

        return FAILURE;
    }

    // valid key header
    bool key_tag = true;
    short temp;
    // key header

    // key len
    if (decrypt_key->len != strlen(YAOLING_ENCRYPT_LIB))
    {
        err_msg("decrypt key is ", "illegal");
        goto fail;
    }

    for (int i = 0; i < decrypt_key->len; i++)
    {
        int a = fscanf(fp, "%hd", &temp);
        temp <<= 1;
        temp >>= 5;
        char ch = (char)temp;
        if (toascii(ch) - toascii(decrypt_key->val[i]) != 0 || !ch)
        {
            key_tag = false;
            break;
        }
    }

    if (key_tag == false)
    {
        err_msg("decrypt key is ", "illegal");
        goto fail;
    }

    bool decode = false;

    char *buffer = NULL;

    int reallen = 0;
    // body
    while (!feof(fp))
    {

        int a = fscanf(fp, "%hd", &temp);
        temp <<= 1;
        temp >>= 5;
        char ch = (char)temp;

        if (!ch)
        {
            break;
        }

        // 可以被解密
        if (decode == false)
        {
            struct stat statbuf;
            fstat(fileno(fp), &statbuf);
            int bodylen = statbuf.st_size;
            buffer = malloc(bodylen);
            decode = true;
        }

        buffer[reallen++] = ch;
    }

    // 不能解密，释放内存
    if (decode == false)
    {
        free(buffer);
        goto fail;
    }

    buffer = realloc(buffer, reallen);

    fclose(fp);

    fp = fopen(decrypt_file->val, "wb");
    if (fp == NULL)
    {
        err_msg("Can not create encrypt file", decrypt_file->val);
        return FAILURE;
    }

    fwrite(buffer, 1, strlen(buffer), fp);

    free(buffer);

    fclose(fp);

    alert_msg("success decrypt file ", decrypt_file->val);
    return SUCCESS;

fail:
{
    if (fp)
    {
        fclose(fp);
    }
    err_msg("Can not decrypt file ", decrypt_file->val);
    return FAILURE;
}
}

void err_msg(char *str, char *str2)
{
    printf("\033[40;31m%s%s\033[0m\n", str, str2);
}

void alert_msg(char *str, char *str2)
{
    printf("\033[1;32m%s%s\033[0m\n", str, str2);
}
