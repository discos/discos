#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from socket import *       
import random
import time
from parameters import *

server = servers['MSCU']

# Direct connection used to match the property values
sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.settimeout(60.0)
sockobj.connect(server)
cmd_num = 0

while(True):
    user_command = raw_input('\nInsert a command: ')
    if user_command not in ['stow', 'setup', 'disable', 'setpos', 'getpos']:
        print 'command %s not found\n' %user_command
        continue
    else:
        cmd_num += 1
        if user_command == 'setpos':
            command = '#%s:%d=1,0,0,0' %(user_command, cmd_num)
            for i in range(6):
                if i < 3:
                    command += ',-%d' %random.randrange(40)
                else:
                    command += ',%d' %random.randrange(40)
            command += '\r\n' 
        else:
            command = '#%s:%d=1,0\r\n' %(user_command, cmd_num)

        found = False
        data = ""
        try:
            while(not found):
                print "s> %r" %command
                sockobj.sendall(command)
                while(True):
                    data += sockobj.recv(1)
                    if closers[0] in data:
                        if ':%d=' %cmd_num in data:
                            found = True
                            print 'r> %r' %data
                            if data.startswith('?') or data.startswith('!NAK'):
                                break
                            else:
                                data = ""
        except:
            pass

if __name__ == "__main__":
    t = TestMSCUCommunication()
    t.disable()

