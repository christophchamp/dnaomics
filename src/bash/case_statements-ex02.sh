#!/bin/bash
## SOURCE: http://www.youtube.com/watch?v=clWeiCV_Emg
x=0

function apples(){
	clear
	echo "Do you like Red or Green Apples?"
	read apples
	case "$apples" in
		[Gg]reen) echo "Oh, you like Green Apples?";;
		[Rr]ed) echo "Good, you like Red Apples!";;
		*) echo "Um? I wouldn't eat apples that colour.";;
	esac
	x=1
}

while [ $x = 0 ]
do
	clear
	echo "Do you like 'apples' or 'oranges'?"
	read fruit

	case "$fruit" in
		[Aa]pples)
		apples
		x=1
		;;
		[Oo]ranges)
		echo "You like 'oranges'"
		x=1
		;;
		*)
		echo "That is not an option."
		sleep 1
		clear
		echo "Please type either 'apples' or 'oranges'"
		clear
		sleep 1
		;;
	esac
done
