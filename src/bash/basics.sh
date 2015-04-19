#!/bin/bash
echo {1,5,10}
expr 5 + 2
for ((i = 0; i <= 100; i++)); do ((j = j + i)); done; echo $j
z=0;for n in `seq 1 100`; do z=`expr $z + $n`; done; echo $z && unset z
n=100; echo $(seq $n | tr '\n' '+'; echo 0)|bc -l
uid=40; if (( uid > 10 && uid < 100 )); then echo True; fi # arithmetic condition
i=5; let i+=5 && echo $i # 10
echo 24/9|bc -l # 2.6666..
a=24; let "a %= 9" && echo $a # 6
echo "270 modulo 8 = $a  (270 / 8 = 33, remainder $a)"
a=6;let "t = a<7?7:11" && echo $t # 7
solaris=(serv01 serv02 serv07 ns1 ns2)
echo solaris is installed on ${solaris[2]}
tar zcvf foobar{.tar.gz,} # create a tarball of dir "foobar"
