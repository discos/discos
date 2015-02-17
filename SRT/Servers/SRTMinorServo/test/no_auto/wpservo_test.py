#!/usr/bin/env python
# Author: Marco Buttu <m.buttu@oa-cagliari.inaf.it>
# Copyright: This module has been placed in the public domain.
"""
Use this module to test the minor servo properties.

Generate the Utils
==================
To use properly this module you have to build some executables,
needed for a conversion of SRP positions (from real to virtual and viceversa).
Go to the ``test`` directory and run make::

    $ make

That's all to build the executables and to put them in the ../bin directory.


How to Set the Server
=====================
You can test the component with one of the following servers:

    * MSCU: the real server for SRT minor servos, available only
      in a private network.  If you want to use this server you have
      to set ACS_CDB (in acsconf) to Medicina
    * MSCUSimulator: a MSCU simulator. If you want to use this server you have
      to set ACS_CDB (in acsconf) to SRT and execute `run_server`
      before to execute `getup_mscontainer`. See README for more infos.

Now you have to change the key of servers dictionary in this module. To use 
the MSCU::

    server = servers['MSCU']

Instead to use the MSCUSimulator::

    server = servers['MSCUSimulator']

See README file for more details.

How to Test the Component
=========================
Run ACS::

    $ acsStart

Run the MSCU simulator::

    $ run_mscuserver

For more informations about IP and PORT of MSCU see the mscu.py
docstrings (mscu.py lives in SRT/Servers/SRTMinorServo/test).

Run the container::

    $ getup_mscontainer

Start Object Explorer to test your MSCU client::

    $ objexp

or run the unittest located in SRT/Server/SRTMinorServo/test. If you
would run the unittest change the IP values of ``server`` dictionary 
defined in  ``parameters.py`` and pay attention to set properly the ``server ``
variable of ``SRT/Servers/SRTMinorServo/test/wpservo_test.py``.
That will be::
    
    server = servers['MSCUSimulator']

if you are testing the component with the MSCUSimulator, or::
    
    server = servers['MSCU']

if you are connecting to the real server. 
Now you can run the test::

    $ python wpservo_test.py
"""

import unittest
import threading
import os
import random
from socket import *       
from Acspy.Clients.SimpleClient import PySimpleClient
from maciErrTypeImpl import CannotGetComponentExImpl
from parameters import *

# Set server = servers['MSCU'] to connect directly to MSCU
# Set server = servers['MSCUSimulator'] to connect to MSCU Simulator
server = servers['MSCUSimulator']

class NackEx(Exception):
    pass


