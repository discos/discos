#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
"""
This module defines a dummy server. 

How To Use This Module
======================
(See the individual classes, methods, and attributes for details)

1. Import it: ``import board_server`` or ``from board_server import ...``.

2. Create a BoardServer object::

       rs = BoardServer(host, port)

3. Run the server::

       rs.run()

An alternative way is to execute directly this module from a shell::

       ./board_server.py

To stop the server::

       rs.stop()
"""

import socket, traceback, os, sys
import binhex
from random import randrange
from multiprocessing import Value


# Index of the byte that stores che command code
CMD_IDX = 3
# Index of the byte that stores che parameters length
PAR_LEN_IDX = 5
AD24_LEN = 8

# Commands
CMD_INQUIRY = chr(0x41)  # Answer with data
CMD_RESET = chr(0x42)
CMD_VERSION = chr(0x43)  # Answer with data
CMD_SAVE = chr(0x44)
CMD_RESTORE = chr(0x45)
CMD_GET_ADDR = chr(0x46)  # Answer with data
CMD_SET_ADDR = chr(0x47)
CMD_GET_TIME = chr(0x48)  # Answer with data
CMD_SET_TIME = chr(0x49)
CMD_GET_FRAME = chr(0x4A)  # Answer with data
CMD_SET_FRAME = chr(0x4B)
CMD_GET_PORT = chr(0x4C)  # Answer with data
CMD_SET_PORT = chr(0x4D)
CMD_GET_DATA = chr(0x4E)  # Answer with data
CMD_SET_DATA = chr(0x4F)

answers_with_data = [CMD_INQUIRY, CMD_VERSION, CMD_GET_ADDR, CMD_GET_TIME, \
        CMD_GET_FRAME, CMD_GET_PORT, CMD_GET_DATA]

requests_with_data = [CMD_SET_ADDR, CMD_SET_TIME, \
        CMD_SET_FRAME, CMD_GET_PORT, CMD_SET_PORT, CMD_GET_DATA, CMD_SET_DATA]

CMD_TYPE_MIN_EXT = CMD_INQUIRY
CMD_TYPE_MAX_EXT = CMD_SET_DATA

CMD_TYPE_NOCHECKSUM = chr(0x20)
CMD_TYPE_MIN_ABB = chr(ord(CMD_TYPE_MIN_EXT) | ord(CMD_TYPE_NOCHECKSUM))
CMD_TYPE_MAX_ABB = chr(ord(CMD_TYPE_MAX_EXT) | ord(CMD_TYPE_NOCHECKSUM))
MCB_CMD_DATA_TYPE_B01 = chr(0x03)
MCB_CMD_DATA_TYPE_F32 = chr(0x18)


# Anser header
CMD_STX = chr(0x02)
CMD_ETX = chr(0x03)
CMD_EOT = chr(0x04)

stop_server = Value('i', False)

class BoardServer:
    """A simulator of Franco Fiocchi (Medicina) board."""

    def __init__(self, host='', port=5002):
        """Initialize a `Listener` object.

        Parameters:

        - `host`: a string, the IP address allowed. All IPs to default.
        - `port`: a positive integer, the port the Listener lists for connections.
        """
        self.host = host
        self.port = port
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)


    def run(self):
        """Run the Listener."""
        self.s.bind((self.host, self.port))
        self.s.listen(1)
        # Display a welcome message
        print "*"*40
        print "BoardServer - Waiting for connections..."
        print "*"*40

        # Number of connections counter
        counter = 0
        while True:
            if stop_server.value:
                sys.exit(0)
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
            BoardServer.reap()

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
                        data = connection.recv(1024)
                        if '#stop' in data:
                            stop_server.value = True
                            connection.close()
                            break
                        # Is the command short?
                        is_short = False
                        cmd = chr(0x00)
                        if(data):
                            if len(data) > 10:
                                for c in data:
                                    print hex(ord(c)),
                                print
                            # print "\nReceived message from %s: %r" % (connection.getpeername(), 
                            # binhex.binascii.hexlify(data))
                            # Initialize the answer with a wrong message
                            answer = "".join([CMD_STX, data[2], data[1], data[3], data[4], 
                                chr(0x00), chr(0x02), chr(0x22), chr(0x22), chr(0x33), CMD_ETX])
                            
                            if CMD_TYPE_MIN_EXT > data[CMD_IDX] or data[CMD_IDX] > CMD_TYPE_MAX_ABB:
                                # The last byte, 0x01, is the execution result of an "unknow command"
                                answer = "".join([CMD_STX, data[2], data[1], data[3], data[4], chr(0x01)])
                                # print "Sending message: %r" %binhex.binascii.hexlify(answer)
                                connection.send(answer)
                                continue
                            elif data[CMD_IDX] >= CMD_TYPE_MIN_ABB:
                                is_short = True
                                cmd = chr(ord(data[CMD_IDX]) - ord(CMD_TYPE_NOCHECKSUM))
                            else:
                                cmd = data[CMD_IDX]

                            # Set the base answer for short and extended commands without errors
                            answer = "".join([CMD_STX, data[2], data[1], data[3], data[4], chr(0x00)])
                            
                            data_list = list()
                            data_type = chr(0x01)
                            if cmd in set(answers_with_data) & set(requests_with_data):
                                par_len = ord(data[PAR_LEN_IDX]) # Length of the request parameters
                                data_type =  data[PAR_LEN_IDX + 1]
                                for item in data[PAR_LEN_IDX + 1:]:
                                    if par_len == 0:
                                        break
                                    else:
                                        data_list.append(ord(item))
                                        par_len -= 1

                            if cmd in answers_with_data:
                                if data_type == MCB_CMD_DATA_TYPE_B01:
                                    data_list += [randrange(0, 2)]
                                elif data_type == MCB_CMD_DATA_TYPE_F32:
                                    data_list += [156, 157, 158, 159] * AD24_LEN # Add four bytes
                                    pass
                                else:
                                    data_list += [item + 1 for item in range(randrange(1, 7))]
                                answer += chr(len(data_list))
                                for item in data_list:
                                    answer += chr(item)

                            # If the command is extended, add the checksum and the answer terminator
                            if not is_short:
                                checksum = 0 
                                for item in answer:
                                    checksum = checksum ^ ord(item)
                                answer += chr(checksum)
                                answer += CMD_ETX
                            
                            # print "Sending message: %r" %binhex.binascii.hexlify(answer)
                            connection.send(answer)
                except:
                    raise

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

    @staticmethod
    def stop(server=('127.0.0.1', 5002)):
            sockobj = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sockobj.settimeout(2)
            sockobj.connect(server) 
            sockobj.sendall('#stop\r\n')

    @staticmethod
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
    bs = BoardServer()
    bs.run()
