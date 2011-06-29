#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

"""
This module defines a dummy server. 

How To Use This Module
======================
(See the individual classes, methods, and attributes for details)

1. Import it: ``import rserver`` or ``from rserver import ...``.

2. Create a RServer object::

       rs = RServer(response_type, host, port)

3. Run the server::

       rs.run()

An alternative way is to execute directly this module from a shell::

       ./rserver.py
"""

import socket, traceback, os, sys
import binhex
from random import randrange

# Index of byte that store che command code
CMD_IDX = 3
# Commands
GET_DATA = chr(0x6E)
SET_DATA = chr(0x6F)
# Anser header
HEADER = chr(0x02)

# Response to generate
response_types = (
        'ack', 
        'nak', 
        'wrong_header', 
        'random'
)

class RServer:
    """A simulator of Franco Fiocchi (Medicina) board."""

    def __init__(self, response_type="ack", host='', port=8000):
        """Initialize a `Listener` object.

        Parameters:

        - `host`: a string, the IP address allowed. All IPs to default.
        - `port`: a positive integer, the port the Listener lists for connections.
        - `response_type`: a string; an unexpected string generates an unexpected answer.
        """
        self.host = host
        self.port = port
        self.response_type = response_type
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    def run(self):
        """Run the Listener."""
        self.s.bind((self.host, self.port))
        self.s.listen(1)
        # Display a welcome message
        print "*"*36
        print "RServer - Waiting for connections..."
        print "*"*36

        # Number of connections counter
        counter = 0
        while True:
            try:
                connection, clientaddr = self.s.accept()
                counter += 1 
                print "\n%d. Got connection from %s" %(counter, connection.getpeername())
            except KeyboardInterrupt:
                raise
            except:
                traceback.print_exc()
                continue

            # Clean up old children
            reap()

            # Fork a process for this connection
            pid = os.fork()

            if pid:
                # This is the parent process. Close the child's socket and return to the top of the loop.
                connection.close()
                continue
            else:
                # From here on, this is the child
                # Close the parent's socket
                self.s.close()

                # Process the data
                counter = 0
                try:
                    while True:
                        data = connection.recv(2048)
                        print "\nReceived message from %s: %r" % (connection.getpeername(), 
                                binhex.binascii.hexlify(data))
                        # Initialize the answer with a wrong message
                        answer = "".join([chr(randrange(1, 3)), data[randrange(0, 5)], 
                            data[1], data[3], data[randrange(3, 5)], chr(randrange(0, 3))])
                        # If the request is for a set_data (0x6F)
                        if(data[CMD_IDX] == SET_DATA):
                            if self.response_type == "ack":
                                answer = "".join([HEADER, data[2], data[1], data[3], data[4], chr(0x00)])
                            elif self.response_type == "nak":
                                answer = "".join([HEADER, data[2], data[1], data[3], data[4], chr(randrange(1, 5))])
                            elif self.response_type == "random":
                                answer = "".join([HEADER, data[2], data[1], data[3], data[4], chr(randrange(0, 2))])
                            if self.response_type == "wrong_header":
                                answer = "".join([chr(0x01), data[2], data[1], data[3], data[4], chr(0x00)])
                        elif(data[CMD_IDX] == GET_DATA):
                            answer = "".join([HEADER, data[2], data[1], data[3], data[4], chr(0x00), chr(0x08)] + map(chr, range(8)))

                        print "Sending message: %r" %binhex.binascii.hexlify(answer)
                        connection.send(answer)

                except (KeyboardInterrupt, SystemExit):
                    raise
                except:
                    pass

                # Close the connection
                try:
                     connection.close()
                except KeyboardInterrupt:
                    raise
                except:
                    traceback.print_exc()

                # Done handling the connection. Child process *must* terminate
                # and not go back to the top of the loop
                sys.exit(0)

def reap():
    """Collect any child processes that may be outstanding."""
    while True:
        try:
            result = os.waitpid(-1, os.WNOHANG)
            if not result[0]: 
                break
        except:
            break
        print "Reaped process %d" % result[0]


if __name__ == "__main__":
    rs = RServer(response_type="ack")
    rs.run()


