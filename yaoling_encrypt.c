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

static zend_op_array *(*orig_compile_file)(zend_file_handle *file_handle, int type);

// 运行时解密
zend_op_array *yaoling_compile_file(zend_file_handle *file_handle, int type)
{
	// 打开源文件
	FILE *fp = fopen(file_handle->filename->val, "rb");
	if (!fp)
	{
		fp = zend_fopen(file_handle->filename, &file_handle->opened_path);
		if (!fp)
		{
			return orig_compile_file(file_handle, type);
		}
	}

	/**==============解密开始=====================*/

	size_t reallen = 0;
	bool decode = false;
	char *realbuf = NULL;
	short temp;

	// key header
	bool key_tag = true;
	for (int i = 0; i < strlen(YAOLING_ENCRYPT_LIB); i++)
	{
		int a = fscanf(fp, "%hd", &temp);
		temp <<= 1;
		temp >>= 5;
		char ch = (char)temp;
		
	
		if (toascii(ch) -toascii(YAOLING_ENCRYPT_LIB[i])  != 0 || !ch)
		{
			key_tag = false;
			break;
		}
	}

	// header
	if (key_tag == false)
	{
		fclose(fp);
		return orig_compile_file(file_handle, type);
	}


	//body
	while (!feof(fp))
	{
		int a = fscanf(fp, "%hd", &temp);

		if (temp > 0)
		{
			break;
		}

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
			realbuf = malloc(bodylen);
			decode = true;
		}

		realbuf[reallen++] = ch;
	}

	
	// 不能解密，释放内存
	if (decode == false)
	{
		free(realbuf);
		fclose(fp);
		return orig_compile_file(file_handle, type);
	}
	realbuf = realloc(realbuf,reallen);
	
	fclose(fp);
	/**=============解密结束======================*/

	if (file_handle->type == ZEND_HANDLE_FP)
		fclose(file_handle->handle.fp);
#ifdef ZEND_HANDLE_FD
	if (file_handle->type == ZEND_HANDLE_FD)
		close(file_handle->handle.fd);
#endif
	file_handle->type = ZEND_HANDLE_FP;

	fp = tmpfile();

	fwrite(realbuf, 1, reallen, fp);

	rewind(fp);

	file_handle->handle.fp = fp;

	free(realbuf);

	return orig_compile_file(file_handle, type);
}

/* {{{ bool yaoling_encrypt_file( string $souceFile, string $destFile) */
PHP_FUNCTION(yaoling_encrypt_file)
{
	zend_string *encrypt_file = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
	Z_PARAM_STR(encrypt_file)
	ZEND_PARSE_PARAMETERS_END();

	int status = encrypt_file_code(encrypt_file);

	RETURN_BOOL(0 + 1);
}
/* }}} */

/* {{{ bool yaoling_decrypt_file( string $souceFile, string $destFile) */
PHP_FUNCTION(yaoling_decrypt_file)
{
	zend_string *decrypt_file = NULL;
	zend_string *decrypt_key = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
	Z_PARAM_STR(decrypt_file)
	Z_PARAM_STR(decrypt_key)
	ZEND_PARSE_PARAMETERS_END();

	int status = decrypt_file_code(decrypt_file, decrypt_key);
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

	return SUCCESS;
}
/* }}} */

PHP_MSHUTDOWN_FUNCTION(yaoling_encrypt)
{
	zend_compile_file = orig_compile_file;

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
