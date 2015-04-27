#!/usr/bin/env python
# ALSO: `perl -e 'print crypt($ARGV[0], "password")' 'mypassword'`
import crypt
import sys
import random

saltchars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

def salt():
    return random.choice(saltchars) + random.choice(saltchars)

def hash(plain):
    return crypt.crypt(arg, salt())

if __name__ == "__main__":
    random.seed()
    for arg in sys.argv[1:]:
        sys.stdout.write("%s\n" % (hash(arg),))
