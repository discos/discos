#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.

from socket import *       
from parameters import *

server = servers['MSCUSimulator']

class NackEx(Exception):
    pass


class TestMSCUCommunication():

    def __init__(self):
        # Direct connection used to match the property values
        self.sockobj = socket(AF_INET, SOCK_STREAM)
        self.sockobj.settimeout(socket_timeout)
        # Connect to the selected server
        self.sockobj.connect(server) 
        self.cmd_num = 0

    def actPos(self):
        print "\nACTUAL POSITION..."
        positions = []
        for app_num, servo_type in app_nr.items():
            self.cmd_num += 1
            # Retrieve the positions directly from the server (without ACS)
            command = headers[0] + commands[4] + ':%d=' %self.cmd_num + str(app_num) + closers[0]

            found = False
            while(not found):
                self.sockobj.sendall(command)
                data = ""
                while(True):
                    data += self.sockobj.recv(1)
                    if closers[0] in data:
                        if ':%d=' %self.cmd_num in data:
                            found = True
                            break
                        else:
                            data = ""
            print "Answer from %s: %s" %(servo_type, data)


    def status(self):
        print "\nSTATUS..."
        # Index of status bit to flip
        for app_num, servo_type in app_nr.items():
            self.cmd_num += 1
            # Retrieve the positions directly from the server (without ACS)
            command = headers[0] + commands[2] + ':%d=' %self.cmd_num + str(app_num) + closers[0]

            found = False
            while(not found):
                self.sockobj.sendall(command)
                data = ""
                while(True):
                    data += self.sockobj.recv(1)
                    if closers[0] in data:
                        if ':%d=' %self.cmd_num in data:
                            found = True
                            break
                        else:
                            data = ""
            print "Answer from %s: %s" %(servo_type, data)



    def appstatus(self):
        print "\nAPPSTATUS..."
        # Index of status bit to flip
        for app_num, servo_type in app_nr.items():
            self.cmd_num += 1
            # Retrieve the positions directly from the server (without ACS)
            command = headers[0] + commands[6] + ':%d=' %self.cmd_num + str(app_num) + closers[0]

            found = False
            while(not found):
                self.sockobj.sendall(command)
                data = ""
                while(True):
                    data += self.sockobj.recv(1)
                    if closers[0] in data:
                        if ':%d=' %self.cmd_num in data:
                            found = True
                            break
                        else:
                            data = ""
            print "Answer from %s: %s" %(servo_type, data)


    def engTemperature(self):
        print "\nENGINE TEMPERATURE..."
        for app_num, servo_type in app_nr.items():
            par = self.get_parameter(servo_type, app_num, ENG_TEMPERATURE_IDX, ENG_TEMPERATURE_SUB)
            print "Answer from %s: %s" %(servo_type, par)


    def counturingErr(self):
        print "\nCOUNTURING ERROR..."
        for app_num, servo_type in app_nr.items():
            par = self.get_parameter(servo_type, app_num, COUNTURING_ERR_IDX, COUNTURING_ERR_SUB)
            print "Answer from %s: %s" %(servo_type, par)


    def dcTemperature(self):
        print "\nDC TEMPERATURE..."
        for app_num, servo_type in app_nr.items():
            par = self.get_parameter(servo_type, app_num, DC_TEMPERATURE_IDX, DC_TEMPERATURE_SUB, to_plc=True)
            print "Answer from %s: %s" %(servo_type, par)


    def get_parameter(self, servo_type, app_num, index, sub_index, to_plc=False):
        parameters = []
        number_of_req = 1 if to_plc else number_of_slaves[servo_type]
        starting_index = 0 if to_plc else 1
        for slave in range(starting_index, number_of_req):
            self.cmd_num += 1
            # Retrieve the parameter directly from the server (without ACS)
            command = headers[0] + commands[5] + ':%d=' %self.cmd_num + str(app_num) \
                      + ',%s,%s,%s' %(slave, index, sub_index) + closers[0]

            found = False
            while(not found):
                self.sockobj.sendall(command)
                data = ""
                while(True):
                    data += self.sockobj.recv(1)
                    if closers[0] in data:
                        if ':%d=' %self.cmd_num in data:
                            found = True
                            break
                        else:
                            data = ""
                            continue

            if data.startswith("!NAK"):
                raise NackEx("Nak answer")
            
            found = False
            for cmd in commands:
                if cmd in data:
                    found = True
                    break

            if not found:
                continue

            try:
                # Retrieve the message header
                sent, answer = data.split(">")
                paremeters_list = answer.split(',')
                parameters.append(float(answer.strip()))
            except:
                continue

        return parameters


if __name__ == "__main__":
    t = TestMSCUCommunication()
    t.actPos()
    t.status()
    t.appstatus()
    t.engTemperature()
    t.counturingErr()
    t.dcTemperature()

