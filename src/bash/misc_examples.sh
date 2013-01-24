# divide lines of one file by lines of another
echo "`wc -l bar|gawk '{print $1}'` / `wc -l foo|gawk '{print $1}'`"|bc -l

# use sed like grep
df -h|awk '/sda3/ {print $5}'
df -h|awk '/sda3/ {print $1 " " $5}'
df -h|awk '/sda[0-9]/ {print $1 " " $5}'
echo "123 Testing" | awk '{ if($1 ~ /^123/) print }'
