# SEE: http://en.wikipedia.org/wiki/Pig_Latin
"""
Similar languages to Pig Latin are Opish, in which "op" is added after each consonant (thus, "cat" becomes "copatop"); Turkey Irish, in which "ab" is added before each vowel (thus, "run" becomes "rabun"), and Double Dutch, in which each consonant is replaced with a different consonant cluster (thus, "how are you" becomes "hutchowash aruge yubou").
"""
def makePigLatin(word):
    """ convert one word into pig latin """ 
    m  = len(word)
    vowels = "a", "e", "i", "o", "u", "y" 
    # short words are not converted 
    if m<3 or word=="the":
        return word
    else:
        for i in vowels:
            if word.find(i) < m and word.find(i) != -1:
                m = word.find(i)
        if m==0:
            return word+"way" 
        else:
            return word[m:]+word[:m]+"ay" 
 
# OVERPLAY and UNDERPLAY are Pig Latin for PLOVER and PLUNDER 
sentence = "Hooray for pig latin plover plunder" 
pigLatinSentence = "" 
# iterate through words in sentence 
for w in sentence.split(' '):
    pigLatinSentence += makePigLatin(w) + " " 

print pigLatinSentence.strip()
