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
MATRIX2PNG	= /home/champ/bin/matrix2png
RMSFIT	= /home/champ/bin/rmsfit
SSM	= /sgpp/CCP4/ccp4-5.99.3/bin/superpose
NACCESS	= /home/champ/bin/naccess
NACCESSVDW	= /home/champ/lib/vdw.radii
NACCESSSTD	= /home/champ/lib/standard.data
TEXSHADE	= /usr/share/texmf/tex/latex/texshade/texshade.sty
#\usepackage{/usr/share/texmf/tex/latex/texshade/texshade}
ROSEPLOT	= /home/champ/bin/roseplot.pl
AAUSAGE	= /home/champ/bin/aausage2.pl
CH_RBN	= "W"
CH_LBN	= "X"
CH_RUN	= "Y"
CH_LUN	= "Z"
SSM_STRAND	= CAFFD8
SSM_HELIX	= FFCEFF
SSM_HYDROPHOBIC	= FFA8A8
SSM_HYDROPHYLIC	= 8CD1E6

report_colours = cc<-c("#0000ff","#00ff00","#ff00ff","#ff0000","#00ffff","#000000")
report_grid_colours := par(col="grey50",fg="grey50",col.axis="grey50")
bezier	:= use strict;use lib qw(blib/lib);use Math::Bezier;$$_=<>;s/\r?\n//;push my @p,[split(/,/,$$_)];my $$bezier=Math::Bezier->new(@p);my @curve=$$bezier->curve(20);while(@curve){my($$x,$$y)=splice(@curve,0,2);print "$$x\t$$y\n";}

base2aa := ("AAA"=>"K","AAC" =>"N","AAG"=>"K","AAT"=>"N","ACA"=>"T","ACC"=>"T","ACG"=>"T","ACT"=>"T","AGA"=>"R","AGC"=>"S","AGG"=>"R","AGT"=>"S","ATA"=>"I","ATC"=>"I","ATG"=>"M","ATT"=>"I","CAA"=>"Q","CAC"=>"H","CAG"=>"Q","CAT"=>"H","CCA"=>"P","CCC"=>"P","CCG"=>"P","CCT"=>"P","CGA"=>"R","CGC"=>"R","CGG"=>"R","CGT"=>"R","CTA"=>"L","CTC"=>"L","CTG"=>"L","CTT"=>"L","GAA"=>"E","GAC"=>"D","GAG"=>"E","GAT"=>"D","GCA"=>"A","GCC"=>"A","GCG"=>"A","GCT"=>"A","GGA"=>"G","GGC"=>"G","GGG"=>"G","GGT"=>"G","GTA"=>"V","GTC"=>"V","GTG"=>"V","GTT"=>"V","TAA"=>"-","TAC"=>"Y","TAG"=>"-","TAT"=>"Y","TCA"=>"S","TCC"=>"S","TCG"=>"S","TCT"=>"S","TGA"=>"-","TGC"=>"C","TGG"=>"W","TGT"=>"C","TTA"=>"L","TTC"=>"F","TTG"=>"L","TTT"=>"F")
aaa2aa := ("ALA","A","CYS","C","ASP","D","GLU","E","PHE","F","GLY","G","HIS","H","ILE","I","LYS","K","LEU","L","MET","M","MSE","M","ASN","N","PRO","P","GLN","Q","ARG","R","SER","S","THR","T","VAL","V","TRP","W","TYR","Y","GLX","Z")
# ======================================================================
# Clear out pre-defined suffixes
.SUFFIXES:
.SUFFIXES:	.seg .pdb .dat .log .mat .png .fa .fsa

# Extract CAs from PDB
%.ca.pdb:	%.pdb
	grep -E '^ATOM.{8} CA .*' $< >$@

# Download TLSMD logfile for given run
%.tls.log:	%.pdb
	curl $(SKULD)/jobs/$(shell cat $$(basename $< .pdb).tlsid)/log.txt >$@

# Save the TLSMD analysis index.html (i.e. dump) to file
%.tls.index_dump:	%.tlsid
	lynx -dump $(SKULD)/jobs/$(shell cat $<)/ANALYSIS/XXXX_CHAIN$(shell basename $<|sed -e 's/.....\.\(.un\)\.tlsid/\1/g'|gawk '{if($$0=="lun"){print "Z"}else if($$0=="run"){print "Y"}}')_ANALYSIS.html >$@

%.pdb.chain:	%.pdb
	head -1 $<|gawk '{print substr($$0,22,1)}' >$@

