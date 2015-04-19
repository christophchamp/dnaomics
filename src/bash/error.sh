#!/bin/bash

function error
{
    echo -e "\033[1;31m${1}\033[0m" 1>&2
}

error "ERROR: please run this program as 'root'"
