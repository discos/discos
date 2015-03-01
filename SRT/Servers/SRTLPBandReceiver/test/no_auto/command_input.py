#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from socket import *       
import random
import time
import binhex

# Direct connection used to match the property values
sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.settimeout(3.0)
sockobj.connect(('192.168.200.136', 5002))
cmd_num = 0

while(True):
    user_command = raw_input('\nInsert a command: ')
    if user_command not in ['vacuum']:
        print 'command %s not found\n' %user_command
        continue
    else:
        cmd_num += 1
        if user_command == 'vacuum':
            command = '\x01\x01\x7C\x6E\x00\x03\x18\x08\x60'

        data = ""
        try:
            print "Sent from the client: %r" % binhex.binascii.hexlify(command)
            sockobj.sendall(command)
            while(True):
                data += sockobj.recv(1)
        except:
            print "Received from the board: %r" % binhex.binascii.hexlify(data)
            data = ""
            pass
