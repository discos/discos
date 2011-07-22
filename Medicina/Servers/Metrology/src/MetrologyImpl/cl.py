# simple illustration of thread module

# two clients connect to server; each client repeatedly sends a letter,
# stored in the variable k, which the server appends to a global string
# v, and reports v to the client; k = '' means the client is dropping
# out; when all clients are gone, server prints the final string v

# this is the client; usage is

#    python clnt.py server_address port_number

from xml.dom import minidom


def getText(nodelist):
    rc = ""
    for node in nodelist:
        if node.nodeType == node.TEXT_NODE:
            rc = rc + node.data
    return rc





import socket  # networking module
import sys  


# create Internet TCP socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

host = '192.167.8.211'# sys.argv[1]  # server address
port = 5000 # int(sys.argv[2])  # server port

# connect to server

s.connect((host, port))
#	while 1:	

s.send('date')  # send k to server
#sys.sleep(100)
v = s.recv(1024)  # receive v from server (up to 1024 bytes)
dom=minidom.parseString(v)
date=dom.getElementsByTagName("date")
print "Server Date: " ,getText(date[0].childNodes)
		

print v  


s.close() # close socket

#print 'the final value of v is', v
