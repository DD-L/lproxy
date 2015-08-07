# Store::push & Store::pop 性能测试
----------------------------------

## test.cpp
	测试用例源码

## Test Case
	1. ./bin/test1
		实例化底层容器为std::queue(默认)的Store用例

	2. ./bin/test2 
		实例化底层容器为std_priority_queue的Store用例, 
		std_priority_queue 是对std::priority_queue的二次封装.

	3. ./bin/test3 
		实例化底层容器为 boost::lockfree::queue 的Store用例

	4. ./bin/test4 
		实例化底层容器为 boost::lockfree::stack 的Store用例

## 关于test.cpp的一些说明

#####1. 面向 多生产者-多消费者， 进行 push/pop 的测试
#####2. 可接收参数
	producer_steps: 每个生产者线程生产的数据的个数.
	producer_count: 生产者线程个数.
	customer_count: 消费者线程最大个数.
	capacity:       仓库容量. 可选参数, 默认值为200001.
#####3. xxx
* 不管有多少个生产者线程，生产的数据永远不会有重复，并且数据范围在 [0，producer_steps X producer_count -1] 区间, 这样便于统计数据.
* 每个生产者分担的生产任务相同，都是 producer_steps 个.
* 消费者线程的个数永远都是小于等于给定的参数 customer_count 值. 比如当消费能力远大于生产能力时，消费者线程数还没来得及增加到指定的数目，生产和消费任务就结束了，此时会小于customer_count.
* 两种计时器，分别计算生产者或消费者线程处理数据的耗时.
	1. 如果一个生产者线程已经结束的同时，另外没有其他生产者线程在工作，则生产者计时器会统计一次耗时. 这样的结果是，有可能生产者线程组会不止一次的输出所用耗时，但绝不会计时重叠，将它们相加便是生产者线程组的总耗时.
	2. 消费者线程组的耗时，在输出结果中只会出现一次。但也有时它永远不会出现，见 bug
* bug (保留): 此bug是用例中的bug, 而非Store中的bug .由于Store::pop的设计需要，仓库为空时，pop操作会阻塞。
>>在pop测试函数里面:
>><pre><code>
// ...
while (true) {
    if (_arg.is_push_done() && buff.empty()) break;
    buff.pop(val);
    // ...
}
// ...
</code></pre>
>>当if 表达式 buff.empty() 返回false时, 就不会break.而当 再次执行到 buff.pop() 时，buff 可能已经为空, 进而引发 pop 阻塞. 保留此bug的详细原因，见代码中的注释[file:test.cpp:customer()]


##测试方法
这里以./bin/test1为例， 其他几个测试程序同样的步骤。

1. 将输出同时定向到文件 test1.log
	> <pre>$ ./test1 10000 7 3 30000 | tee test1.log </pre>
	> 
	>  如果最后出现用例中的死锁，Ctrl + C 结束掉程序即可

2. 分析 test1.log。 如果下列步骤中有一个不符合预期，就证明Store未能通过测试。
	1. 分别统计生产的数据[in: val] 个数 和 消费的数据[out: val] 个数
		><pre>
		$ grep -o "in" test1.log | wc -w
		$ grep -o "out" test1.log | wc -w
		</pre>
		> 预期结果都是 70000 （producer_steps X producer_count = 10000 X 7）
	2. 验证 val
		> 取出 val
		><pre>
		$ grep -o "\[in:\s[0-9]\+" test1.log | grep -o "[0-9]\+" | tee in1.tmp
		$ grep -o "\[out:\s[0-9]\+" test1.log | grep -o "[0-9]\+" | tee out1.tmp
		</pre>
		> 分别对in1.tmp 和 out1.tmp 排序
		> <pre>
		$ sort -n in1.tmp | tee in1.log
		$ sort -n out1.tmp | tee out1.log
		</pre>
		> 生成比较文件
		> <pre>
		$ seq -f "%.0f" 0 1 69999 | tee cmp.log 
		$ #69999 = producer_steps X producer_count - 1 = 10000 X 7 -1
		</pre>
		> 分别比对 in1.log 和 cmp.log 以及 out1.log 和 cmp.log
		> <pre>
		$ cmp in1.log  cmp.log
		$ cmp out1.log cmp.log
		</pre>
		> 预期结果是 in1.log / out1.log / cmp.log 3个文件无任何差别.
	3. 检验是形成有效争抢。
		> 方法是检验是在test1.log中 是否存在 in 和 out 共在同一行的情况
		> <pre>
		$ grep -n -m 1 -Po '\[in.{1,50}out.+?\]' test1.log | head -n 1
		</pre>
		> 如果没有形成有效争抢，说明给定的参数使得生产任务过于轻松，这样体现不出性能优劣。建议将producer_steps值设置的大一些，或适当减小capacity的值。
	4. 统计耗时。
		> 方法是将每行的前几个单词取出，读取有效行即可
		> <pre>
		$ awk '{printf("%03d: %s %s %s\n", FNR, $1, $2, $3 $4 $5 $6 $7)}' test1.log
001: producer_steps = 10000
002: producer_count = 7
003: customer_count = 3
004: capacity = 30000
005: 回车键继续...
006:
007: thread push start@0[6TTimerIZ8producerP7PushargE4PushE]开始计时...
008: [in: 0] [in:1][in:2][in:
009: thread push start@1...
010: [in: 1132] [in:10000][in:1133]
011: thread push start@2...
012: [in: 10001] [in:1134][in:20000][in:
013: thread push start@3...
014: [in: 1156] [in:20022][in:10024][in:
015: thread push start@4...
016: [in: 30001] [in:1158][in:20024][in:
017: thread push start@5...
018: [in: 40003] [in:30005][in:1162][in:
019: thread push start@6...
020: [in: 10030] [in:50000][in:40004][in:
021: thread pop start@0x2005cc28[6TTimerIZ8customerP6PopargE3PopE]开始计时...
022: [in: 10035] [in:50005][in:40009][in:
023: thread pop start@0x2005d350...
024: [out: 0] [in:10036][in:50006][in:
025: thread pop start@0x2005d648...
026: [out: 6] [in:10039][in:50009][in:
027: thread push exit@0...
028: [out: 5824] [in:68811][out:14691][in:
029: thread push exit@3...
030: [out: 15354] [in:69507][out:65323][in:
031: thread push exit@1...
032: [out: 35337] [in:29522][out:15360][in:
033: thread push exit@5...
034: [in: 49538] [out:35345][in:29536][out:
035: thread push exit@4...
036: [in: 29998] [in:69992][out:45541][out:
037: thread push exit@2...
038: [in: 69994] [out:65536][out:45542][out:
039: thread push exit@6[6TTimerIZ8producerP7PushargE4PushE]共耗时:9.141s...
040: [out: 55542] [out:35547][out:15570][out:
041: buff.empty() = true@0x2005d350
042: thread pop exit@0x2005d350[6TTimerIZ8customerP6PopargE3PopE]共耗时:11.311s...
		</pre>
		> 分别取出 07行 21行 39行 和 42行。
		> 
		> 由此分析可知，生产者线程组完成任务总耗时 9.141s，消费者线程组总耗时 11.311s

	5. 清除垃圾： make clean 即可
	
##对比结果

> 对 ./test1 ./test2 ./test3 ./test4 耗时结果初略统计，得出以下结论：
> 
1. 底层容器 std::queue 的仓库性能 略快于 std::priority_queue。
2. 底层容器 boost::lockfree::queue 和 boost::lockfree::stack 性能相当，但都普遍明显快于前两个。
3. 无锁队列 胜出。
>
> 统计数据表格，稍后追加。
