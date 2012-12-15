# OBJECTIVES
# The purpose of this exercise is to become familiar with the construction of genome atlases, with their analysis and use as a visualization tool. We will make three different kinds of atlases for each genome, and see what we can learn from them.
# 
# Key tools used in this exercise are:
# We will be using a system of makefiles, which is designed for the compilation of programs, but which is used for running pipelines of different tools and format conversions at cbs. For this purpose we use the GNU Make program.
# To construct the atlases, the GeneWiz program is used. An online java version is soon to be released.
# We will be using a MySQL database to access results.
# 
# 
# 
#    1.
#       LOG IN
#       Open a session on 'organism.cbs.dtu.dk', and from this login to ibiology and create directories for you to work in:
# 
       ssh -X ibiology
       mkdir Ex2
       cd Ex2
# 
#    2.
#       Link data from last week
#       Remember that we are working on the following genomes:
# 
#       +-----------+-----------------------------------------------------------------+
#       | accession | organism                                                        |
#       +-----------+-----------------------------------------------------------------+
#       | AE016879  | Bacillus anthracis str. Ames, complete genome.                  |
#       | AE017042  | Yersinia pestis biovar Microtus str. 91001, complete genome.    |
#       | AL111168  | Campylobacter jejuni subsp. jejuni NCTC 11168 complete genome.  |
#       | AL645882  | Streptomyces coelicolor A3(2) complete genome.                  |
#       | AP008232  | Sodalis glossinidius str. 'morsitans' DNA, complete genome.     |
#       | AP009048  | Escherichia coli W3110 DNA, complete genome.                    |
#       | BA000021  | Wigglesworthia glossinidia endosymbiont of Glossina brevipalpis |
#       | CP000034  | Shigella dysenteriae Sd197, complete genome.                    |
#       +-----------+-----------------------------------------------------------------+
# 
#       Today we will use mainly the genbank files and the data from the database. Thus we start out by linking the genbank files from the directory we used last week.
# 
#        
       ln -s ../Ex1/source/*.gbk .
# 
#    3.
#       Making a base atlas
#       Using the Makefile system at cbs, a lot of common tasks are made easy by using an ordered pipeline of commands. We will use this a lot in this exercise.
#       First we make the configuration files for the base atlases. This is done using GNU Make.
# 
       foreach genome  (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.baseatlas.cf &
       end
       wait
# 
#       When this command has finished, inspect the files using your favourite editor. One thing to notice is that the config file for Streptomyces coelicolor creates a circular atlas. This is easily remedied by replacing all occurences of circle with linear. This can be done quickly with the command:
# 
       sed 's/circle/linear/g' AL645882.baseatlas.cf
# 
#       Now we only need to make the actual atlases.
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.baseatlas.ps &
       end
       wait
# 
#       This creates a postscript file of the base atlas for each genome. These files can be viewed with ghostview.
#    4.
#       Making a structure atlas
#       This step is almost the same as the former, only we make a DNA structure atlas this time. Again we start by making the configuration files for the atlases.
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.structureatlas.cf &
       end
       wait
# 
#       Inspecting the files you should start to see a pattern in the format. Again the config file for S. coelicolor creates a circular atlas. We fix it with the command:
# 
       sed 's/circle/linear/g' AL645882.structureatlas.cf
# 
#       Now we only need to make the actual atlases.
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.structureatlas.ps
       end
       wait
# 
#       This creates a postscript file of the structure atlas for each genome.
#    5.
#       Making a genome atlas
#       For the genome atlas, the procedure is the same as the two previous atlases. Thus we make it in one big step
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.genomeatlas.cf &
       end
       wait
       sed 's/circle/linear/g' AL645882.structureatlas.cf
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       gmake $genome.genomeatlas.ps
       end
       wait
# 
#       And once this is done we have a postscript file of the genome atlas for each genome.
#    6.
#       Making a genome atlas
#       Now last time, we could see from the gene counts that for some organisms the number of predicted genes was much lower than expected. Let us try to have a look at this on an atlas and compare it to the genbank annotations. We do this by first making a new annotation file.
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       mysql -B -e "select CONCAT(featuretype,'2'), start, stop, dir, featuretype from features where user = USER() \
       and accession = '$genome' order by start" cmp_genomics > $genome.pred.ann
# 
#       Now we make a backup of the atlases that we already have:
# 
       mkdir atlases
       mv *.ps atlases
# 
#       Now we edit the baseatlas configuration file for each genome by inserting the following after the 'ann' entries already present.
# 
       ann CDS2 pos 0.0 0.0 1.0 "pred CDS +" fillarrow mark;
       ann CDS2 neg 1.0 0.0 0.0 "pred CDS -" fillarrow mark;
       ann rRNA2 0.0 1.0 1.0 "pred rRNA" fillarrow mark;
       ann rRNA2 pos 0.0 1.0 1.0 "pred rRNA +" fillarrow mark;
       ann rRNA2 neg 0.0 1.0 1.0 "pred rRNA -" fillarrow mark;
       ann tRNA2 0.0 1.0 0.0 "pred tRNA" fillarrow mark;
       ann tRNA2 pos 0.0 1.0 0.0 "pred tRNA +" fillarrow mark;
       ann tRNA2 neg 0.0 1.0 0.0 "pred tRNA -" fillarrow mark;
# 
#       Then insert this line with the other circle entries. Use linear instead of circle for S. coelicolor.
# 
       circle CDS2 pos, CDS2 neg, rRNA2, tRNA2 by dir;
# 
#       And this line with the other file entries.
# 
       file ACCESSION.pred.ann ann;
# 
#       Now save the file, and remake the base atlases.
# 
       foreach genome (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       sed "s/ACCESSION/$genome/g" $genome.baseatlas.cf
       gmake $genome.baseatlas.ps &
       end
       wait
# 
#       Obviously the predictions are not very good for a number of the organisms.
# 
#       Do you see a pattern in the genomes with bad predictions?
# 
#       Easygene has a number of different models available, which gives specificity to different organisms.
# 
#       	Model	Organism
#       	---------------------------------------
#       	aerpe	Aeropyrum pernix
#       	bacan	Bacillus anthracis
#       	bachd	Bacillus halodurans
#       	bacsu	Bacillus subtilis
#       	biflo	Bifidobacterium longum
#       	borbu	Borrelia burgdorferi 
#       	brume	Brucella melitensis
#       	brusu	Brucella suis
#       	camje	Campylobacter jejuni
#       	caucr	Caulobacter crescentus
#       	chltr	Chlamydia trachomatis 
#       	ecoli	Escherichia coli K-12
#       	haein	Haemophilus influenzae
#       	helpj	Helicobacter pylori J-99
#       	hybut	Hyperthermus butylicus
#       	lacla	Lactococcus lacti
#       	myctu	Mycobacterium tuberculosis
#       	mycge	Mycoplasma genitalium
#       	oceih	Oceanobacillus iheyensis
#       	ricpr	Rickettsia prowazekii
#       	salti	Salmonella typhi
#       	staan	Staphylococcus aureus
#       	sulac	Sulfolobus acidocaldarius
#       	sulso	Sulfolobus solfataricus
#       	sulto	Sulfolobus tokodaii
#       	thema	Thermotoga maritima
#       	vibrio	Vibrio cholerae
# 
#       They can be used by calling easygene with the '-o model_name' option. Since there isn't a model for every organism, we simply choose the model for the most similar organism. An example is doing it for S. coelicolor
# 
       easygene -o myctu AL645882.fsa > AL645882.gff
# 
#       Now these results can be inserted into the database by using the same code snippet that we used last time.
# 
       set accession AL645882
       echo "delete from cmp_genomics.features where user=USER() and featuretype='CDS' and accession='$accession'" | mysql
       perl -ane 'chomp;next unless $F[1] eq "EasyGene" and $F[5] <= 2;\
               print "insert into cmp_genomics.features (accession,user,featuretype,start,stop,dir) \
               VALUES (\"$F[0]\",USER(),\"CDS\",$F[3],$F[4],\"$F[6]\");\n";' < source/$accession.gff  | mysql 
       unset accession
# 
#       When you have repeated this for all appropriate genomes, then you are done for today. 