# Extract segments from TLSMD run (number of segments determined by number of residues: numres/30 > 5 ? n:5)
#%.seg:	%.pdb
#cat $(shell cat `basename $<|sed -e 's/\(.....\)\.\([r|l][u|b]n\).*/\1.\2.tlsid/g'`)/ANALYSIS/XXXX_CHAIN$(shell head -1 $<|gawk '{print substr($$0,22,1)}')_NTLS$(shell echo "`grep -c '^ATOM .* CA .*' $<|sed -e 's/\(.....\)\.\([r|l][u|b]n\).*/\1.\2.pdb/g'`/30"|bc -ql|gawk '{print int($$0+0.5)}').tlsout|sed -n '/^RANGE/p' |sed -e "s/'/_/g"|sed -e 's/RANGE[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._.*/\1\t\2/g' >$@
%.pdb.seg:	%.tlsid %.pdb.chain %.pdb.ssm.maxrmsd
	cat $(shell cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$(shell cat $*.pdb.chain)_NTLS$(shell echo "`grep -c '^ATOM .* CA .*' $*.pdb`/30"|bc -ql|gawk '{print int($$0+0.5)}').tlsout|sed -n '/^RANGE/p' |sed -e "s/'/_/g"|sed -e 's/RANGE[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._.*/\1\t\2/g' >$@
#CHANGE TO '*.chain': cat $(shell cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$(shell head -1 $*.pdb|gawk '{print substr($$0,22,1)}')_NTLS$(shell echo "`grep -c '^ATOM .* CA .*' $*.pdb`/30"|bc -ql|gawk '{print int($$0+0.5)}').tlsout|sed -n '/^RANGE/p' |sed -e "s/'/_/g"|sed -e 's/RANGE[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._.*/\1\t\2/g' >$@

%.pdb.seg.R:	%.tlsid %.pdb.chain %.pdb.ssm.maxrmsd
	echo "# TLSMD Segments Section (*.seg.R):" >$@
	cat $(shell cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$(shell cat $*.pdb.chain)_NTLS$(shell echo "`grep -c '^ATOM .* CA .*' $*.pdb`/30"|bc -ql|gawk '{print int($$0+0.5)}').tlsout|sed -n '/^RANGE/p' |sed -e "s/'/_/g"|sed -e 's/RANGE[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._[ \t]*_[A-Z][ \t]*\([0-9]\{1,\}\)\._.*/segments(\2,0,\2,y1max,col=\"red\",lty=\"dashed\");/g' >>$@

# Find the max RMSD from bound/unbound structures:
%.ssm.maxrmsd:	%.ssm
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\3|\4|\1|\2|\5|\6/g'|gawk -F\| '{print $$6}'|sort -n|tail -1 >$@

#head -$(echo "`grep -c '^ATOM .* CA .*' 1AVXh.lun.pdb`/50"|bc -ql|gawk '{print int($0+0.5)}')
#***NEW*** sed -e 's/\(([A-Z]:[0-9-]\{1,9\})\{1,20\}\)[ ].*$/\1/g'|sed 's/(\([A-Z]\):\([0-9-]\{3,9\}\); \([0-9-]\{3,9\}\)\?)/(\1:\2)(\1:\3)/g'|sed -e 's/)/\n/g'|sed -e 's/^([A-Z]://g' -e 's/-/\t/g'|sort -nk1

# Extract secondary structure from SSM for TeXshade
%.pdb.ssm.strand.R:	%.pdb.ssm
	echo "# SSM Strand Section:" >$@
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\3|\4|\1|\2|\5|\6/g'|gawk -F\| '{if($$3=="S"){print "segments("$$2",y2max-(y2max*0.15),"$$2",y2max-(y2max*0.10),col=\"#$(SSM_STRAND)\",lty=\"solid\")"}}' >>$@
%.pdb.ssm.helix.R:	%.pdb.ssm
	echo "# SSM Helix Section:" >$@
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\3|\4|\1|\2|\5|\6/g'|gawk -F\| '{if($$3=="H"){print "segments("$$2",y2max-(y2max*0.15),"$$2",y2max-(y2max*0.10),col=\"#$(SSM_HELIX)\",lty=\"solid\")"}}' >>$@
%.pdb.ssm.hydrophobic.R:	%.pdb.ssm
	echo "# SSM Hydrophobic Section:" >$@
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\4\t\2/g'|gawk '{if($$2=="-"){print "segments("$$1",y2max-(y2max*0.05),"$$1",y2max,col=\"#$(SSM_HYDROPHOBIC)\",lty=\"solid\")"}}' >>$@
%.pdb.ssm.hydrophylic.R:	%.pdb.ssm
	echo "# SSM Hydrophilic Section:" >$@
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\4\t\2/g'|gawk '{if($$2=="+"){print "segments("$$1",y2max-(y2max*0.05),"$$1",y2max,col=\"#$(SSM_HYDROPHYLIC)\",lty=\"solid\")"}}' >>$@
%.pdb.ssm.rmsd.dat:	%.pdb.ssm
	cat $<|sed -n '/^|[A-Z ][+.-] [A-Z]/p'|perl -ne 'print $$_ unless substr($$_,16,10)eq" "x10;'|sed -e 's/^|\([A-Z ]\)\([+.-]\) [A-Z]:\([A-Z]\{3,3\}\)[ \t]*\([0-9]\{1,\}\)[ \t]*| <\([*+=:.-]\)[*+=:.-]\(.*\)[*+=:.-]\{2\}>.*/\4\t\6/g' >$@

# TERNARY: echo "|E..|"|perl -ne 'printf("%s\n",substr($_,1,1)=~/[A-Z]/?substr($_,1,1):"_");'

# Extract Translation (T) values:
%.pdb.translation:	%.pdb.seg %.pdb.chain %.tlsid
	for i in $$(seq 1 `wc -l $*.pdb.seg|gawk '{print $$1}'`);do \
		cat $$(cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')_TRANSLATION.txt|\
		sed -e's/^ \+//' -e's/ \+/,/g' -e's/,?//g' -e's/,/ /g'|grep -E '^[0-9]'|\
		head -$$(head -$$i $*.pdb.seg|tail -1|gawk '{print $$2}')|\
		tail -$$(echo "`head -$$i $*.pdb.seg|tail -1|gawk '{print $$2-$$1+1}'`"|bc -l)|\
		gawk '{if($$2==""){print $$1" 0"}else{print $$1" "$$2}}' - >$@.a$$i.dat;\
		head -1 $@.a$$i.dat >$@.b$$i.dat && tail -1 $@.a$$i.dat >>$@.b$$i.dat;\
	done

%.pdb.translation.R:	%.pdb.translation
	for i in $*.pdb.translation.b*.dat;do \
		cat $$i|sed -e :a -e '$$!N;s/\n/,/;ta'|sed -e's/ /,/g'|gawk '{print "segments("$$0",col=cc[n],lty=\"solid\");n=n+1;"}' >>$@.tmp;\
	done
	sort -nk3 -t, $@.tmp >$@
	rm -f $*.pdb.translation.b*.dat $*.pdb.translation.a*.dat $@.tmp

# Separate (T): cat 1AVXh.lun.translation|perl -e '$last="";while(<>){@f=split(/\s+/,$_);$first=$f[0];if($f[1]==$last){print "$f[0]\t$f[1]\n";}else{print "######\n$f[0]\t$f[1]\t$first\n";$last=$f[1]}}'

# Extract Libration (L) values:
%.pdb.libration:	%.pdb.seg %.pdb.chain %.tlsid
	for i in $$(seq 1 `wc -l $*.pdb.seg|gawk '{print $$1}'`);do \
		cat $$(cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')_LIBRATION.txt|\
		sed -e 's/^ \+//' -e 's/ \+/,/g' -e 's/,?//g' -e 's/,/ /g'|\
		head -$$(head -$$i $*.pdb.seg|tail -1|gawk '{print $$2}')|\
		tail -$$(echo "`head -$$i $*.pdb.seg|tail -1|gawk '{print $$2-$$1+1}'`"|bc -l)|\
		gawk '{if($$2==""){print $$1" 0"}else{print $$1" "$$2}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g'|\
		perl -e '$(bezier)' - >$@.a$$i.dat;\
		cat $$(cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')_LIBRATION.txt|\
		sed -e 's/^ \+//' -e 's/ \+/,/g' -e 's/,?//g' -e 's/,/ /g'|\
		head -$$(head -$$i $*.pdb.seg|tail -1|gawk '{print $$2}')|\
		tail -$$(echo "`head -$$i $*.pdb.seg|tail -1|gawk '{print $$2-$$1+1}'`"|bc -l)|\
		gawk '{if($$3==""){print $$1" 0"}else{print $$1" "$$3}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g'|\
		perl -e '$(bezier)' - >$@.b$$i.dat;\
		cat $$(cat $*.tlsid)/ANALYSIS/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')/XXXX_CHAIN$$(cat $*.pdb.chain)_NTLS$$(wc -l $*.pdb.seg|gawk '{print $$1}')_LIBRATION.txt|\
		sed -e 's/^ \+//' -e 's/ \+/,/g' -e 's/,?//g' -e 's/,/ /g'|\
		head -$$(head -$$i $*.pdb.seg|tail -1|gawk '{print $$2}')|\
		tail -$$(echo "`head -$$i $*.pdb.seg|tail -1|gawk '{print $$2-$$1+1}'`"|bc -l)|\
		gawk '{if($$4==""){print $$1" 0"}else{print $$1" "$$4}}'|sed -e :a -e'$$!N;s/\n/,/;ta' -e's/ \+/,/g'|\
		perl -e '$(bezier)' - >$@.c$$i.dat;\
	done

%.pdb.libration.R:	%.pdb.libration %.pdb.seg
	#echo "n=1;" >$@; for i in $*.pdb.libration.a*.dat;do echo -e "lines(read.table(\"$$i\"),col=cc[n],type=\"l\");n=n+1;" >>$@; done
	#echo "n=1;" >>$@; for i in $*.pdb.libration.b*.dat;do echo -e "lines(read.table(\"$$i\"),col=cc[n],type=\"l\");n=n+1;" >>$@; done
	#echo "n=1;" >>$@; for i in $*.pdb.libration.c*.dat;do echo -e "lines(read.table(\"$$i\"),col=cc[n],type=\"l\");n=n+1;" >>$@; done
	echo "n=1;" >$@; for i in `seq 1 $$(wc -l $*.pdb.seg|gawk '{print $$1}')`;do echo -e "lines(read.table(\"$(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb.libration/g').a$$i.dat\"),col=cc[n],type=\"l\");n=n+1;" >>$@;done
	echo "n=1;" >>$@; for i in `seq 1 $$(wc -l $*.pdb.seg|gawk '{print $$1}')`;do echo -e "lines(read.table(\"$(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb.libration/g').b$$i.dat\"),col=cc[n],type=\"l\");n=n+1;" >>$@;done
	echo "n=1;" >>$@; for i in `seq 1 $$(wc -l $*.pdb.seg|gawk '{print $$1}')`;do echo -e "lines(read.table(\"$(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb.libration/g').c$$i.dat\"),col=cc[n],type=\"l\");n=n+1;" >>$@;done

#===============================================================================================
# Use SSM/superpose to superimpose two structures and find RMSD
%.run.pdb.ssm:	%.rbn.pdb %.run.pdb
	$(SSM) $*.run.pdb $*.rbn.pdb `basename $*.run.pdb|$(SED) 's/\([A-Z0-9]\{5,5\}\)\.run\.pdb/\1.run.fit.pdb/g'`|tee $@|\
	$(SED) -n '/ at RMSD =/p'|$(SED) -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'|\
	perl -ne '($$rmsd,$$nalign)=split(/\t/,$$_);printf("UPDATE sandbox.pdbinfo SET ssm_rmsd=\"%.3f\",ssm_nalign=\"%d\",ssm_q=\"%.3f\" WHERE mygroup=\"%s\" AND pair=\"%s\";",$$rmsd,$$nalign,$$nalign**2/((1+($$rmsd/3.0)**2)*$(shell grep -c "^ATOM .* CA .*" $*.rbn.pdb)*$(shell grep -c "^ATOM .* CA .*" $*.run.pdb)),"$(shell basename $*.run.pdb|$(SED) "s/\([A-Z0-9]\{5,5\}\)\.run\.pdb/\1/g")","run");'|mysql

%.lun.pdb.ssm:	%.lbn.pdb %.lun.pdb
	$(SSM) $*.lun.pdb $*.lbn.pdb `basename $*.lun.pdb|$(SED) 's/\([A-Z0-9]\{5,5\}\)\.lun\.pdb/\1.lun.fit.pdb/g'`|tee $@|\
	$(SED) -n '/ at RMSD =/p'|$(SED) -e 's/^ at RMSD =[ \t]*\([0-9.]\{1,\}\) and alignment length[ \t]*\([0-9]\{1,\}\)/\1\t\2/g'|\
	perl -ne '($$rmsd,$$nalign)=split(/\t/,$$_);printf("UPDATE sandbox.pdbinfo SET ssm_rmsd=\"%.3f\",ssm_nalign=\"%d\",ssm_q=\"%.3f\" WHERE mygroup=\"%s\" AND pair=\"%s\";",$$rmsd,$$nalign,$$nalign**2/((1+($$rmsd/3.0)**2)*$(shell grep -c "^ATOM .* CA .*" $*.lbn.pdb)*$(shell grep -c "^ATOM .* CA .*" $*.lun.pdb)),"$(shell basename $*.lun.pdb|$(SED) "s/\([A-Z0-9]\{5,5\}\)\.lun\.pdb/\1/g")","lun");'|mysql

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

# ===============================================================================================
# Create complex (from original bound structures)
%.rbn-lbn.pdb:	%.rbn.pdb %.lbn.pdb
	cat $^ >$@

# Calculate ASA (using "naccess")
%.rsa:	%.pdb
	$(NACCESS) $< -p 1.40 -v $(NACCESSVDW) -s $(NACCESSSTD) 1> /dev/null
	mv `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.rsa/g'` `basename $<|sed -e 's/\(.....\.[r|l]bn\(-lbn\)\?\)\.pdb/\1.rsa/g'`
	rm `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.log/g'` `basename $<|sed -e 's/\(.....\)\.[r|l]bn\(-lbn\)\?\.pdb/\1.asa/g'`

# Return interface residues (rbn)
%.rbn.pdb.int:	%.rbn-lbn.rsa %.rbn.rsa
	sed '1,4d' $*.rbn-lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D'|gawk '{if(substr($$0,9,1)=="W"){print $$0}}' >$*.rbn-lbn.rsa.tmp
	sed '1,4d' $*.rbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D' >$*.rbn.rsa.tmp
	gawk '{print $$2,$$3,$$4,$$5}' $*.rbn.rsa.tmp|paste - $*.rbn-lbn.rsa.tmp|gawk '{print $$1,$$2,$$3,$$4,$$9}'|\
	gawk '{if($$4==$$5){print $$0,"0.00"}else{print $$0,1-($$5/$$4)}}'|gawk '{if($$6>=0.01){print $$0}}' >$@
	rm $*.rbn-lbn.rsa.tmp $*.rbn.rsa.tmp

# int->fsa (interface residues -> FASTA(aa))
%.rbn.pdb.int.fsa:	%.rbn.int
	cat $<|perl -e 'while(<STDIN>){push(@aas,substr($$_,0,3));}%symbols=$(aaa2aa);for($$i=0;$$i<@aas;$$i++){print $$symbols{$$aas[$$i]};}' >$@

# Return interface residues (lbn)
%.lbn.pdb.int:	%.rbn-lbn.rsa %.lbn.rsa
	sed '1,4d' $*.rbn-lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D'|gawk '{if(substr($$0,9,1)=="X"){print $$0}}' >$*.rbn-lbn.rsa.tmp
	sed '1,4d' $*.lbn.rsa|sed -e :a -e '$$d;N;2,4ba' -e 'P;D' >$*.lbn.rsa.tmp
	gawk '{print $$2,$$3,$$4,$$5}' $*.lbn.rsa.tmp|paste - $*.rbn-lbn.rsa.tmp|gawk '{print $$1,$$2,$$3,$$4,$$9}'|\
	gawk '{if($$4==$$5){print $$0,"0.00"}else{print $$0,1-($$5/$$4)}}'|gawk '{if($$6>=0.01){print $$0}}' >$@
	rm $*.rbn-lbn.rsa.tmp $*.lbn.rsa.tmp

# Return interface residues (rbn/lbn) for R:
%.pdb.int.R:	%.pdb.int
	cat $<|gawk '{print "segments("$$3",0,"$$3",0.05*y1max,col=\"green\",lty=\"solid\");"}' >$@

#./FindCore.pl 1AVXh.lbn.pdb 1AVXh.rbn.pdb 6|gawk '{print $3}'|sort -n|uniq|gawk '{print "segments("$0",0.05*y1max,"$0",0.10*y1max,col=\"green\",lty=\"solid\");"}'

# PDB-to-FASTA(aa): Translate PDB file to FASTA(amino acid) sequence file
#%.pdb.fa:	%.pdb
%.fa:	%.pdb
	@echo ">$@" >$@ | \
	cat $<|perl -e '$$prevres="";$$prevresnum=0;while(<STDIN>){if(/^ATOM/){if(substr($$_,13,2) eq "CA"){$$currentres=substr($$_,17,3);$$currentresnum=substr($$_,22,5);$$currentresnum=~s/\s//g;if($$prevres ne $$currentres || $$prevresnum ne $$currentresnum){push(@aas,substr($$_,17,3));}$$prevres=$$currentres;$$prevresnum=$$currentresnum;}}elsif(/^END/){}}%symbols=$(aaa2aa);for($$i=0;$$i<@aas;$$i++){print $$symbols{$$aas[$$i]};}print"\n";' - |\
	sed -e 's/\(.\{60\}\)/\1\n/g' - >>$@

# FASTA(DNA)-to-FASTA(aa): Translate DNA sequences into amino acid sequences
%.fsa:	%.fa
	cat $<|$(PERL) -ne 'chomp;push @seq,$$1 if /^(>.*)/;$$dna{$$seq[$$#seq]} .=$$1 if(/^([A-Z]+)$$/);%base2aa=$(base2aa);foreach $$s (@seq){print "$$s\n";for($$x=0;$$x<length($$dna{$$s});$$x=$$x+180){$$line=substr($$dna{$$s},$$x,180);for($$y=0;$$y<length($$line);$$y=$$y+3){$$triplet=substr($$line,$$y,3);$$base2aa{$$triplet}="X" unless defined $$base2aa{$$triplet};print $$base2aa{$$triplet} unless $$base2aa{$$triplet} eq "-";}print "\n"}}' >$@

#=============================================================================================================
# GENERATE MASTER REPORT
%.pdb.report.R:	%.pdb.libration.R %.pdb.seg.R %.pdb.ssm.helix.R %.pdb.ssm.hydrophobic.R %.pdb.ssm.hydrophylic.R %.pdb.ssm.strand.R %.pdb.translation.R %.pdb.ssm.rmsd.dat
	echo 'postscript(file="$@.ps",onefile=TRUE,title="FILENAME",paper="letter",width=6.0,height=8.0,horizontal=FALSE)' >$@
	echo 'cc<-c("#0000ff","#00ff00","#ff00ff","#ff0000","#00ffff","#ffff00","#ff7fff","#7f00ff","#ff997f","#7fff7f","#7f7fff","#00ff7f","#ff007f","#ff7f00","#7fff00","#7f00ff","#007fff","#bfbf00","#bf00bf","#00bfbf");n=1;' >>$@
	echo -e "xmax<-scan(pipe(\"grep -c '^ATOM .* CA .*' $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb/g')\"))" >>$@
	echo 'layout(matrix(1:4,ncol=1),heights=1:4);' >>$@
	echo '##===== Translation plot =====##' >>$@
	echo 'par(mar=c(0.5,4,2,2));plot.new();' >>$@
	#echo -e "Tmin<-scan(pipe(\"cat $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\.pdb\).*/\1.translation.R/g')|gawk -F, 'BEGIN{min=1000}NR==1{min=\$$2}{if(\$$2<min){min=\$$2}}END{printf\\\"%.01f\\\",min-0.05}'\"))" >>$@
	echo -e "Tmin<-scan(pipe(\"cat $*.pdb.translation.R|gawk -F, 'BEGIN{min=1000}NR==1{min=\$$2}{if(\$$2==\\\"0\\\"){}else if(\$$2<min){min=\$$2}}END{printf\\\"%.01f\\\",min-0.05}'\"))" >>$@
	echo -e "Tmax<-scan(pipe(\"cat $*.pdb.translation.R|gawk -F, '\$$2>max{max=\$$2;};END{printf\\\"%.01f\\\",max+0.05}'\"))" >>$@
	echo -e "Tnum<-scan(pipe(\"wc -l $*.pdb.translation.R|gawk '{print \$$1}'\"))" >>$@
	echo 'plot.window(c(0,xmax),c(Tmin,Tmax));grid();par(col="grey50",fg="grey50",col.axis="grey50");' >>$@
	cat $*.pdb.translation.R >>$@
	echo 'axis(2,at=seq(Tmin,Tmax,0.1))' >>$@
	echo 'box()' >>$@
	echo 'mtext("A dis.",side=2,line=2,cex=0.8)' >>$@
	echo '## REPORT TITLE:' >>$@
	echo 'mtext("'`$(MYSQL) 'SELECT CONCAT(mygroup,"-",pair,": ",pdbid," [",header,"]") FROM pdbinfo WHERE mygroup="$(shell echo "$(shell basename $@)"|sed -e's/\(.....\)\.\(.un\).*/\1\" AND pair=\"\2/g')";'`'",side=3,line=1,cex=0.8)' >>$@
	echo 'mtext(paste("S=",Tnum),side=4,line=1,adj=0,cex=0.8)' >>$@
	echo '##===== Libration plot =====##' >>$@
	echo 'par(mar=c(0.5,4,0,2));plot.new()' >>$@
	echo -e "Lmax<-scan(pipe(\"cat $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\.pdb\).*/\1.libration.*.dat/g')|gawk '\$$2>max{max=\$$2;};END{printf\\\"%0.1f\\\",max}'\"))" >>$@
	echo 'plot.window(c(0,xmax),c(0,Lmax));grid();par(col="grey50",fg="grey50",col.axis="grey50")' >>$@
	cat $*.pdb.libration.R >>$@
	echo 'axis(2,at=seq(0,Lmax,Lmax/10))' >>$@
	echo 'box()' >>$@
	echo 'mtext("A dis.",side=2,line=2,cex=0.8)' >>$@
	echo 'mtext(paste("Christoph Champ, ",Sys.time()),side=4,line=1,adj=1,cex=0.45)' >>$@
	echo '##===== B factor plot =====##' >>$@
	echo 'par(mar=c(0.5,4,0,2));plot.new()' >>$@
	echo -e "y1max<-scan(pipe(\"gawk '{if(substr(\$$0,14,3)==\\\"CA \\\"){print substr(\$$0,61,6)}}' $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb/g')|sed -e's/ //g'|sort -n|tail -1\"))" >>$@
	echo 'plot.window(c(0,xmax),c(0,y1max));grid();par(col="grey50",fg="grey50",col.axis="grey50")' >>$@
	echo -e "lines(scan(pipe(\"gawk '{if(substr(\$$0,14,3)==\\\"CA \\\"){print substr(\$$0,61,6)}}' $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb/g')|sed -e's/ //g'\")),col=\"blue\",type=\"s\")" >>$@
	echo '# "complex interface sites":' >>$@
	cat $(shell echo "$(shell basename $@)"|sed -e's/\(.....\)\.\([r|l]\)un.*/\1.\2bn.pdb.int.R/g') >>$@
	#cat soon.R >>$@
	cat $*.pdb.seg.R >>$@
	echo 'axis(2,at=seq(0,y1max,10))' >>$@
	echo 'box()' >>$@
	echo 'mtext("B factor",side=2,line=2,cex=0.8)' >>$@
	#echo 'mtext("D=R; C=E; RMSD=0.47; dASA=1585",side=4,line=1,adj=0,cex=0.8)' >>$@
	echo -e "mtext(\""`$(MYSQL) 'SELECT CONCAT("D=",b.difficulty,"; C=",b.cat,"; RMSD=",b.rmsd,"; dASA=",b.dasa) FROM bench b, pdbinfo p WHERE LEFT(b.complex,4)=LEFT(p.mygroup,4) AND mygroup="$(shell echo "$(shell basename $@)"|sed -e's/\(.....\)\.\(.un\).*/\1\" AND pair=\"\2/g')";'`"\",side=4,line=1,adj=0,cex=0.8)" >>$@
	echo '##===== RMSD plot =====##' >>$@
	echo 'par(mar=c(5,4,0,2));plot.new()' >>$@
	#echo -e "y2max<-scan(pipe(\"cat $(shell echo "$(shell basename $@)"|sed -e's/\(.....\.[r|l]un\).*/\1.pdb.ssm.maxrmsd/g')\"))" >>$@
	echo 'y2max<-c(10)' >>$@
	echo 'plot.window(c(0,xmax),c(0,y2max));grid();par(col="grey50",fg="grey50",col.axis="grey50")' >>$@
	cat $*.pdb.ssm.hydrophylic.R $*.pdb.ssm.hydrophobic.R $*.pdb.ssm.helix.R $*.pdb.ssm.strand.R >>$@
	echo '# SSM RMSD Section:' >>$@
	echo 'lines(read.table("$*.pdb.ssm.rmsd.dat"),col="blue",type="s")' >>$@
	echo 'axis(1)' >>$@
	echo 'axis(2,at=seq(0,y2max,y2max/10))' >>$@
	echo 'box()' >>$@
	echo 'mtext(paste("residue number (n=",xmax,")"),side=1,line=2,cex=0.8)' >>$@
	echo 'mtext("RMSD (A)",side=2,line=2,cex=0.8)' >>$@
	echo -e "mtext(\""`$(MYSQL) 'SELECT CONCAT("I=",inertia,"; Nalign=",ssm_nalign,"; Q=",ssm_q) FROM pdbinfo WHERE mygroup="$(shell echo "$(shell basename $@)"|sed -e's/\(.....\)\.\(.un\).*/\1\" AND pair=\"\2/g')";'`"\",side=4,line=1,adj=0,cex=0.8)" >>$@
	cat $*.pdb.seg.R >>$@
%.pdb.report.R.ps:	%.pdb.report.R
	cat $<|R --vanilla

#=============================================================================================================

%.sql:       %.pdblist
	for pdbid in $$(cat $<|gawk '{print $$1}'|sort|uniq);\
	do\
	head -1 $$pdbid.pdb|sed -e ':a;$$!N;s/\n//;ta\
	s/\(HEADER\)[ ]*\(.*\)\([0-9][0-9].[A-Z].\{2\}.[0-9][0-9]\)[ ]*\([0-9A-Z].\{3\}\).*/\4,\2,\3/;\
	s/^[ \t]*//;s/[ \t]*$$//;s/^/"/;s/$$/"/;s/[ ]*,/","/g;' >>$@;\
	done

# Extract TLS analysis table for given segment (from TLSMD Server)
%.tex:	%.tlsid
	lynx -dump $(SKULD)/jobs/$(shell cat $<)/ANALYSIS/XXXX_CHAIN$(shell basename $< .tlsid|sed -e "s/[R|L]-[A-Z0-9].\{3\}_\([A-Z]\)/\1/")_ANALYSIS.html |\
	sed -f $(TDIR)/tls_colours.sed|\
	head -20|tail -5|\
	sed -f $(TDIR)/tls_rm_colours.sed |sed -e 's/, /,/g'|sed -e 's/ / \& /g' -e 's/$$/\\\\/' >$@

%.report.tex:	%.tex
	cat $(TDIR)/report_head_template.tex | sed 's/REPORTTITLE/'$(shell basename $< .tex)'/g' |\
	cat - \
		$(TDIR)/report_table_head_template.tex \
		$<\
		$(TDIR)/report_table_tail_template.tex \
		$(TDIR)/report_figure_template.tex \
		$(TDIR)/report_tail_template.tex \
	>$@

# Generate PostScript (.ps) file from LaTeX (.tex) file
%.tex.ps:	%.tex
	latex $< && dvips -f $(shell basename $< .tex).dvi >$@

%.bf.R.epsi:	%.pdb
	cat $(TDIR)/bf_template.R |sed 's/FILENAME/$</g' |R --vanilla
	ps2epsi $<.ps $@

#==========================================================================
# PDB-to-MATRIX: Translate PDB file x,y,z to contact matrix
%.pdb.mat:	%.pdb
	cat $<|perl -e '$$count=$$i=$$j=0;while($$line=<STDIN>){@line=split(/\s+/,$$line);if($$line=~ /^ATOM/){push @field,[@line];$$count++;}}print("$(shell basename $< .pdb)");for($$i=0;$$i<$$count;$$i++){printf("\tE%-2d",$$i+1);}print("\n");for($$i=0;$$i<$$count;$$i++){printf("E%-2d",$$i+1);for($$j=0;$$j<$$count;$$j++){$$r=sqrt(($$field[$$i][6]-$$field[$$j][6])**2+($$field[$$i][7]-$$field[$$j][7])**2+($$field[$$i][8]-$$field[$$j][8])**2);printf("\t%5.2f",$$r);}print("\n");}' - >$@
# PDB(CAa)-to-MATRIX: Translate PDB file x,y,z to contact matrix (CAs only)
%.ca.pdb.mat:	%.ca.pdb
	cat $<|perl -e 'sub trim($$);$$count=$$i=$$j=$$k=0;while($$line=<STDIN>){@line=split(/\s+/,$$line);if($$line=~/^ATOM/){push @x,[trim(substr($$line,30,8))];push @y,[trim(substr($$line,38,8))];push @z,[trim(substr($$line,46,8))];push @c,[substr($$line,71,1)];$$count++;}}print("$(shell basename $< .pdb.ca)");for($$i=0;$$i<$$count;$$i++){printf("\tE%-2d",$$i+1);}print("\n");for($$i=0;$$i<$$count;$$i++){printf("E%-2d",$$i+1);if($$c[$$i][0] eq "*"){for($$k=0;$$k<$$count;$$k++){printf("\t%5.2f","0.00");}}else{for($$j=0;$$j<$$count;$$j++){if($$c[$$j][0] eq "*"){printf("\t%5.2f","0.00");}else{$$r=sqrt(($$x[$$i][0]-$$x[$$j][0])**2+($$y[$$i][0]-$$y[$$j][0])**2+($$z[$$i][0]-$$z[$$j][0])**2);printf("\t%5.2f",$$r);}}}print("\n");}sub trim($$){my $$string=shift;$$string=~s/^\s+//;$$string=~s/\s+$$//;return $$string;}' - >$@

# MATRIX-to-PNG: Generate a PNG from a matrix file
%.pdb.mat.png:	%.pdb.mat
	`$(MATRIX2PNG) -data $< -size 8:8 -mincolor darkred -maxcolor white -c -r -s -con 4.0 >$@`
# MATRIX-to-PNG: Generate a PNG from a matrix file (for CAs)
%.ca.pdb.mat.png:	%.ca.pdb.mat
	`$(MATRIX2PNG) -data $< -size 2:2 -mincolor darkred -maxcolor white -s -con 4.0 >$@`
#ROSEPLOT
#cat 1A2Kh.rbn.int|perl -e 'while(<STDIN>){push(@aas,substr($_,0,3));}%symbols=("ALA","A","CYS","C","ASP","D","GLU","E","PHE","F","GLY","G","HIS","H","ILE","I","LYS","K","LEU","L","MET","M","MSE","M","ASN","N","PRO","P","GLN","Q","ARG","R","SER","S","THR","T","VAL","V","TRP","W","TYR","Y","GLX","Z");for($i=0;$i<@aas;$i++){print $symbols{$aas[$i]};}print"\n";'|perl aausage2.pl |perl roseplot.pl -labelFontSize 15  -axistitle "Frequency" -fcolor blue -Xcol 2 -Ycol 4 -steps 1 -T "Amino Acid Usage" -ST "Test" -output foo.ps
