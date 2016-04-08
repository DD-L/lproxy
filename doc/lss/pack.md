

### 一、包的结构组成


| version| pack_type | data_len | data          |
|:------:|:---------:|:--------:|:-------------:|
| 版本   |  包类型   | 数据长度 |  数据         |
| 1 byte | 1 byte    | 2 byte   | data_len byte |



* version

	`当前版本为定值 0x00`

	| version |
	|---------|
	| 0x00    | 



* data_len 构成

	`data_len 的网路字节序 表示 数据的长度`
	
	|data_len_high_byte | data_len_low_byte|
	|-------------------|------------------|
	| 1 byte            | 1 byte           |


### 二、pack_type 及其 对应的 data_len, data


| pack_type |  data_len| data  |意义| 适用方向 |
|-----------|----------|------:|----|----------|
|0x00| 0 | null | local发出的请求连接 | to server|
|0x01| len(模长 + 公钥) | 模长, 公钥 |回应local的请求连接, 把<br/>rsa 模长和公钥返馈给 local。<br/>data部分：头两个字节是模<br/>长的网络字节序，剩下的是<br/>公钥 | to local |
|0x02|len(data)|rsa: `公钥(认证key, 随机数)`|将'认证key'和'随机数',经<br/>公钥ras加密后的数据,发<br/>给server| to server|
|0x03|len(data)|aes: `认证key(随机key, 随机数)`|local端被认证通过。并把<br/>随机生成的 key，和之前<br/>local发来的'随机数'一起，<br/>经'认证key'进行aes加密后<br/>反馈给local|to local|
|0x04|0|null|local端被认证失败，或需<br/>要对local重新认证的情况|to local|
|0x05|0|null|session 超时, server 端<br/>session 断开|to local|
|0x06|len(data)|aes: `随机key(Data)`|数据交换，data是经'随机<br/>key'进行ase加密的数据|双向|
|0x16|len(data)|`zip(aes: 随机key(Data))`|数据交换，data是压缩后<br/>的加密数据|双向|
|0xff|0|null|明确要求断开session, 或<br/>遇到不支持的包类型|双向|


注:

1. 图表中的 'Data' 是指认证通过后, 需要交换的 socks5 数据(request/reply)；

2. 图表中的 '认证key' 和 '随机key' 都是256bit的数据(定长)；

3. local 和 server 双向验证：

	| 由 server 端生成 | 由 local 端生成 | 共同持有         |
	|------------------|-----------------|------------------|
	| rsa 公钥/密钥    | 随机数          |256bit 的认证 key |
	| 256bit 的随机 key|
	
	>
   * 256bit的'认证key' 是server端用来验证local端的合法性的. 由local端和server端共同持有；
   * '随机数' 是 local 端用来验证 server 端返回的'随机key'是否被中间人非法篡改. 由local端生成；
   * 'rsa公钥' 是用来传输 '认证key' 和 '随机数' 的. 由server端生成；
   * 256bit的'随机key' 是当前会话, 双向认证过后, 数据字段加密用的key. 由server端生成。
