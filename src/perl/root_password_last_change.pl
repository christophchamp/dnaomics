#!/usr/bin/env perl
use strict;
#use 5.010;
use Term::ANSIColor;
my $last_change = `awk -F: /^root/'{print \$3}' /etc/shadow`;
chomp($last_change);
my $maxdays = 90;
my $now = int(time()/(86400));
my $days = $now - $last_change;
if ($days < $maxdays) {
	print colored("OK - Root password last changed $days ago\n",'green'); exit(0);
} elsif ($days == $maxdays) {
	print colored("WARNING - Root password last changed $days ago\n",'yellow'); exit(1);
} elsif ($days > $maxdays) {
	print colored("CRITICAL - Root password last changed $days ago\n",'red'); exit(2);
} else {
	print colored("UNKNOWN - Root password last changed $days ago\n",'red'); exit(3);
}
