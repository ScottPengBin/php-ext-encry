/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 280a10e509828106f817ee471305af23d95e1d83 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_encrypt_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sourceFile, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, destFile, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_yaoling_decrypt_file arginfo_yaoling_encrypt_file

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_encrypt_string, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, sourceStr, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_yaoling_decrypt_string arginfo_yaoling_encrypt_string


ZEND_FUNCTION(yaoling_encrypt_file);
ZEND_FUNCTION(yaoling_decrypt_file);
ZEND_FUNCTION(yaoling_encrypt_string);
ZEND_FUNCTION(yaoling_decrypt_string);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(yaoling_encrypt_file, arginfo_yaoling_encrypt_file)
	ZEND_FE(yaoling_decrypt_file, arginfo_yaoling_decrypt_file)
	ZEND_FE(yaoling_encrypt_string, arginfo_yaoling_encrypt_string)
	ZEND_FE(yaoling_decrypt_string, arginfo_yaoling_decrypt_string)
	ZEND_FE_END
};
