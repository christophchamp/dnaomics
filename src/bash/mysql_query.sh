#!/bin/bash

set -e

function run_query {
    mysql -B -N -e "$1"
}

SQL=$(run_query "SELECT * FROM sandbox.foo;")

echo -e "$SQL\n"
