# protein-protein docking prep. makefile
# by Christoph Champ, 12-Oct-2006
#include /home/champ/bin/makefiles/Makeconfig

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
SSM	= /sgpp/CCP4/ccp4-5.99.3/bin/superpose
NACCESS	= /home/champ/bin/naccess
NACCESSVDW	= /home/champ/lib/vdw.radii
NACCESSSTD	= /home/champ/lib/standard.data
ROSEPLOT	= /home/champ/bin/roseplot.pl
AAUSAGE	= /home/champ/bin/aausage2.pl
CH_RBN	= "W"
CH_LBN	= "X"
CH_RUN	= "Y"
CH_LUN	= "Z"

base2aa := ("AAA"=>"K","AAC" =>"N","AAG"=>"K","AAT"=>"N","ACA"=>"T","ACC"=>"T","ACG"=>"T","ACT"=>"T","AGA"=>"R","AGC"=>"S","AGG"=>"R","AGT"=>"S","ATA"=>"I","ATC"=>"I","ATG"=>"M","ATT"=>"I","CAA"=>"Q","CAC"=>"H","CAG"=>"Q","CAT"=>"H","CCA"=>"P","CCC"=>"P","CCG"=>"P","CCT"=>"P","CGA"=>"R","CGC"=>"R","CGG"=>"R","CGT"=>"R","CTA"=>"L","CTC"=>"L","CTG"=>"L","CTT"=>"L","GAA"=>"E","GAC"=>"D","GAG"=>"E","GAT"=>"D","GCA"=>"A","GCC"=>"A","GCG"=>"A","GCT"=>"A","GGA"=>"G","GGC"=>"G","GGG"=>"G","GGT"=>"G","GTA"=>"V","GTC"=>"V","GTG"=>"V","GTT"=>"V","TAA"=>"-","TAC"=>"Y","TAG"=>"-","TAT"=>"Y","TCA"=>"S","TCC"=>"S","TCG"=>"S","TCT"=>"S","TGA"=>"-","TGC"=>"C","TGG"=>"W","TGT"=>"C","TTA"=>"L","TTC"=>"F","TTG"=>"L","TTT"=>"F")
# ======================================================================
# Clear out pre-defined suffixes
.SUFFIXES:
.SUFFIXES:	.seg .pdb .dat .log .mat .png .fa .fsa

# Download TLSMD logfile for given run
%.tls.log:	%.pdb
	curl $(SKULD)/jobs/$(shell cat $$(basename $< .pdb).tlsid)/log.txt >$@

# Save the TLSMD analysis index.html (i.e. dump) to file
#lynx -dump $(SKULD)/jobs/$(shell cat $<)/ANALYSIS/XXXX_CHAIN$(shell basename $< .tlsid | sed -e "s/[R|L]-[A-Z0-9].\{3\}_\([A-Z]\)/\1/")_ANALYSIS.html >$@
%.tls.index_dump:	%.tlsid
	lynx -dump $(SKULD)/jobs/$(shell cat $<)/ANALYSIS/XXXX_CHAIN$(shell basename $<|sed -e 's/.....\.\(.un\)\.tlsid/\1/g'|gawk '{if($$0=="lun"){print "Z"}else if($$0=="run"){print "Y"}}')_ANALYSIS.html >$@

# Extract all (20) segments from TLSMD log file
#%.seg:	%.tls.log
#$(SED) -n '/^[0-9]/{n;p;}' $< | \
#$(SED) -e 's/  [0-9].*\.\(.*\)//' >$@
%.seg:	%.tls.log
	$(SED) -n '/^[0-9]/{n;p;}' $<|\
	head -$$(echo "`grep -c '^ATOM .* CA .*' $(shell basename $<|sed -e 's/\(.....\)\.\([r|l][u|b]n\).*/\1.\2.pdb/g')`/30"|bc -ql|gawk '{print int($$0+0.5)}')|tail -1|\
	sed -e 's/\(([A-Z]:[0-9-]\{1,9\})\{1,20\}\)[ ].*$$/\1/g'|\
	sed 's/(\([A-Z]\):\([0-9-]\{3,9\}\); \([0-9-]\{3,9\}\)\?)/(\1:\2)(\1:\3)/g'|\
	sed -e 's/)/\n/g'|sed -e 's/^([A-Z]://g' -e 's/-/\t/g'|\
	sort -nk1 >$@

#=== NEW .seg: cat XXXX_CHAINA_NTLS2.tlsout|sed -n '/^RANGE/p' |sed -e "s/'/_/g"|sed -e 's/RANGE[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._.*/\1\t\2/g'

