### lproxy v0.2.1 下行速度测试

前两天，我这里升级了宽带(10M 升 20M)，心血来潮，再测试一下 `v0.2.1` 版本的 lproxy  和 SS 的下行速度对比。 

*以下讨论的 lproxy 均是 `v0.2.1` 版本。*

##### 试验方法：

相同环境下，对比 SS 和 lproxy 下行速度。 （同 [LoadTesting](./LoadTesting.md)）

##### 试验原料：

1. SS （版本同 [LoadTesting](./LoadTesting.md)）
2. lproxy `v0.2.1`
3. Google Chrome 浏览器 “多线程下载 插件 [Chrono](http://www.chronodownloader.net/)”

##### 试验步骤：（同 [LoadTesting](./LoadTesting.md)）

由于和 [LoadTesting](./LoadTesting.md) 测试方法和步骤均相同，这里就不再啰嗦。

##### 试验结果


1. 来源一

	| 来源 | 文件大小 |
	|------|----------|
	|  http://dlsw.baidu.com/sw-search-sp/soft/20/12742/SketchUpPro_zh_CN_16.1.1450.1458023723.exe | 117.94MB|

	共测试出 7 组（次）数据： (平均速度，耗时)

	| SS            |     lproxy    |
	|---------------|---------------|
	| 2.51MB/s, 47s | 2.55MB/s, 46s |
	| 2.5MB/s,  47s | 2.46MB/s, 48s |
	| 2.5MB/s,  47s | 2.51MB/s, 47s |
	| 2.56MB/s, 46s | 2.52MB/s, 47s |
	| 2.5MB/s,  47s | 2.52MB/s, 46s |
	| 2.47MB/s, 47s | 2.5MB/s,  47s |
	| 2.51MB/s, 47s | 2.51MB/s, 47s |

	SS 总平均速度为 2.507 MB/s；lproxy 总平均速度是 2.510 MB/s。总耗时都是 328 s。

	从结果上来看，lproxy 以微乎其微的优势领先。因为下载文件只有 110 多兆，不足分辨出差异。

2. 来源二

	| 来源 | 文件大小 | 本机无代理  |
	|------|----------|-------------|
	|  http://dlsw.baidu.com/sw-search-sp/soft/20/12742/SketchUpPro_zh_CN_16.1.1450.1458023723.exe | 1.45GB | 2.52MB/s, 9m 49s|

	共测试出 5 组（次）数据： (平均速度，耗时)

	| SS               |     lproxy       |
	|------------------|------------------|
	| 2.65MB/s, 9m 20s | 2.66MB/s, 9m 19s |
	| 2.64MB/s, 9m 22s | 2.65MB/s, 9m 21s |
	| 2.65MB/s, 9m 21s | 2.65MB/s, 9m 20s |
	| 2.64MB/s, 9m 22s | 2.65MB/s, 9m 21s |
	| 2.66MB/s, 9m 19s | 2.66MB/s, 9m 18s |

	SS 总平均速度 2.648 MB/s；lproxy 总平均速度 2.654 MB/s。

	SS 总耗时 2804 s； lproxy 总耗时 2799 s。

	这次下载的文件够大，差异也就体现了出来。从结果上来看，**平均速度和耗时方面，lproxy 均略优于 SS** 。 

	值得一提的是， SS 和 lproxy 均优于 本机无代理，这只能说明，SS 和 lproxy 的服务端所在主机，比我 local 端所在的本机 网络要好，并不能说明 SS 和 lproxy 可以提升网速。
