#!/bin/sh

awk 'FNR==NR{t=(length>=t)?length:t;next}length<t{for(o=1;o<=t-length;o++)s=s "|";$0=$0s;s=""}1' filename filename

wc -L filename
wc --max-line-length filename

perl -ne 'print ($l = $_) if (length > length($l));' filename | tail -1

awk '{print length, $0}' filename | sort -nr | head -1
awk '{ if ( length > x ) { x = length } } END{ print x }'

awk '{ if (length($0) > max) {max = length($0); maxline = $0} } END { print maxline }' filename
