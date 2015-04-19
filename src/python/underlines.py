#!/usr/bin/env python
# SEE: http://www.lleess.com/
class A(object):
    def __method(self):
        print "I'm a method in A"

    def method(self):
        self.__method()


class B(A):
    def __method(self):
        print "I'm a method in B"


a = A()
a.method()

b = B()
b.method()


class CrazyNumber(object):

    def __init__(self, n):
        self.n = n

    def __add__(self, other):
        return self.n - other

    def __sub__(self, other):
        return self.n + other

    def __str__(self):
        return str(self.n)


num = CrazyNumber(10)
print num       # 10
print num + 5   # 5
print num - 20  # 30


class Room(object):

    def __init__(self):
        self.people = []

    def add(self, person):
        self.people.append(person)

    def __len__(self):
        return len(self.people)

room = Room()
room.add("Igor")
print len(room)  # 1
