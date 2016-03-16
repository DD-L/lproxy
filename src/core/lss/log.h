#ifndef _LSS_LOG_H_1
#define _LSS_LOG_H_1
/*************************************************************************
	> File Name:    log.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/
#include <lss/typedefine.h>
#include <log/init_simple.h>

namespace lproxy {
namespace log {

enum Which { LOCAL = 0, SERVER = 1 };

// function lproxy::log::output_thread
void output_thread(Which which);


// 返回一个 basename(filename) 的临时对象
static inline sdata_t basename(const sdata_t& filename) {
    // http://www.cplusplus.com/reference/string/string/find_last_of/
    const std::size_t found = filename.find_last_of("/\\");
    return filename.substr(found + 1);
}

// 日志输出格式化函数。
// 因为在编译时使用的是全路径，所以宏 __FILE__ 得到的也是全路径, 所以有必要
// 重新写个日志输出格式化函数，以替换默认的日志输出格式。
sdata_t output_format(const std::shared_ptr<LogVal>& val);

} // namespace lproxy::log
} // namespace lproxy

// 特别地, 重新包装下 debug 日志输出
#ifdef LSS_DEBUG
    #define lsslogdebug(log) logdebug(log)
    #define lsslogdebugEx(log, shared_ptr_extra) logdebugEx(log, shared_ptr_extra)
#else
    #define lsslogdebug(log) 
    #define lsslogdebugEx(log) 
#endif


#endif // LSS_LOG_H_1
