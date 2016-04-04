language/lang.h

# Lang

```cpp
struct Lang: std::enable_shared_from_this<Lang>;
```
多国语言类

### 类摘要

```cpp
struct Lang: std::enable_shared_from_this<Lang> {
	Lang(const std::string& str_key) : _key(str_key) {}
	Lang(void) = default;
	operator const char* (void) {
		return _lang ? _lang->m[_key].c_str() : "";
	}
	std::string operator+ (const std::string& str) const {
		if (!_lang) return str;
		return _lang->m[_key] + str;
	}
	std::string operator+ (const char* str) const {
		return *this + std::string(str);
	}
	static void setLang(Lang* lang) {
        assert(lang);
        _lang.reset(lang);
	}
	std::shared_ptr<Lang>& getCurLang(void) {
		return _lang;
	}
	virtual ~Lang(void) {}
protected:
	std::map<std::string, std::string> m;
private:
	const std::string            _key;
    static std::shared_ptr<Lang> _lang;
};
```

* `operator const char* (void);`

	使 `Lang` 类型可以转换为 `const char*` 类型

* `std::string operator+ (const std::string& str) const;`
* `std::string operator+ (const char* str) const;`
	
	使 `Lang` 类型拥有 可以和 `std::string` 或 `char*` 类型 进行字符串拼接 的功能。

### LangEN

```cpp
struct LangEN:  Lang {
	LangEN(void) {
		m["hello"] = "hello";
		m["hehe"] = "hehe";
	}
};
```

### LangCN

```cpp
// 注意此类所在文件的编码格式

struct LangCN : public Lang {
	LangCN(void) {
		m["hello"] = "你好";
		m["hehe"] = "呵呵";
	}
};
```

