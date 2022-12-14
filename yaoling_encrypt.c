/* yaoling_encrypt extension for PHP */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_yaoling_encrypt.h"
#include "yaoling_encrypt_arginfo.h"
#include "lib/code.h"
#include "zend_alloc.h"
#include "zend_string.h"
#include "php_main.h"
#include "zend_stream.h"

#include <stdio.h>
#include <stdlib.h>

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE()  \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

#define KEY 0x86
#define HEAD "yaoling"

static zend_op_array *(*orig_compile_file)(zend_file_handle *file_handle, int type);
static zend_op_array *(*orig_compile_string)(zend_string *source_string, const char *filename);

zend_op_array *yaoling_compile_string(zend_string *source_string, const char *filename)
{

	size_t head_size = strlen(HEAD);

	zend_string *destStr = zend_string_alloc(sizeof(char) * head_size, 0);

	int i, j = 0;
	// header
	for (i = 0; i < head_size; i++)
	{
		destStr->val[i] = source_string->val[i] ^ KEY;
	}
	
	// head 头信息校验
	if (strcmp(destStr->val, HEAD) != 0)
	{
		zend_string_release(destStr);
		return orig_compile_string(source_string, filename);
	}

	//body解密
	destStr = zend_string_realloc(destStr, sizeof(char) * (source_string->len - head_size), 0);

	for (j = 0; j < source_string->len; j++)
	{
		destStr->val[j] = source_string->val[i + j] ^ KEY;
	}
	
	return orig_compile_string(destStr, filename);
}
// 运行时解密
zend_op_array *yaoling_compile_file(zend_file_handle *file_handle, int type)
{

	// 打开源文件
	FILE *fp1 = fopen(ZSTR_VAL(file_handle->filename), "rb");

	if (!fp1)
		return orig_compile_file(file_handle, type);

	int stream = fileno(fp1);
	struct stat statbuf;
	fstat(stream, &statbuf);
	int bodylen = statbuf.st_size;
	char *buf = (char *)malloc(sizeof(char) * statbuf.st_size);

	// 读取文件
	if (read(stream, buf, bodylen) != statbuf.st_size)
	{
		php_printf("Failed to readed ");
		return orig_compile_file(file_handle, type);
	}

	// 先申请6个子节
	char *realbuf = malloc(6);
	size_t reallen = 0;
	bool decode = false;
	for (int i = 0; i < bodylen; i++)
	{
		if (i < 6)
		{
			realbuf[reallen++] = buf[i];
			continue;
		}

		char temp[6];
		strncpy(temp, buf + i, 6);
		short stemp = atoi(temp);
		stemp <<= 1;
		stemp >>= 5;
		char ch = (char)stemp;

		if (!ch)
			break;

		// 可以被解密
		if (decode == false)
		{
			// 扩容
			realbuf = realloc(realbuf, bodylen);
			decode = true;
		}

		realbuf[reallen++] = ch;

		i += 5;
	}

	if (decode == false)
	{
		free(realbuf);
		free(buf);
		fclose(fp1);
		return orig_compile_file(file_handle, type);
	}

	char *tmp_buf = NULL;
	size_t tmp_size = 0;

	if (file_handle->type == ZEND_HANDLE_STREAM)
	{
		file_handle->type = ZEND_HANDLE_FP;
	}

	if (file_handle->handle.fp != NULL)
	{
		fclose(file_handle->handle.fp);
	}

	file_handle->handle.fp = tmpfile();

	fwrite(realbuf, 1, reallen, file_handle->handle.fp);

	rewind(file_handle->handle.fp);

	free(realbuf);
	free(buf);
	fclose(fp1);

	return orig_compile_file(file_handle, type);
}

