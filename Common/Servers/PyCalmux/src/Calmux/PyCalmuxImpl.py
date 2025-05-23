# Author:
#    Giuseppe Carboni, <giuseppe.carboni@inaf.it>

from Backends__POA import CalMux
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from IRAPy import logger, Connection

from xml.etree import ElementTree
from Acspy.Util import ACSCorba

import ComponentErrorsImpl
import cdbErrType
import time


class PyCalmuxImpl(CalMux, cc, services, lcycle):

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.attributes = {}
        self.mapping = {}
        self.current_backend = ''

    def initialize(self):
        self._set_cdb_attributes('alma/BACKENDS/PyCalmux')
        self._set_cdb_mapping('alma/DataBlock/PyCalmux/Mapping')
        self.attributes['PORT'] = int(self.attributes['PORT'])
        self.attributes['MAX_CHANNELS'] = int(self.attributes['MAX_CHANNELS'])
        self.attributes['INTERNAL_CHANNEL'] = int(self.attributes['INTERNAL_CHANNEL'])
        self.connection = Connection((self.attributes['IP'], self.attributes['PORT']))

        try:
            self.calOff()
        except Exception:
            pass

    def cleanUp(self):
        pass

    def setup(self, backend_name):
        backend_name = backend_name.lower()
        try:
            channel, polarity = self.mapping[backend_name]
        except KeyError:
            reason = 'Backend %s is not handled by the Calmux component!' \
                % backend_name
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValueOutofRangeExImpl()
            exc.setData('Reason', reason)
            raise exc
        with self.connection as s:
            self._set_input(s, channel, polarity)
            self.current_backend = backend_name

    def getSetup(self):
        return self.current_backend

    def calOn(self):
        try:
            channel = self.attributes['INTERNAL_CHANNEL']
            with self.connection as s:
                self._set_calibration(s, 1)
                self._set_input(s, channel, 0)
                self.current_backend = ''
        except ComponentErrorsImpl.SocketErrorExImpl as ex:
            raise ex.getComponentErrorsEx()

    def calOff(self):
        try:
            channel = self.attributes['INTERNAL_CHANNEL']
            with self.connection as s:
                self._set_calibration(s, 0)
                self._set_input(s, channel, 0)
                self.current_backend = ''
        except ComponentErrorsImpl.SocketErrorExImpl as ex:
            raise ex.getComponentErrorsEx()

    def _get_status(self, s):
        return self._send_command(s, '?\n')

    def getChannel(self):
        try:
            with self.connection as s:
                channel, polarity, status = self._get_status(s)
                return channel
        except ComponentErrorsImpl.SocketErrorExImpl as ex:
            raise ex.getComponentErrorsEx()

    def getPolarity(self):
        try:
            with self.connection as s:
                channel, polarity, status = self._get_status(s)
                return polarity
        except ComponentErrorsImpl.SocketErrorExImpl as ex:
            raise ex.getComponentErrorsEx()

    def getCalStatus(self):
        try:
            with self.connection as s:
                channel, polarity, status = self._get_status(s)
                return bool(status)
        except ComponentErrorsImpl.SocketErrorExImpl as ex:
            raise ex.getComponentErrorsEx()

    def _set_input(self, s, channel, polarity):
        if channel not in range(self.attributes['MAX_CHANNELS']):
            reason = 'Channel out of range!'
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValueOutofRangeExImpl()
            exc.setData('Reason', reason)
            raise exc
        elif polarity not in [0, 1]:
            reason = 'Polarity value must be 0 or 1!'
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValueOutofRangeExImpl()
            exc.setData('Reason', reason)
            raise exc
        command = 'I %d %d\n' % (channel, polarity)
        return self._send_command(s, command)

    def _set_calibration(self, s, calon):
        if calon not in [0, 1]:
            reason = 'Calon value must be 0 or 1!'
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValueOutofRangeExImpl()
            exc.setData('Reason', reason)
            raise exc
        command = 'C %d\n' % calon
        return self._send_command(s, command)

    def _send_command(self, s, command):
        """This method sends the given command and returns an answer.

        :param s: the socket on which the command will be sent
        :param command: the command to be sent to the device.
        """
        s.sendall(command.encode())

        response = s.recv(1024)
        response = response.strip().split()
        if len(response) == 1:
            response = response[0]
            if response == 'ack':
                response = True
            elif response == 'nak':
                response = False
        else:
            for index, val in enumerate(response):
                response[index] = int(val)
        return response

    def _set_cdb_attributes(self, path):
        """This method reads the attributes of the component and conveniently
        stores them in a dictionary.

        :param path: the CDB path of the component configuration.
        """
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO_Servant(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            logger.logError(reason)
            exc = ComponentErrorsImpl.CouldntGetAttributeExImpl()
            exc.setData('Reason', reason)
            raise exc
        
        attributes = [
            'IP',
            'PORT',
            'MAX_CHANNELS',
            'INTERNAL_CHANNEL'
        ]

        for name in attributes:
            try:
                self.attributes[name] = dao.get_field_data(name).strip()
            except cdbErrType.CDBFieldDoesNotExistEx:
                reason = "CDB field %s does not exist" % name
                logger.logError(reason)
                exc = ComponentErrorsImpl.CouldntGetAttributeExImpl()
                exc.setData('Reason', reason)
                raise exc

    def _set_cdb_mapping(self, path):
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc

        try:
            for child in list(ElementTree.fromstring(dao)):
                backend, channel, polarity = [c.text.strip() for c in child]
                backend = backend.lower()
                value = (int(channel), int(polarity))
                if backend in self.mapping:
                    self.mapping[backend].update(value)
                else:
                    self.mapping[backend] = value
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc
