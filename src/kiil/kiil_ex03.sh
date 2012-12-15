# Exercise 3
# 
# OBJECTIVES
# To get familiar with methods to visualize codon usage, amino acid usage and AT content in complete genome sequences and genome annotations. It is essential that previous exercises have completed with success, before you can continue with Exercise 3.
# 
# Key tools used in this exercise are:
# Scripts used for this exercise
# roseplot.pl 	Perl script for generating a PostScript rose plot
# aausage2.pl 	Generates a table of amino acid usage in a protein list ( fasta file )
# codon-usage.pl 	Generates a table of codon usage in a gene list ( fasta file )
# groups.txt 	Categories of amino acids used by 'aausage2.pl'
# trans.txt 	Codon Table used by 'codon-usage.pl'
# logo.pl 	Compiles all triplets to prepare for logo generation
# 
#    1.
#       LOG IN
#       Open the session like you would normally do and login to ibiology and create directories for you to work in:
# 
       ssh -X ibiology
       mkdir Ex3
       cd Ex3
       mkdir source
# 
#    2.
#       CALCULATE GENOMIC AT CONTENT
#       We will start out soft, by calculating the AT content of the genome sequences stored in the database. We do this by extracting the genome sequence from the database and let Perl do the calculation calculate.
# 
       mysql -B -N -e "select organism,seq,length(seq) from cmp_genomics.genomes where user = USER()" | \
          perl -ne 'my %bases;($organism,$seq,$l) = split(/\t+/,$_);\
            for ( $x = 0 ; $x < $l ; $x++) { $bases{substr($seq,$x,1)}++;} ;\
            printf ("%0.2f\t%s\n" , ($bases{A}+$bases{T})/$l,$organism)' > ATcontent.txt
       cat ATcontent.txt
# 
#    3.
#       GENERATE GENE SEQUENCES AND TRANSLATIONS USING ANNOTATIONS
#       We will now have a look at codons and how the global AT content affects the codon usage and the amino acid usage of the organisms. To do this, we first need to generate the Open Reading Frames and the proteins. For doing this we are going to use the predictions from Exercise 1 which were stored in a MySQL table (cmp_genomics.features). In the other MySQL table (cmp_genomics.genomes) all the full genome sequences are stored as single lines of text. In other words, you need to relate these two tables by accession number.
# 
#       In Exercise 1, you started the MySQL Client in an 'interactive' mode. That is, a client was started, waiting for you to type in your query. This time, we will be using mysql in batch-mode operation, which allows us to simply send the command to the client, and obtain the output of the query. This approach is especially suited for automation and for processing large amounts of data.
# 
#          1. First, let's review the first 10 predicted features you have place in the database:
# 
             mysql -D cmp_genomics -e "select start,stop,dir,accession from features where user=user() limit 10"
#              
# 
#          2. Second, just for fun, lets try and extract the first 10 basepairs of the first 10 genes you predicted on the positive strand:
# 
             mysql -D cmp_genomics -e "select substr(seq,start,10) as sequence,start,stop,dir,genomes.accession \
               from genomes,features \
               where genomes.accession = features.accession and genomes.user = features.user \
                                               and features.user = user() and featuretype='CDS' and dir = '+'limit 10"
#              
# 
#             The next bit of code will make a similar query but here we will extract the full gene sequence and generate a gene name of each orf. We could have used MySQL to also generate the DNA complementation but this is more simple to do in Perl. We then pipe ('|') the MySQL result into Perl, and for those genes located on the reverse strand, Perl will generate the complement. After ensuring complementation, the Perl script prints output in Fasta format (60 columns wide). It may look at bit complex, but try have a look at see what the code does.
#          3.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
                mysql -B -N -D cmp_genomics -e "select substr(seq,start,stop-start+1) as sequence,\
                  concat('CDS_',start,'-',stop,'_DIR',dir) as name,dir from genomes,features where \
                  genomes.accession = features.accession and genomes.user = features.user and \
                  features.user = user() and featuretype='CDS' and features.accession = '$accession'" | \
                perl -ane '$F[0] = reverse ( $F[0] ) if $F[2] eq "-";$F[0] =~ tr/ATGC/TACG/ if $F[2] eq "-";\
                  print ">$F[1]\n"; \
                  for ( $x = 0 ; $x < length ( $F[0] ) ; $x = $x+60) { print substr ( $F[0] , $x , 60),"\n"; } ' > source/$accession.orfs.fsa
             end
