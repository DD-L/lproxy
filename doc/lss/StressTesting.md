# lproxy Stress Testing

这里的压力测试重点是并发测试。

使用 WEB 压力测试工具  [http_load](https://github.com/DD-L/DailyNotes/blob/centos/centos/centos-config.md#http_load) 对 lproxy 服务进行压力测试（并发）。HTTP 短连接特性，比较容易测得出并发性能。

虽然测试不专业，但绝对详尽。


说明：*因为 http_load 的代理设置只支持 http 的代理方式，所以又使用了 Privoxy（ver 3.0.24） 进行了 http -> socks5 的转换。*

## 详细配置说明及结果如下：

在 host (windows10 64bit, 8G内存) ip: 192.168.33.124 上运行: `Privoxy-3_0_24` 和 `Hyper-V 虚拟机`

##### 1. Privoxy-3_0_24
配置如下：

	listen-address  192.168.33.124:8118
	forward-socks5 / 192.168.33.202:8087 .

含义是 监听本地 192.168.33.124:8118 端口用来代理 http，并将流量转发至 socks5://192.168.33.202:8087 服务器。

##### 2. Hyper-V 虚拟机

1. Hyper-V 里运行 CentOS7 64bit 动态内存, ip: 192.168.33.202
	1. CentOS7 中- 运行 lproxy docker 容器。在 lproxy 容器中，编译 LSS_DEBUG 版本的 lproxy (会有巨细的日志在终端输出)，为了能够及时发现问题，又外面在加一层 gdb, 具体如下：
		```shell
		$ gdb ./bin/lssserver.exe # and run
		$ gdb ./bin/lsslocal.exe  # and run
		```
		容器中 同时运行 lssserver 和 lsslocal 两个服务.

		更详尽的信息：
		* lsslocal :  left_io_service 和 right_io_service 分别占用一个线程
		* lssserver:  left_io_service 和 right_io_service 分别占用一个线程 (与 lsslocal 情况完全相同)

		left 和 right 的含义因此而来：`client <---> lss <---> remote`，client 在 lss 左边，remote 在 lss 右边。

		lproxy 运行机制 与 'ssh 隧道' 基本相同：
		`app <---> lsslocal.exe <---> lssserver.exe <---> web`
	
		对 `lsslocal` 而言， app 是其 left, lssserver 是其 right；对 `lssserver` 而言，lsslocal 是其 left， web 是其 right

	2. CentOS7 中- 运行 http_load （每秒1000 http 并发, 这是 http_load 允许的最大值）
		```shell
		$ cat urls.txt
		http://news.baidu.com/
		http://internet.baidu.com/
		http://yule.baidu.com/
		http://fashion.baidu.com/

		$ http_load -rate 1000 -fetches 100000 -timeout 150 -proxy 192.168.33.124:8118 urls.txt
		...
		100005 fetches, 517 max parallel, 4.3615e+07 bytes, in 100.327 seconds
		436.129 mean bytes/connection
		996.793 fetches/sec, 434730 bytes/sec
		msecs/connect: 56.0327 mean, 1080.13 max, 0.485 min
		msecs/first-response: 10765.7 mean, 19199.7 max, 265.239 min
		1130 bad byte counts
		HTTP response codes:
		  code 200 -- 379
		  code 503 -- 751
		```

## 总结

lproxy 服务(lsslocal 和 lssserver)两个运行情况都良好。如果编译 不带 LSS_DEBUG 版本的 lproxy, 并且不再 gdb 下运行 lproxy，可能情况会更好。

##### TODO
* 不知道是不是因为加了一层 Privoxy 转换的缘故，使得 并发请求, 到达 lsslocal 时可能会有所减弱。有待进一步验证。
* 压力测试工具 http_load 运行时本身需要消耗很大的资源(CPU 60%+)，所以应当用一台独立的主机单独运行 http_load 进行测试。 

----------

#### 其他途径的并发测试

因为 lproxy 目前暂时还没有实现：对到来的请求进行原子计数功能，所以下面的土方法并不能得出具体的并发数，但从运行情况来看肯定比 每秒 1000 次 要大很多很多，而且没有 `Privoxy`中转 干扰项，在真实环境中进行测试。

1. 还是对 带 LSS_DEBUG 版本的 lproxy 进行并发测试, 在 gdb 模式下运行；lsslocal 绑定端口 192.168.33.202:8087；left_io_service 和 right_io_service 分别占用一个线程，lsslocal 和 lssserver 情况相同。

2. 安装基于 Chrome 的[猎豹浏览器](http://www.liebao.cn/) （Google官方版本的Chrome, 没有`刷新所有标签页 Ctrl+Shift+R`的按钮，后面要用，所以这里选了猎豹），我这里的选用的版本是 猎豹`V5.3.108.10728`
3. 在猎豹浏览器上安装 [SwitchyOmega](https://github.com/FelisCatus/SwitchyOmega/releases)，并配置好 socks5 代理。比如 socks5://192.168.33.202:8087
4. 分别在不同标签页随机打开 百度域名下的几个网页 （之所以选百度，是因为至少百度的 CND 做的肯定比小站点要好，不至于响应请求要等好久），比如： 
	* https://www.baidu.com/
	* http://news.baidu.com/
	* http://internet.baidu.com/
	* http://fashion.baidu.com/
	* http://guonei.news.baidu.com/
	* http://guoji.news.baidu.com/
	* http://baijia.baidu.com/
	* http://finance.baidu.com/
	* http://sports.baidu.com/
	* http://shipin.news.baidu.com/
	* http://media.baidu.com/
	* http://jian.news.baidu.com/
	* http://news.hao123.com/?src=baidunews_1016
	* http://news.baidu.com/ns?cl=2&rn=20&tn=news&word=cnd
	* http://tech.baidu.com/
	
	打开这么多标签页后，按`刷新所有标签页`的快捷键`Ctrl+Shift+R`, 稍等片刻，页面全部加载完毕，lproxy 运行情况良好。
	
	[[写在中间]]：

	1. **之所以选了这么多的百度新闻页，是因为这些页面里的资源很多，每一个资源都是一个 HTTP 请求，其中还有很多 Ajax 请求。比如统计了下我此刻的 http://tech.baidu.com/ 页面，资源加载完整就需要完成 77 次请求，而且这还不算 Chrome 的 `multiple requests`策略带来的重复请求，所以这么多百度新闻的页面放在一起同时刷新，肯定很热闹**

	2. **之所以选基于 Chrome 的浏览器，是因为它每次页面刷新，Chrome 都会发出多个相同的请求（实测一般是3~4个）`multiple requests`，这可能是 Chrome 为了提高浏览器对页面响应的成功率，进行的自身优化策略。更多资料可以 google 一下 `Google Chrome sends multiple requests to fetch a page`**

	因为 `刷新所有标签页` 一次，执行情况良好。
	
	所以加大强度，极快速的连续按下 `Ctrl+Shift+R` 3次：稍等片刻，仍然运行情况良好。

	更大的强度测试，极快速的连续按下 `Ctrl+Shift+R` 4次：稍等片刻，仍然运行情况良好。
	
	进一步加大强度测试，极快速的连续按下 `Ctrl+Shift+R` 5次：稍等片刻，仍然运行情况良好。

	更进一步加大强度测试，极快速的连续按下 `Ctrl+Shift+R` 6次：稍等片刻，仍然运行情况良好。

	极快速的连续按下 `Ctrl+Shift+R` 7次：稍等片刻，仍然运行情况良好。

	极快速的连续按下 `Ctrl+Shift+R` 8次：稍等片刻，lsserver.exe 出现了一个已知的 bug (descriptor_data 被释放了，可能的原因之一是 socket 执行了 close 操作，具体原因仍在跟踪), lssserver.exe 停止。
	
	```
	bug 详情：
	2016-Mar-27 06:37:45.694879 [DEBUG] unpack data from local: 5 1 0 3 12 77 77 77 2e 67 6f 6f 67 6c 65 61 70 69 73 2e 63 6f 6d 1 bb 	[pid:7ffff7fef780] session_server.cpp:231 left_read_handler
	2016-Mar-27 06:37:45.694890 [DEBUG] start socks5_request_processing...	[pid:7ffff7fef780] session_server.cpp:269 left_read_handler
	2016-Mar-27 06:37:45.694896 [DEBUG] lproxy::socks5::req::AddrType = domain	[pid:7ffff7fef780] session_server.cpp:937 socks5_request_processing
	2016-Mar-27 06:37:45.694909 [DEBUG] lproxy::socks5::req::Addr.domain = www.googleapis.com	[pid:7ffff7fef780] session_server.cpp:943 socks5_request_processing
	2016-Mar-27 06:37:45.694915 [DEBUG] lproxy::socks5::req::Port = 443	[pid:7ffff7fef780] session_server.cpp:967 socks5_request_processing
	2016-Mar-27 06:37:45.694924 [DEBUG] lproxy::socks5::req::Cmd = TCP-CONNECT	[pid:7ffff7fef780] session_server.cpp:971 socks5_request_processing
	2016-Mar-27 06:37:45.694956 [DEBUG] start async_read local...[pid:7ffff7fef780] session_server.cpp:282 left_read_handler
	
	Program received signal SIGSEGV, Segmentation fault.
	0x00000000004172dd in boost::asio::detail::epoll_reactor::start_op (this=0x1785fd0, op_type=0, descriptor=24, 
	    descriptor_data=@0x17c12a8: 0x0, op=0x17bdff0, 
	    is_continuation=false, allow_speculative=true)
	    at /opt/lproxy/src/core/lss/../../..//contrib/boost/boost_1_57_0/boost/asio/detail/impl/epoll_reactor.ipp:219
	219	  if (descriptor_data->shutdown_)
	(gdb) 
	(gdb) p descriptor_data
	$1 = (boost::asio::detail::epoll_reactor::per_descriptor_data &) @0x17d1368: 0x0
	(gdb) l
	214	    return;
	215	  }
	216	
	217	  mutex::scoped_lock descriptor_lock(descriptor_data->mutex_);
	218	
	219	  if (descriptor_data->shutdown_)
	220	  {
	221	    post_immediate_completion(op, is_continuation);
	222	    return;
	223	  }
	(gdb) f
	#0  0x0000000000416b2d in boost::asio::detail::epoll_reactor::start_op (this=0x17ceb40, op_type=0, descriptor=21, descriptor_data=@0x17d1368: 0x0, op=0x17d1740, is_continuation=false, 
	    allow_speculative=true) at /opt/lproxy/src/core/lss/../../..//contrib/boost/boost_1_57_0/boost/asio/detail/impl/epoll_reactor.ipp:219
	219	  if (descriptor_data->shutdown_)
	(gdb) bt
	#0  0x00000000004172dd in boost::asio::detail::epoll_reactor::start_op (this=0x1785fd0, op_type=0, descriptor=24, 
	    descriptor_data=@0x17c12a8: 0x0, op=0x17bdff0, 
	    is_continuation=false, allow_speculative=true)
	    at /opt/lproxy/src/core/lss/../../..//contrib/boost/boost_1_57_0/boost/asio/detail/impl/epoll_reactor.ipp:219
	#1  0x000000000041a0cc in boost::asio::detail::reactive_socket_service_base::start_op (this=0x1783c78, impl=..., 
	    op_type=0, op=0x17bdff0, is_continuation=false, 
	    is_non_blocking=true, noop=false)
	    at /opt/lproxy/src/core/lss/../../..//contrib/boost/boost_1_57_0/boost/asio/detail/impl/reactive_socket_service_base.ipp:214
	#2  0x0000000000456be0 in boost::asio::detail::reactive_socket_service_base::async_receive<boost::array<boost::asio::mutable_buffer, 5ul>, boost::_bi::bind_t<void, boost::_mfi::mf5<void, lproxy::server::session, boost::system::error_code const&, unsigned long, std::shared_ptr<lproxy::server::request>, std::shared_ptr<std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > >, std::shared_ptr<std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > > >, boost::_bi::list6<boost::_bi::value<std::shared_ptr<lproxy::server::session> >, boost::arg<1>, boost::arg<2>, boost::_bi::value<std::shared_ptr<lproxy::server::request> >, boost::_bi::value<std::shared_ptr<std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > > >, boost::_bi::value<std::shared_ptr<std::basic_string<unsigned char, std::char_traits<unsigned char>, std::allocator<unsigned char> > > > > > > (
	    this=0x1783c78, impl=..., buffers=..., flags=0, 
	    handler=...)
	    at /opt/lproxy/src/core/lss/../../..//contrib/boost/boost_---Type <return> to continue, or q <return> to quit---

	这次有点奇怪的是，日志显示，百度的页面要请求 Google 的资源（api） www.googleapis.com。这可能是 猎豹浏览器本身需要，也可能是 某些页面的资源真的需要（这也正常），还有一点是 我另外一个 浏览器 Google Chrome 也在用这个端口，也可能是它发出来的。
 
	目前正在着手解决这个bug (偶现), 暂时无视它，继续测试。
	重新 在 gdb 模式下启动 lssserver.exe
	```

	极快速的连续按下 `Ctrl+Shift+R` 9次：稍等片刻，仍然运行情况良好。

	极快速的连续按下 `Ctrl+Shift+R` 10次：稍等片刻，仍然运行情况良好。
	
	极快速的连续按下 `Ctrl+Shift+R` 11次：稍等片刻，仍然运行情况良好。
	
	。。。

	极快速的连续按下 `Ctrl+Shift+R` 15次：稍等片刻，仍然运行情况良好。

	极快速的连续按下 `Ctrl+Shift+R` 20次：稍等片刻，电脑有些卡顿，但仍然运行情况良好。
	
	直到这次测试结束，那个 已知的 Bug 依然没有复现，遗留。测试时间：2016-03-27