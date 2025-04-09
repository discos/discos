content = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>

<lan xmlns="urn:schemas-cosylab-com:lan:1.0" IPAddress="%s" port="%d">
    <delay units="mSec" description="response delay"/>
    <status description="converter status"/>
</lan>"""

port_offset = 0
for sector in range(1, 9):
    for lan in range(1, 13):
        ip = '127.0.0.1'
        port = 11000 + port_offset
        port_offset += 1
        lan_file = 'SECTOR%02d/LAN%02d/LAN%02d.xml' % (sector, lan, lan)
        with open(lan_file, 'w') as f:
            f.write(content % (ip, port))
