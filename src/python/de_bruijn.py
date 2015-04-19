def de_bruijn(k, n):
    import operator
    """
    De Bruijn sequence for alphabet k
    and subsequences of length n.
    """
    try:
        # let's see if k can be cast to an integer;
        # if so, make our alphabet a list
        _ = int(k)
        alphabet = list(range(k))
        alphabet = map(str, alphabet)
 
    except (ValueError, TypeError):
        alphabet = k
        k = len(k)
 
    a = [0] * k * n
    sequence = []
    def db(t, p):
        if t > n:
            if n % p == 0:
                for j in range(1, p + 1):
                    sequence.append(a[j])
        else:
            a[t] = a[t - p]
            db(t + 1, p)
            for j in range(a[t - p] + 1, k):
                a[t] = j
                db(t + 1, t)
    db(1, 1)
    return "".join(map(alphabet.__getitem__, sequence))
 
print(de_bruijn(2, 3))
print(de_bruijn("abcd", 2))