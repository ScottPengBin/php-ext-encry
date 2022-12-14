#include "code.h"

//加密函数
zend_result encrypt_file(char *sourceFile, char *destFile)
{

    FILE *fp1 = fopen(sourceFile, "r");
    FILE *fp2 = fopen(destFile, "w");

    if (!fp1 || !fp2)
    {
        return FAILURE;
    }

    int ch;
    int i = 6;
    bool tag = false;

    while ((ch = fgetc(fp1)) != EOF)
    {

        i--;

        if (i < 0)
        {

            short temp = (short)ch;
            temp <<= 4;

            temp |= 0x8000;

            temp += rand() % 16;
            //加密字符放入文件中

            fprintf(fp2, "%hd", temp);
        }
        else
        {
            fprintf(fp2, "%c", ch);
        }
    }
    fprintf(fp2, "%s", ";");
    fclose(fp1);
    fclose(fp2);
    return SUCCESS;
}

//解密函数
zend_result decrypt_file(char *sourceFile, char *destFile)
{
    FILE *fp1 = fopen(sourceFile, "r");
    FILE *fp2 = fopen(destFile, "w");

    if (!fp1 || !fp2)
    {
        return FAILURE;
    }

    short temp;

    int i = 7;

    while (!feof(fp1))
    {
        i--;
        if (i > 0)
        {
            char ch;
            int b = fscanf(fp1, "%c", &ch);

            fputc(ch, fp2);
            continue;
        }
        int a = fscanf(fp1, "%hd", &temp);
        temp <<= 1;
        temp >>= 5;
        char ch = (char)temp;

        if (!ch)
        {
            break;
        }
        fputc(ch, fp2);
    }

    fclose(fp1);
    fclose(fp2);
    return SUCCESS;
}