# Authors:
#    Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#    Giuseppe Carboni <gcarboni@oa-cagliari.inaf.it>

from Receivers__POA import SRTIFDistributor14
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl

from xml.etree import ElementTree
from Acspy.Util import ACSCorba

import socket
import Receivers 
import ComponentErrorsImpl
import ComponentErrors
import cdbErrType

class SRTIFDistributor14Impl(SRTIFDistributor14, cc, services, lcycle):
 

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.backend_name = ''
        self.attributes = {}
        self.mapping = {}

    def initialize(self):
        self._set_cdb_attributes('alma/RECEIVERS/SRTIFDistributor14')
        self._set_cdb_mapping('alma/DataBlock/SRTIFDistributor14/Mapping')

    def cleanUp(self):
        pass

    def setup(self, backend_name):
        self.backend_name = backend_name

    def setAttenuation(self, channel, value):
        """Attenuate the backend channel to the value `value`.
  
        The channel is an integer starting from 0. For instance, if
        a backend has 5 channels, the channel value will be between
        0 and 4.  Given a specified backend (self.backend), and given
        its channel, we read a table and get the correspondig attenuator
        address.

        The values are in decibel.  The IFDistributor firmware takes
        a code and coverts it to the corresponding value.
        For instance, we take 0.025 as value, and to tell the
        firmware to set it, we should send the code 001 to it.
  
            0.00 -> 000  (value*4)
            0.25 -> 001  (value*4)
            0.50 -> 002  (value*4)
            ...
        
        The command to send is '#AAA 99 999\n', where AAA is
        ATT (it means ATTenuate).  The 99 is the address, and
        999 is the value.  For instance, given setAttenuation(6, 0.50),
        we have first of all to find the address associated at the
        couple (self.backend, channel 6).  Let's suppose this address
        is 47.  At this point we have to find the code associated to
        the decibel 0.50, and we just have to multiply the value by 4.
        Therefore the code will be: 0.50*4 -> 002.
        The final command will be:
        
            #ATT 47 002\n
        """

        self._check_backend()

        if channel not in self.mapping[self.backend_name]:
            raise ValueError('Channel %s not in %s.'
                    % (channel, self.mapping[self.backend_name].keys())
                    )

        address = int(self.mapping[self.backend_name][channel])

        if address not in range(97):
            raise ValueError('Address %d out of range.' % (address))

        code = '%03d' % int(round(value*4))
        command = b'#ATT %02d %s\n' % (address, code)
        self._send_command(command)

    def getAttenuation(self, channel):
        """Gets the current attenuation value for the channel `channel`.
        Then it returns it as a floating point number.
        The `channel` parameter is used to retrieve its
        associated address (as in setAttenuation() method).
        """

        self._check_backend()

        if channel not in self.mapping[self.backend_name]:
            raise ValueError('Channel %s not in %s.'
                    % (channel, self.mapping[self.backend_name].keys())
                    )

        address = int(self.mapping[self.backend_name][channel])

        if address not in range(97):
            raise ValueError('Address %d out of range.' % (address))

        command = b'#ATT %02d?\n' % (address)
        return float(self._send_command(command, True)[1:-1])

    def setSwitch(self, enable):
        """Enable or disable the backend switch.
        `enable` parameter is a boolean that enables the switch when `True`,
        and disable it when `False`.
        """

        self._check_backend()

        command = b'#SWT 00 %03d\n' % (1 if enable else 0)
        self._send_command(command)

    def getSwitch(self):
        """Gets the current state of the backend switch and
        returns it as a boolean.
        `True` means that the switch is enabled, otherwise it returns `False`.
        """

        self._check_backend()

        command = b'#SWT 00?\n'
        response = int(self._send_command(command, True)[1:-1])
        return True if response == 1 else False

    def getInfo(self):
        """Gets the IFDistributor infos, like its name and its hardware
        and firmware versions.
        """

        self._check_backend()

        command = b'#*IDN?\n'
        response = self._send_command(command, True)
        return response

    def setDefault(self):
        """Sets the IFDistributor to its default values"""

        self._check_backend()

        command = b'#*RST\n'
        self._send_command(command)

    def getSetup(self):
        return self.backend_name

    def _check_backend(self):
        if not self.backend_name:
            raise LookupError('Select a backend first.')
        elif self.backend_name not in self.mapping:
            raise LookupError('Backend %s not in %s.'
                    % (self.backend_name, self.mapping.keys())
                    )

    def _send_command(self, command, expect_response=False):
        """Open a socket and send the command `command`.
        Parameter `expect_response` is a boolean which lets the method
        expect a response or return immediately.
        """
        
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.attributes['IP'], int(self.attributes['PORT'])))
        s.send(command)

        response = b''
        if expect_response:
            response = s.recv(1024)
        s.close()
        return response

    def _set_cdb_attributes(self, path):
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO_Servant(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            raeson = "CDB record %s does not exists" % path
            # logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc
        
        for name in ('IP', 'PORT'):
            try:
                self.attributes[name] = dao.get_field_data(name).strip()
            except cdbErrType.CDBFieldDoesNotExistEx:
                raeson = "CDB field %s does not exist" % name
                # logger.logWarning(raeson)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(raeson)
                raise exc

    def _set_cdb_mapping(self, path):
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            raeson = "CDB record %s does not exists" % path
            # logger.logError(raeson)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(raeson)
            raise exc

        try:
            children = ElementTree.fromstring(dao).getchildren()
            for child in children:
                backend, channel, address = [c.text.strip() for c in child]
                value = {int(channel): int(address)}
                if backend in self.mapping:
                    self.mapping[backend].update(value)
                else:
                    self.mapping[backend] = value
        except cdbErrType.CDBRecordDoesNotExistEx:
           raeson = "CDB record %s does not exists" % path
           # logger.logError(raeson)
           exc = ComponentErrorsImpl.ValidationErrorExImpl()
           exc.setReason(raeson)
           raise exc
