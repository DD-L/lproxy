#ifndef LANG_H
#define LANG_H

#include <string>
#include <map>
//using std::string;

struct Lang {
	Lang(const std::string& str_key) : _key(str_key) {}
	Lang(void) {}
	operator const char* (void) {
		return lang ? lang->m[_key].c_str() : "";
	}
	std::string operator+ (const std::string& str) const {
		if (!lang) return str;
		return lang->m[_key] + str;
	}
	std::string operator+ (const char* str) const {
		return *this + std::string(str);
	}
	static void setLang(Lang* lang) {
		delete Lang::lang;
		Lang::lang = lang;
	}
	Lang* getCurLang(void) {
		return lang;
	}
	virtual ~Lang(void) {}
protected:
	std::map<std::string, std::string> m;
private:
	const std::string _key;
	static Lang*      lang;
};


#include "langEN.hpp"
#include "langCN.hpp"

#endif //LANG_H

