def valid_ip_address(ipv4):
    socket.inet_pton(socket.AF_INET, ipv4)
valid_ip_address('162.209.102.241')

def validuuid(uuid):
    regex = re.compile('^[a-f0-9]{8}-?[a-f0-9]{4}-?4[a-f0-9]{3}-?[89ab][a-f0-9]{3}-?[a-f0-9]{12}\Z', re.I)
    if regex.match(uuid): return True
    return False
