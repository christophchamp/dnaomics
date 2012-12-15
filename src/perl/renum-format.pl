#!/usr/bin/perl

## AUTHOR: P. Christoph Champ <christoph.champ@gmail.com>
## DATE: 26-Oct-2004

# This program will take a sequence that has multiple residues with the 
# same number and renumber them (so CHARMM can run it as a single chain).

$file = $ARGV[0]; print STDERR " >>Setting 'file' = $file\n"; ## Reads in file for later writing

$prevresnum = 0;
$prevrestype = "";
$i = 0;

&opening_file_read($file);
open(FILE, "<$file") || die &error_reading($file);
@line = <FILE>;
close FILE;

&opening_file_write($file);
open(FILE, ">$file") || die &error_writing($file);
foreach $line (@line)
{
	if ($line =~ /^ATOM/)
	{
#0         10        20        30        40        50        60        70        80
#|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|----|
# ATOM    112  CG  PHE A 116      -6.655   6.846  59.756  1.00 47.53      ZIFA
# ATOM   1598  N   ASP     1      42.386  16.230  -1.871  1.00  0.00      1AAA
# ATOM   4845  N   ARG P   1      54.576  -2.462  38.781  1.00 62.82           N

		$begin = substr($line, 0, 20);
		$end = substr($line, 72, 5);
		$xyz = substr($line, 27, 28); # 28 chars
		$atomtype = substr($line, 12, 4);
		$restype = substr($line, 17, 3);
		$resnum = substr($line, 24, 5);

		$resnum =~ s/\s+//g;
		$end =~ s/\n//g;

		if($ARGV[1] eq "A") {    $trash = "1.00  0.00      1AAA"; }
		elsif($ARGV[1] eq "B") { $trash = "1.00  0.00      1BBB"; }

		if( ($prevresnum eq $resnum) && ($prevrestype eq $restype) ) {
			# DO NOTHING
		} else {
			$i++;
		}
		#printf FILE ("%20s  %4d %-50s\n", $begin,$i,$end);
		printf FILE ("%20s  %4d %-28s %-22s\n", $begin,$i,$xyz,$trash);
		$prevresnum = $resnum;
		$prevrestype = $restype;
	}
}
print FILE ("TER\n");
close FILE;

sub error_reading
{
	use strict;
	my $error_file = shift;
	print "\n*** Error! *** Can't open $error_file for reading: $!.\n(Note: The script '$0' did not finish properly!)\n\n";
	exit;
}
sub error_writing
{
	use strict;
	my $error_file = shift;
	print "\n*** Error! *** Can't open $error_file for reading: $!.\n(Note: The script '$0' did not finish properly!)\n\n";
	exit;
}
sub opening_file_read
{
	use strict;
	my $open_file = shift;
	print STDERR " >>Opening \"$open_file\" for reading . . .\n";
}
sub opening_file_write
{
	use strict;
	my $open_file = shift;
	print STDERR " >>Opening \"$open_file\" for writing . . .\n";
}
