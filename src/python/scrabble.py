#Anagram dictionary creator code
"""

f = open('/usr/share/dict/words')
d = {}
lets = set('abcdefghijklmnopqrstuvwxyz\n')
for word in f:
  if len(set(word) - lets) == 0 and len(word) > 2 and len(word) < 9:
    word = word.strip()
    key = ''.join(sorted(word))
    if key in d:
      d[key].append(word)
    else:
      d[key] = [word]
f.close()
anadict = [' '.join([key]+value) for key, value in d.iteritems()]
anadict.sort()
f = open('anadict.txt','w')
f.write('\n'.join(anadict))
f.close()
"""

#Scrabble cheating code

from bisect import bisect_left
from itertools import combinations
from time import time

def loadvars():
  f = open('anadict.txt','r')
  anadict = f.read().split('\n')
  f.close()
  return anadict

scores = {"a": 1, "c": 3, "b": 3, "e": 1, "d": 2, "g": 2, 
         "f": 4, "i": 1, "h": 4, "k": 5, "j": 8, "m": 3, 
         "l": 1, "o": 1, "n": 1, "q": 10, "p": 3, "s": 1, 
         "r": 1, "u": 1, "t": 1, "w": 4, "v": 4, "y": 4, 
         "x": 8, "z": 10}

def score_word(word):
  return sum([scores[c] for c in word])

def findwords(rack, anadict):
  rack = ''.join(sorted(rack))
  foundwords = []
  for i in xrange(2,len(rack)+1):
    for comb in combinations(rack,i):
      ana = ''.join(comb)
      j = bisect_left(anadict, ana)
      if j == len(anadict):
        continue
      words = anadict[j].split()
      if words[0] == ana:
        foundwords.extend(words[1:])
  return foundwords

if __name__ == "__main__":
  import sys
  if len(sys.argv) == 2:
    rack = sys.argv[1].strip()
  else:
    print """Usage: python cheat_at_scrabble.py <yourrack>"""
    exit()
  t = time()
  anadict = loadvars()
  print "Dictionary loading time:",(time()-t)
  t = time()
  foundwords = set(findwords(rack, anadict))
  scored = [(score_word(word), word) for word in foundwords]
  scored.sort()
  for score, word in scored:
    print "%d\t%s" % (score,word)
  print "Time elapsed:", (time()-t)