class TestProperties(unittest.TestCase):
    """Test the minor servo properties."""

    def setUp(self):
        """Get a reference to every minor servo component."""
        # Direct connection used to match the property values
        self.sockobj = socket(AF_INET, SOCK_STREAM)
        self.sockobj.settimeout(socket_timeout)
        # Connect to the selected server
        self.sockobj.connect(server) 
        self.pyclient = PySimpleClient()
        self.cmd_num = 0
        for servo_type in app_nr.values():
            self.__dict__[servo_type] = self.pyclient.getComponent("MINORSERVO/" + servo_type)

    def _tearDownClass():
        print "in tearDown"
        self.sockobj.close()
        for servo_type in app_nr.values():
            self.pyclient.releaseComponent("MINORSERVO/" + servo_type)

    def test_get_actPos(self):
        """Test the actPos property of every minor servo."""
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

            if data.startswith("!NAK"):
                continue
            pos_obj = self.__dict__[servo_type]._get_actPos()
            acs_positions, completion = pos_obj.get_sync()
            if(completion.code):
                print "\nError code found in getActPos..."
                continue
            self.assertEqual(len(acs_positions), number_of_axis[servo_type])
            try:
                # Retrieve the message header
                sent, answer = data.split(">")
                positions_list = answer.split(',')[1:]
                positions = [float(position.strip()) for position in positions_list]
            except:
                continue

            # If the servo is a SRP, you have to covert the real positions to virtual positions
            if servo_type == "SRP":
                r2v_cmd = "../bin/real2virtual"
                for position in positions:
                    r2v_cmd += " %s" %position
                put, out = os.popen4(r2v_cmd)
                
                positions = []
                try:
                    for line in out.readlines():
                        if line.startswith('x'):
                            positions.append(float(line.split("=")[1].strip()))
                except:
                    positions = [0] * number_of_axis[servo_type]

            for idx in range(number_of_axis[servo_type]):
                self.assertAlmostEqual(acs_positions[idx], positions[idx], places=1)

    def test_cmdPos(self):
        """Test the cmdPos property of every minor servo."""

        global working_apps
        working_apps = []
        for app_num, servo_type in app_nr.items():
            positions = []
            positions.append(random.randrange(15, 25))
            positions += [0] * (number_of_axis[servo_type] - 1)
            pos_obj = self.__dict__[servo_type]._get_cmdPos()
            cp = pos_obj.set_sync(positions)
            if not cp.type:
                working_apps.append((app_num, servo_type))

        import time
        time.sleep(1)
        for app_num, servo_type in working_apps:
            cmd_pos_obj = self.__dict__[servo_type]._get_cmdPos()
            cmd_positions, completion = cmd_pos_obj.get_sync()
            act_pos_obj = self.__dict__[servo_type]._get_actPos()
            act_positions, completion = act_pos_obj.get_sync()

            if act_positions:
                for idx in range(number_of_axis[servo_type]):
                    self.assertAlmostEqual(cmd_positions[idx], act_positions[idx], places=1)

    def test_get_status(self):
        """Test the status property of every minor servo."""
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

            if data.startswith("!NAK"):
                continue
            status_obj = self.__dict__[servo_type]._get_status()
            acs_status, completion = status_obj.get_sync()

            if(completion.code):
                print "\nError code found in status..."
                continue
            try:
                # Retrieve the message header
                sent, answer = data.split(">")
                status = int(answer.strip())
            except:
                continue

            self.assertAlmostEqual(acs_status, status, places=1)


    def test_get_engTemperature(self):
        """Test the engTemperature property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, ENG_TEMPERATURE_IDX, ENG_TEMPERATURE_SUB)
                param_obj = self.__dict__[servo_type]._get_engTemperature()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in engTemperature..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_counturingErr(self):
        """Test the counturingErr property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, COUNTURING_ERR_IDX, COUNTURING_ERR_SUB)
                param_obj = self.__dict__[servo_type]._get_counturingErr()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in counturingErr..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_torquePerc(self):
        """Test the torquePerc property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, TORQUE_PERC_IDX, TORQUE_PERC_SUB)
                param_obj = self.__dict__[servo_type]._get_torquePerc()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in torquePerc..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_engCurrent(self):
        """Test the engCurrent property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, ENG_CURRENT_IDX, ENG_CURRENT_SUB)
                param_obj = self.__dict__[servo_type]._get_engCurrent()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in engCurrent..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_engVoltage(self):
        """Test the engVoltage property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, ENG_VOLTAGE_IDX, ENG_VOLTAGE_SUB)
                param_obj = self.__dict__[servo_type]._get_engVoltage()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in engVoltage..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_driTemperature(self):
        """Test the driTemperature property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, DRI_TEMPERATURE_IDX, DRI_TEMPERATURE_SUB)
                param_obj = self.__dict__[servo_type]._get_driTemperature()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in driTemperature..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_utilizationPerc(self):
        """Test the utilizationPerc property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, UTILIZATION_PERC_IDX, UTILIZATION_PERC_SUB)
                param_obj = self.__dict__[servo_type]._get_utilizationPerc()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in utilizatioinPerc..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_dcTemperature(self):
        """Test the dcTemperature property of minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, DC_TEMPERATURE_IDX, DC_TEMPERATURE_SUB, to_plc=True)
                param_obj = self.__dict__[servo_type]._get_dcTemperature()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in dcTemperature..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_driverStatus(self):
        """Test the driverStatus property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, DRIVER_STATUS_IDX, DRIVER_STATUS_SUB)
                param_obj = self.__dict__[servo_type]._get_driverStatus()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in driverStatus..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_get_errorCode(self):
        """Test the errorCode property of every minor servo."""
        for app_num, servo_type in app_nr.items():
            try:
                par = self.get_parameter(servo_type, app_num, ERROR_CODE_IDX, ERROR_CODE_SUB)
                param_obj = self.__dict__[servo_type]._get_errorCode()
                acs_par, completion = param_obj.get_sync()
                if(completion.code):
                    print "\nError code found in errorCode..."
                    continue
                self.data_match(acs_par, par)
            except NackEx:
                continue

    def test_parallelRequests(self):
        """Test all properties concurrently."""
        for property in properties:
            th = threading.Thread(target=getattr(self, 'test_get_%s' %property))
            th.setDaemon(1)
            th.start()
            th.join()

    def get_parameter(self, servo_type, app_num, index, sub_index, to_plc=False):
        """Return a list of slave parameters retrieved directrly from the MSCU."""
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

    def data_match(self, acs_par, par):
        self.assertEqual(len(acs_par), len(par))

        for idx in range(len(acs_par)):
            self.assertAlmostEqual(acs_par[idx], par[idx], places=1)


def int2bin(n, bits=13):
    """Return the binary of integer n. The default result is on 13 bits.

    >>> int2bin(8, 4)
    '1000'
    >>> int2bin(8, 5)
    '01000'
    """
    return "".join([str((n >> y) & 1) for y in range(bits - 1, -1, -1)])
                

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(TestProperties)
    unittest.TextTestRunner(verbosity=2).run(suite)
    print "\n" + "="*70
    # The following line is commented because working_apps is defined in cmd_pos, whitch is bypassed
    # print "Enabled remote control for: %s" %", ".join([app[1] for app in working_apps]) + "\n"

