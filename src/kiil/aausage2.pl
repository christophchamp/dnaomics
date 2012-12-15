#!/usr/bin/perl -w

my $tot=0;
my @orfs;
my $orf = "";
my %usage;
my $x;
my @shorts;
my @names;
my @groups;

foreach $T ('A' .. 'Z') {
	$usage{$T} = 0; 
}

while (<STDIN>) {
	chomp;
	my $aa = $1 if /^([A-Z]+)$/;
	next unless defined ( $aa);
	for ( $x = 0 ; $x < length($aa) ; $x++) {
		$usage{substr($aa,$x,1)}++;
		$tot++;
	}
}

open GROUPS , 'groups.txt';
while (<GROUPS>) {
	chomp;
	@F = split (/\t+/,$_);
	push @shorts , $F[0];
	push @names , $F[1];
	push @groups , $F[2];
}
close GROUPS;

for ($x = 0 ; $x <= $#names ; $x++) {
	$name  = $shorts[$x];
	$group = $groups[$x];
	$short = $shorts[$x];
	$usg   = $usage{$name};
	$freq  = sprintf("%0.5f",$usg/$tot);
	print "$name\t$short\t$usg\t$freq\t$group\n";
}
