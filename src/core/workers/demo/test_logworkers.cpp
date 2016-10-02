#include <iostream>
#include "workers/logworkers.h"

void produce_log() {
	loginfo("thread id:[" << boost::this_thread::get_id() << "] Start..." );
	for (int i = 0; i < 5; ++i) {
		logwarn("test...");
	}
}

void test() {
    using namespace std;
	bool ret = LogOutput_t::get_instance().bind(cout);
	cout << boolalpha << ret << endl;
	
	Workers<LogWorkers>& workers = LogWorkers::get_mutable_instance();
	//workers.start(2);
	workers.start(3, true);
	boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
	workers.interrupt_all(/*false*/);
	//boost::this_thread::sleep_for(chrono::milliseconds(100));
}

int main(int argc, char** argv) {
	produce_log();
	test();
	return 0;
}
///////////////////////////////////////////////////

//#include "store.h"
//
//class rw_data {
//	private:
//		int m_x;
//		shared_mutex rw_mu;
//	public:
//		rw_data() : m_x(0) {}
//		void write() {
//			unique_lock<shared_mutex> ul(rw_mu);
//			++m_x;
//		}
//		void read(int *x) {
//			shared_lock<shared_mutex> sl(rw_mu);
//			*x = m_x;
//		}
//};
//void writer(rw_data &d) {
//	for (int i = 0; i < 20; ++i) {
//		this_thread::sleep_for(chrono::milliseconds(10));
//		d.write();
//	}
//}
//mutex io_mu;
//void reader(rw_data &d) {
//	int x;
//	for (int i = 0; i < 10; ++i) {
//		this_thread::sleep_for(chrono::milliseconds(5));
//		d.read(&x);
//		mutex::scoped_lock lock(io_mu);
//		cout << "reader: " << x << endl;
//	}
//}
//void test2() {
//	rw_data d;
//	thread_group pool;
//	pool.create_thread(boost::bind(writer, boost::ref(d)));
//	pool.create_thread(boost::bind(writer, boost::ref(d)));
//
//	pool.create_thread(boost::bind(reader, boost::ref(d)));
//	pool.create_thread(boost::bind(reader, boost::ref(d)));
//	pool.create_thread(boost::bind(reader, boost::ref(d)));
//	pool.create_thread(boost::bind(reader, boost::ref(d)));
//
//	pool.join_all();
//}
//
//int main(int argc, char** argv) {
//	test2();
//	return 0;
//}
