#!/usr/bin/env python

def IP2Int(ip):
    o = map(int, ip.split('.'))
    res = (16777216 * o[0]) + (65536 * o[1]) + (256 * o[2]) + o[3]
    return res


def Int2IP(ipnum):
    o1 = int(ipnum / 16777216) % 256
    o2 = int(ipnum / 65536) % 256
    o3 = int(ipnum / 256) % 256
    o4 = int(ipnum) % 256
    return '%(o1)s.%(o2)s.%(o3)s.%(o4)s' % locals()

# Example
print('192.168.0.1 -> %s' % IP2Int('192.168.0.1'))
print('3232235521 -> %s' % Int2IP(3232235521))

import socket
# 128.93.23.234 -> 2153584618
ip = '128.93.23.234'
print('%s -> %s' % (ip, int(socket.inet_aton(ip).encode('hex'),16)))
