#!/usr/bin/perl

## AUTHOR: P. Christoph Champ <christoph.champ@gmail.com>
## DATE: 26-Oct-2004

# Checks for duplication atomtypes

$file = $ARGV[0];
$linenum = 1;

open(FILE, "<$file") || die "Can't open file: $!\n";
@line = <FILE>;
close FILE;

foreach $line (@line) {
        if ($line =~ /^ATOM/) {
		print $line;
	}
}
