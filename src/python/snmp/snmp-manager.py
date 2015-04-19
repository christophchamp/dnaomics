#!/usr/bin/env python
import sys
from ConfigParser import SafeConfigParser
from pysnmp.entity.rfc3413.oneliner import cmdgen
  
class SnmpManager:
    def __init__(self):
        self.systems = {}
 
    def add_system(self, id, descr, addr, port, comm_ro):
        self.systems[id] = {'description' : descr,
                            'address'     : addr,
                            'port'        : int(port),
                            'communityro' : comm_ro,
                            'checks'      : {}
                           }
 
    def add_check(self, id, oid, descr, system):
        oid_tuple = tuple([int(i) for i in oid.split('.')])
        self.systems[system]['checks'][id] = {'description': descr,
                                              'oid'        : oid_tuple,
                                             }

    def query_all_systems(self):
        cg = cmdgen.CommandGenerator()
        for system in self.systems.values():
            comm_data = cmdgen.CommunityData('my-manager', system['communityro'])
            transport = cmdgen.UdpTransportTarget((system['address'], system['port']))
            for check in system['checks'].values():
                oid = check['oid']
                errInd, errStatus, errIdx, result = cg.getCmd(comm_data, transport, oid)
                if not errInd and not errStatus:
                    print "%s/%s -> %s" % (system['description'],
                                           check['description'],
                                           str(result[0][1]))

 
def main(conf_file=""):
    if not conf_file:
        sys.exit(-1)
    config = SafeConfigParser()
    config.read(conf_file)
    snmp_manager = SnmpManager()
    for system in [s for s in config.sections() if s.startswith('system')]:
        snmp_manager.add_system(system,
                                config.get(system, 'description'),
                                config.get(system, 'address'),
                                config.get(system, 'port'),
                                config.get(system, 'communityro'))

    for check in [c for c in config.sections() if c.startswith('check')]:
        snmp_manager.add_check(check,
                               config.get(check, 'oid'),
                               config.get(check, 'description'),
                               config.get(check, 'system'))
 
if __name__ == '__main__':
    main(conf_file='snmp-manager.cfg')
