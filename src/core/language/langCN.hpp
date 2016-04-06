#ifndef _LANGCN_HPP_1_
#define _LANGCN_HPP_1_
// 注意此文件的编码格式

#include <language/lang.h>
struct LangCN: Lang {
	LangCN(void) {
		m["hello"] = "你好";
		m["hehe"] = "呵呵";
	}
};
#endif //_LANGCN_HPP_1_
