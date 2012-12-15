# Exercise 1
# 
# Introduction to UNIX
# 
# Unless you are already familiar with operating in a UNIX environment it might be worthwhile to go through the Pedestrians guide to unix.
# OBJECTIVES
# The purpose of this exercise is to become familiar with tools for prokarypotic gene prediction (protein coding, as well as tRNA, and rRNA genes) and to introduce methods allowing faster and easier retrieval and comparison of the data.
# 
# Key tools used in this exercise are:
# Perl - a platform independent scripting language commonly used in bioinformatics.
# Perl stands for Practical Extraction and Report Language.
# EasyGene was developed by Thomas Schou Larsen and Anders Krogh at CBS and it uses HMMs (Hidden Markov Models) to predict location of genes in prokaryotic genomes.
# The HMMer package, developed by Sean Eddy is used to build Profile HMMs which will be used to scan genome sequences. This package is utilised by RNAmmer - an in-house tool to predict rRNA.
# tRNAscan-SE by Sean Eddy is a very common tool for predicting tRNA genes.
# We will be using a MySQL database to contain these results.
# 
# 
# 
# We will start out by acquiring 8 complete genome sequences. This will demonstrate how prediction and comparison tools can assist in the characterization and comparison of genome sequences.
# 
#    1.
#       LOG IN
#       Open a session on 'organism.cbs.dtu.dk', and from this login to ibiology and create directories for you to work in:
# 
       ssh -X ibiology
       mkdir Ex1
       cd Ex1
       unset correct
# 
#       The last command turns off spelling correction, which can interfere with complex commands.
#    2.
#       DOWNLOAD GENBANK FILES
#       We will download genbank files for the following strains using a Perl script that contacts NCBI's website:
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
#       Hopefully soon it will become trivial to type in all the accession numbers, and we will take advantage of the C-shell to wrap the perl script in a foreach-loop from the shell. Execute the following to download all the GenBank files:
# 
       mkdir source
       foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       ~pfh/scripts/mygetgene/mygetgene $accession > source/$accession.gbk &
       end
       wait
# 
#    3.
#       CONVERT AND STORE SEQUENCE INFORMATIfON
#       We will now need to extract the DNA sequence from each of the genbank file (click here to see a sample genbank file), by using a little Perl one-liner written for this exercise.
# 
       echo "delete from cmp_genomics.genomes where user=USER()" | mysql
       foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
       perl -ne 'print ">$1\n" if /^LOCUS\s+(\S+)/;\
               if ( /^\s+\d+\s+([ATGCRYWSMKHBVDN\s]+)$/gi ) { $l = uc($1); $l =~ s/\s//g;print "$l\n"; } ' \
               < source/$accession.gbk > source/$accession.fsa
       perl -ne 'chomp;$accession = $1 if />(.*)/; next unless /^([ATGCRYWSMKHBVDN]+)$/; $seq .= $1; \
               END{ print "insert into cmp_genomics.genomes (accession,user,seq) VALUES (\"$accession\",USER(),\"$seq\");\n";}' \
               < source/$accession.fsa | mysql
       perl -ne 'chomp;$def = $1 if /^DEFINITION\s+(.*)/; $accession = $1 if /^LOCUS\s+(\S+)/;\
               END{ print "update cmp_genomics.genomes set organism=\"$def\" where user=USER() and accession=\"$accession\";\n";}' \
               < source/$accession.gbk | mysql
       end
# 
#    4.
#       PREDICT THE NUMBER OF CDS
#       We have now downloaded the sequence information, converted it into FASTA format and it is time to begin to generate some information about the characteristics for these genomes. First, Easygene will be used to predict the open reading frames. This program was developed for our older generation servers, so using these commands, we temporarily log in to those computers using SSH. This time, we are going to append an & (Ampersand) to each step end the foreach loop. This will cause the execution of easygene to run on individual processors - one processor per genome.
# 
# 
#          1. Execute EasyGene on 'organism' and save results
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             rsh -n organism "easygene /home/people/$user/Ex1/source/$accession.fsa > /home/people/$user/Ex1/source/$accession.gff" &
             end
             wait
# 
#             The output of EasyGene is in so-called GFF-format ( GFF stands for Gene Finding Format) and we will use this output to predict the number of genes. First, try and have a look at the output.
# 
             head source/AP008232.gff
# 
#          2.
#             The 6th column contains the R-values which is the expected number of genes that one would find pr. megabase random DNA with a standard score greater than that of the CDS. The authors of the program recommend using an R-value with a max. value of 2, so we are now going to count the number of genes, having this or lower R-values. We are going to store the gene count the shell variable 'genes' and use mysql to store this number along with your account username and the accession number.
# 
             echo "delete from cmp_genomics.features where user=USER() and featuretype='CDS'" | mysql
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             perl -ane 'chomp;next unless $F[1] eq "EasyGene" and $F[5] <= 2;\
                     print "insert into cmp_genomics.features (accession,user,featuretype,start,stop,dir) \
                     VALUES (\"$F[0]\",USER(),\"CDS\",$F[3],$F[4],\"$F[6]\");\n";' < source/$accession.gff  | mysql 
             end
# 
#    5.
#       PREDICT THE NUMBER OF tRNA
#       The tRNA genes will be predicted using tRNAscan-SE by Sean Eddy.
# 
#          1. Execute trnascan and redirect the results:
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             rsh -n organism "trnascan-1.4 /home/people/$user/Ex1/source/$accession.fsa > /home/people/$user/Ex1/source/$accession.trnascan.out" &
             end
             wait