# 
#          4. To verify the the output try and view the first part of one of the output files. ( use space bar to scroll, q to quit)
# 
             less source/AP008232.orfs.fsa
# 
#             A database solution is worth considering when planning to store biological information and hopefully this small example has demonstrated some basic functionality of the database. We will now translate the DNA sequences into amino acid sequences using Perl. Somewhere in the code, you should be able to find the translation table. This is kept as so-called 'hash'. The hash serves as a convinient look-up table.
#          5.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             cat source/$accession.orfs.fsa | perl -ne 'chomp;push @seq , $1 if /^(>.*)/; $dna{$seq[$#seq]} .= $1 if (/^([A-Z]+)$/);\
                      END { %base2aa =  ("AAA" => "K", "AAC" => "N", "AAG" => "K", "AAT" => "N", "ACA" => "T", "ACC" => "T", "ACG" => "T", \
                           "ACT" => "T", "AGA" => "R", "AGC" => "S", "AGG" => "R", "AGT" => "S", "ATA" => "I", "ATC" => "I", "ATG" => "M", \
                           "ATT" => "I", "CAA" => "Q", "CAC" => "H", "CAG" => "Q", "CAT" => "H", "CCA" => "P", "CCC" => "P", "CCG" => "P", \
                           "CCT" => "P", "CGA" => "R", "CGC" => "R", "CGG" => "R", "CGT" => "R", "CTA" => "L", "CTC" => "L", "CTG" => "L", \
                           "CTT" => "L", "GAA" => "E", "GAC" => "D", "GAG" => "E", "GAT" => "D", "GCA" => "A", "GCC" => "A", "GCG" => "A", \
                           "GCT" => "A", "GGA" => "G", "GGC" => "G", "GGG" => "G", "GGT" => "G", "GTA" => "V", "GTC" => "V", "GTG" => "V", \
                           "GTT" => "V", "TAA" => "-", "TAC" => "Y", "TAG" => "-", "TAT" => "Y", "TCA" => "S", "TCC" => "S", "TCG" => "S", \
                           "TCT" => "S", "TGA" => "-", "TGC" => "C", "TGG" => "W", "TGT" => "C", "TTA" => "L", "TTC" => "F", "TTG" => "L", "TTT" => "F") ;\
                           foreach $s (@seq) { print "$s\n";  for ($x = 0 ; $x < length ( $dna{$s}) ; $x=$x+180) { $line = substr  ( $dna{$s},$x,180 ) ;\
                           for ( $y = 0; $y < length ( $line ) ; $y = $y+3 ) { $triplet = substr($line,$y,3) ; \
                           $base2aa{$triplet} = "X" unless defined $base2aa{$triplet}; \
                           print $base2aa{$triplet} unless $base2aa{$triplet} eq "-";} print "\n"}}}' > source/$accession.proteins.fsa &
             end
             wait
# 
#          6. To verify the the output try and view the first part of one of the output files. (use space bar to scroll, q to quit)
# 
             less source/AP008232.proteins.fsa
# 
#    4.
#       VISUALIZING THE CODON USAGE
#       We have now extracted all gene sequences using MySQL, generated the complement for all genes located on the reverse strand, and made translations of all DNA sequences into amino acid sequences. Well done!
# 
#       Codon Usage Plots can assist as an important tool for comparative genomics and as mentioned in lecture M10, it can also reveal taxonomic relation. By counting occurences of amino acids and nucleotides ( as triplets ) you can get an overview of the distribution within the individual organsims. This part of the exercise will demonstrate how to generate these statistics.
# 
#          1. First, you will need to generate a summary file containing the counts of the codons
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
                perl /home/people/pfh/scripts/codon-usage/codon-usage.pl < source/$accession.orfs.fsa > source/$accession.codon-usage.triplets.list
             end
