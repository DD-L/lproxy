#ifndef LANG_H
#define LANG_H

#include <string>
#include <map>
using std::string;

struct Lang {
	Lang(const string& str_key) : _key(str_key) {}
	Lang(void) {}
	operator const char* (void) {
		return lang ? lang->m[_key].c_str() : "";
	}
	string operator+ (const string& str) const {
		if (!lang) return str;
		return lang->m[_key] + str;
	}
	string operator+ (const char* str) const {
		return *this + string(str);
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
	std::map<string, string> m;
private:
	const string _key;
	static Lang* lang;
};


#include "langEN.hpp"
#include "langCN.hpp"

#endif //LANG_H

