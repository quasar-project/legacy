#!/usr/bin/python3
# -*- coding: utf-8 -*-
import os
import sys
import config
import serviced
import argparse
import logging

def main(cmd):
    defaults = {'daemonize': False, 'address': '127.0.0.1:10000'}
    
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s [%(levelname)s] %(message)s",
        handlers=[
            logging.FileHandler("fsend.log"),
            logging.StreamHandler()
        ]
    )
    
    
    cfg = config.read('main', defaults)
    
    
    parser = argparse.ArgumentParser(description='Сервис для передачи файлов')
    parser.add_argument('-f', type=str, default=None, help='Имя файла')
    parser.add_argument('-a', type=str, default=cfg['address'], help='Адрес сервера для приема')
    parser.add_argument('-s', help='Статус', action='store_true')
    parser.add_argument('-d', default=cfg['daemonize'], help='Запуск в фоновом режиме', action='store_true')
    parser.add_argument('-k', help='Остановка процесса в фоновом режиме', action='store_true')
    
    args = parser.parse_args()
    
    s = serviced.serviced(args)
    
    if(args.s):
        s.status()
        return
    
    s.append(args.a, args.f)



if __name__ == "__main__":
    cmd = ''
    if(len(sys.argv) == 1):
        cmd = 'status'
    else:
        #print(sys.argv)
        #cmd = ' '.join( map(str, sys.argv[1:]) )
        for c in sys.argv[1:]:
            cmd += "{}".format(c)
    main(cmd)
