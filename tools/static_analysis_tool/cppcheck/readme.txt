$ # make
$ make SRCDIR=build CFGDIR=cfg HAVE_RULES=yes
$
$ # install
$ mkdir -p /opt/cppcheck/cfg/
$ cp -a ./cppcheck /opt/cppcheck/
$ cp -a ./htmlreport/cppcheck-htmlreport /opt/cppcheck/
$ cp -a ./cfg/std.cfg /opt/cppcheck/cfg/
$ # export PATH=${PATH}:/opt/cppcheck

cppcheck ��ʹ�ã�
$ cp /opt/cppcheck/cfg/std.cfg ./
$ cppcheck --xml --std=c++11 --template=gcc --enable=all -I /usr/lib/gcc/i686-pc-cygwin/4.8.3/include/c++ -j 4 SRC_DIR  2> err.xml 
$ rm ./std.cfg


cppcheck-htmlreport��ʹ��
$ #pip install Pygments
cppcheck-htmlreport --file=err.xml --report-dir=./report --source-dir=SRC_DIR



1. ���߼��

	1. ���ذ�װ��Linux����
		1. �ٷ���ַ��http://sourceforge.net/projects/cppcheck/
		2. ��װ��ʽ����ѹ��ֱ��make���б��룬��ɺ�make install���а�װ
		3. ��֤��װ��cppcheck -v �鿴�Ƿ�װ�ɹ�

	2. �ص����
		1. ��Ҫ��������ͨ�����ܷ��ֵ�ȱ�ݡ�
		2. cppcheck�ܹ����ֺܶ���󣬵����ܷ������еĴ���

2. ����ʹ��
	1. ��鵥���ļ���cppcheck foo.c
	2. ����ļ��У�cppcheck path
	3. �ų�һ���ļ�����һ���ļ��У�cppcheck -isrc/foo.c src

	4. ʹ�ܼ�����
		1. Ĭ�ϣ�--enable=error
		2. --enable=all 
		3. --enable=unusedFuntion path
		4. --enable=style

	5. �����壺
		error�����ֵĴ���
		warning��Ϊ��Ԥ��bug�����Ա�̽�����Ϣ
		style�������ʽ���⣨û��ʹ�õĺ���������Ĵ���ȣ�
		portablity����ֲ�Ծ��档�ò��������ֲ������ƽ̨�ϣ����ܳ��ּ���������
		performance�������Ż��ò��ִ��������
		information��һЩ��Ȥ����Ϣ�����Ժ��Բ����ġ�

	6. ���������ļ��У��ض���>��
	7. ���̼߳����루��߼���ٶȣ��������CPU���ܣ���cppcheck -j 4 src

3. �߼�ʹ��

	1. xml ���
		1. ʹ�÷�ʽ��cppcheck --xml-version=2 foo.cpp

		2. error�����Ԫ��

				id��error��id
				severity��error�����ʣ�error��warning......��
				msg��error�ľ�����Ϣ���̸�ʽ��
				verbose��error����Ϣ������ʽ��
				location�����Ԫ�أ�
				file��������Ի��߾���·�����ļ���
				line������
				msg����Ϣ����

	2. ����������
		cppcheck --template=vs path ��Visual Studio ����ģʽ��
		cppcheck --template=gcc path ��Gcc����ģʽ��
		cppcheck --template={"{file},{line},{severity},{id},{message}"} ���Զ���ģʽ��

	3. �����������ѡ�ض��Ĵ�����Ϣ�����
		1. ������ģʽ��cppcheck --suppress=memleak:src/file1.cpp src/
		2. ʹ���ļ�ģʽ�������˹���浽�ļ��У���cppcheck --suppressions suppressions.txt src/

4. ��������
	1. ./cppcheck test.cpp --xml 2> err.xml
	2. htmlreport/cppcheck-htmlreport --file=err.xml --report-dir=test1 --source-dir=.

5 GUIӦ�ã�cppcheck�ṩGUI���û��ӿڣ��Ƚϼ򵥣���ȥ��д��

6. �ο�cppcheck�û��ֲ