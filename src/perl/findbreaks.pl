#!/usr/bin/perl

## AUTHOR: P. Christoph Champ <christoph.champ@gmail.com>
## DATE: 26-Oct-2004

# CAUTION: Strongly recommended that the pdb file supplied is very basic: no multi_conformational
#          or non-standard residues, no multiple chains, no NMR structures, no DNA etc.

($pdbfile, $outfile) = @ARGV;

chomp $outfile;
if ($outfile =~ /\S/) {
} else { 
	$outfile = "findbreaks.out";
}

#$dir = tempdir( CLEANUP => 1);

%symbols = ("ALA", "A",
            "CYS", "C",
            "ASP", "D",
            "GLU", "E",
            "PHE", "F",
            "GLY", "G",
            "HIS", "H",
            "ILE", "I",
            "LYS", "K",
            "LEU", "L",
            "MET", "M",
            "ASN", "N",
            "PRO", "P",
            "GLN", "Q",
            "ARG", "R",
            "SER", "S",
            "THR", "T",
            "VAL", "V",
            "TRP", "W",
            "TYR", "Y",
            "GLX", "Z");

#if ($pdbrenum =~ /[Nn]/) {} else { system("perl pdbrenum.pl $pdbfile"); }

$prevresnum = 0;
open (TMP, "grep ' CA ' $pdbfile|");
while (<TMP>)
{
	$resnum = substr($_, 23, 3);
	$resnum =~ s/\s+//g;
	if ($resnum > $prevresnum)
	{
	    push (@calines, $_);
	    $res[$resnum] = $symbols{substr($_, 17, 3)};
	    $xca[$resnum] = substr($_, 31, 7);
	    $xca[$resnum] =~ s/\s+//g;
	    $yca[$resnum] = substr($_, 39, 7);
	    $yca[$resnum] =~ s/\s+//g;
	    $zca[$resnum] = substr($_, 47, 7);
	    $zca[$resnum] =~ s/\s+//g;
	}
	$prevresnum = $resnum;

	push (@resnums, $resnum);
}
close TMP;

$firstresindex = substr($calines[0], 23, 3);
print "First residue index is $firstresindex\n\n\n";

##### Calculation of CA-distances between consecutive pdb-residues (to check for continuity)
foreach $i (@resnums)
{
        if ($i>$firstresindex) {
		$pcadist[$i] = sqrt(($xca[$i]-$xca[$i-1])**2 + ($yca[$i]-$yca[$i-1])**2 + ($zca[$i]-$zca[$i-1])**2); }
        if ($i<$firstresindex+@calines-1) {
		$ncadist[$i] = sqrt(($xca[$i]-$xca[$i+1])**2 + ($yca[$i]-$yca[$i+1])**2 + ($zca[$i]-$zca[$i+1])**2); }

        if ($i>$firstresindex && $pcadist[$i] < 5) {
		$bc[$i]=1;
	} else {
		$bc[$i]=0;
	} # Backward continuity = yes
        if ($i<$firstresindex+@calines-1 && $ncadist[$i] < 5) {
		$fc[$i]=1;
	} else {
		$fc[$i]=0;
	} # Forward continuity = yes
}

##### Calculation of maximum exposed areas of each residue considering it's immediate 2 neighbours
open (OUT, ">findbreaks.out") || die "Cannot open findbreaks.out\n";
foreach $cline (@calines)
{
        $aa3 = substr($cline, 17, 3);
        $aa = $symbols{$aa3};
        $aaindex = substr($cline, 23, 3);
        $aaindex =~ s/\s+//g;

	if ($aaindex > $firstresindex && $bc[$aaindex] == 0 && $fc[$prevaaindex] == 0)
	{
		if ($prevaaindex < $aaindex-3 && $pcadist[$aaindex] < 12) {
			# DO NOTHING
		} else {
			print "$prevaa ".($prevaaindex)." - $aa ".$aaindex."\t Discontinuity ($pcadist[$aaindex] A)\n";
		}
	}

	print OUT "$prevaa ".($aaindex-1)." - $aa $aaindex $pcadist[$aaindex]\n";

	$prevaa = $aa;
	$prevaaindex = $aaindex;
}
close OUT;
