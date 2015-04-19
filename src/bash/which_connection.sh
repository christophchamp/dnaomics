#!/bin/bash
# SEE: http://kenfallon.com/
 
result="$(wget --timeout=2 http://www.icanhazip.com -O - 2>/dev/null)"
#if [ "${result}" != "success" ]
if [ 1 != 1 ]
then
  echo "No connection to www.example.com found"
  exit 1
else
  myip="$(wget --timeout=2 http://www.icanhazip.com -O - 2>/dev/null)"
  asnum=$(geoiplookup "${myip}" | grep 'GeoIP ASNum Edition: ' | awk '{print $4}' )
  case "${asnum}" in
    AS6300)
      location="home"
    ;;
    AS2222)
      location="work"
    ;;
    AS3333)
      location="roaming"
    ;;
    AS5555)
      location="roaming"
    ;;
    *)
      echo "No location found for AS Number \"${asnum}\""
      exit 2
    ;;
  esac
  interface=$(route | awk '/default/ {print $(NF)}')
fi
 
if [ "$( iwconfig 2>&1 ${interface} | grep 'ESSID' | wc -l )" -eq 1 ] || [ $(echo ${interface} | grep ppp | wc -l ) -eq 1 ]
then
  type="wireless"
  essid=$( iwconfig 2>&1 ${interface} | awk -F '"' '{print $2}')
else
  type="wired"
fi
 
echo "Connection Found: $myip $asnum $location $interface $type $essid"
 
case "${location}_${type}" in
  work_wired)
    echo "Work Network"
  ;;
  work_wireless)
    echo "Work Wireless Network"
  ;;
  roaming_wireless)
    echo "Mobile Network"
  ;;
  home_wired)
    echo "Home Wired Network"
  ;;
  home_wireless)
    echo "Home Wireless Network"
  ;;
  *)
    echo "No custom configuration applied"
  ;;
esac
