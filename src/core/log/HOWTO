1. Build log library
	
	target:
		lib/loglib.a
		lib/loglib.so

	command:
		$ Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE'
		$ make loglib Container_Type=$Container_Type

	tips:
		-DLOG_PRIORITY_QUEUE 
			Log container using 'priority queue'. 
			by default, it will output priority the 'FATAL' level logs  
		-DLOG_LOCKFREE_QUEUE
			Log container using 'lockfree queue', 

2. Build log demo

	target:
		demo/bin/demo.*

	command:
		$ Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE'
		$ make demo Container_Type=$Container_Type
		$ # or
		$ # make demo Container_Type=$Container_Type USE_LOCK=-DLOG_USE_LOCK
		$ # make demo Container_Type=$Container_Type USE_LOCK=-DNO_LOG_USE_LOCK

	tips:
		...
		'-DLOG_USE_LOCK' see log/init_simple.h


3. Build test
	target:
		test/bin/*

	command:
		$ make test

