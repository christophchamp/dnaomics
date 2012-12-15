#!/usr/local/bin/perl

@fields = qw(seqname source feature start end score strand frame group);

$width = 50;

use SeqFileIndex;
use GFFTransform;

$usage .= "$0 - extract GFF-specified sequences from a sequence database\n";
$usage .= "\n";
$usage .= "Usage: $0 [-prefix prefix|-name expr] [-trans <GFF transformation file>] <sequence file> [<GFF file>]\n";
$usage .= "\n";

while (@ARGV) {
    last unless $ARGV[0] =~ /^-/;
    $opt = lc shift;
    if ($opt eq "-prefix") { defined($prefix = shift) or die $usage }
    elsif ($opt eq "-name") { defined($nameexpr = shift) or die $usage }
    elsif ($opt eq "-trans") { defined($trans = shift) or die $usage }
    else { die "$usage\nUnknown option: $opt\n" }
}

if (defined $nameexpr) { for ($i=0;$i<@fields;$i++) { $nameexpr =~ s/\$gff$fields[$i]/\$gff->[$i]/g } }

if (defined $trans) { read_transformation($trans); sort_seqnames() }

@ARGV>=1 or die $usage;
$seqfile = shift;

$index = new SeqFileIndex($seqfile);

while (<>) {
    s/#.*//;
    next unless /\S/;
    chomp;
    @f = split /\t/;
    $gff = \@f;
    ++$count;
    if (defined $trans) { @nse = back_transform(@f[0,3,4]) }
    else { @nse = ([@f[0,3,4]]) }
    if (@nse) {
	if (defined $prefix) { $name = "$prefix$count" }
	elsif (defined $nameexpr) { $name = eval $nameexpr }
	else { $name = "$f[0]/$f[3]-$f[4]" }
	$seq = "";
	foreach $nse (@nse) { $seq .= $index->getseq(@$nse) }
	if (length $seq) {
	    print ">$name\n";
	    for ($i=0;$i<length $seq;$i+=$width) { print substr($seq,$i,$width),"\n" }
	}
    }
}

