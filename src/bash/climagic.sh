## Random assortment of "CLI magic" commands

ffmpeg -f x11grab -s wxga -r 25 -i :0.0 -sameq ./out.mpg

echo #/[\x80-\xff]
head -1 ../../screen_record-01.mkv  >>foo
echo "$(TZ=GMT date -d @0) | $(TZ=GMT date -d @$((2**31-1)))"
apt-cache search swaks
sudo apt-get install swaks
swaks --to foobar@comcast\.net --server mx1.comcast\.com --quit-after RCPT
sudo apt-get install ascii
ascii -s "show me the codes" | column -t
ascii -s "a" | column -t
ascii -s "0" | column -t
ascii -x
ascii || man ascii
find . -name '*\ *' # Find files under the current directory tree that have a space in the filename.
printf "hello" | wc -l 
#getent passwd|while IFS=: read -r user n uid n n home n;do if [[ $uid -ge 500 ]];then printf "$user ";du -sh $home;fi;done # space per user
sudo apt-get install sox
play -n synth sine 8000 bend 0.5,-1800,5 flanger 0 3 0 20 10 tri 20 quad trim 0 7 # Virtual fireworks noises (WARNING: screamer)
#play -n synth whitenoise 200 fade 0.2 1 1 trim 0 0.5 ; sleep 1 ; play -n synth whitenoise 200 fade 0 1 1 trim 0 1 # Bottle rocket sound
look frag
look .| egrep "^[a-z]{1,5}$" |while read word; do [ -e /tmp/$word ] && echo $word ; done # Maybe find file/dirs in an unreadable /tmp dir.
awk '(NR%2==0) { print $NF }' data.txt # Print the last field of every other line in the file data.txt. Use NR%2==1 for the "other" line.
printf "\xE2\x98\x95\n"
#awk '{print $4}' apache_log|sort -n|cut -c1-15|uniq -c|awk '{b="";for(i=0;i<$1/10;i++){b=b"#"}; print $0 " " b;}' # Request by hour graph.
#for vid in *.avi ; do mplayer -ao "pcm:file=${vid%%.avi}.wav:fast" -vo null "$vid" ; done # Fast extract audio track from a set of videos.
dpkg -S $( which notify-send )
#sox -m -v1 24.flac -v-1 24_CTF.flac signal.wav silence 2 5 2% # Use sox to invert track and get hidden audio signal
yes "$(seq 231 -1 16)" | while read i; do printf "\x1b[48;5;${i}m\n"; sleep .02; done # A rainbow in your shell.
ls -l --time=atime 
ls -l
awk '/session opened/ {print $(NF-2)}' /var/log/auth.log 
rec -t wav - >foo.wav
play foo.wav 
#rec -t wav - | ssh remotehost play -t wav - # Remote intercom system. "Kent, this is God!"
#lsof -p 1234 |awk '{if ($4=="txt"){print $NF}}' |head -1 # Show the path to the executable that was run to start PID 1234. Need permission.
mplayer -vo png -frames 1 tv:// -tv width=1280:height=720 # Take a single 1280x720 picture of your TV input, which is often your webcam.
qiv 00000001.png 
df -hP |column -t |tee >( head -n1 > /dev/stderr ) |grep % |sort -k5nr # Order filesystems by percent usage and keep header in place.
function box(){ t="$1xxxx";c=${2:-=}; echo ${t//?/$c}; echo "$c $1 $c"; echo ${t//?/$c}; } # Make box around text
box text
#awk '{a[$1] += $10} END {for (h in a) print h " " a[h]}' access_log | sort -k 2 -nr | head -10 # Display top bandwidth hogs on website.
lsof -i TCP:80 # Show what processes are using port 80 either locally or remotely. Need to be root for unowned processes.
apt-cache search xdotool
#x=0;y=0;while [[ $y -lt 500 ]] ; do xdotool mousemove --polar $x $y ; x=$(($x+3));y=$(($y+1)); sleep 0.001; done # Mouse spiral
sudo apt-get install xdotool
ssh bandit0@bandit.labs.overthewire.org
for i in {A..Z}; do echo $i; done |nl |grep "[XKCD]" |awk '{sum+=$1} END {print sum}' # The real meaning of #XKCD
echo "scale=10; 3.1415926535 * (1337.0/100.0)" | bc 
printf '%d\n' "'*"  # returns "42"
#tar xvf program-1.2.3.tar.gz ; cd ${_%%.tar.gz} # Untar a program file and cd into the directory it created without the .tar.gz extension.
#!#:$:s/.png/.jpg/
for f in *.wav; do lame --vbr-new -V 3 "$f" "${f%.wav}.mp3"; done # Convert to mp3.
for f in *.wav; do lame --vbr-new -V 3 "$f" "${f%.wav}.mp3"; done # Convert to mp3.
#xmodmap -e "keycode 166 = 0x0000" -e "keycode 167 = 0x0000" # Disable the Back/Forward keys 
#find . -maxdepth 1 -name '*.svg' |while IFS=$'\n' read f ; do inkscape "$f" --export-png="PNG/${f%%.svg}.png"; done # SVG 2 PNG in CWD.
play -q -n synth sine F2 sine C3 remix - fade 0 4 .1 norm -4 bend 0.5,2399,2 fade 0 4.0 0.5
echo '(play -q -n synth sine F2 sine C3 remix - fade 0 4 .1 norm -4 bend 0.5,2399,2 fade 0 4.0 0.5 &)' >> ~/.bashrc # THXsh startup sound.
#find ./music -name \*.mp3 -exec cp {} ./new \; # Idea: Backslashing the * glob instead of quoting the expression
tarbomb(){ [[ $( tar tf "$1" |sed 's,^\./,,' |awk -F/ '{print $1}' |sort |uniq |wc -l ) -eq 1 ]] && echo "OK" || echo 'Tarbomb!'; } # Detect
tarbomb ~/tmp/supercat-0.5.5.tar.gz 
#while [[ $( date +%A ) != "Friday" ]]; do echo Its not Friday yet ; sleep 1h ; done ; echo "Yea Friday" # Its not Friday yet?
#find ~/path/to/files -mmin -2 -execdir mv -t ./target/ {} + # Move all files modified in the last 2 mins to ./target
#42 = 101010 in binary
#diff <(cd dir1 ; ls -1 | sort) <(cd dir2 ; ls -1 | sort) # Show the differences between two directories. comm can also be good for this
last -da | egrep -v "^(root|reboot|asmith) " # See the last logged in users, but filter out entries for root, reboots and asmith.
grep -o --binary-files=text '[[:alpha:]]' /dev/urandom |tr -d '[a-zA-Z]' |xargs -n $(($COLUMNS/2)) |tr -d ' '| lolcat -f | pv -L80k # 4fun
#egrep -v "(^#|^\ *$)" httpd.conf | less # Only view the active configuration in a heavily commented httpd.conf file.
egrep -v "(^#|^\ *$)" /etc/apache2/apache2.conf|less
sed -ni '1h;1!p;${x;p}' queuefile  # move 1st line to last
vi queuefile
echo a b c c d e f
convert -quality 75 foo.jpg !#:$:s/.jpg/.png/
echo foo bar !#:$:s/bar/baz/
join -o 1.1,2.2,1.3,1.4,1.5,1.6,1.7 -1 1 -2 1 -t: passwd shadow

==2015-02-23==
if [ -x `which screen` ]; then screen -q -ls; [ $? -ge 10 ] && screen -ls; fi #in .profile, lists screen sessions on login
tmux list-sessions 2> /dev/null # Put this in your .bashrc so that on login it will list your tmux sessions. If none, don't show the error.
grep -o -P "(?<=CRON\[)\d+" /var/log/syslog # GNU grep supports perl regex expressions. This gives only the PID on CRON lines in syslog.
sudo shutdown -h 60 & mpg123 ambientmusic/*.mp3 # Play music for 60 minutes and then shutdown. Like sleep on alarm clock.
kill -USR1 $( pidof dd ) # In Linux, find running dd processes and send them a signal to print out their progress.
[ -f /etc/shadow ]&&while :;do N=$(($RANDOM%$(tput cols)));for i in $(seq 1 $N);do echo -n " ";done;echo \*;done # Happy Groundhog Day!
apt-file search /usr/bin/apxs2 # On Debian, find out which non-installed package provides a file. Need to install apt-file package first.
comm -23 <(grep -rl foo . |sort) <(grep -rl bar . |sort) # When adding "bar" to project, which files contain foo but not bar?

==ubuntu==
dpkg-awk -f=/var/lib/dpkg/available "Package:^[aA]s.*" -- Package Version
dpkg -l | awk '/^ii/ {print $2, $3}'
dpkg -l | awk -FS="\t" '/^ii/ {print $2, $3}'|head
dpkg -l | awk '/^ii/ {print $2, $3}' |head
apt-config dump | less
aptitude search ~o
dpkg --get-selections | head
dpkg-query -Wf '${Package}\t${Version}\n'|head

==External links==
* http://overthewire.org/wargames/bandit/
* http://howfuckedismydistro.com/
* http://vim-adventures.com/
* http://www.climagic.org/txt/difference-between-bash-shell-last-argument-reference-methods.html
