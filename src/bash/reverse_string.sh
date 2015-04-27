#!/bin/bash
# Other solutions:
#
# $ echo "Alice" | rev
# $ rev<<<"Alice"
# $ echo Alice | python -c 'import sys;print(sys.stdin.read().strip()[::-1])'
# $ python -c 'import sys;print(sys.stdin.read().strip()[::-1])' <<< Alice
# $ perl -ne 'chomp;print scalar reverse . "\n";'<<<"Alice"
# $ echo 'Alice' | perl -ne 'chomp;print scalar reverse . "\n";'
# $ echo Alice | php -r 'print strrev(trim(fgets(STDIN)));'
# $ php -r 'print strrev(trim(fgets(STDIN)));' <<< Alice

input="$1"
reverse=""
 
len=${#input}
for (( i=$len-1; i>=0; i-- )); do
    reverse="$reverse${input:$i:1}"
done
 
echo "$reverse"
