##== ZDB makefile ==
## by Christoph Champ, August 2007

##=== Columns ===
MYSQLe 		= mysql -B -N -e #comment
ATLASDB 	= zdb.atlasdb2
SEGMENTID 	= CONCAT(LEFT(genus,1),species,'_',strain,'_',segment)
SEGMENTIDesc 	= CONCAT(LEFT(genus,1),species,\"_\",strain,\"_\",segment)

# Return the fraction of NFIs found in a given GenBank
%.nfi.fraction:	%.nfi.gff
	echo -e "scale=4;`cat $<|$(PERL) -e 'while(<>){next if($$_=~/^#/);@row=split(/\s+/,$$_);$$len+=($$row[4]-$$row[3]);}print $$len;'`/`$(MYSQLe) "SELECT length FROM $(ATLASDB) WHERE $(SEGMENTID)='$(shell basename $<|sed -e 's/\(.*\)\.nfi\.gff/\1/g')';"`"|bc -l|tee $@|$(PERL) -ne 'chomp;printf("UPDATE $(ATLASDB) SET nfi_fraction=\"%s\" WHERE $(SEGMENTIDesc)=\"$(shell basename $<|sed -e 's/\(.*\)\.nfi\.gff/\1/g')\";\n",$$_);'|mysql

#phyla.archaea:
#	$(MYSQLe) "SELECT DISTINCT(phyla) FROM $(ATLASDB) WHERE kingdom='Archaea' and segment='Main' and phyla!='unknown' ORDER BY phyla;"
#
#phyla.bacteria:
#	$(MYSQLe) "SELECT DISTINCT(phyla) FROM $(ATLASDB) WHERE kingdom='Bacteria' and phyla!='unknown' ORDER BY phyla;"
#
#phyla.eukaryotes:
#	$(MYSQLe) "SELECT DISTINCT(phyla) FROM $(ATLASDB) WHERE kingdom='Eukaryotes' and phyla!='unknown' ORDER BY phyla;"

## SAVE:
#for i in `mysql -B -N -e "SELECT CONCAT(phyla,'(n=',count(*),')') FROM zdb.atlasdb2 WHERE kingdom='Bacteria' and segment='Main' and phyla!='unknown' GROUP BY phyla ORDER BY phyla;"`; do echo $i|sed -e 's,/,-,g'; done
#for i in `mysql -B -N -e "SELECT DISTINCT(phyla) FROM zdb.atlasdb2 WHERE kingdom='Bacteria' and segment='Main' and phyla!='unknown' ORDER BY phyla;"`; do mysql -B -N -e "SELECT atcontent FROM zdb.atlasdb2 WHERE phyla='$i' AND segment='Main';"; done
#for i in `cat Bacteria.phyla`; do mysql -B -N -e "SELECT atcontent FROM zdb.atlasdb2 WHERE phyla='`echo $i|sed -e 's,_,/,g'`' AND segment='Main';" >>$i.atcontent;done
