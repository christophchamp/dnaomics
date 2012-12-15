#!/usr/bin/perl

## AUTHOR: P. Christoph Champ <christoph.champ@gmail.com>
## DATE: 26-Oct-2004

$datfile = $ARGV[0];	print STDERR " >>Setting 'datfile' = $datfile\n";
$file = $ARGV[1];	print STDERR " >>Setting 'file' = $file\n";
$numout = $ARGV[2];	print STDERR " >>Setting 'numout' = $numout\n";
$i = 0;

&opening_file_read($file);
open(LIG, "$file") || die &error_reading($file);
while($line = <LIG>)
{
	if ($line =~ /^ATOM/)
	{
		$begin[$i] = substr($line, 0, 30);
		$x[$i] = substr($line, 30, 8);
		$y[$i] = substr($line, 38, 8);
		$z[$i] = substr($line, 46, 8);
		$end[$i] = substr($line, 54, 50);
       	 	$x[$i] =~ s/\s+//g;
        	$y[$i] =~ s/\s+//g;
        	$z[$i] =~ s/\s+//g;
		$end[$i] =~ s/\n//g;

		$i++;
	}
}
close LIG;

$chk_datfile = `wc -l $datfile`;
@chk_datfile = split(/\s+/, $chk_datfile);
if($chk_datfile[0] == 0)
{
	print STDERR "*** Error! **** The file '$datfile' is empty!\n";
}

$j = 1;
&opening_file_read($datfile);
open(FILE, "<$datfile") || die &error_reading($datfile);
while($line = <FILE>)
{
	$line =~ s/^\s+//g;
	($score, $cmx, $cmy, $cmz, $rand2, $rand1, $rand3) = split(/\s+/, $line);
#print STDERR "$score, $cmx, $cmy, $cmz, $rand2, $rand1, $rand3\n";

	$a11 = cos($rand1)*cos($rand3) - sin($rand1)*cos($rand2)*sin($rand3);
	$a21 = sin($rand1)*cos($rand3) + cos($rand1)*cos($rand2)*sin($rand3);
	$a31 = sin($rand2)*sin($rand3);

	$a12 = -1*cos($rand1)*sin($rand3) - sin($rand1)*cos($rand2)*cos($rand3);
	$a22 = -1*sin($rand1)*sin($rand3) + cos($rand1)*cos($rand2)*cos($rand3);
	$a32 = sin($rand2)*cos($rand3);

	$a13 = sin($rand1)*sin($rand2);
	$a23 = -1*cos($rand1)*sin($rand2);
	$a33 = cos($rand2);

	$newfile = "1lig.dot.$j";
	open(NEW, ">$newfile") || die &error_writing($newfile);
	{
		for($i = 0; $i < @begin; $i++)
		{
#print STDERR "$score, $cmx, $cmy, $cmz, $rand2, $rand1, $rand3\n";

			$newx = $a11*$x[$i] + $a12*$y[$i] + $a13*$z[$i] + $cmx;
			$newy = $a21*$x[$i] + $a22*$y[$i] + $a23*$z[$i] + $cmy;
			$newz = $a31*$x[$i] + $a32*$y[$i] + $a33*$z[$i] + $cmz;

			printf NEW ("%30s%8.3f%8.3f%8.3f%-50s\n", $begin[$i], $newx, $newy, $newz, $end[$i]);
		}		
	}
	close NEW;

	$j++;
	if ($j > $numout)
	{
		last;
	}
}	
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
