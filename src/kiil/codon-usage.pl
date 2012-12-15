#! /usr/bin/perl -w

my %usage;
foreach $a ("A","U","C","G") {
	foreach $b ("A","U","C","G") {
		foreach $c ("A","U","C","G") {
			$usage{"$a$b$c"} = 0;
		}
	}
}

my $tot=0;
my $orf = "";
my $cds = "";

open TRANS , '/home/people/pfh/scripts/codon-usage/trans.txt';
while (<TRANS>) {
	chomp;
	($triplet,$aa) = split (/\t+/,$_);
	$translationTable{$triplet} = $aa;
}
close TRANS;

while (defined($line = <STDIN>)) {
	chomp($line);
	if (($tag) = ($line =~ /^>(.*)/)) {
		$cds++;
		if ($orf ne "") {
			for ($x = 0 ; $x < length($orf);$x +=3) {
				$usage{substr($orf,$x,3)}++;
				$tot++;
			}
			warn "WARNING: FRAMESHIFT DETECTED at $prevTag\n" if (length($orf) % 3);
			$orf = "";
		$prevTag = $tag;
		}
	} elsif (($dna) = ($line =~ /^([ATGCatgcXxNn]+)/)) {
		$dna =~ tr/Tt/Uu/;
		$orf .= uc($dna);
	}
}

print "#CODONS:$tot\tCDS:$cds\n";
foreach $a ("A","U","C","G") {
	foreach $b ("A","U","C","G") {
		foreach $c ("A","U","C","G") {
			print "$a$b$c\t".sprintf("%0.5f",$usage{"$a$b$c"}/$tot)."\t".$usage{"$a$b$c"}."\t".$translationTable{"$a$b$c"}."\n";
		}
	}
}
