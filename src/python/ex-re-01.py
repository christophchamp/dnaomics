import re
m = re.search(r'\(([\S]+),([\S]+)\)', '(-3.1,4)')
real, imaginary = [float(x) for x in m.groups()]
#~OR~
foo, bar = eval('(-3.4,4.0)')
