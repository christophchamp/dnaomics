# Project Gutenberg to plain text
%.pgn.txt:	%.pg.txt
	grep ^Title: $<>$@;grep ^Author: $<>>$@;grep '^Release Date:' $<>>$@;grep ^Edition: $<>>$@;grep ^Language: $<>>$@;grep '^Character set encoding:' $<>>$@
	sed -n '/^\*\*\* START/,/\*\*\* END/p' $<|sed -e 's/\*\*\* START .*$$/<text>/g' -|sed -e 's/\*\*\* END .*$$/<\/text>/g' ->>$@
