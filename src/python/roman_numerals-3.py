#!/usr/bin/python
TABLE = (['M',1000],['CM',900],['D',500],['CD',400],['C',100],['XC',90],
         ['L',50],['XL',40],['X',10],['IX',9],['V',5],['IV',4],['I',1])

def int_to_roman(integer):
    parts = []
    for letter, value in TABLE:
        while value <= integer:
            integer -= value
            parts.append(letter)
    return ''.join(parts)

def roman_to_int(string):
    result = 0
    for letter, value in TABLE:
        while string.startswith(letter):
            result += value
            string = string[len(pairs[0]):]
    return result

print int_to_roman(47)
print roman_to_int('XLVII')
