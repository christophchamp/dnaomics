#!/usr/bin/python
table = (['M',1000],['CM',900],['D',500],['CD',400],['C',100],['XC',90],
         ['L',50],['XL',40],['X',10],['IX',9],['V',5],['IV',4],['I',1])

def int_to_roman(integer):
    returnlist = []
    for (rom, num) in table:
        while integer-num >= 0:
            integer -= num
            returnlist.append(rom)

    return ''.join(returnlist)

def roman_to_int(string):
    returnint = 0
    for pair in table:
        continueyes = True

        while continueyes:
            if len(string) >= len(pair[0]):

                if string[0:len(pair[0])] == pair[0]:
                    returnint += pair[1]
                    string = string[len(pair[0]):]

                else: continueyes = False
            else: continueyes = False

    return returnint

print int_to_roman(47)
print roman_to_int('XLVII')
