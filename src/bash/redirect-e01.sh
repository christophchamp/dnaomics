#!/bin/bash

exec >  >(tee -a foo.log)
exec 2> >(tee -a foo.log >&2)

echo "foo"
echo "bar" >&2

# exec > >(tee -a $LOG) trap "kill -9 $! 2>/dev/null" EXIT exec 2> >(tee -a $LOG >&2) trap "kill -9 $! 2>/dev/null" EXIT
