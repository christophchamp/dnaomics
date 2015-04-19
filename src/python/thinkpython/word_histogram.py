import string

def process_file(filename):
    h = dict()
    fp = open(filename)
    for line in fp:
        process_line(line, h)
    return h

def process_line(line, h):
    line = line.replace('-', ' ')

    for word in line.split():
        word = word.strip(string.punctuation + string.whitespace)
        word = word.lower()

        h[word] = h.get(word, 0) + 1

#To count the total number of words in the file, we can add up the frequencies in the histogram:
def total_words(h):
    return sum(h.values())

#The number of different words is just the number of items in the dictionary:
def different_words(h):
    return len(h)

#To find the most common words, we can apply the DSU pattern; most_common takes a histogram and returns a list of word-frequency tuples, sorted in reverse order by frequency:
def most_common(h):
    t = []
    for key, value in h.items():
        t.append((value, key))

    t.sort(reverse=True)
    return t

def print_most_common(hist, num=10):
    t = most_common(hist)
    print 'The most common words are:'
    for freq, word in t[0:num]:
        print word, '\t', freq

def subtract(d1, d2):
    """Takes dictionaries d1 and d2 and returns a new dictionary that contains 
    all the keys from d1 that are not in d2. Since we do not really care about 
    the values, we set them all to None.
    """
    res = dict()
    for key in d1:
        if key not in d2:
            res[key] = None
    return res

import random
def random_word(h):
    """To choose a random word from the histogram, the simplest algorithm is to 
    build a list with multiple copies of each word, according to the observed 
    frequency, and then choose from the list.
    """
    t = []
    for word, freq in h.items():
        # The expression [word] * freq creates a list with freq copies of the 
        # string word.
        # The extend method is similar to append except that the argument is a 
        # sequence.
        t.extend([word] * freq)

    return random.choice(t)

hist = process_file('emma.txt')

print 'Total number of words:', total_words(hist)
print 'Number of different words:', different_words(hist)
print_most_common(hist)

#words = process_file('words.txt')
#diff = subtract(hist, words)
#print "The words in the book that aren't in the word list are:"
#for word in diff.keys():
#    print word,

print "RANDOM_WORD: ", random_word(hist)
