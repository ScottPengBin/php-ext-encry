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
	struct stat statbuf;
	fstat(fileno(fp), &statbuf);
	int bodylen = statbuf.st_size;


	// 先申请6个子节
	char *realbuf = malloc(6);
	size_t reallen = 0;
	bool decode = false;
	int i = 7;
	short temp;
	while (!feof(fp))
	{
		i--;
		if (i > 0)
		{
			char ch;
			int b = fscanf(fp, "%c", &ch);
			
			realbuf[reallen++] = ch;
			continue;
		}
		int a = fscanf(fp, "%hd", &temp);
		//不能被解密
		if(temp > 0){
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
			// 扩容
			realbuf = realloc(realbuf, bodylen);
			decode = true;
		}
		realbuf[reallen++] = ch;
	}

	//不能解密，释放内存
	if (decode == false)
	{
		free(realbuf);
		fclose(fp);
		return orig_compile_file(file_handle, type);
	}

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
