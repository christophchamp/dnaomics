#!/bin/bash
# Other solutions:
#
# $ echo "Alice" | rev
# $ rev<<<"Alice"
# $ perl -ne 'chomp;print scalar reverse . "\n";'<<<"Alice"
# $ echo 'Alice' | perl -ne 'chomp;print scalar reverse . "\n";'

input="$1"
reverse=""
 
len=${#input}
for (( i=$len-1; i>=0; i-- )); do
    reverse="$reverse${input:$i:1}"
done
 
echo "$reverse"
