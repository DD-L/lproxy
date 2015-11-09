1. clone cryptopp on github.com
    $ #git clone https://github.com/weidai11/cryptopp.git
    $ git clone https://github.com/DD-L/cryptopp
	$ # or
	$ # tar xvfJ ./cryptopp.bak.tar.xz; mv cryptopp.bak cryptopp
    
2. build libcryptopp.a
    $ cd cryptopp
    $ make libcryptopp.a -j 4
    $ #make libcryptopp.a libcryptopp.so cryptest.exe

