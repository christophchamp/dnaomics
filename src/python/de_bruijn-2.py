text = "asdftommarvoloriddle"
patt = "iamlordvoldemort"

def perm_matching_dict(text, patt):
    m = len(text)
    n = len(patt)

    cfreq = {}
    for ch in patt:
        try:
            cfreq[ch] += 1
        except KeyError:
            cfreq[ch] = 1
            pass

    # Matching with string
    idx = 0
    while idx < m:
        if idx + n > m:
            break
        curr = text[idx: idx+n]
        _freq = {}
        for ch in curr:
            try:
                _freq[ch] += 1
            except KeyError:
                _freq[ch] = 1
                pass

        is_matched = True
        for k,v in cfreq.iteritems():
            if k not in _freq or _freq[k] != v:
                print k,v
                is_matched = False
                break
        if is_matched:
            print 'match at', idx
        idx += 1

def perm_matching_xor(text, patt):
    """
    Very efficient substring permutation matching algorithm
    Keeps only one extra variable
    But can lead to false positive if 'patt' contains duplicate
    """
    m = len(text)
    n = len(patt)

    patt_sum = 0
    for ch in patt:
        patt_sum ^=ord(ch)

    # Perform the initial case
    idx = 0
    curr_sum = patt_sum
    curr_text = text[:n]
    for ch in curr_text:
        curr_sum ^= ord(ch)
    if curr_sum == 0:
        print 'match at', idx

    # Loop over the rest
    curr_sum ^= ord(text[0])
    for idx,ch in enumerate(text[1:-n+1]):
        curr_sum ^= ord(text[idx+n]) # idx is one less that of 'text'
        if curr_sum == 0:
            print 'match at', idx, '(but can be false positive)'
        curr_sum ^= ord(ch)

perm_matching_dict(text, patt)
perm_matching_xor(text, patt)
