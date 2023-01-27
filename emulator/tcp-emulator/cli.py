#!/usr/bin/python
# -*- coding: utf-8 -*-
import socket
import sys
import time
import os

chunk_size = 1024
fileName = 'm1-15-12-2022_09-37-16.jpg'
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_address = ('127.0.0.1', 10001)

sock.connect(server_address)

filesize = os.path.getsize(fileName)

print(filesize)
fileinfo = (fileName + '\0').encode() + filesize.to_bytes(4, byteorder='little')

print(fileinfo)

sock.send(fileinfo)

f = open(fileName, 'rb')

for chunk in iter(lambda: f.read(chunk_size), b''):
    sock.send(chunk)
    
    # Имитация медленного канала
    time.sleep(0.015)
print('Sending done')
sock.shutdown(socket.SHUT_WR)
f.close()
sock.close()