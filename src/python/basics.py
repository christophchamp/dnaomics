## DESCRIPTION: A random collection of misc python coding examples
## LAST UPDATE: 2013-01-17

# Getting around leading zeros as octal values
print "%05d" % int('02132')

# Multi-line regex
# You could tell the regex to stop matching at the next line that starts with
# A: (or at the end of the string):
re.findall(r'A:(.*?)\nB:(.*?)\n(.*?)(?=^A:|\Z)',
    f.read(), re.DOTALL | re.MULTILINE)

string = "Cape Town 1234"
comma_string = re.sub(r"(.+?)\s*(\d+)", r"\1,\2", string)
# "Cape Town,1234"

def min_max(t): return min(t), max(t)  # in/out a tuple
def printall(*args):
    """Gather the arguments into a tuple
    gather: The operation of assembling a variable-length argument tuple.
    scatter: The operation of treating a sequence as a list of arguments.
    """
    print args  # printall(1, 2.0, '3')
def pointless(required, optional=0, *args):
    # >>> pointless(1,2,3,4,5,6,7)
    # 1 2 (3, 4, 5, 6, 7)
    print required, optional, args

t = (7, 3)
divmod(t)  # BAD!
divmod(*t)  # GOOD ("scatter" tuple in arguments)
divmod(*(7, 3))

t = ['a', 'b', 'c']  # or: t = ('a','b','c')
s = "{}.{}.{}".format(*t)
'.'.join(t)  # <- Best way

# zip
s = 'abc'
t = [0 ,1, 2]
zip(s,t)  # [('a', 0), ('b', 1), ('c', 2)]
for letter, number in zip('abcd', '123'): print number, letter
[ (number, letter) for letter, number in zip('abcd', '123') ]
for index, element in enumerate('abc'): print index, element

def has_match(t1, t2):
    # has_match('abc','ade') -> True
    for x, y in zip(t1, t2):
        if x == y:
            return True
    return False

#==============================================================================
l = [[2], [3], [2, 2], [5], [2], [3], [7], [2, 2, 2], [3, 3], [2], [5], [11],
     [2, 2], [3], [13], [2], [7], [3], [5], [2, 2, 2, 2], [17], [2], [3, 3],
     [19], [2, 2], [5]]

from collections import defaultdict

my_dict = defaultdict(list)

for ele in l:
    if len(my_dict[ele[0]]) < len(ele):
        my_dict[ele[0]] = ele
my_dict.values() # [[2, 2, 2, 2], [3, 3], [5], [7], [11], [13], [17], [19]]
#==============================================================================
message="""Dear {recipient},
 
I wish you to leave Sunnydale and never return.
 
Not Quite Love,
{sender}
"""
print(message.format(sender='Christoph Champ', recipient='John Doe'))
#==============================================================================

# default return value
return 1 if x > y else 0

def area(radius):
    return math.pi * radius**2

def distance(x1, y1, x2, y2):
    dx = x2 - x1
    dy = y2 - y1
    dsquared = dx**2 + dy**2
    return  math.sqrt(dsquared)

def circle_area(xc, yc, xp, yp):
    return area(distance(xc, yc, xp, yp))

def is_divisible(x, y):
    return x % y == 0

def factorial(n):
    """0! = 1; n! = n(n-1)!"""
    if n == 0: return 1
    else:
        recurse = factorial(n-1)
        result = n * recurse
        return result

def factorial (n):
    if not isinstance(n, int):
        # First "guardian" (i.e., a programming pattern that uses a conditional
        # statement to check for and handle circumstances that might cause an
        # error.
        print 'Factorial is only defined for integers.'
        return None
    elif n < 0:
        # 2nd guardian
        print 'Factorial is only defined for positive integers.'
        return None
    elif n == 0:
        return 1
    else:
        return n * factorial(n-1)

def factorial(n):
    """Debugging with scaffolding (i.e., Code that is used during program
    development but is not part of the final version.)"""
    space = ' ' * (4 * n)
    print space, 'factorial', n
    if n == 0:
        print space, 'returning 1'
        return 1
    else:
        recurse = factorial(n-1)
        result = n * recurse
        print space, 'returning', result
        return result

def fibonacci(n):
    """Algorithm:
    fibonacci(0) = 0
    fibonacci(1) = 1
    fibonacci(n) = fibonacci(n-1) + fibonacci(n-2);
    """
    if n == 0: return 0
    elif n == 1: return 1
    else: return fibonacci(n-1) + fibonacci(n-2)

# to find the product of n and 9, you can write n − 1 as the first digit and 10 − n as the second digit

# Find sqrt(n)
epsilon = 0.0000001
while True:
    print x
    y = (x + a/x) / 2
    if abs(y-x) < epsilon:
        break
    x = y

def ispalindrome(word):
    if len(word) < 2: return True
    if word[0] != word[-1]: return False
    return ispalindrome(word[1:-1])
def ispalindrome(word):
    return word == word[::-1]
def ispalindrome(s):
    return len(s) < 2 or s[0] == s[-1] and ispalindrome(s[1:-1])
def is_palindrome(word):
    i = 0
    j = len(word)-1
    while i<j:
        if word[i] != word[j]:
            return False
        i = i+1
        j = j-1
    return True
def is_palindrome(word):
    return is_reverse(word, word)

def find(word, letter):
    """same as method word.find(letter,index_start,index_stop)"""
    index = 0
    while index < len(word):
        if word[index] == letter:
            return index
        index = index + 1
    return -1

