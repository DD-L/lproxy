// file: logrotate.cpp
#include "logrotate/logrotate.h"
#include "python/getpyfunc.h"
using namespace boost::python;

/*
public:
	typedef std::streampos	filesize_t; // byte
private:
	typedef std::pair<std::string, filesize_t>	value_t;
	std::map<std::ofstream*, value_t>			logm;
*/

//public:
/*static*/std::string	Logrotate::pyfilename = "./logrotate.py";
/*static*/std::string		Logrotate::pyfunc = "logrotate";

// 如果绑定失败会返回false.
// 例如绑定时文件被占用或当前ofstream之前未close(), 都将引发绑定失败.
bool Logrotate::bind(std::ofstream& logfile, 
					const std::string& filename, 
					filesize_t maxsize) {
	logfile.open(filename.c_str(), 
				std::ofstream::out | std::ofstream::app);
	if (logfile) {
		logm[&logfile] = value_t(filename, maxsize);
		return true;
	}
	return false;
}

void Logrotate::action(void) {
	for (auto it = logm.begin(); it != logm.end(); ++it) {
		//cit->first->seekp(std::ios::end);
		if (it->first->tellp() >= it->second.second/*maxsize*/) {
			// 压缩分片, 并清空原有文件日志
			if(rotate(it->second.first/*logfilename*/)) {
				// 压缩成功后就开始分片
				it->first->close();
				it->first->open(it->second.first.c_str(),
						std::ofstream::out | std::ofstream::trunc);
				it->first->close();
				it->first->open(it->second.first.c_str(), 
						std::ofstream::out | std::ofstream::app);
			}
		}
	}
}

bool Logrotate::rotate(const std::string& filename) {
	// 调用python函数压缩分片
	GetPyFunc py_logrotate(pyfilename.c_str(), pyfunc.c_str());
	object ret = py_logrotate(filename.c_str());
	return extract<bool>(ret);
}

