import os, subprocess, re
command = "cat /proc/cpuinfo"
all_info = subprocess.check_output(command, shell=True).strip()
for line in all_info.split("\n"):
    if "model name" in line:
        print re.sub( ".*model name.*:", "", line,1)

print "\n".join([re.sub(".*model name.*:", "",line,1) for line in all_info.split("\n") if "model name" in line])
# Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz
# Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz
# Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz
# Intel(R) Core(TM) i5-3320M CPU @ 2.60GHz