# String comparisons
print "True" if 'zed' < 'bed' else "False" # False alphabetical order test
print "True" if 'Zed' < 'bed' else "False" # True (note the caps!)

## 'and'/'&&' operator usage examples:
if (self.a != 0) and (self.b != 0) :

#"&" is the bit wise operator and does not suit for boolean operations.
# The equivalent of "&&" is "and" in Python.
#
#A shorter way to check what you want is to use the "in" operator:

if O not in (self.a, self.b) :

# You can check if anything is part of a an iterable with "in", it works for:
#
#    Tuples. I.E : "foo" in ("foo", 1, c, etc) will return true
#    Lists. I.E : "foo" in ["foo", 1, c, etc] will return true
#    Strings. I.E : "a" in "ago" will return true
#    Dict. I.E : "foo" in {"foo" : "bar"} will return true

# Yes, using "in" is slower since you are creating an Tuple object, but really
# performances are not an issue here, plus readability matters a lot in Python.

#For the triangle check, it's easier to read :
O not in (self.a, self.b, self.c)
#Than
(self.a != 0) and (self.b != 0) and (self.c != 0) 
#It's easier to refactor too.

#==============================================================================
#==Bit-wise operations==
# TODO(PCC): Provide actual Python example code
#100 >> 3 = 12
#0110 0100 >> 3 = 0000 1100
##E.g.,
#100
#128 64 32 16 8 4 2 1
#  0  1  1  0 0 1 0 0 = 100
#100 >> 1
#128 64 32 16 8 4 2 1
#  0  0  1  1 0 0 1 0  = 50
#100 >> 2
#128 64 32 16 8 4 2 1
#  0  0 0  1  1 0 0 1  = 25
#100 >> 3
#128 64 32 16 8 4 2 1
#  0  0  0  0 1 1 0 0  = 12

# Example of using operator.itemgetter() to retrieve specific fields from a
# tuple record:
>>> inventory = [('apple', 3), ('banana', 2), ('pear', 5), ('orange', 1)]
>>> getcount = itemgetter(1)
>>> map(getcount, inventory)
[3, 2, 5, 1]
>>> sorted(inventory, key=getcount)
[('orange', 1), ('banana', 2), ('apple', 3), ('pear', 5)]

#== Dictionaries ==
def histogram(s):
    d = dict()
    for c in s:
        if c not in d:
            d[c] = 1
        else:
            d[c] += 1
    return d
h = histogram('brontosaurus')

def print_hist(h):
    for c in h:
        print c, h[c]

#=== Reverse lookup ===
def reverse_lookup(d, v):
    for k in d:
        if d[k] == v:
            return k
    #raise ValueError
    raise ValueError, 'value does not appear in the dictionary'

## Here is a function that inverts a dictionary:
def invert_dict(d):
    """Invert a dictionary. E.g.,
    >>> hist = histogram('parrot')
    >>> print hist
    {'a': 1, 'p': 1, 'r': 2, 't': 1, 'o': 1}
    >>> inv = invert_dict(hist)
    >>> print inv
    {1: ['a', 'p', 't', 'o'], 2: ['r']}
    """
    inv = dict()
    for key in d:
        val = d[key]
        if val not in inv:
            inv[val] = [key]
        else:
            inv[val].append(key)
    return inv

d = dict()
bad = [1,2,3] # lists can't be used as keys
good = (1,2,3) # tuples can
d[bad] = 'BAD'
d[good] = 'GOOD'

## Memoization
known = {0:0, 1:1}
def fibonacci(n):
    if n in known:
        return known[n]
    res = fibonacci(n-1) + fibonacci(n-2)
    known[n] = res
    return res

## Dictionary and set comprehensions
{ i: i*2 for i in range(3)}

'{:20,d}'.format(18446744073709551616) # '18,446,744,073,709,551,616'

# Tuples + Dictionaries
t = [('a', 0), ('c', 2), ('b', 1)]
d = dict(t) # {'a': 0, 'b': 1, 'c': 2}
d = dict(zip('abc', range(3))) # {'a': 0, 'b': 1, 'c': 2}

def sort_by_length(words):
    """Sort a list of words from longest to shortest
    sort_by_length(['one','three','four','asdfged'])"""
    t = []
    for word in words:
        t.append((len(word), word))
    t.sort(reverse=True)
    res = []
    for length, word in t:
        res.append(word)
    return res
## *** Think Python page 152 ***

##==Random==
import random
for i in range(10):
    print random.random()
[ random.random() for i in range(10) ]

random.randint(5, 10)

t = [1, 2, 3]
random.choice(t)

for dc in ['dfw', 'ord', 'lon', 'syd', 'iad']:
    url = 'http://reports.ohthree.com/{}/instance/{}/json'.format(dc, str(uuid))
    response = json.load(urllib2.urlopen(req))[0]['Instance_Data']['host'][2:].replace('-', '.')

ds = dict((chr(i), range(i, i+5)) for i in range(65,70))

## Dictionary comprehensions (PEP 274)
# SEE: http://www.python.org/dev/peps/pep-0274/

## args/kwargs
def print_all(*args):
    return [x for x in enumerate(args)]
print_all('A','b','b','a')
# [(0, 'A'), (1, 'b'), (2, 'b'), (3, 'a')]

def f(a, b, c, d, e): print(a, b, c, d, e)
f(10, *(20,), c=30, **{'d':40, 'e':50})
# 10 20 30 40 50
