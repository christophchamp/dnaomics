# Bioinformatics using Unix commands
# Awksome programing languages (awk, nawk & gawk)
# awk, nawk & gawk are different versions of the same programming language, and are very similar. It is recommended to use gawk or nawk , rather than the original version: awk, since they are more stable and have more features!
# Basically gawk will read a file and do something with each line.
# Examples of using gawk:
 gawk '{print $1}' epitope2protein.HLA-D_m13.out      Print first field in file
# 
 gawk '{print $1, $3}' epitope2protein.HLA-D_m13.out      Print first and third field in file
# 
 cat epitope2protein.HLA-D_m13.out|gawk '{print $1}'      Print first field in file getting data from standard input
# 
 cat epitope2protein.HLA-D_m13.out|gawk '{if (/NP/) {print $1}}'      Print first field in lines containing "NP"
# 
 cat epitope2protein.HLA-D_m13.out|gawk '{if (/^NP/) {print $1}}'      Print first field in lines starting with "NP"
# 
 gawk '{print substr($7,2,5)}' epitope2protein.HLA-D_m13.out      Print five characters of the seventh column, starting with the second letter (in the seventh column). NB awk numbers strings from 1, but perl number them from 0!
# 
 gawk '{print substr($7,length($7)-3,4)}' epitope2protein.HLA-D_m13.out      Print last four letters in seventh column
# 
 echo "Mary had a little lamb" |gawk '{line = $0; gsub (" ","",line);print line}'      Remove all spaces in all lines
# 
 gawk -v name=Mary -v animal=lamb '{print name,$1,animal}' epitope2protein.HLA-D_m13.out      Passing variables to gawk
# 
 gawk -F "\t" '{print $1}' epitope2protein.HLA-D_m13.out      Split only input on tabulators (rather than on any whitespace as is the default)
# 
# head epitope2protein.HLA-D_m13.out | gawk 'BEGIN{print "Here comes the data"}{print $1}END{print "No more data"}'      statements in BEGIN{} and END{} are executed before and after the data are read, respectively
# 
# 
# 
# A more complex example: You have a file called epitope2protein.HLA-D_m13.out with a protein sequence in the 7th column and the residuenumber in the sequence where an epitope starts in the third column. You want to print out the sequence surounding the start of the epitope (in this case the first five resigues of the epitope and the four residues before the epitope) in a format that can be read by the sequence motif visualization program logo. The first line in the output must be "* Aligned protein sequences.", and each sequence motif must be followed by a ".". Furthermore only motifs that are nine amino acids long must be printed. This is what the command can look like:
 cat epitope2protein.HLA-D_m13.out | gawk 'BEGIN{print "* Aligned protein sequences."}{s=substr($7,$3-4,9);if (length(s)==9){print s"."}}' | /home/projects/projects/vaccine/bin/logo2 -p - | gawk 'BEGIN{pr=0}{if (/\%\!PS-Adobe-3.0 EPSF-3.0/){pr=1} if(pr==1){print $0}}' > logo.ps
# 
# 
# Search and replace - sed
# the sed program is a command line programe that corresponds to the search and replace function in for example word. As the following examples show it can do some more advanced replacements.
# Example of using sed: echo "Mary had a little lamb" | sed 's/little/big/g'      Replace (s=substitute) little by big (g=global, i.e replace all)
 echo "Mary had a little lamb" | sed 's/\(.*\)lamb/\1goat/g'      Print everything before lamb followed by goat. what is matched by \(.*\) is put in the variable \1. "." means any character, and "*" means repeated zero or more times
 echo "Mary had a little lamb. John had a little goat" | sed 's/\([A-Za-z]*\) had a little \([A-Za-z]*\)/The \2 is owned by \1/g'      Try that with your old search and replace
# 
# 
# 
# Sort file - sort
# Example of using sort: Getting a test filecp /usr/opt/www/pub/CBS/researchgroups/immunology/intro/Unix/test.out .      sort myfile      Sort file
 sort -n test.out      sort file numerically
 sort -n -k3 test.out      sort file numerically (big numbers last) after 3rd column
 sort -r -n -k3 test.out      sort file reverse numerically after 3rd column
 sort -u pdb.mhc.spnam      Keep only one copy of each unique line
 sort pdb.mhc.spnam|uniq -c      Count the number of each unique line
# 
# 
# Execute a string
# putting `` around a command makes a unix execute the command corresponding to the string: echo pwd
#       print the string pwd
 `echo pwd`
#       execute the command pwd
 echo pwd|sh
#       echo the string pwd to the shell - which will then execute it
# This will be used in the next example.
# 
# Do something with many variables - foreach
# Example 1: print each entry in list to screen
# 
 foreach entry (a b c)
       echo $entry
 end
# 
# Example 2: get each swissprot entry from list and print it
# 
 foreach entry (`gawk '{print $1}' test.out`)
       echo $entry |sed 's/.*|//'| xargs getsprot
 end
# 
# NB:echo ENV_HV1H2| xargs getsprot      is the same as getsprot ENV_HV1H2     
# 
# Warning: the string within () is limited to a few thousand charectors
# 
# Contatinate side by side - paste
# Example:
 paste pdb.mhc.nam pdb.mhc.spnam     
# 
# 
# Get lines matching a patern - grep/egrep
# Example:
 grep 1A68_HUMAN pdb.mhc.spnam      Get lines with "1A68_HUMAN"
 grep -v HUMAN pdb.mhc.spnam      Get lines that do not contain "HUMAN"
 grep _HUMAN pdb.mhc.spnam      Get lines with "_HUMAN" (Human swiss prot sequences)
 grep ^KA pdb.mhc.spnam      Get lines starting with "KA" (Human swiss prot sequences)
 grep "^KA.*MOUSE" pdb.mhc.spnam      Get lines matching "KA" - something ("." is a wildcard; "*" means repeated zero or more times) - "MOUSE"
