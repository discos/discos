#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from socket import *       
import random
import time

address = ('192.168.200.16', 10000)

# Direct connection used to match the property values
sockobj = socket(AF_INET, SOCK_STREAM)
sockobj.settimeout(1.0)
sockobj.connect(address)

servos = {
    'PFP': 0, # Prime Focus Positioner
    'SRP': 1, # SubReflector Positioner
    'GFR': 2, # Gregorian Feed Rotator
    'M3R': 3  # Mirror 3 Rotator
}

commands = ['stow', 'setup', 'disable', 'setpos', 'getpos', 'clean']
headers = ('#', '!', '?', '@')
closers = ('\r\n', '\n\r', '\r', '\n')
cmd_num = 0
command = '#setpos:1=2,0,0,0,210.0\r\n'

while(True):
    print
    servo = raw_input('\nInsert the target servo (SRP, PFP, M3R, GFR): ')
    if servo not in servos:
        print 'Error: servo %s unknown\n' %servo
        continue
    user_command = raw_input('\nInsert a command (%s): ' %' ,'.join(commands))
    if user_command not in commands:
        print 'command %s not found\n' %user_command
        continue
    else:
        cmd_num += 1
        if user_command == 'setpos':
            positions = raw_input('\nInsert the positions, separated by commas (0,0,1,7,3,5): ')
            command = '#%s:%d=%d,0,0,0,%s' %(user_command, cmd_num, servos[servo], positions.strip())
        else:
            command = '#%s:%d=%d,0' %(user_command, cmd_num, servos[servo])

        command += '\r\n' 

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


