#!/bin/bash
## Checks for un-read emails in your Gmail inbox
curl -u your_username --silent "https://mail.google.com/mail/feed/atom" |
perl -ne 'print "\t" if //;
print "$2\n" if /<(title|name)>(.*)<\/\1>/;'
