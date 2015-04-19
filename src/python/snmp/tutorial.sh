snmpwalk -v2c -c public -On 192.168.1.9
curl icanhazip.com
snmpwalk -v2c -c public -On 127.0.0.1
sudo snmpwalk -v2c -c public -On 192.168.1.68
sudo vi /etc/snmp/snmpd.conf 
sudo vi /etc/snmp/snmp.conf 
dpkg -la|grep snmp
sudo service snmpd status
sudo service snmp status
sudo service snmpd start
sudo snmpwalk -v2c -c public -On 192.168.1.68
snmpwalk -v1 -c public localhost
vi snmp-manage.cfg
snmpwalk -v1 -c public 209.169.121.34
snmpget -v 1 -c public solarisbox .1.3.6.1.2.1.1.1.0
snmpget -v 1 -c public localhost .1.3.6.1.2.1.1.1.0
snmpgetnext -v1 -c public localhost
netstat -nr
sudo apt-get install snmp-mibs-downloader
sudo vi /etc/snmp/snmp.conf 
sudo vi /etc/snmp/snmpd.conf 
sudo service snmpd restart
sudo snmpwalk -v2c -c public -On 192.168.1.68
snmpwalk -v1 -c public 209.169.121.34
netstat -nr
ip a show
snmpwalk -v1 -c public 192.168.1.9
sudo snmpwalk -v2c -c public -On 192.168.1.9
sudo snmpwalk -v3 -c public -On 192.168.1.9
sudo snmpwalk -v3 -On 192.168.1.9
sudo snmpwalk -v3 -u bootstrap -On 192.168.1.9
snmpget -u bootstrap -l authPriv -a MD5 -x DES -A temp_password -X temp_password 192.168.1.9 1.3.6.1.2.1.1.1.0
snmpget -u bootstrap -l authPriv -a MD5 -x DES -A temp_password -X temp_password 192.168.1.29 1.3.6.1.2.1.1.1.0
snmpusm -u bootstrap -l authPriv -a MD5 -x DES -A temp_password -X temp_password 192.168.1.29 create demo bootstrap
snmpusm -u bootstrap -l authPriv -a MD5 -x DES -A temp_password -X temp_password 192.168.1.9 create demo bootstrap
snmpusm -u demo -l authPriv -a MD5 -x DES -A temp_password -X temp_password 192.168.1.9 passwd temp_password Argo1280
snmpget -u demo -l authPriv -a MD5 -x DES -A Argo1280 -X Argo1280 192.168.1.9 sysUpTime.0
uptime
mkdir ~/.snmp
vi .snmp/snmp.conf
snmpget 192.168.1.9 sysUpTime.0
sudo vi /etc/snmp/snmpd.conf 
snmpusm 192.168.1.9 delete bootstrap
sudo service snmpd restart
snmpget 192.168.1.9 1.3.6.1.2.1.1.1.0
snmpget 192.168.1.9 sysDescr.0
snmpget 192.168.1.9
snmpgetnext 192.168.1.9 sysDescr.0
snmpgetnext 192.168.1.9 sysObjectID.0
snmpwalk 192.168.1.9 system
snmpwalk -v2 192.168.1.9 system
snmpwalk 192.168.1.9 system
snmpwalk 192.168.1.9 .
snmpwalk 192.168.1.9 . |grep -i sys
snmpwalk 192.168.1.9 . |grep -i uptime
snmptranslate 1.3.6.1.2.1.1.1.0
snmptranslate -On HOST-RESOURCES-MIB::hrSystemUptime.0
snmptranslate -Tp 1.3.6.1.2.1.1.1.0
snmptranslate -Tp 1.3.6.1.2.1.1.1.0 -Of
snmpwalk 192.168.1.9 udpTable
snmptable 192.168.1.9 udpTable
sudo vi /etc/snmp/snmpd.conf 
sudo service snmpd restart
snmpset 192.168.1.9 sysLocation.0 = "Seattle"
snmpget 192.168.1.9 sysLocation.0

# To retrieve all of the variables under system for host zeus
snmpwalk -Os -c public -v 1 zeus system

# To retrieve the scalar values, but omit the sysORTable for host zeus
snmpwalk -Os -c public -v 1 -CE sysORTable zeus system
