hello = "hello"
hello[::-1]
reversed(hello)
s = ''.join(reversed(hello))
s = ['o', 'l', 'l', 'e', 'h']
"".join(s)

( x for x in xrange(0, 10)  if not x % 2 )

def infnum():
    num = 0
    while True:
        yield num
        num+=1
num  = infnum()
( n for n in xrange() )

>>> Fraction(1, 2)
<Fraction: 1/2>
'{0}'.format(F)

str(Fraction(1,2))
assert ( Fraction(1,2) + Fraction(1,4) ).numerator == 6

class Fraction(object):
  def __init__(self, numerator, denominator):
      self.numerator = numerator
      self.denom = denom

  def __add__(self, other):
      numerator =  self.numerator * other.denominator + other.numerator * self.denominator
      denominator = self.denominator * other.denominator
      return Fraction(numerator, denominator)

  def __eq__(self, other):
      return bool(self.numerator == other.numerator && self.denominator == self.denominator)

  def __repr__(self):
      return '<Fraction {0}/{1}>'.format(self.numerator, self.denom)

F = Fraction(1, 2)
assert F.numerator == 1
assert F.denom == 2

comment = "Hello Christoph bad word ASS"
bad_words = ['word', 'awesome', '@@@@@', 'ass']
bad_words = set(bad_words)

def has_bad_word(text):
    words = text.split(' ')
    words = set([word.lower() for word in words])
    return bool(words & bad_words)

O(N*M) where n == number of words in comment, and m == number of bad words

class Author(models.Model):
  name = models.CharField(max_length=50, db_index=True)

class Book(models.Model):
  id = integer
  title = models.CharField(max_length=100)
  pub_date = models.DateField()
  
Book.objects.filter(author__name__startswith="M")
SELECT * from book INNER JOIN ON(book.author_id = author.id ) where author.name LIKE M%

for book in Book.objects.select_related('author').all():
  print book.author.name

