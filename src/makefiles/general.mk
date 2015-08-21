# protein-protein docking prep. makefile
# by Christoph Champ, 12-Oct-2006
#include /home/champ/bin/makefiles/Makeconfig
include /home/champ/bin/makefiles/zdb.mk
include /home/champ/bin/makefiles/baker.mk

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
# Clear out pre-defined suffixes
#.SUFFIXES:
#.SUFFIXES:	.seg .pdb .dat .log .mat .png .fa .fsa

# Download PDB from rcsb (by pdb id)
PDB	= .pdbid
%.pdb:	%.pdbid
	wget $(RCSB)/$(shell basename $< $(PDB)).pdb.gz &
	wait
	gunzip $(shell basename $< $(PDB)).pdb.gz

# FASTA(DNA)-to-FASTA(aa): Translate DNA sequences into amino acid sequences
%.fsa:	%.fa
	cat $<|$(PERL) -ne 'chomp;push @seq,$$1 if /^(>.*)/;$$dna{$$seq[$$#seq]} .=$$1 if(/^([A-Z]+)$$/);%base2aa=$(base2aa);foreach $$s (@seq){print "$$s\n";for($$x=0;$$x<length($$dna{$$s});$$x=$$x+180){$$line=substr($$dna{$$s},$$x,180);for($$y=0;$$y<length($$line);$$y=$$y+3){$$triplet=substr($$line,$$y,3);$$base2aa{$$triplet}="X" unless defined $$base2aa{$$triplet};print $$base2aa{$$triplet} unless $$base2aa{$$triplet} eq "-";}print "\n"}}' >$@

# Generate  PostScript (.ps) file from LaTeX (.tex) file
%.tex.ps:	%.tex
	latex $< && dvips -f $(shell basename $< .tex).dvi >$@

%.bf.R.epsi:	%.pdb
	cat $(TDIR)/bf_template.R |sed 's/FILENAME/$</g' |R --vanilla
	ps2epsi $<.ps $@

