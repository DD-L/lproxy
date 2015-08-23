# Log 模块组件测试
----------------------------------
<pre>
* test_priority_queue.cpp

	log_tools::priority_queue 的测试用例

	用来检验优先队列的运行情况

* test_logval_extra.cpp

	日志记录（LogVal）扩展字段(LogVal::extra)的测试用例

	用来测试 日志记录/数据 LogVal 的扩展能力

* test_lockfree.cpp
	
	boost::lockfree::queue 的测试用例
	
	用来检验 Store< LogVal*, boost::lockfree::queue< LogVal* > > 的使用方式

	这里要特别注意 Store<T> 和 boost::lockfree::queue<T> 的T类型如果是MyClass, 那么必须要使用指针的形式.

* test_loglevel.cpp

	日志级别(类型)的 测试用例

	用来检验 日志级别 的可定制化是否符合预期

	这个很有意思, 日志级别不再是死的enum类型, 而是可以追加常量值的类型.

</pre>
