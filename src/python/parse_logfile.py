'''
Jan 20 03:26:21 fakehost anacron[28969]: Normal exit (1 job run)
Jan 20 03:30:01 fakehost CROND[31462]: (root) CMD (/usr/lib64/sa/sa1 1 1)
Jan 20 03:30:01 fakehost CROND[31461]: (root) CMD (/var/system/bin/sys-cmd -F > /dev/null 2>&1)
Jan 20 05:03:03 fakehost ntpd[3705]: synchronized to time.faux.biz, stratum 2
Jan 20 05:20:01 fakehost rsyslogd: [origin software="rsyslogd" swVersion="5.8.10" x-pid="20438" x-info="http://www.rsyslog.com"] start
Jan 20 05:22:04 fakehost cs3[31163]:  Q: ".../bin/rsync -LD ": symlink has no referent: "/var/syscmds/fakehost/runit_scripts/etc/runit/service/superImportantService/run"#012Q: ".../bin/rsync -LD ": rsync error: some files/attrs were not transferred (see previous errors) (code 23) at main.c(1039) [sender=3.0.6]
Jan 20 05:22:04 fakehost cs3[31163]:  I: Last 2 quoted lines were generated by "/usr/local/bin/rsync -LD --recursive --delete --password-file=/var/syscmds/modules/rsync_password /var/syscmds/fakehost syscmds@fakehost::syscmds_rsync"
Jan 20 05:22:08 fakehost cs3[31163]:  Q: ".../sbin/sv restart": ok: run: /export/service/cool-service: (pid 32323) 0s
Jan 20 05:22:08 fakehost cs3[31163]:  I: Last 1 quoted lines were generated by "/sbin/sv restart /export/service/cool-service"
Jan 20 05:22:09 fakehost cs3[31163]:  R: cs3:  The cool service on fakehost does not appear to be communicating with the cool service leader.  Automating a restart of the cool service in attempt to resolve the communication problem.
Jan 20 05:22:37 fakehost ACCT_ADD: WARNING: Manifest /var/syscmds/inputs/config-general/doit.txt has been processed already, bailing

---------- end sample log extract ----------
Write a script which parses /var/log/messages and generates a CSV with two columns: minute, number_of_messages
---------- begin sample output ----------
minute, number_of_messages
Jan 20 03:25,2
Jan 20 03:26,2
Jan 20 03:30,2
Jan 20 05:03,1
Jan 20 05:20,1
Jan 20 05:22,6
---------- end sample output ---------- 
'''


found = 0
last = ""
with open("/var/log/messages") as log:
    data = log.readlines()
    for line in data:
        line = line.split(" ")
        minute = line[2].split(":")[1]
        if minute == last: found +=1
        last = minute
        print line[0],line[1], minute, found
        found = 0

'''
Extract the program name from the field between the hostname and the log message and output those values in columns.
Sample Output (when run against the lines containing "Jan 20 05:2" in the log above):
---------- begin sample output ----------
minute,total_count,rsyslogd,cs3,ACCT_ADD
Jan 20 05:20,1,1,0,0
Jan 20 05:22,6,0,5,1 
---------- end sample output ----------
'''
process_dict = {}
found = 1
process_list = {}
last = ""
i=0
hour = "05"
with open("test.log") as log:
    data = log.readlines()
    for line in data:
        line = line.split(" ")
        minute = line[2].split(":")[1]
        if minute == last:
            found +=1
            process = line[4].split("[")[0]
            #{05:20: (foo,bar,foo), 05:22: (foo,)}
            #dict[0]=3,2,1
            #dict[1]=1,1,0
            timestamp = "%s %s %s:%s" % (line[0], line[1], hour, minute)
            process_dict[i] = {timestamp: process_list.append(process)}
        last = minute
        process_list = ""
        i+=1

print [t,p for p in process_dict]




p1 = subprocess.Popen(cmd_1, stdout = subprocess.PIPE)
p1
p2 = subprocess.Popen(cmd_2, stdin = p1.stdout, stdout=subprocess.PIPE)
p2
id = p2.communicate()[0]
print id
p2.stdout
p2.communicate()
p1 = subprocess.Popen(cmd_1, stdout = subprocess.PIPE)
p2 = subprocess.Popen(cmd_2, stdin = p1.stdout, stdout=subprocess.PIPE)
p2.communicate()
p1.communicate()
p1
def print_all(*args):
    return [x for x in enumerate(args)]
print_all('A','b','b','a')
def f(a, b, c, d, e): print(a, b, c, d, e)
f(10, *(20,), c=30, **{'d':40, 'e':50})
%history
print_all('A','b','b','a')
home = os.path.expanduser("~")
home
for dirpath, dirs, files in os.walk("."):
    print dirpath
dirpath
os.path.basename(dirpath)
frozenset([1,2,3])
f = frozenset([1,2,3])
s = {f}
s
%history
print range(100)
%paste
%paste
found = 0
last = ""
with open("test.log") as log:
    data = log.readlines()
with open("test.log") as log:
             data = log.readlines()
    for line in data:
                line = line.split(" ")
                minute = line[2].split(":")[1]
                if minute == last: found +=1
                last = minute
                print line[0],line[1], minute, found
                found = 0
with open("test.log") as log:
    data = log.readlines()
    for line in data:
        line = line.split(" ")
        minute = line[2].split(":")[1]
        if minute == last: found +=1
        last = minute
        print line[0],line[1], minute, found
        found =0
process_dict = {}
found = 1
process_list = ""
last = ""
i=0
with open("test.log") as log:
        data = log.readlines()
        for line in data:
                line = line.split(" ")
                minute = line[2].split(":")[1]
                if minute == last:
                        found +=1
                        process = line[4].split("[")[0]
                        #{05:20: (foo,bar,foo), 05:22: (foo,)}
                        #dict[0]=3,2,1
                        #dict[1]=1,1,0
                        timestamp = "%s %s %s:%s" % (line[0] + line[1] + hour + minute)
                        process_dict[i] = {timestamp: process_list.append(process)}
                    last = minute
                    process_list = ""
                    i+=1
%paste
process_dict = {}
found = 1
process_list = ""
last = ""
i=0
hour = "05"
with open("test.log") as log:
        data = log.readlines()
        for line in data:
                line = line.split(" ")
                minute = line[2].split(":")[1]
                if minute == last:
                        found +=1
                        process = line[4].split("[")[0]
                        #{05:20: (foo,bar,foo), 05:22: (foo,)}
                        #dict[0]=3,2,1
                        #dict[1]=1,1,0
                        timestamp = "%s %s %s:%s" % (line[0] + line[1] + hour + minute)
                        process_dict[i] = {timestamp: process_list.append(process)}
                    last = minute
                    process_list = ""
                    i+=1
