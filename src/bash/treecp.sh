#!/bin/bash
function treecp {
    if [ "${#}" != 2 ] ; then
        echo "Usage: treecp source destination";
        return 1;
    else
        tar cf - "${1}" | (cd "${2}" ; tar xpf -) ; 
    fi ;
};

treecp
