#!/bin/bash

set -eu

function run_pep_test {
  echo "Running PEP8 and PEP257 checks ...."
  echo ""
  srcfiles=`find * -type f -name "*.py" | grep -v "^env"`
  pep8_opts="--ignore=E202,W602 --repeat"
  python pep8_test.py ${pep8_opts} ${srcfiles}
}


run_pep_test
