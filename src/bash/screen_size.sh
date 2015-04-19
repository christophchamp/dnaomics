#!/bin/bash
SERVER=`basename $0`
PORT="3389"

h=$(echo "scale=0;(($(xrandr | grep '*+' | sed 's/x/ /g' | awk '{print $1}' | sort -n | head -1 )/100)*90)" | bc)
v=$(echo "scale=0;(($(xrandr | grep '*+' | sed 's/x/ /g' | awk '{print $2}' | sort -n | head -1 )/100)*90)" | bc)
SIZE=${h}x${v}
echo -e "${SIZE}\n"
