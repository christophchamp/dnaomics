import numpy as np

def min_gt(seq, val):
    """
    Return smallest item in seq for which item > val applies.
    None is returned if seq was empty or all items in seq were <= val.

    >>> min_gt([1, 3, 6, 7], 4)
    6
    >>> min_gt([2, 4, 7, 11], 5)
    7
    """
    ## FIXME: There could be trouble with NaNs
    for v in np.sort(seq):
        if v > val:
            return v
    return None

#def min_gt(seq, val):
#    return min(v for v in seq if v > val)

def min_ge(seq, val):
    """
    Same as min_gt() except items equal to val are accepted as well.

    >>> min_ge([1, 3, 6, 7], 6)
    6
    >>> min_ge([2, 3, 4, 8], 8)
    8
    """

    for v in seq:
        if v >= val:
            return v
    return None

def max_lt(seq, val):
    """
    Return greatest item in seq for which item < val applies.
    None is returned if seq was empty or all items in seq were >= val.

    >>> max_lt([3, 6, 7, 11], 10)
    7
    >>> max_lt((5, 9, 12, 13), 12)
    9
    """

    idx = len(seq)-1
    while idx >= 0:
        if seq[idx] < val:
            return seq[idx]
        idx -= 1
    return None

def max_le(seq, val):
    """
    Same as max_lt(), but items in seq equal to val apply as well.

    >>> max_le([2, 3, 7, 11], 10)
    7
    >>> max_le((1, 3, 6, 11), 6)
    6
    """

    idx = len(seq)-1
    while idx >= 0:
        if seq[idx] <= val:
            return seq[idx]
        idx -= 1

    return None

"""

def min_ge(seq, val):
    return min([v for v in seq if v >= val])

def max_lt(seq, val):
    return max([v for v in seq if v < val])

def max_le(seq, val):
    return max(v for v in seq if v <= val)

http://code.activestate.com/recipes/415233-getting-minmax-in-a-sequence-greaterless-than-some/
"""
