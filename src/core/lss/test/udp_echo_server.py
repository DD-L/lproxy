#! /usr/bin/env python
# -*- coding: utf-8 -*-  

# UDP Echo Server

import socket, traceback

host = ''
port = 54321

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((host, port))

while True:
    try:
        message, address = s.recvfrom(8192)
        print("Got data from", address, ": ", message)
        s.sendto(message, address)
    except (KeyboardInterrupt, SystemExit):
        raise
    except:
        traceback.print_exc()
