
ssh qb%s.slicehost.com 'sudo ssh %s \"{ 
{ type -p xe | grep -qw xe && { xe vm-reboot name-label=slice%s --force & }; } | 
{ type -p xm | grep -qw xm && xm destroy slice%s && sleep 5 && xm create slice%s; } }

ret = subprocess.call("ssh qb%s.slicehost.com 'sudo ssh %s \"{ { type -p xe | grep -qw xe && { xe vm-reboot name-label=slice%s --force & }; } | { type -p xm | grep -qw xm && xm destroy slice%s && sleep 5 && xm create slice%s; } }\"'" % (
                        reboot['huddle'],
                        reboot['host'],
                        reboot['slice'],
                        reboot['slice'],
                        reboot['slice']),
                        shell=True,
                        stdout=open('/dev/null', 'w'),
                        stderr=subprocess.STDOUT)