# 
#          2. Try and view the file you just made:
# 
             less source/AP008232.codon-usage.triplets.list
# 
#             The following code will execute the 'roseplot' Perl script using the list of summary you just created. Note, that the title of the chart is generated by looking up in your MySQL table
#          3.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
                set title = `mysql -B -N -e "select organism from cmp_genomics.genomes where user=USER() and accession='$accession'"`
                cat source/$accession.codon-usage.triplets.list  | \
                /home/people/pfh/scripts/roseplot/roseplot.pl -axistitle "Frequency" -fcolor red -steps 4 -T "Codon Usage" -ST "$title" -output $accession.codon-usage.triplets.ps
             end
# 
#             Try and examine the rose plots by using 'ghostview'. For doing this - AND ONLY THIS - it is recommended to start a new seperate SSH session and INSTEAD OF LOGGING IN TO 'IBIOLOGY', LOG IN TO 'ORGANISM'! Go to the Exercise directory and type 'ghostview <FILENAME>'. Once you see the roseplot, please press CTRL-R to redraw the figure if it does not show up correctly. You will have to use the command 'ls' to see which files have been generated. Can you see differences?
#    5.
#       CODING REDUNDANCY
#       We are going to look further at the triplets encoding the amino acids of Leucine ( Leu ) and Arginine ( Arg ). Try and see if these codon usages make sense in comparison with the global AT content.
# 
#          1. Find codons encoding Arginine in W. glossinidia
# 
             cat source/BA000021.codon-usage.triplets.list |grep Arg
# 
#          2. Looking at Leucine...:
# 
             cat source/BA000021.codon-usage.triplets.list |grep Leu
# 
#       Comment on these findings and spend a few minutes looking for these or other redundant codons in other genomes. Do you see a pattern?
#    6.
#       VISUALIZING NUCLEOTIDE BIAS WITHIN CODONS
#       Above, you may have discovered a relationship between the global A+T content of the organism and its adaption to the A+T content by using different codons. One way to visualize this effect is by using Shannon Information Content and so-called Logo plots. Next, you will generate 8 PostScript files containing the Logos of Shannon Information. The height of stacked bares corrospond to the total information content, and the height of each base is related to the frequency of that base. Try and examine the plots. Are there specific positions within the codon with stronger preference?
#          1. Generate fasta file of all triplets
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             cat source/$accession.orfs.fsa | /home/people/pfh/scripts/codon-usage/logo.pl > source/$accession.codon-usage.logo.fsa &
             end
             wait
# 
#          2. Generate Logo
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             rsh -n organism "cat /home/people/$user/Ex3/source/$accession.codon-usage.logo.fsa | fasta2how -g | gethow -r 0 | logo -barbits 0.5 -bitinc 0.1 -r - -l Ex3/$accession.codon-usage.logo.ps" &
             end
             wait
# 
#             Inspect the output PostScript by using ghostview just like you did before. Does the output show what you would expect? 
#    7.
#       VISUALIZING THE AMINO ACID USAGE
#          1. First, you will need to generate a summary file containing the counts of the individual aminoacids
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
                perl /home/people/pfh/scripts/codon-usage/aausage2.pl < source/$accession.proteins.fsa > source/$accession.codon-usage.aa.list
             end
# 
#          2. Try and view the file you just made:
# 
             less source/AP008232.codon-usage.aa.list
# 
#             The following code will execute the 'roseplot' Perl script using the list of summary you just created. Note, that the title of the chart is generated by looking up in your MySQL table
#          3.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
                set title = `mysql -B -N -e "select organism from cmp_genomics.genomes where user=USER() and accession='$accession'"`
                cat source/$accession.codon-usage.aa.list | \
                /home/people/pfh/scripts/roseplot/roseplot.pl -labelFontSize 15  -axistitle "Frequency" -fcolor blue -Xcol 2 -Ycol 4 -steps 1 -T "Amino Acid Usage" -ST "$title" -output $accession.codon-usage.aa.ps
             end
