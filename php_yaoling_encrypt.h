/* yaoling_encrypt extension for PHP */

#ifndef PHP_YAOLING_ENCRYPT_H
# define PHP_YAOLING_ENCRYPT_H

extern zend_module_entry yaoling_encrypt_module_entry;
# define phpext_yaoling_encrypt_ptr &yaoling_encrypt_module_entry

# define PHP_YAOLING_ENCRYPT_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_YAOLING_ENCRYPT)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_YAOLING_ENCRYPT_H */
