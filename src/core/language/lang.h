#ifndef LANG_H
#define LANG_H

#include <string>
#include <map>
#include <memory>
#include <assert.h>

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


//#include "langEN.hpp"
//#include "langCN.hpp"

#endif //LANG_H