/* {{{ string yaoling_decrypt_string( string $souceStr) */
PHP_FUNCTION(yaoling_encrypt_string)
{
	zend_string *souceStr = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(souceStr)
	ZEND_PARSE_PARAMETERS_END();

	size_t head_size = strlen(HEAD);

	zend_string *destStr = zend_string_alloc(sizeof(char) * (souceStr->len + head_size), 0);

	int i, j = 0;
	// header
	for (i = 0; i < head_size; i++)
	{
		destStr->val[i] = HEAD[i] ^ KEY;
	}

	// body
	for (j = 0; j < souceStr->len; j++)
	{
		destStr->val[i + j] = souceStr->val[j] ^ KEY;
	}

	RETURN_STR(destStr);
}
/* }}} */

/* {{{ string yaoling_decrypt_string( string $souceStr) */
PHP_FUNCTION(yaoling_decrypt_string)
{
	zend_string *souceStr = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(souceStr)
	ZEND_PARSE_PARAMETERS_END();

	size_t head_size = strlen(HEAD);

	zend_string *destStr = zend_string_alloc(sizeof(char) * head_size, 0);

	int i, j = 0;
	// header
	for (i = 0; i < head_size; i++)
	{
		destStr->val[i] = souceStr->val[i] ^ KEY;
	}

	// head 头信息校验
	if (strcmp(destStr->val, HEAD) != 0)
	{
		zend_string_release(destStr);
		RETURN_STR(souceStr);
	}

	
	destStr = zend_string_realloc(destStr, sizeof(char) * (souceStr->len - head_size), 0);

	//body解密
	for (j = 0; j < souceStr->len; j++)
	{
		destStr->val[j] = souceStr->val[i + j] ^ KEY;
	}

	RETURN_STR(destStr);
}
/* }}} */

/* {{{ bool yaoling_encrypt_file( string $souceFile, string $destFile) */
PHP_FUNCTION(yaoling_encrypt_file)
{
	char *souceFile = NULL;
	char *destFile = NULL;
	size_t str_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STRING(souceFile, str_len)
	Z_PARAM_STRING(destFile, str_len)
	ZEND_PARSE_PARAMETERS_END();

	int status = encrypt_file(souceFile, destFile);
	RETURN_BOOL(status + 1);
}
/* }}} */

/* {{{ bool yaoling_decrypt_file( string $souceFile, string $destFile) */
PHP_FUNCTION(yaoling_decrypt_file)
{
	char *souceFile = NULL;
	char *destFile = NULL;
	size_t str_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STRING(souceFile, str_len)
	Z_PARAM_STRING(destFile, str_len)
	ZEND_PARSE_PARAMETERS_END();

	int status = decrypt_file(souceFile, destFile);
	RETURN_BOOL(status + 1);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(yaoling_encrypt)
{
#if defined(ZTS) && defined(COMPILE_DL_YAOLING_ENCRYPT)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(yaoling_encrypt)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "yaoling_encrypt support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(yaoling_encrypt)
{

	orig_compile_file = zend_compile_file;

	zend_compile_file = yaoling_compile_file;

	orig_compile_string = zend_compile_string;

	zend_compile_string = yaoling_compile_string;

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(yaoling_encrypt)
{
	zend_compile_file = orig_compile_file;

	zend_compile_string = orig_compile_string;

	return SUCCESS;
}

/* {{{ yaoling_encrypt_module_entry */
zend_module_entry yaoling_encrypt_module_entry = {
	STANDARD_MODULE_HEADER,
	"yaoling_encrypt",				/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(yaoling_encrypt),		/* PHP_MINIT - Module initialization */
	PHP_MSHUTDOWN(yaoling_encrypt), /* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(yaoling_encrypt),		/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(yaoling_encrypt),		/* PHP_MINFO - Module info */
	PHP_YAOLING_ENCRYPT_VERSION,	/* Version */
	STANDARD_MODULE_PROPERTIES};
/* }}} */

#ifdef COMPILE_DL_YAOLING_ENCRYPT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(yaoling_encrypt)
#endif
