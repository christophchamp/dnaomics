#!/usr/bin/env python
# vim: tabstop=4 shiftwidth=4 softtabstop=4

import inspect
import os
import re
import sys

import pep8

DOCSTRING_TRIPLE = ['"""', "'''"]
#Add a few openstack style pep257 docstring tests.


def pep257_docstring_start_space(physical_line):
    """Check for docstring not start with space.
    P257
    """
    pos = max([physical_line.find(i) for i in DOCSTRING_TRIPLE])  # start
    if (pos != -1 and len(physical_line) > pos + 1):
        if (physical_line[pos + 3] == ' '):
            return (pos, "PEP P257: one line docstring should not start with"
                    " a space")


def pep257_docstring_one_line(physical_line):
    """Check one line docstring end.
    P257
    """
    pos = max([physical_line.find(i) for i in DOCSTRING_TRIPLE])  # start
    end = max([physical_line[-4:-1] == i for i in DOCSTRING_TRIPLE])  # end
    if (pos != -1 and end and len(physical_line) > pos + 4):
        if (physical_line[-5] != '.'):
            return pos, "PEP P257: one line docstring needs a period"


def pep257_docstring_multiline_end(physical_line):
    """Check multiline docstring end.
    P257
    """
    pos = max([physical_line.find(i) for i in DOCSTRING_TRIPLE])  # start
    if (pos != -1 and len(physical_line) == pos):
        if (physical_line[pos + 3] == ' '):
            return (pos, "PEP P257: multi line docstring end on new line")


def add_pep257():
    for name, function in globals().items():
        if not inspect.isfunction(function):
            continue
        args = inspect.getargspec(function)[0]
        if args and name.startswith("pep257"):
            exec("pep8.%s = %s" % (name, name))


if __name__ == "__main__":
    sys.path.append(os.getcwd())
    pep8.ERRORCODE_REGEX = re.compile(r'[EWP]\d{3}')
    add_pep257()

    try:
        pep8._main()
        sys.exit(False)
    except Exception, e:
        print e
