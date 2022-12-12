#ifndef YAOLING_ENCRYPT_LIB
#define YAOLING_ENCRYPT_LIB "yaoling_encrypt"

#include "zend_types.h"
#include <stdio.h>

zend_result encrypt_file(char *sourceFile, char *destFile);

zend_result decrypt_file(char *sourceFile, char *destFile);


#endif