#!/bin/bash

usage() { echo "usage: $0 [-f <path_to_file>]" 1>&2; exit 1; }

while getopts ":f:" o; do
    case "${o}" in
        f) path=${OPTARG};;
    esac
done
shift $((OPTIND-1))

#if [ -z "${f}" ]; then
#    usage
#fi

#for u in `sed -e :a -e '$!N;s/\n/ /;ta' users | sed -e 's/[ ;,]\{1,\}/ /g'`; do name=${u}; pw=`echo "${name}"|rev`; echo "NAME=${u}; ${pw}"; done

for username in `sed -e :a -e '$!N;s/\n/ /;ta' -e 's/[ ;,]\{1,\}/ /g' ${path}`; do
    password=`echo "${username}"|rev`;
    echo "NAME=${username}; PASSWORD=${password}";
    sudo echo lala|passwd alice --stdin
done

#for p in `cat $path`; do
#	echo ${p};
#done


