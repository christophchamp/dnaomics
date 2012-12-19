#!/bin/bash
# 2012-12-19
# Pair the following files:
# 01_DE.htm 01_EN.htm 01_FR.htm 01_IT.htm 02_EN.htm 02_IT.htm 02_DK.htm
# like so:
# 01_EN.htm 01_DE.htm
# 01_EN.htm 01_FR.htm
# 01_EN.htm 01_IT.htm
# 02_EN.htm 02_IT.htm
# 02_EN.htm 02_DK.htm

ls -1 *EN.htm | while IFS= read -r file
do 
    ls -1 ${file%_*}* | while IFS= read -r match
    do
    if [ "$file" != "$match" ]; then 
        echo "$file" "$match"
    fi 
    done 
done
