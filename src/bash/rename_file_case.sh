#!/bin/bash

# Python: `python -c 'import os, sys; [os.rename(a, a.upper()) for a in sys.argv[1:]]' *`
# Perl: `perl -e 'for(@ARGV){rename$_,uc}' *.jpg`
# Awk: # Insecure if the filenames contain an apostrophe or newline!
#      # `eval "$(ls -- *.jpg | awk '{print"mv -- \x27"$0"\x27 \x27"toupper($0)"\x27"}')"`
# tr (SEE): http://www.gnu.org/software/bash/manual/html_node/Shell-Parameter-Expansion.html
# tr (std): `for f in * ; do mv -- "$f" "$(tr [:lower:] [:upper:] <<< "$f")" ; done`
# tr (uppercase): `for file in *; do mv -- "$file" "${file^^}"; done`
# tr (lowercase): `for file in *; do mv -- "$file" "${file,,}"; done`
# tr (not safe): `ls *.txt | tr '[a-z]' '[A-Z]' | sort | uniq -c | sort -n`

usage() { echo "usage: $0 [-c <case_change_value>]" 1>&2; exit 1; }

while getopts ":c:" o; do
    case "${o}" in
        c) case_change=${OPTARG};;
    esac
done
shift $((OPTIND-1))

#if [ -z "${case_change}" ]; then
#    usage
#fi

old_dir=`pwd`
path=/home/champ/tmp/bbg

if [ ${case_change} == "upper" ]; then
    case_from="lower";
    case_to="upper";
elif [ ${case_change} == "lower" ]; then
    case_from="upper";
    case_to="lower";
else
    echo "Unrecognize case change value";
    exit 1;
fi

cd $path
echo -e "Changing case of files from ${case_from} to ${case_to}\n"
for f in * ; do
    mv -- "$f" "$(tr [:${case_from}:] [:${case_to}:] <<< "$f")";
done
cd $old_dir
echo -e "Done!\n"
