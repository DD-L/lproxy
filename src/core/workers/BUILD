1. Build logworkers link

    target:
        link/logworkers.o

    command:
        e.g.
        $ make Container_Type=-DLOG_PRIORITY_QUEUE
        $ # Container_Type=-DLOG_PRIORITY_QUEUE # or '-DLOG_LOCKFREE_QUEUE'

    tips:
        -DLOG_PRIORITY_QUEUE
            Log container using 'priority queue'.
            by default, it will output priority the 'FATAL' level logs
        -DLOG_LOCKFREE_QUEUE
            Log container using 'lockfree queue'

2. Build demo
    
    target:
        ./demo/bin/test_logworkers.static
        ./demo/bin/test_logworkers

    commad:
        e.g.
        $ cd ./demo
        $ make Container_Type=-DLOG_PRIORITY_QUEUE PYTHON_INCLUDE=/usr/include/python2.7 PYTHON_LIB_PATH=/usr/lib PYTHON_LIB=-lpython2.7
    tips:
        PYTHON_LIB_PATH is the path to python library files, libpython2.7.so libpython2.7.a ...
                        default path: /usr/lib
        PYTHON_LIB is '-l' option of gcc, -lpython2.7, -lpython2.7.dll ...
                        please using -lpython2.7.dll on Cygwin.



