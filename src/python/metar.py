#!/usr/bin/python

# You might have to download the pymetar stuff from
# http://www.schwarzvogel.de/software-pymetar.shtml or
# "emerge pymetar" if you're cool.
# SEE: http://www.nws.noaa.gov/tg/siteloc.php for station codes
# Note: I was too lazy to make this script print the metar info
# to a file so I just used a cron command like
# "* * * * ~/.metar/metar.py > ~/.metar/metar_status" and then
# ${exec cat ~/.metar/metar_status} in Conky to finally show the result.

import pymetar
import sys

station = "KSAT"

rf = pymetar.ReportFetcher(station)
re = rf.FetchReport()
rp = pymetar.ReportParser()
pr = rp.ParseReport(re)

# I have multiple revisions of this script for different stations
# and print the location simply so I know which revision I'm using.
print "loc = San Antonio, Texas"
print "temp =" , pr.getTemperatureCelsius() , "C"
print "wind =" , pr.getWindSpeedMilesPerHour(),"m/h ->" , pr.getWindCompass()
