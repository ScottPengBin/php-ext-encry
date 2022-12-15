/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: c4cad05de55269f27193cee08648b6e3c0c86d32 */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_encrypt_file, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source_file, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_yaoling_decrypt_file, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, source_file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, decrypt_key, IS_STRING, 0)
ZEND_END_ARG_INFO()


ZEND_FUNCTION(yaoling_encrypt_file);
ZEND_FUNCTION(yaoling_decrypt_file);


static const zend_function_entry ext_functions[] = {
	ZEND_FE(yaoling_encrypt_file, arginfo_yaoling_encrypt_file)
	ZEND_FE(yaoling_decrypt_file, arginfo_yaoling_decrypt_file)
	ZEND_FE_END
};
