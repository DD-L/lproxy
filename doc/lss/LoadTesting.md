### lproxy Load Testing

有小伙伴关心 lproxy 的下行速度，所以这里也追加测试一下“下载速度”。

本次测试版本： `lproxy v0.1.0.20160418_Beta`

*提示：这里的负载测试，只是简单的下行速度测试，不够专业严谨*

##### 实验方法：

相同环境下，对比 SS 和 lproxy 下行速度。

> **更新：`lproxy v0.2.1` 和 SS 的对比结果在这里：[LoadTesting_v0.2.1](./LoadTesting_v0.2.1.md)，lproxy 在下行速度方面，取得了微弱的优势。**

##### 实验原料：

* SS

	1. 服务端：

		```shell
		docker pull shadowsocks/shadowsocks-libev
		```
	
		因为是 latest , 所以这里*应该*是最新版本 v2.2.3
		
		![图示](https://raw.githubusercontent.com/DD-L/DailyNotes/master/lproxy/img/docker_ss_libev.png)

	2. 本地端：
		
		[Windows 版本 3.0](https://github.com/shadowsocks/shadowsocks-windows/releases/tag/3.0)

* lproxy

	1. 服务端：

		```shell
		docker pull deel/lproxy:v0.1.0.20160418_Beta
		```
		lproxy 版本 v0.1.0.20160418_Beta

	2. 本地端：

		[Windows 版本 v0.1.0.20160418_Beta](https://github.com/DD-L/lproxy/releases/tag/0.1.0.20160418_Beta)

* 下载工具

	Google Chrome 浏览器 “多线程下载 插件 [Chrono](http://www.chronodownloader.net/)”

##### 实验步骤：

1. 第一次实验

	从百度软件中心下载 `Google Chrome` 软件

	1. 先测试 SS 的运行情况。

		* 服务端：

			```shell
			docker run --name ss -p 8338:8338 shadowsocks/shadowsocks-libev -k xxxxxxxxx -m aes-256-cfb
			```
		* 本地端：（Windows）
			
			* 服务端 IP:  192.168.33.202
			* 服务端端口：8338
			* 密码：xxxxxxxxx
			* 加密：aes-256-cfb

		* 实验结果：

			![ss1](https://raw.githubusercontent.com/DD-L/DailyNotes/master/lproxy/img/ss1.png)

	2. 再来测试 lproxy 的运行情况

		* 服务端：

			```shell
			docker run -d -p 8088:8088 --name lproxy -i deel/lproxy:v0.1.0.20160418_Beta
			docker exec -i lproxy ./lssserver.exe -k
			```
			认证 key 集合中包含 “xxxxxxxxx”

		* 本地端：（Windows）

			```javascript
			{
    			// see https://github.com/DD-L/lproxy/blob/documents/doc/lss/local-config.json.md
    			"lsslocal":
    			{
        			"bind_addr": "0.0.0.0",
        			"bind_port": 8087,
        			"server_name": "192.168.33.202",
        			"server_port": "8088",
        			"auth_key": "xxxxxxxxx",
        			"zip_on": false,
        			//"logfile": "/var/log/lproxy/local.err", 
        			"timeout": 30 
    			}
			}
			```
		* 实验结果：

			![lproxy1](https://raw.githubusercontent.com/DD-L/DailyNotes/master/lproxy/img/lproxy1.png)

	3. 对比结果：

		SS 和 lproxy 耗时和平均速度均一样。
			
2. 第二次实验

	因为第一次实验下载的文件体积太小，没有显示出一丝的差异，所以进行第二次实验：在百度软件中心页：http://rj.baidu.com/soft/detail/23411.html 下载体积更大的 `Microsoft .NET Framework 3.5`

	**SS 和 lproxy 服务端 和 本地端 的配置均和第一次实验一致，这里直接给出实验结果:**
	
	1. SS

		![ss2](https://raw.githubusercontent.com/DD-L/DailyNotes/master/lproxy/img/ss2.png)

	2. lproxy

		![lproxy2](https://raw.githubusercontent.com/DD-L/DailyNotes/master/lproxy/img/lproxy2.png)

	3. 对比结果

		在耗时方面， lproxy 比 SS 少了 1 秒；平均速度方面， lproxy 比 SS 大了 0.01MB/s 。非常细微的差异。**可当做实验误差，认为二者相等。**


##### 实验总结

单从结果上来看，似乎 lproxy 有极其微弱的优势，但因为实验样本比较少，环境因素复杂，**不能**就此判断 lproxy 比  SS 下载速度更快。只能初步的认为 **lproxy 和 SS 单客户端的下载速度一致**。此外程序的运行效率还和加密算法有关，SS 是采用 aes-256bit 的加密算法，和 lproxy 基本一致（注：此版本的 lproxy 的“握手阶段”是采用 RSA 1024bit 的加密算法）。