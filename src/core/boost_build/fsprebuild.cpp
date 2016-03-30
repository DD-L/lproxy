/*************************************************************************
	> File Name:    fsprebuild.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/30 6:40:05
 ************************************************************************/

#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif

#include <libs/filesystem/src/codecvt_error_category.cpp>
#include <libs/filesystem/src/operations.cpp>
#include <libs/filesystem/src/path_traits.cpp>
//#include <libs/filesystem/src/path.cpp> 
// 为避免冲突，可能要在 path.cpp 中注释掉:
// default_codecvt_buf_size / dot / dot_path / dot_dot_path 的定义
#include <boost_1_57_0__libs__filesystem__src__path.cpp>
// boost_1_57_0__libs__filesystem__src__path.cpp 是经过修改后的
// boost-1.57.0 版本的 libs/filesystem/src/path.cpp
// 这里修改的内容是: 
// 1. 修改 # include "windows_file_codecvt.hpp" 为
//      include <libs/filesystem/src/windows_file_codecvt.hpp>
// 2. 将两处 dot 定义 注释掉
//    1. const wchar_t dot = L'.';
//    2. const char dot = '.';
// 即可编译通过
//
#include <libs/filesystem/src/portability.cpp>
#include <libs/filesystem/src/unique_path.cpp>
#include <libs/filesystem/src/utf8_codecvt_facet.cpp>
#include <libs/filesystem/src/windows_file_codecvt.cpp>