#head -$(echo "`grep -c '^ATOM .* CA .*' 1AVXh.lun.pdb`/50"|bc -ql|gawk '{print int($0+0.5)}')
#***NEW*** sed -e 's/\(([A-Z]:[0-9-]\{1,9\})\{1,20\}\)[ ].*$/\1/g'|sed 's/(\([A-Z]\):\([0-9-]\{3,9\}\); \([0-9-]\{3,9\}\)\?)/(\1:\2)(\1:\3)/g'|sed -e 's/)/\n/g'|sed -e 's/^([A-Z]://g' -e 's/-/\t/g'|sort -nk1

# Extract individual segments from the TLSMD log output
SEG	= 05 10 20
#SEG	= 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 16 17 18 19 20
# OLD: $(SED) 's/[(('$(shell basename $< .seg | sed -r "s/t[A-Z0-9]{4}-[A-Za-z]{3}_//")':)| ]//g' - |
# R-5cha_A.tls.seg
#$(SED) 's/[(('$(shell basename $< .tls.seg | sed -r "s/[R|L|A]-[a-z0-9]{4}_//")':)| ]//g' - |
%.seg.s:	%.seg
	MYCH=`basename $< .tls.seg|sed -e 's/.....\.\(.un\)/\1/g'|gawk '{if($$0=="lun"){print "Z"}else if($$0=="run"){print "Y"}}'`
	for n in $(SEG);\
	do \
	head -$$(expr $$n - 1) $< | \
	tail -1 | \
	$(SED) 's/[)|;]/\n/g' - | \
	head -$$n | \
	$(SED) 's/[(('$(basename $<|sed -e 's/.....\.\(.un\).*/\1/g'|gawk '{if($$0=="lun"){print "Z"}else if($$0=="run"){print "Y"}}')':)| ]//g' - | \
	$(SED) 's/-/\t/g' - | \
	sort -nk1 - | \
	$(SED) = - | $(SED) 'N;s/\n/\t/' >$@$$n ;\
	done

# Extract libration values
%.T.R:	%.T.txt
	for i in $$(seq 1 5);do \
		cat $<|sed -e's/^ \+//' -e's/ \+/,/g' -e's/,?//g' -e 's/,/ /g'|\
		head -$$(head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|gawk '{print $$3}')|\
		tail -$$(echo "`head -$$i $$(basename $< |sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$3}'` - `head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$2}'` +1"|bc -q)|\
		gawk '{if($$2==""){print $$1" 0"}else{print $$1" "$$2}}' - >>$@;
	done