# 
#             You can view the PostScript by using 'ghostview' just like you did before 
#    8.
#       AT CONTENT PROFILES
#       As the last part of this exercise, we are going to build so-called AT content profiles. The profiles visualizes the AT content in a +/- 400bp window around predicted / annotated gene start. In most cases the upstream region displays an elevated AT contant which allows the DNA helix to meld more easily.
#          1. First of all, we will need the gene predictions you have made and assembly them into an annotation-
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             mysql -B -D cmp_genomics -N -e "select featuretype,start,stop,dir,'CDS' from features where accession='$accession' and user=USER()"\
              > source/$accession.ann &
             end
             wait
             less source/AP009048.ann
# 
#          2. Print a file of integers: 1 of A or T, 0 of G or C and inspect the output. This file has a line for every base pair of the genome.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             mysql -B -D cmp_genomics -N -e "select seq from genomes where accession='$accession' and user=USER()" | \
                perl -ne 'chomp ( $dna =  $_ ) ; for ( $x = 0 ; $x < length ( $dna ) ; $x++ ) { $n = substr($dna,$x,1) ; $n =~ tr/ATGC/1100/;print "$n\n"}'\
                 > source/$accession.ATcontentBits &
             end
             wait
             less source/AP009048.ATcontentBits
# 
#          3. Print a file of integers: 1 of A or T, 0 of G or C. ( and inspect output ) The rows correspond to
#             the genes predicted and the columns correspond to the position relative to translation start.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             /home/people/pfh/scripts/profile/profile.pl -E "rRNA,tRNA,CDS" -ann source/$accession.ann -min 400 -max 400 -datafile source/$accession.ATcontentBits\
                > source/$accession.ATcontentBits.400profile400.data &
             end
             wait
             less source/AP009048.ATcontentBits.400profile400.data
# 
#          4. Generate global average and standard deviation, and calculate Z-scores of the individual position (+/- 400 bp )
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             gawk '(NF >= 1){a++; b += $NF; c += $NF^2}END{print (a > 0 ? b/a : 0) "\t" (a > 1 ? sqrt((c-b^2/a)/(a-1)) : 1E99);}' source/$accession.ATcontentBits\
                > source/$accession.ATcontentBits.stddev &
             end
             wait
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             cat source/$accession.ATcontentBits.400profile400.data \
                | /home/people/pfh/scripts/profile/profile.avg.pl `gawk '{print $1}' < source/$accession.ATcontentBits.stddev` `gawk '{print $2}' < source/$accession.ATcontentBits.stddev` \
                > source/$accession.ATcontentBits.400profile400.poszscores &
             end
             wait
             less source/AP009048.ATcontentBits.400profile400.poszscores
# 
#             Extract the two columns 1 and 4 containing X and Y values. We are using a small R-script, profile.R which takes care of the plotting. Note, that the script contains no references to the accession numbers you are worksing with. We pipe the R-script into the program 'sed' which will replace the word 'ACCESSION' with the actual accession number you want to generate the plot for.
#          5.
# 
             foreach accession (AE017042 AE016879 AL111168 AL645882 AP008232 AP009048 BA000021 CP000034)
             gawk '{print $1}' source/$accession.ATcontentBits.400profile400.poszscores > source/$accession.ATcontentBits.400profile400.poszscores.x
             gawk '{print $4}' source/$accession.ATcontentBits.400profile400.poszscores > source/$accession.ATcontentBits.400profile400.poszscores.y
             cat /home/people/kiil/Exercises/files/profile.R | sed "s/ACCESSION/$accession/g" | R --vanilla -q
             end
# 
#       You can view the output using 'ghostview' like you have done previously. Examine differences between the plots, and give explanations as to the shape and extension of the variation of AT-content downstream of the translation.
# 
# 
# This file is last modified Thursday 5th of October 2006 13:15:19 GMT
