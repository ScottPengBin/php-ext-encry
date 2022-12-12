/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: dc80ee45d365e099fe3c26d3e99c8a387d1e56cd */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_encrypt_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, sourceFile, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, destFile, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_decrypt_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, sourceFile, sstring, 0)
	ZEND_ARG_TYPE_INFO(0, destFile, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(yaoling_encrypt_file);
ZEND_FUNCTION(yaoling_decrypt_file);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(yaoling_encrypt_file, arginfo_yaoling_encrypt_file)
	ZEND_FE(yaoling_decrypt_file, arginfo_yaoling_decrypt_file)
	ZEND_FE_END
};
