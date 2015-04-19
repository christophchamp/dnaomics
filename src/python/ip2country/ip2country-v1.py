#!/usr/bin/python
import urllib
response = urllib.urlopen('http://api.hostip.info/get_html.php?ip=12.215.42.19&position=true').read()
print response
