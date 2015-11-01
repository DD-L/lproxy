#! /usr/bin/python
# encoding=utf-8
# python gzip module [logrotate]

'''
    日志转储 python部分
    
    1. 方法摘要:
        1. read_gz_file(path) 读取gz文件内容
        2. write_gz_file(path, content) 创建gz文件
        3. read_txt_write_gz(tpath, gzpath) 读取text文件并创建gz文件
        4. logrotate(filename) 暴露给C程序的函数, 参数为日志文件名
    2. 遗留部分:
        只保留最近N天的日志分片. 由于开发进度要求, 此版本暂不实现, 留作遗留.    

假如日志文件为: example.log

则生成压缩分片文件名的格式如下所示:

可能的情况:
example.log.2014-12-24_00-37-33.gz
example.log.2014-12-24_00-37-34.gz
example.log.2014-12-24_00-37-34_01.gz
example.log.2014-12-24_00-37-34_02.gz
example.log.2014-12-24_00-37-34_03.gz
example.log.2014-12-24_00-37-34_04.gz
example.log.2014-12-24_00-37-35.gz
example.log.2014-12-24_00-37-35_01.gz
example.log.2014-12-24_00-37-35_02.gz
example.log.2014-12-24_00-38-51.gz
    

详见 logrotate.h
'''


import os
import datetime
import time
import gzip
'''
    gzip.open(filename, mode='rb', compresslevel=9, encoding=None, errors=None, newline=None)
        打开一个gzip已经压缩好的gzip格式的文件，并返回一个文件对象：file object.
        参数filename可以是真是的文件名(a str or bytes对象)，或着是已经存在的读写文件对象。
        参数mode在操作二进制的时候使用：'r','rb','a','ab','wb'
                 操作text的时候使用：'rt,'at','wt'
                 默认是：'rb'
        参数compresslevel是0-9的数值。

    class gzip.GzipFile(filename=None, mode=None, compresslevel=9, fileobj=None, mtime=None)
        
'''

#是否显示日志信息
SHOW_LOG = True

def read_gz_file(path):
    '''read the existing gzip-format file,and return the content of this file'''
    if os.path.exists(path):
        #the function open(filename, mode = 'rb'),so the mode argument is default is 'rb'
        if SHOW_LOG:
            print('open file:[{}]'.format(path))
        with gzip.open(path, 'rb') as pf:
            return pf.read()
    else:
        print('the path [{}] is not exist!'.format(path))

def write_gz_file(path, content):
    '''write the byte-format content into the gzip-format file
    so,with this way,we can creat the file if the path doesn't exist.will
    we can write the content into the file if the file existing'''
    if SHOW_LOG:
        print('write file:[{}] content:[{}]'.format(path, content))
    with gzip.open(path, 'wb') as f:
        f.write(content)
        
def read_txt_write_gz(tpath, gzpath):
    '''read the txt-format file with 'rb' and write this file content
    to the gzip-format file'''

    i = 1
    if os.path.exists(gzpath + '.gz'):
        newgzfile = gzpath + '_%02d' %(i)
        while (os.path.exists(newgzfile + '.gz')):
            i = i + 1
            newgzfile = gzpath + '_%02d' %(i)
        gzpath = newgzfile
    
    gzpath = gzpath + '.gz'
    
    if os.path.exists(tpath):
        if SHOW_LOG:
            print('open file:[{}]'.format(tpath))
        with open(tpath, 'rb') as t:
            if SHOW_LOG:
                print('open file:[{}]'.format(gzpath))
            with gzip.open(gzpath, 'wb') as g:
                if SHOW_LOG:
                    print('write content: [{}]'.format(t))
                g.writelines(t)
                if SHOW_LOG:
                    print('write completed...')
        return True
    else:
        print('the path [{}] is not exist!'.format(tpath))
        return False

def logrotate(filename):
    global SHOW_LOG
    SHOW_LOG = True

    ltime = time.localtime(time.time())
    timeStr = time.strftime("%Y-%m-%d_%H-%M-%S", ltime)
    gzfile = filename + '.' + timeStr

    return read_txt_write_gz(filename, gzfile)


def main():
    print logrotate("./example.log")

    #init()
    #content = b'this is a byte message!'
    #write_gz_file(GZ_FILE_PATH, content)
    #con =  read_gz_file(GZ_FILE_PATH)
    #print(con)
    #print('#' * 50)
    #content_str = 'this is a str message!'
    #write_gz_file(GZ_FILE_PATH, bytes(content_str, 'utf-8'))
    #con = read_gz_file(GZ_FILE_PATH)
    #print(con)
    #print('#' * 50)
    #read_txt_write_gz(TXT_FILE_PATH, GZ_FILE_PATH)
    #con = read_gz_file(GZ_FILE_PATH)
    #print(con)

#if __name__ == '__main__':
#    main()


