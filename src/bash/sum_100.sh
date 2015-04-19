#!/bin/bash
# sum = 5050
i=0
sum=0
until [ $i -gt 100 ]; do
    sum=$((sum+$i))
    let "i+=1"
done
echo $sum

# Other versions w/time:
#Here's a tricky version:

sum_to () ( 
    set -- $(seq $1)
    IFS=+
    echo "$*" | bc
)

sum=$(sum_to 5)
echo $sum         # => 15

#That's very slow for large numbers though:

time sum_to 1000000
#500000500000
#
#real    0m2.545s
#user    0m2.513s
#sys 0m0.189s

#More efficient:
sum_to_2 () { { seq $1 | tr '\n' '+'; echo 0; } | bc; }
time sum_to_2 1000000
#500000500000
#
#real    0m0.727s
#user    0m0.981s
#sys 0m0.037s

#Better:
sum_to_3 () { perl -le '$n=$ARGV[0]; $sum += $n-- while $n; print $sum' $1; }
time sum_to_3 1000000
#500000500000
#
#real    0m0.075s
#user    0m0.071s
#sys 0m0.002s

#Worser: invoking an "external" program for each number
sum_to_slow () { 
    sum=0
    for i in $(seq $1); do
        sum=$(expr $sum + $i)
    done
    echo $sum
}
date; time sum_to_slow 1000000; date
#Mon Mar 17 14:00:53 EDT 2014
#^C
date
#Mon Mar 17 14:07:01 EDT 2014
