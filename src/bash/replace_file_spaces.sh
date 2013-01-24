#!/bin/bash
## Replace all spaces in filenames with '_'
for i in *; do mv "$i" `echo "$i"|tr " " "_"`; done
