wordlist = open('/usr/share/dict/words').read().split()
len(wordlist)
wordlist = filter(lambda w: len(w) == 3, wordlist) # keep 3-letter words
wordlist = filter(str.isalpha, wordlist) # no punctuation
wordlist = filter(str.islower, wordlist) # no proper nouns or acronyms
import numpy as np
wordlist = np.sort(wordlist)
wordlist.shape
