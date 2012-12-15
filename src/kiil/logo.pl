#! /usr/bin/perl -w
# Reads a *.orfs.fsa (fasta fil of open reading frames)
# and extracts all triplets and print a fasta file

my $orf = "";
$entry = -1;
while ( <STDIN> ) {
	chomp;
	if (/^>/) { 
		$entry++; 
		next;
	}    
	next unless /^([a-z]+)$/i;
	$a = $1;
	$a =~ tr/T/U/;
	$dna_fsa{$entry} .= $a if $a ne '';
}

foreach $entry (keys  %dna_fsa) {
	$orf = $dna_fsa{$entry};
	for ($x = 0 ; $x < length($orf)-2;$x +=3) {
		$substr = substr($orf,$x,3);
		print ">$entry..$x\n$substr\n" if $substr =~ /[AUGC]{3,3}/;
	}
}


