--TEST--
Check if yaoling_encrypt is loaded
--EXTENSIONS--
yaoling_encrypt
--FILE--
<?php
echo 'The extension "yaoling_encrypt" is available';
?>
--EXPECT--
The extension "yaoling_encrypt" is available
