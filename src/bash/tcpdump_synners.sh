#!/bin/bash
while :; do
  date; 
  tcpdump -i wlan0 -n -c 100 \
    'tcp[tcpflags] & (tcp-syn) != 0' and \
    'tcp[tcpflags] & (tcp-ack) == 0' 2> /dev/null \
  | awk '{ print $3}' \
  | sort | uniq -c | sort | tail -5;
  echo;
  sleep 1
done

# ~OR~ just: 'tcp[tcpflags] == tcp-syn'
