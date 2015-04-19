#!/bin/bash
# SOURCE: http://mywiki.wooledge.org/BashFAQ/020:wq
# rename all the *.mp3 files in the current directory to use underscores in
# their names instead of spaces:
for file in ./*\ *.mp3; do
  mv "$file" "${file// /_}"
done

# NOTE:
# $ foo="with spaces"
# $ echo ${foo// /_}
# with_spaces
