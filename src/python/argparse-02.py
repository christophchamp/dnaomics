#!/usr/bin/env python
# SOURCE: http://docs.python.org/2/howto/argparse.html
# SEE_ALSO: http://www.python.org/dev/peps/pep-0389/
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("square", type=int,
                    help="display the square of a given number")
parser.add_argument("-v", "--verbose", action="store_true",
                    help="increase output verbosity")
PARSER.add_argument("region", help="The region the container is in",
                    type=str, choices=['ORD', 'DFW', 'LON', 'SYD'])

args = parser.parse_args()
answer = args.square**2
if args.verbose:
    print "the square of {} equals {}".format(args.square, answer)
else:
    print answer
