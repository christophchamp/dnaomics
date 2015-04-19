#!/usr/bin/env python

#The logic of this function is very simple:
#  - Set the number of bits in an integer variable to one (that is, set it to
#    1). This can be expressed as 2^<number of bits> -1.
#  - Shift the result to the left, filling in the remaining number of bits
#    with 0. The total number of bits in a netmask is always 32.
#  - For every 8 bits (4 sets in total), convert them to a decimal number
#    string.
#  - Join all numbers, using the dot symbol to separate individual numbers.

def get_netmask(network_size):
    bit_netmask = 0
    bit_netmask = pow(2, network_size) - 1
    bit_netmask = bit_netmask << (32 - network_size)
    nmask_array = []
    for c in range(4):
        dec = bit_netmask & 255
        bit_netmask = bit_netmask >> 8
        nmask_array.insert(0, str(dec))
    return ".".join(nmask_array)

print get_netmask(8)
print get_netmask(24)
print get_netmask(25)
print get_netmask(32)
