# Paths used in Master Makefile
# ======================================================================
AWK	= awk
SED	= sed
RM	= rm
TDIR	= /home/champ/lib/templates
MYSQL	= mysql -D sandbox -B -N -e
RCSB	= http://www.rcsb.org/pdb/files
SKULD	= http://skuld.bmsc.washington.edu/~tlsmd
PERL 	= /usr/bin/perl
RMSFIT	= /home/champ/bin/rmsfit
#SSM	= /sgpp/CCP4/ccp4-5.99.3/bin/superpose
NACCESS	= /home/champ/bin/naccess
NACCESSVDW	= /home/champ/lib/vdw.radii
NACCESSSTD	= /home/champ/lib/standard.data
ROSEPLOT	= /home/champ/bin/roseplot.pl
AAUSAGE	= /home/champ/bin/aausage2.pl

base2aa := ("AAA"=>"K","AAC" =>"N","AAG"=>"K","AAT"=>"N","ACA"=>"T","ACC"=>"T","ACG"=>"T","ACT"=>"T","AGA"=>"R","AGC"=>"S","AGG"=>"R","AGT"=>"S","ATA"=>"I","ATC"=>"I","ATG"=>"M","ATT"=>"I","CAA"=>"Q","CAC"=>"H","CAG"=>"Q","CAT"=>"H","CCA"=>"P","CCC"=>"P","CCG"=>"P","CCT"=>"P","CGA"=>"R","CGC"=>"R","CGG"=>"R","CGT"=>"R","CTA"=>"L","CTC"=>"L","CTG"=>"L","CTT"=>"L","GAA"=>"E","GAC"=>"D","GAG"=>"E","GAT"=>"D","GCA"=>"A","GCC"=>"A","GCG"=>"A","GCT"=>"A","GGA"=>"G","GGC"=>"G","GGG"=>"G","GGT"=>"G","GTA"=>"V","GTC"=>"V","GTG"=>"V","GTT"=>"V","TAA"=>"-","TAC"=>"Y","TAG"=>"-","TAT"=>"Y","TCA"=>"S","TCC"=>"S","TCG"=>"S","TCT"=>"S","TGA"=>"-","TGC"=>"C","TGG"=>"W","TGT"=>"C","TTA"=>"L","TTC"=>"F","TTG"=>"L","TTT"=>"F")
# ======================================================================
