## A collection of CLI commands/magic
## Sources include:
##   - @climagic

## http://en.wikipedia.org/wiki/Netcat
# Setting up a one-shot webserver on port 8080 to present the content of a file
{ echo -ne "HTTP/1.0 200 OK\r\n\r\n"; cat some.file; } | nc -l 8080
{ echo -ne "HTTP/1.0 200 OK\r\nContent-Length: $(wc -c <some.file)\r\n\r\n"; cat some.file; } | nc -l 8080
# Then point your browser to http://localhost:8080

#  In a file with a lot of words, grep out each alphanumeric sequence and rank.
grep -o "[[:alpha:]]*" names | sort | uniq -c | sort -n
