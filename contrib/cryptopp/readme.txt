1. clone cryptopp on github.com
    $ #git clone https://github.com/weidai11/cryptopp.git
    $ git clone https://github.com/DD-L/cryptopp
	$ # or
	$ # tar xvfJ ./cryptopp.bak.tar.xz; mv cryptopp.bak cryptopp
    
2. build libcryptopp.a
    $ cd cryptopp
    $ make libcryptopp.a -j 4
    $ #make libcryptopp.a libcryptopp.so cryptest.exe

3. build libcryptopp.a for Android on Linux

	http://cryptopp.com/wiki/Android_(Command_Line)

	e.g.
	$ export ANDROID_NDK_ROOT=/path/to/android-ndk-r10e
	$ . ./setenv-android.sh armeabi-v7a gnu-static
	$ cd cryptopp
	$ mv GNUmakefile-cross GNUmakefile-cross.old
	$ cp ../GNUmakefile-cross ./GNUmakefile-cross
	$ #make -f GNUmakefile-cross static dynamic cryptest.exe
	$ make -f GNUmakefile-cross static
	$ mv GNUmakefile-cross.old GNUmakefile-cross
      
4. build libcryptopp.a for Android on 'Cygwin-win32 / NDK for win32-64bit'

	http://cryptopp.com/wiki/Android_(Command_Line)
	
	e.g.
	$ export ANDROID_NDK_ROOT=/d/Qt/android/android-ndk-r10e
	$ . ./setenv-android.sh armeabi-v7a gnu-static
	$ export ANDROID_NDK_ROOT=/cygdrive/d/Qt/android/android-ndk-r10e
	$ . ./setenv-android.sh armeabi-v7a gnu-static
	$ export ANDROID_NDK_ROOT="D:/Qt/android/android-ndk-r10e"
	$ . ./setenv-android.sh armeabi-v7a gnu-static
	$ cd cryptopp
	$ mv GNUmakefile-cross GNUmakefile-cross.old
	$ cp ../GNUmakefile-cross ./GNUmakefile-cross
	$ #make -f GNUmakefile-cross static dynamic cryptest.exe
	$ make -f GNUmakefile-cross static
	$ mv GNUmakefile-cross.old GNUmakefile-cross