# Extract libration values
%.L.R:	%.L.txt
	for i in $$(seq 1 5);do \
		cat $<|sed -e's/^ \+//' -e's/ \+/,/g' -e's/,?//g' -e 's/,/ /g'|\
		head -$$(head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|gawk '{print $$3}')|\
		tail -$$(echo "`head -$$i $$(basename $< |sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$3}'` - `head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$2}'` +1"|bc -q)|\
		gawk '{if($$2==""){print $$1" 0"}else{print $$1" "$$2}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g' -|\
		perl -e 'use strict;use lib qw(blib/lib);use Math::Bezier;$$_=<>;s/\r?\n//;push my @p,[split(/,/,$$_)];my $$bezier=Math::Bezier->new(@p);my @curve=$$bezier->curve(20);while(@curve){my($$x,$$y)=splice(@curve,0,2);print "$$x\t$$y\n";}' - >$@a$$i;\
		cat $<|sed -e's/^ \+//' -e's/ \+/,/g' -e's/,?//g' -e's/,/ /g'|\
		head -$$(head -$$i $$(basename $< |sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|gawk '{print $$3}')|\
		tail -$$(echo "`head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$3}'` - `head -$$i $$(basename $< |sed -e 's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$2}'` +1"|bc -q)|\
		gawk '{if($$3==""){print $$1" 0"}else{print $$1" "$$3}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g' -|\
		perl -e 'use strict;use lib qw(blib/lib);use Math::Bezier;$$_=<>;s/\r?\n//;push my @p,[split(/,/,$$_)];my $$bezier=Math::Bezier->new(@p);my @curve=$$bezier->curve(20);while(@curve){my($$x,$$y)=splice(@curve,0,2);print "$$x\t$$y\n";}' - >$@b$$i;\
		cat $<|sed -e's/^ \+//' -e's/ \+/,/g' -e's/,?//g' -e's/,/ /g'|\
		head -$$(head -$$i $$(basename $< |sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|gawk '{print $$3}')|\
		tail -$$(echo "`head -$$i $$(basename $<|sed -e's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$3}'` - `head -$$i $$(basename $< |sed -e 's/\([R|L]-.*_[A-Z]\)\..*/\1/g').s05.seg|tail -1|\
		gawk '{print $$2}'` +1"|bc -q)|\
		gawk '{if($$4==""){print $$1" 0"}else{print $$1" "$$4}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g' -|\
		perl -e 'use strict;use lib qw(blib/lib);use Math::Bezier;$$_=<>;s/\r?\n//;push my @p,[split(/,/,$$_)];my $$bezier=Math::Bezier->new(@p);my @curve=$$bezier->curve(20);while(@curve){my($$x,$$y)=splice(@curve,0,2);print "$$x\t$$y\n";}' - >$@c$$i;\
	done

# Extract only given chain ATOMs (DOES NOT WORK YET!)
#	grep -E '^ATOM.* [A-Z]{3} '$(shell basename $@ .pdb | sed -r "s/^[R|L]-[A-Z0-9].{3}_//")' .*' $< >$@
#	grep -E '^ATOM.* [A-Z]{3} '$(mysql -D sandbox -B -N -e "select run_ch from chu_list where run=\"$(shell basename $< .pdb)\"")' .*' $< >$@
# $(chid=`mysql -D sandbox -B -N -e "select run_ch from chu_list where run='$(basename $< .pdb)'"`)
R-%_.pdb:	%.pdb
	grep -E '^ATOM.* [A-Z]{3} '`$(MYSQL) "select run_ch from chu_list where run='$(basename $< .pdb)'"`' .*' $< >$@

# Download PDB from rcsb (by pdb id)
PDB	= .pdbid
%.pdb:	%.pdbid
	wget $(RCSB)/$(shell basename $< $(PDB)).pdb.gz &
	wait
	gunzip $(shell basename $< $(PDB)).pdb.gz

# PDB-to-MATRIX: Translate PDB file x,y,z to contact matrix
%.pdb.mat:	%.pdb
	cat $< | perl -e '$$count=$$i=$$j=0;while($$line=<STDIN>){@line=split(/\s+/,$$line);if($$line=~ /^ATOM/){push @field,[@line];$$count++;}}print("$(shell basename $< .pdb)");for($$i=0;$$i<$$count;$$i++){printf("\tE%-2d",$$i+1);}print("\n");for($$i=0;$$i<$$count;$$i++){printf("E%-2d",$$i+1);for($$j=0;$$j<$$count;$$j++){$$r=sqrt(($$field[$$i][6]-$$field[$$j][6])**2+($$field[$$i][7]-$$field[$$j][7])**2+($$field[$$i][8]-$$field[$$j][8])**2);printf("\t%5.2f",$$r);}print("\n");}' - >$@

# MATRIX-to-PNG: Generate a PNG from a matrix file
%.pdb.mat.png:	%.pdb.mat
	`/home/champ/bin/matrix2png -data $< -size 8:8 -mincolor darkred -maxcolor white -c -r -s -con 4.0 >$@`

# Extract CAs from PDB
%.ca.pdb:	%.pdb
	grep -E '^ATOM.{8} CA .*' $< >$@

# ===============================================================================================
# Use SSM/superpose to superimpose two structures and find RMSD
# cat 1AVXh.run.ssm|sed -n '/ at RMSD =/p'|sed -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'
# cat 1AVXh.run.ssm|sed -n '/ at RMSD =/p'|sed -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'|perl -ne '($rmsd,$nalign)=split(/\t/,$_);printf("%.3f\n",$nalign**2/((1+($rmsd/3.0)**2)*`grep -c "^ATOM .* CA .*" 1AVXh.rbn.pdb`*`grep -c "^ATOM .* CA .*" 1AVXh.run.pdb`));'
%.run.pdb.ssm:	%.rbn.pdb %.run.pdb
	$(SSM) $*.run.pdb $*.rbn.pdb `basename $*.run.pdb|$(SED) 's/\([A-Z0-9]\{5,5\}\)\.run\.pdb/\1.run.fit.pdb/g'` >$@
	cat $@|$(SED) -n '/ at RMSD =/p'|$(SED) -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'|\
	perl -ne '($$rmsd,$$nalign)=split(/\t/,$$_);printf("UPDATE sandbox.pdbinfo SET ssm_rmsd=\"%.3f\",ssm_nalign=\"%d\",ssm_q=\"%.3f\" WHERE mygroup=\"%s\" AND pair=\"%s\";",$$rmsd,$$nalign,$$nalign**2/((1+($$rmsd/3.0)**2)*$(shell grep -c "^ATOM .* CA .*" $*.rbn.pdb)*$(shell grep -c "^ATOM .* CA .*" $*.run.pdb)),"$(shell basename $*.run.pdb|$(SED) "s/\([A-Z0-9]\{5,5\}\)\.run\.pdb/\1/g")","run");'|mysql
#cat $@|$(SED) -n '/ at RMSD =/p'|$(SED) -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'|\
#perl -ne '($$rmsd,$$nalign)=split(/\t/,$$_);printf("%.3f\t%d\t%.3f\n",$$rmsd,$$nalign,$$nalign**2/((1+($$rmsd/3.0)**2)*223*223));' >$@.rnq
#echo 'UPDATE sandbox.pdbinfo SET ssm_rmsd="" WHERE mygroup="'`basename $*.run.pdb|$(SED) 's/\([A-Z0-9]\{5,5\}\)\.run\.pdb/\1/g'`'" AND pair="run";'
#PARSE SSM: cat 1AVXh.run.ssm|sed -n '/^|[A-Z ][+.-]/p'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\) .* | <\([*+=:.-]\).*/\1|\2|\3|\4/g'

# cat sequences together
%.rbn-run.fas:	%.rbn.fa %.run.fa
	cat $^ >$@

%.lbn-lun.fas:	%.lbn.fa %.lun.fa
	cat $^ >$@

# fasta2aln: Align input sequences (.fa) using ClustalW
%.rbn-run.aln:	%.rbn-run.fas
	clustalw -INFILE=$< -OUTFILE=$@
#rm `basename $<|sed -e 's/\(.....\.rbn-run\)\.fas/\1.dnd/g'`

%.lbn-lun.aln:	%.lbn-lun.fas
	clustalw -INFILE=$< -OUTFILE=$@
#rm `basename $<|sed -e 's/\(.....\.lbn-lun\)\.fas/\1.dnd/g'`

# aln2fasta: Convert ClustalW (aln) output to FASTA format (60char/col)
#%.aln.fas:	%.aln
#	(((grep '^[R|L]' $<|head -1|gawk '{print ">"$$1}' 1>&1;grep '^[R|L]' $< |sed -n '1~2p'|gawk '{print $$2}')2>&2);) >$@
#	(((grep '^[R|L]' $<|head -2|tail -1 |gawk '{print ">"$$1}' 1>&1;grep '^[R|L]' $< |sed -n '2~2p'|gawk '{print $$2}')2>&2);) >>$@

%.rbn-run.aln.dif:	%.rbn-run.aln
	cat $<|perl -ne 'if($$_=~/\.rbn\./){print substr($$_,18,60);}'|\
	perl -ne 'for($$i=0;$$i<=length($$_);$$i++){if(substr($$_,$$i,1)eq "-"){printf"run %d\n",$$i+1;}}'>$@.tmp
	cat $<|perl -ne 'if($$_=~/\.run\./){print substr($$_,18,60);}'|\
	perl -ne 'for($$i=0;$$i<=length($$_);$$i++){if(substr($$_,$$i,1)eq "-"){printf"rbn %d\n",$$i+1;}}'>>$@.tmp
	cat $@.tmp|sort -nk2|uniq>$@
	rm $@.tmp

%.lbn-lun.aln.dif:	%.lbn-lun.aln
	cat $<|perl -ne 'if($$_=~/\.lbn\./){print substr($$_,18,60);}'|\
	perl -ne 'for($$i=0;$$i<=length($$_);$$i++){if(substr($$_,$$i,1)eq "-"){printf"lun %d\n",$$i+1;}}'>$@.tmp
	cat $<|perl -ne 'if($$_=~/\.lun\./){print substr($$_,18,60);}'|\
	perl -ne 'for($$i=0;$$i<=length($$_);$$i++){if(substr($$_,$$i,1)eq "-"){printf"lbn %d\n",$$i+1;}}'>>$@.tmp
	cat $@.tmp|sort |uniq>$@
	rm $@.tmp

#%.run.caf.pdb:	%.rbn-run.aln.dif
#	perl -e '$$j=0;open(PDB,"<'`basename $<|sed -e 's/\(.....\)\.rbn-run\.aln\.dif/\1.run.pdb/g'`'");open(DIF,"<$<");while($$d=<DIF>){if($$d=~/^run/){push @rm,[split(/\s+/,$$d)];}}close(DIF);while($$p=<PDB>){if($$p=~/^ATOM .* CA .*/){if(substr($$p,22,4)==$$rm[$$j][1]){$$j++;}else{print $$p;}}}close(PDB);'>$@

%.run.aaf.pdb:	%.run.pdb %.rbn-run.aln.dif
	perl -e 'open(PDB,"<$*.run.pdb");open(DIF,"<$*.rbn-run.aln.dif");while(<DIF>){@col=split(/\s+/,$$_) if(/^run/);$$diff{$$col[1]}{$$cnt}++;}close(DIF);while(<PDB>){$$resnum=substr($$_,22,4) if(/^ATOM/);$$resnum=~s/\s+//g;if(!exists($$diff{$$resnum})){print $$_;}}close(PDB);'>$@

%.lun.aaf.pdb:	%.lun.pdb %.lbn-lun.aln.dif
	perl -e 'open(PDB,"<$*.lun.pdb");open(DIF,"<$*.lbn-lun.aln.dif");while(<DIF>){@col=split(/\s+/,$$_) if(/^lun/);$$diff{$$col[1]}{$$cnt}++;}close(DIF);while(<PDB>){$$resnum=substr($$_,22,4) if(/^ATOM/);$$resnum=~s/\s+//g;if(!exists($$diff{$$resnum})){print $$_;}}close(PDB);'>$@

%.rbn.aaf.pdb:	%.rbn.pdb %.rbn-run.aln.dif
	perl -e 'open(PDB,"<$*.rbn.pdb");open(DIF,"<$*.rbn-run.aln.dif");while(<DIF>){@col=split(/\s+/,$$_) if(/^rbn/);$$diff{$$col[1]}{$$cnt}++;}close(DIF);while(<PDB>){$$resnum=substr($$_,22,4) if(/^ATOM/);$$resnum=~s/\s+//g;if(!exists($$diff{$$resnum})){print $$_;}}close(PDB);'>$@

%.lbn.aaf.pdb:	%.lbn.pdb %.lbn-lun.aln.dif
	perl -e 'open(PDB,"<$*.lbn.pdb");open(DIF,"<$*.lbn-lun.aln.dif");while(<DIF>){@col=split(/\s+/,$$_) if(/^lbn/);$$diff{$$col[1]}{$$cnt}++;}close(DIF);while(<PDB>){$$resnum=substr($$_,22,4) if(/^ATOM/);$$resnum=~s/\s+//g;if(!exists($$diff{$$resnum})){print $$_;}}close(PDB);'>$@

%.run.f.pdb:	%.rbn.aaf.pdb %.run.aaf.pdb
	cat $*.rbn.aaf.pdb|perl -e '$$prevresnum=$$i=0;$$prevrestype="";while(<>){if($$_=~/^ATOM/){if(($$prevresnum==substr($$_,22,4))&&($$prevrestype eq substr($$_,17,3))){}else{$$i++;}printf("%21s%1s%4d %-38s%12s\n",substr($$_,0,21),"W",$$i,substr($$_,27,39),substr($$_,13,1));$$prevresnum=substr($$_,22,4);$$prevrestype=substr($$_,17,3);}}'>$*.rbn.aaf.pdb.tmp
	cat $*.run.aaf.pdb|perl -e '$$prevresnum=$$i=0;$$prevrestype="";while(<>){if($$_=~/^ATOM/){if(($$prevresnum==substr($$_,22,4))&&($$prevrestype eq substr($$_,17,3))){}else{$$i++;}printf("%21s%1s%4d %-38s%12s\n",substr($$_,0,21),"Y",$$i,substr($$_,27,39),substr($$_,13,1));$$prevresnum=substr($$_,22,4);$$prevrestype=substr($$_,17,3);}}'>$*.run.aaf.pdb.tmp
	$(RMSFIT) $*.rbn.aaf.pdb.tmp $*.run.aaf.pdb.tmp $*.run.aaf.pdb.tmp $@ 2
	rm $*.rbn.aaf.pdb.tmp $*.run.aaf.pdb.tmp

%.lun.f.pdb:	%.lbn.aaf.pdb %.lun.aaf.pdb
	cat $*.lbn.aaf.pdb|perl -e '$$prevresnum=$$i=0;$$prevrestype="";while(<>){if($$_=~/^ATOM/){if(($$prevresnum==substr($$_,22,4))&&($$prevrestype eq substr($$_,17,3))){}else{$$i++;}printf("%21s%1s%4d %-38s%12s\n",substr($$_,0,21),"X",$$i,substr($$_,27,39),substr($$_,13,1));$$prevresnum=substr($$_,22,4);$$prevrestype=substr($$_,17,3);}}'>$*.lbn.aaf.pdb.tmp
	cat $*.lun.aaf.pdb|perl -e '$$prevresnum=$$i=0;$$prevrestype="";while(<>){if($$_=~/^ATOM/){if(($$prevresnum==substr($$_,22,4))&&($$prevrestype eq substr($$_,17,3))){}else{$$i++;}printf("%21s%1s%4d %-38s%12s\n",substr($$_,0,21),"Z",$$i,substr($$_,27,39),substr($$_,13,1));$$prevresnum=substr($$_,22,4);$$prevrestype=substr($$_,17,3);}}'>$*.lun.aaf.pdb.tmp
	$(RMSFIT) $*.lbn.aaf.pdb.tmp $*.lun.aaf.pdb.tmp $*.lun.aaf.pdb.tmp $@ 2
	rm $*.lbn.aaf.pdb.tmp $*.lun.aaf.pdb.tmp

# ===============================================================================================
# Create complex
%.rbn-lbn.pdb:	%.rbn.pdb %.lbn.pdb
	cat $^ >$@

# Calculate ASA (using "naccess")
%.rsa:	%.pdb
	$(NACCESS) $< -p 1.40 -v $(NACCESSVDW) -s $(NACCESSSTD) 1> /dev/null
	mv `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.rsa/g'` `basename $<|sed -e 's/\(.....\.[r|l]bn\(-lbn\)\?\)\.pdb/\1.rsa/g'`
	rm `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.log/g'` `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.asa/g'`

# Return interface residues (rbn)
%.rbn.int:	%.rbn-lbn.rsa %.rbn.rsa
	sed '1,4d' $*.rbn-lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D'|gawk '{if(substr($$0,9,1)=="W"){print $$0}}' >$*.rbn-lbn.rsa.tmp
	sed '1,4d' $*.rbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D' >$*.rbn.rsa.tmp
	gawk '{print $$2,$$3,$$4,$$5}' $*.rbn.rsa.tmp|paste - $*.rbn-lbn.rsa.tmp|gawk '{print $$1,$$2,$$3,$$4,$$9}'|\
	gawk '{if($$4==$$5){print $$0,"0.00"}else{print $$0,1-($$5/$$4)}}'|gawk '{if($$6>=0.3){print $$0}}' >$@
	rm $*.rbn-lbn.rsa.tmp $*.rbn.rsa.tmp

# int->fsa (interface residues -> FASTA(aa))
%.rbn.pdb.int.fsa:	%.rbn.int
	cat $<|perl -e 'while(<STDIN>){push(@aas,substr($$_,0,3));}%symbols=("ALA","A","CYS","C","ASP","D","GLU","E","PHE","F","GLY","G","HIS","H","ILE","I","LYS","K","LEU","L","MET","M","MSE","M","ASN","N","PRO","P","GLN","Q","ARG","R","SER","S","THR","T","VAL","V","TRP","W","TYR","Y","GLX","Z");for($$i=0;$$i<@aas;$$i++){print $$symbols{$$aas[$$i]};}' >$@

#ROSEPLOT
#cat 1A2Kh.rbn.int|perl -e 'while(<STDIN>){push(@aas,substr($_,0,3));}%symbols=("ALA","A","CYS","C","ASP","D","GLU","E","PHE","F","GLY","G","HIS","H","ILE","I","LYS","K","LEU","L","MET","M","MSE","M","ASN","N","PRO","P","GLN","Q","ARG","R","SER","S","THR","T","VAL","V","TRP","W","TYR","Y","GLX","Z");for($i=0;$i<@aas;$i++){print $symbols{$aas[$i]};}print"\n";'|perl aausage2.pl |perl roseplot.pl -labelFontSize 15  -axistitle "Frequency" -fcolor blue -Xcol 2 -Ycol 4 -steps 1 -T "Amino Acid Usage" -ST "Test" -output foo.ps

# Return interface residues (lbn)
%.lbn.int:	%.rbn-lbn.rsa %.lbn.rsa
	sed '1,4d' $*.rbn-lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D'|gawk '{if(substr($$0,9,1)=="X"){print $$0}}' >$*.rbn-lbn.rsa.tmp
	sed '1,4d' $*.lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D' >$*.lbn.rsa.tmp
	gawk '{print $$2,$$3,$$4,$$5}' $*.lbn.rsa.tmp|paste - $*.rbn-lbn.rsa.tmp|gawk '{print $$1,$$2,$$3,$$4,$$9}'|\
	gawk '{if($$4==$$5){print $$0,"0.00"}else{print $$0,1-($$5/$$4)}}'|gawk '{if($$6>=0.3){print $$0}}' >$@
	rm $*.rbn-lbn.rsa.tmp $*.lbn.rsa.tmp

# PDB(CAa)-to-MATRIX: Translate PDB file x,y,z to contact matrix (CAs only)
%.ca.pdb.mat:	%.ca.pdb
	cat $< | perl -e 'sub trim($$);$$count=$$i=$$j=$$k=0;while($$line=<STDIN>){@line=split(/\s+/,$$line);if($$line=~/^ATOM/){push @x,[trim(substr($$line,30,8))];push @y,[trim(substr($$line,38,8))];push @z,[trim(substr($$line,46,8))];push @c,[substr($$line,71,1)];$$count++;}}print("$(shell basename $< .pdb.ca)");for($$i=0;$$i<$$count;$$i++){printf("\tE%-2d",$$i+1);}print("\n");for($$i=0;$$i<$$count;$$i++){printf("E%-2d",$$i+1);if($$c[$$i][0] eq "*"){for($$k=0;$$k<$$count;$$k++){printf("\t%5.2f","0.00");}}else{for($$j=0;$$j<$$count;$$j++){if($$c[$$j][0] eq "*"){printf("\t%5.2f","0.00");}else{$$r=sqrt(($$x[$$i][0]-$$x[$$j][0])**2+($$y[$$i][0]-$$y[$$j][0])**2+($$z[$$i][0]-$$z[$$j][0])**2);printf("\t%5.2f",$$r);}}}print("\n");}sub trim($$){my $$string=shift;$$string=~s/^\s+//;$$string=~s/\s+$$//;return $$string;}' - >$@

# MATRIX-to-PNG: Generate a PNG from a matrix file (for CAs)
%.ca.pdb.mat.png:	%.ca.pdb.mat
	`/home/champ/bin/matrix2png -data $< -size 2:2 -mincolor darkred -maxcolor white -s -con 4.0 >$@`

# TLSMD_segments-to-R: Extract start sites for TLSMD predicted segments; formatted for R input
%.seg.R:	%.seg
	for i in $$(gawk '{print $$2}' $<); do (cat $(TDIR)/template_segments.R | sed "s/XXX/$$i/g") >>$@; done

# PDB-to-FASTA(aa): Translate PDB file to FASTA(amino acid) sequence file
#%.pdb.fa:	%.pdb
%.fa:	%.pdb
	@echo ">$@" >$@ | \
	cat $< | perl -e '$$prevres="";$$prevresnum=0;while(<STDIN>){if(/^ATOM/){if(substr($$_,13,2) eq "CA"){$$currentres=substr($$_,17,3);$$currentresnum=substr($$_,22,5);$$currentresnum=~ s/\s//g;if($$prevres ne $$currentres || $$prevresnum ne $$currentresnum){push(@aas,substr($$_,17,3));}$$prevres=$$currentres;$$prevresnum=$$currentresnum;}}elsif(/^END/){}}%symbols=("ALA","A","CYS","C","ASP","D","GLU","E","PHE","F","GLY","G","HIS","H","ILE","I","LYS","K","LEU","L","MET","M","MSE","M","ASN","N","PRO","P","GLN","Q","ARG","R","SER","S","THR","T","VAL","V","TRP","W","TYR","Y","GLX","Z");for($$i=0;$$i<@aas;$$i++){print $$symbols{$$aas[$$i]};}print"\n";' - |\
	sed -e 's/\(.\{60\}\)/\1\n/g' - >>$@

# FASTA(DNA)-to-FASTA(aa): Translate DNA sequences into amino acid sequences
%.fsa:	%.fa
	cat $<|$(PERL) -ne 'chomp;push @seq,$$1 if /^(>.*)/;$$dna{$$seq[$$#seq]} .=$$1 if(/^([A-Z]+)$$/);%base2aa=$(base2aa);foreach $$s (@seq){print "$$s\n";for($$x=0;$$x<length($$dna{$$s});$$x=$$x+180){$$line=substr($$dna{$$s},$$x,180);for($$y=0;$$y<length($$line);$$y=$$y+3){$$triplet=substr($$line,$$y,3);$$base2aa{$$triplet}="X" unless defined $$base2aa{$$triplet};print $$base2aa{$$triplet} unless $$base2aa{$$triplet} eq "-";}print "\n"}}' >$@

#cat $< | perl -ne 'chomp;push @seq,$$1 if /^(>.*)/;$$dna{$$seq[$$#seq]} .= $$1 if(/^([A-Z]+)$$/);%base2aa=("AAA"=>"K","AAC" =>"N","AAG"=>"K","AAT"=>"N","ACA"=>"T","ACC"=>"T","ACG"=>"T","ACT"=>"T","AGA"=>"R","AGC"=>"S","AGG"=>"R","AGT"=>"S","ATA"=>"I","ATC"=>"I","ATG"=>"M","ATT"=>"I","CAA"=>"Q","CAC"=>"H","CAG"=>"Q","CAT"=>"H","CCA"=>"P","CCC"=>"P","CCG"=>"P","CCT"=>"P","CGA"=>"R","CGC"=>"R","CGG"=>"R","CGT"=>"R","CTA"=>"L","CTC"=>"L","CTG"=>"L","CTT"=>"L","GAA"=>"E","GAC"=>"D","GAG"=>"E","GAT"=>"D","GCA"=>"A","GCC"=>"A","GCG"=>"A","GCT"=>"A","GGA"=>"G","GGC"=>"G","GGG"=>"G","GGT"=>"G","GTA"=>"V","GTC"=>"V","GTG"=>"V","GTT"=>"V","TAA"=>"-","TAC"=>"Y","TAG"=>"-","TAT"=>"Y","TCA"=>"S","TCC"=>"S","TCG"=>"S","TCT"=>"S","TGA"=>"-","TGC"=>"C","TGG"=>"W","TGT"=>"C","TTA"=>"L","TTC"=>"F","TTG"=>"L","TTT"=>"F");foreach $$s (@seq){print "$$s\n";for($$x=0;$$x<length($$dna{$$s});$$x=$$x+180){$$line=substr($$dna{$$s},$$x,180);for($$y=0;$$y<length($$line);$$y=$$y+3){$$triplet=substr($$line,$$y,3);$$base2aa{$$triplet}="X" unless defined $$base2aa{$$triplet};print $$base2aa{$$triplet} unless $$base2aa{$$triplet} eq "-";}print "\n"}}' >$@

# Download and format (*.fa) all given CPHmodels
%.cph.fa:	%.cph
	for pdbid in $$(cat $< | gawk '{print $$2}' | sort | uniq); do wget $(RCSB)/$$pdbid.pdb.gz; done
	wait
	for pdbid in $$(cat $< | gawk '{print $$2}' | sort | uniq | gawk '{print $$1".pdb.gz"}'); do gunzip $$pdbid; done
	for pdbid in $$(cat $< | gawk '{print $$2}' | sort | uniq); do gmake $$pdbid.fa; done
	for pdbid in $$(cat $< | gawk '{print $$2}' | sort | uniq | gawk '{print $$1".fa"}'); do cat $$pdbid >>$@; done

# Extract from PDB only ATOMs for a given chain
%.cph.ch.all.fa:	%.cph
	for pdbid in $$(cat $< | gawk '{print $$2"."$$4}');\
		do grep -E '^ATOM.* [A-Z]{3} '$$(echo $$pdbid |$(SED) -r "s/.{4}\.//")' .*'\
		$$(echo $$pdbid |$(SED) -r "s/\.[A-Z]//").pdb\
		>$$(echo $$pdbid |sed -r "s/\.[A-Z]//")_$$(echo $$pdbid |$(SED) -r "s/.{4}\.//").pdb;done
	for pdbid in $$(cat $< | gawk '{print $$2"."$$4}');\
		do gmake $$(echo $$pdbid |sed -r "s/\.[A-Z]//")_$$(echo $$pdbid |$(SED) -r "s/.{4}\.//").fa;done
	for pdbid in $$(cat $< | gawk '{print $$2"."$$4}');\
		do cat $$(echo $$pdbid |sed -r "s/\.[A-Z]//")_$$(echo $$pdbid |$(SED) -r "s/.{4}\.//").fa >>$@;done
#==============

# Doesn't do much for now
#clean:
#	$(RM) *.fsa

#s/\(HEADER\)\(.*\)\([0-9][0-9].[A-Z].\{2\}.[0-9][0-9]\)[ ]*\([0-9A-Z].\{3\}\).*\(TITLE\)[ ]*\([0-9A-Za-z(].*\)[ ]*/\2,\6,\3,\4/
%.sql:       %.pdblist
	#for pdbid in $$(cat $< | gawk '{print $$1}' | sort | uniq); do wget $(RCSB)/$$pdbid.pdb.gz; done
	#wait
	#for pdbid in $$(cat $< | gawk '{print $$1}' | sort | uniq | gawk '{print $$1".pdb.gz"}'); do gunzip $$pdbid; done
	for pdbid in $$(cat $< | gawk '{print $$1}' | sort | uniq);\
	do\
	head -1 $$pdbid.pdb | sed -e ':a;$$!N;s/\n//;ta\
	s/\(HEADER\)[ ]*\(.*\)\([0-9][0-9].[A-Z].\{2\}.[0-9][0-9]\)[ ]*\([0-9A-Z].\{3\}\).*/\4,\2,\3/;\
	s/^[ \t]*//;s/[ \t]*$$//;s/^/"/;s/$$/"/;s/[ ]*,/","/g;' >>$@;\
	done

# Extract TLS analysis table for given segment (from TLSMD Server)
%.tex:	%.tlsid
	lynx -dump $(SKULD)/jobs/$(shell cat $<)/ANALYSIS/XXXX_CHAIN$(shell basename $< .tlsid|sed -e "s/[R|L]-[A-Z0-9].\{3\}_\([A-Z]\)/\1/")_ANALYSIS.html |\
	sed -f $(TDIR)/tls_colours.sed |\
	head -20 |tail -5 |\
	sed -f $(TDIR)/tls_rm_colours.sed |sed -e 's/, /,/g' |sed -e 's/ / \& /g' -e 's/$$/\\\\/' >$@

%.report.tex:	%.tex
	cat $(TDIR)/report_head_template.tex | sed 's/REPORTTITLE/'$(shell basename $< .tex)'/g' |\
	cat - \
		$(TDIR)/report_table_head_template.tex \
		$<\
		$(TDIR)/report_table_tail_template.tex \
		$(TDIR)/report_figure_template.tex \
		$(TDIR)/report_tail_template.tex \
	>$@

# Generate  PostScript (.ps) file from LaTeX (.tex) file
%.tex.ps:	%.tex
	latex $< && dvips -f $(shell basename $< .tex).dvi >$@

%.bf.R.epsi:	%.pdb
	cat $(TDIR)/bf_template.R |sed 's/FILENAME/$</g' |R --vanilla
	ps2epsi $<.ps $@
