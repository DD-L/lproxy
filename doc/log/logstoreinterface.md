log/logstoreinterface.h

----

# LogStoreInterface

```cpp
class LogStoreInterface;
```

日志仓库接口类

### 类摘要

```cpp
class LogStoreInterface {
public:
	virtual void push(LogVal*) = 0;
	virtual void pop(std::shared_ptr<LogVal>& val) = 0;
	virtual ~LogStoreInterface() {}
};

```