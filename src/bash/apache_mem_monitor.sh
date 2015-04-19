#!/bin/sh
TOTAL=`cat /proc/meminfo | grep MemTotal: | awk '{print $2}'`
USEDMEM=`cat /proc/meminfo | grep Active: | awk '{print $2}'`
LOG=/tmp/test.log
echo > $LOG
if [ "$USEDMEM" -gt 0 ]
 then
     USEDMEMPER=$[$USEDMEM * 100 / $TOTAL ]
     echo "Current used memory = $USEDMEMPER %"
     if [ "$USEDMEMPER" -gt 90 ]; then
         killall -9 apache2
         service apache2 restart
         echo "apache process killed " >> $LOG
     fi
fi
cat $LOG
