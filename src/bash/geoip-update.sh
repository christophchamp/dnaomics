#!/bin/bash
for database in http://geolite.maxmind.com/download/geoip/database/GeoLiteCountry/GeoIP.dat.gz http://geolite.maxmind.com/download/geoip/database/GeoIPv6.dat.gz http://geolite.maxmind.com/download/geoip/database/GeoLiteCity.dat.gz http://geolite.maxmind.com/download/geoip/database/GeoLiteCityv6-beta/GeoLiteCityv6.dat.gz http://download.maxmind.com/download/geoip/database/asnum/GeoIPASNum.dat.gz http://download.maxmind.com/download/geoip/database/asnum/GeoIPASNumv6.dat.gz
do
  wget "$database" -O - | gunzip -c > /usr/share/GeoIP/$(basename "$database" .gz)
done
