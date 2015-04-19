#!/usr/bin/perl
perl -e'$,=",";print sub { grep { $a=$_; !grep { !($a % $_) } (2..$_-1)} (2..$_[0]) }->(shift)' number
