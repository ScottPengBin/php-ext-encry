#include "code.h"

// 加密函数
zend_result encrypt_file_code(zend_string *encrypt_file)
{

    FILE *fp = fopen(encrypt_file, "rw");

    if (!fp)
    {
        err_msg("Fail open ", encrypt_file->val);
        return FAILURE;
    }

    FILE *tmp = tmpfile();

    struct stat statbuf;
    fstat(fileno(fp), &statbuf);
    int bodylen = statbuf.st_size;
    int key_len = strlen(YAOLING_ENCRYPT_LIB);

    char *realbuf = malloc(sizeof(short) * (bodylen + key_len));

    int ch;

    // key header
    for (int i = 0; i < key_len; i++)
    {
        short temp = (short)YAOLING_ENCRYPT_LIB[i];
        temp <<= 4;

        temp |= 0x8000;

        temp += rand() % 16;
        // 加密字符放入文件中
        fprintf(tmp, "%hd", temp);
    }

    // body
    while ((ch = fgetc(fp)) != EOF)
    {

        short temp = (short)ch;

        temp <<= 4;

        temp |= 0x8000;

        temp += rand() % 16;
        // 加密字符放入文件中
        fprintf(tmp, "%hd", temp);
    }
    fprintf(tmp, "%c", ";");
    fclose(fp);
    fp = tmp;
    return SUCCESS;
}

// 解密函数
zend_result decrypt_file_code(zend_string *decrypt_file, zend_string *decrypt_key)
{
    FILE *fp = fopen(decrypt_file, "rw");
    zend_string *result = NULL;

    if (!fp)
    {
        err_msg("Fail open ", decrypt_file->val);

        return FAILURE;
    }

    // valid key header
    int key_len = strlen(YAOLING_ENCRYPT_LIB);

    short temp;

    // key header
    for (int i = 0; i < key_len; i++)
    {
        int a = fscanf(fp, "%hd", &temp);
        temp <<= 1;
        temp >>= 5;
        char ch = (char)temp;

        if (strcmp(ch, YAOLING_ENCRYPT_LIB[i]) != 0 || !ch)
        {
            err_msg("decrypt key  is ", "illegal");

            goto fail;
        }
    }


    FILE *tmp = tmpfile();

    //body
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
        fputc(tmp, fp);
    }

    fclose(fp);
    fp = tmp;

    return SUCCESS;
    
fail:
{
    if (fp)
    {
        fclose(fp);
    }
    err_msg("decrypt key is : ", "illegal");
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