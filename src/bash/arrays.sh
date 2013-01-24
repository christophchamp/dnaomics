#!/bin/bash

names[1]=Bob
names[2]=Alice
names[3]=Linus
names[4]=Christoph
names[15]=Jude
for i in {1..4}; do echo ${names[$i]}; done
#~OR~ (a better way)
for i in "${names[@]}"; do echo $i; done
