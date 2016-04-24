### 简易的 `lproxy` server 端集群部署方案（演示）

| 前端服务器             | 后端服务器              |
|------------------------|-------------------------|
| nginx >= `v1.9.0`      | lproxy-server           |

*由于这里只是演示，前后端服务器暂且都部署在同一主机上。*

* nginx 监听端口 `8088`
* lproxy-server 监听端口：`10001-10008` (比如部署 8 个 lproxy-server，每一个 lproxy-server 监听一个端口)

*`lproxy-server` 可以原生运行并监听 `10001-10008`端口，但是为了在一台主机上 YY 出逼真的集群环境，这里采用 docker 容器来部署 `lproxy-server`*。 **当然，你也可以使用 `Docker Compose`来部署这一整套集群系统。**

注意事项：

1. nginx 直到 `1.9.0` 版本才开始支持 TCP 转发（及负载均衡），而且默认不开启，需要编译前对 `./configure` 添加 ` --with-stream` 选项。
	> nginx-1.9.0 mainline version has been released, with the stream module for generic TCP proxying and load balancing.
2. 因为 lproxy-server 是基于 session 的，所以 Nginx 负载均衡 upstream 的分配策略需要是 `ip hash`（`hash $remote_addr consistent;`），即分配的依据是访客的 ip，进而这就要求 nginx 必须是**最前端**的服务器。


#### 部署步骤

1. 配置前端服务器 nginx

	1. 编译 nginx 使其支持 “TCP proxying and load balancing”

		目前，docker.io 的 OFFICIAL REPOSITORY 中提供的 nginx 貌似没有开启 TCP proxying 支持，所以这里要自己编译一个。

		```shell
		$ _DIR=$PWD
		$ mkdir -p nginx/build
		$ cd nginx/build
		$ wget http://nginx.org/download/nginx-1.9.14.tar.gz
		$ tar zxvf nginx-1.9.14.tar.gz
		$ cd ./nginx-1.9.14
		$ ./configure --prefix=$_DIR/nginx --with-ipv6 --with-stream --without-http_rewrite_module --without-http_gzip_module
		$ make && make install
		$ cd $_DIR/nginx
		$ rm -rf $_DIR/nginx/build
		```
	2. 配置 nginx

		这里，参考 nginx 官方示例：http://nginx.org/en/docs/stream/ngx_stream_core_module.html

		```shell
		$ mkdir -p /var/log/nginx/
		$ cd $_DIR/nginx/conf
		$ # 修改 nginx.conf 如下：
		$ cat nginx.conf
		worker_processes auto;
		
		error_log  /var/log/nginx/error.log warn;
		pid        /var/run/nginx.pid;
		
		events {
		    worker_connections  1024;
		}
		
		stream {
		    upstream backend {
		        hash $remote_addr consistent;
		
		        #server 127.0.0.1:10001 weight=5;
		        server 127.0.0.1:10001 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10002 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10003 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10004 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10005 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10006 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10007 max_fails=3 fail_timeout=30s;
		        server 127.0.0.1:10008;
				# ...
		    }
			
		    #upstream dns {
		    # server 192.168.33.1:53
		    #}
			
		    server {
		        listen 8088;
		        proxy_connect_timeout 1s;
		        proxy_timeout 3s;
		        proxy_pass backend;
		    }
		}
		```
	3. 运行 nginx

		```shell
		$ cd $_DIR/nginx/sbin/
		$ ./nginx
		```

2. 启动后端服务器

	```shell
	$ sudo docker run -d -p 10001:8088 --name lproxy1 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10002:8088 --name lproxy2 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10003:8088 --name lproxy3 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10004:8088 --name lproxy4 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10005:8088 --name lproxy5 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10006:8088 --name lproxy6 -i deel/lproxy ./lssserver.exe -k
	$ sudo docker run -d -p 10007:8088 --name lproxy7 -i deel/lproxy ./lssserver.exe -k
	$ sduo docker run -d -p 10008:8088 --name lproxy8 -i deel/lproxy ./lssserver.exe -k
	$ # ...
	```

3. 至此，server 端的所有配置都已完成。lproxy-local 端配置文件（`local-config.json`）中的 `server_name` 及 `server_port` 字段填写前端服务器 nginx 监听的 ip (或域名) 和 端口（8088）即可使用。