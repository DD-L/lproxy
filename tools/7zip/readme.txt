判断主机是否有支持解压7z的工具
7z   --> 7z x archive.7z
7za  --> 7za x archive.7z
7zr  --> 7zr x archive.7z

如果都没有, 尝试安装 7zr
sudo apt-get install p7zip

或make 7zr 

tar jxvf p7zip_9.38.1_src_all.tar.bz2


先修正7z源码一处错误（否则在某些主机上编译可能会失败）：
CPP/Common/StringConvert.cpp:181:

UString &srcString = src;

看了下源码，它的本意应该是：
const UString &srcString = src;
确实是一个低级错误。

so，

$ sed -i '181c const UString &srcString = src; /*modified by Deel*/' ./CPP/Common/StringConvert.cpp

$ make 7zr



