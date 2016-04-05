
# crypto 异常处理


* `CryptoException` 
	
	crypto 加解密异常， 继承自 `std::exception`

* `EncryptException`

	crypto 加密时异常， 继承自 `CryptoException`

* `DecryptException`

	crypto 解密时异常， 继承自 `CryptoException`