# Required reading
# Here is an excerpt from 'man perlrun' about the important command line switches used when doing perl one-liners.
# 
#      -a   turns on autosplit mode when used with a -n or -p.  An implicit
#           split command to the @F array is done as the first thing inside the
#           implicit while loop produced by the -n or -p.
                perl -ane 'print pop(@F), "\n";'
#           is equivalent to
               while (<>) {
                   @F = split(' ');
                   print pop(@F), "\n";
               }
#           An alternate delimiter may be specified using -F.
# 
#      -e commandline
#           may be used to enter one line of script.  If -e is given, Perl will
#           not look for a script filename in the argument list.  Multiple -e
#           commands may be given to build up a multi-line script.  Make sure to
#           use semicolons where you would in a normal program.
# 
#      -n   causes Perl to assume the following loop around your script, which
#           makes it iterate over filename arguments somewhat like sed -n or
#           awk:
#               while (<>) {
#                   ...             # your script goes here
#               }
#           Note that the lines are not printed by default.  See -p to have
#           lines printed.  If a file named by an argument cannot be opened for
#           some reason, Perl warns you about it, and moves on to the next file.
# 
#      -p   causes Perl to assume the following loop around your script, which
#           makes it iterate over filename arguments somewhat like sed:
#               while (<>) {
#                   ...             # your script goes here
#               } continue {
#                   print or die "-p destination: $!\n";
#               }
#           If a file named by an argument cannot be opened for some reason,
#           Perl warns you about it, and moves on to the next file.  Note that
#           the lines are printed automatically.  An error occuring during
#           printing is treated as fatal.  To suppress printing use the -n
#           switch.  A -p overrides a -n switch.
# 
# 
# Examples
# 
       perl -pe 'tr/ATCG/TAGC/' dna7.fsa
#           # This complements every base in the file dna7.fsa.
# 
       perl -ane 'print $F[0] + $F[3], "\n"' datafile
#           # Add first and forth columns
# 
       perl -ne 'print if 15 .. 17' *.pod
#           # Just lines 15 to 17
# 
       perl -ne '$counter++; END { print "$counter lines"; }' datafile
#           # Count lines
# 
       perl -ne 'BEGIN{ $/=">"; } if(/^\s*(\S+)/){ open(F,">$1.fsa")||warn"$1 write failed:$!\n";chomp;print F ">", $_ }'
#           # Split a multi-sequence FASTA file into individual files
# 
# Subjects covered
# How to call perl from the unix command line in order to perform a simple task, typically a text conversion. Also you will have the opportunity to see the assignments from last years exam. 
