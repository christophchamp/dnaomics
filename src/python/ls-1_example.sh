#!/bin/bash
ls -l | python -c "import sys;[sys.stdout.write(' '.join([line.split(' ')[-1]])) for line in sys.stdin]"
