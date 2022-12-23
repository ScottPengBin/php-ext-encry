#ifndef YAOLING_ENCRYPT_LIB
#define YAOLING_ENCRYPT_LIB "yaoling_encrypt"

#include "php.h"
#include "ext/standard/info.h"

#include "zend_alloc.h"
#include "zend_string.h"
#include "php_main.h"
#include "zend_stream.h"
#include "zend_types.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

zend_result encrypt_file_code(zend_string *encrypt_file);

zend_result decrypt_file_code(zend_string *decrypt_file, zend_string *decrypt_key);

void err_msg(char *str,char *str2);

void alert_msg(char *str,char *str2);

#endif