# 
#          2. Store results in the database:
# 
             echo "delete from cmp_genomics.features where user=USER() and featuretype='tRNA'" | mysql
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             perl -ne 'chomp($line = $_);$accession = $1 if /sequence name= (\S+)/;\
                     ($start,$stop) = ($1,$2) if $line =~ /start position= (\d+) end position= (\d+)/;\
                     $dir = "+"; \
                     $dir="-" if $stop < $start; \
                     ($start,$stop) = ($stop,$start) if $dir eq "-";\
                     $note = "tRNA-$1" if $line =~ /tRNA predict as a tRNA\- \S+/;\
                     print "replace into cmp_genomics.features (accession,user,featuretype,start,stop,dir,note) \
                             VALUES (\"$accession\",USER(),\"tRNA\",$start,$stop,\"$dir\",\"note\");\n" if $line =~  /^number of base pairing/;'\
                     source/$accession.trnascan.out | mysql 
             end
# 
#    6.
#       PREDICT THE NUMBER OF 16s rRNA GENES
#       The next method is still in the development at CBS, and a paper is being written about this method. Basically, the method uses HMM to predict rRNA genes by using structural alignments of rRNA genes. Other methods relies on BLAST to find close homologs but such methods are likely to give too few hits and fails to find start/stop position of the gene very accuratly.
# 
# 
#          1. Execute 'rnammer' to preduct 16s rRNA genes using a Hidden Markov Model:
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             rnammer -multi -S bac -m ssu -gff source/$accession.rnammer.ssu.gff source/$accession.fsa &
             end
             wait
# 
#          2. Again, the output is in GFF, and we do a simple count to extract the number of 16s rRNA genes:
# 
             echo "delete from cmp_genomics.features where user=USER() and featuretype='rRNA'" | mysql
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             perl -ane 'chomp;\
                     next unless $F[1]=~ /RNAmmer/ and $F[2] eq "rRNA";\
                     print "replace into cmp_genomics.features (accession,user,featuretype,start,stop,dir) \
                     VALUES (\"$F[0]\",USER(),\"rRNA\",$F[3],$F[4],\"$F[6]\");\n";' < source/$accession.rnammer.ssu.gff | mysql
             end
# 
#    7.
#       COLLECTING THE RESULTS
#       Now, time for recap. You have now hopefully managed to download and convert full 6 genome sequences, and predicted coding sequences, tRNA, and rRNA in all of the. Also, you have stored all your predictions and a shared mysql table. You are now going to enter the mysql client in to order to access your results. You open the client by writing 'mysql' at the prompt. The new prompt will show 'mysql>' once it's ready. When pasting in mysql-code from the section below, please leave out the 'mysql>'-part. This is included just to make you aware, that the mysql client is active.
# 
# 
#          1. Open the MySQL client:
# 
             mysql
             mysql>use cmp_genomics
# 
#          2. We start out with a simple query. This will list all the genomes you have now analysed.
# 
             mysql>select accession,organism,length(seq) from genomes where user=USER() order by organism;
# 
# 
#          3. Next is to look at the features you have predicted for each accession number:
# 
             mysql>select accession,count(*) from features where user=USER() and featuretype='rRNA' group by accession;
# 
#             Repeat this step for CDS' and tRNA.
# 
#             You could also list all features predicted by doing a group by accession and the feature type:
# 
             mysql>select featuretype,accession,count(*) from features where user=USER()  group by accession,featuretype;
# 
# 
#          4. MySQL also allows you to cross reference tables (relational query). This query will summarize the total length of predicted CDS' for each genome as well as average gene length.
#             Remember that genome length was stored in a
# 
             mysql>SELECT genomes.accession,organism,SUM(stop-start+1) as total_length,AVG(stop-start+1) as avg_length FROM features,genomes \
                     WHERE features.user=USER() AND featuretype='CDS' AND features.user = genomes.user AND genomes.accession = features.accession \
                     GROUP BY genomes.accession;
# 
#             We can also perform a relational query between the genomes table and the features table in order to derive the coding density. The coding density is the total length of all CDS' divided by the total genome (this fraction is the percentage of the total coding capacity of the genome)
#          5.
# 
             mysql>SELECT features.accession,organism,SUM(stop-start+1)/LENGTH(seq) as Coding_Density from features,genomes \
                     WHERE featuretype = 'CDS' AND features.user = genomes.user AND features.user = USER() AND features.accession = genomes.accession \
                     GROUP BY features.accession;
# 
#    8.
#       QUESTIONS
#          1. Do you see any variation among the coding densities and if yes, can you find explanations as to why? (You may need to do some Google)
#          2. Which genome has the most number of genes?
#          3. Which genome has the fewest number of genes?
#          4. Is there a correlation between number of genes and number of tRNAs, and rRNAs?
#          5. In the coming exercise, we will calculate the AT content of the genome sequences. 
# 
#       REFERENCES
#       EasyGene - a prokaryotic gene finder that ranks ORFs by statistical significance.
#       Thomas Schou Larsen and Anders Krogh.
#       BMC Bioinformatics 2003, 4:21
# 
#       RNammer: consistent annotation of rRNA genes in genomic sequences
#       Karin Lagesen, Peter Fischer Hallin, and David W. Ussery
#       Manuscript in preperation
# 
# This file is last modified Thursday 28th of September 2006 11:41:26 GMT
