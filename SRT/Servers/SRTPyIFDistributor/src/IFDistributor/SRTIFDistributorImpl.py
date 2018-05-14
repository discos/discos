# Authors:
#    Giuseppe Carboni <gcarboni@oa-cagliari.inaf.it>

from Receivers__POA import SRTIFDistributor
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl
from copy import copy

from xml.etree import ElementTree
from Acspy.Util import ACSCorba

import socket
import Receivers 
import ComponentErrorsImpl
import ComponentErrors
import cdbErrType
import re

response_schema = [
    'slot',
    'address',
    'type',
    'REFfreq',
    'LOfreq',
    'att1',
    'att2',
    'att3',
    'att4',
    'shift_reg',
    'LOerr',
    'LOlock'
]

class SRTIFDistributorImpl(SRTIFDistributor, cc, services, lcycle):
 
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.backend_name = ''
        self.attributes = {}
        self.mapping = {}

    def initialize(self):
        self._set_cdb_attributes('alma/RECEIVERS/SRTIFDistributor')
        self._set_cdb_mapping('alma/DataBlock/SRTIFDistributor/Mapping')
        # self.setDefault()

    def cleanUp(self):
        pass

    def setup(self, backend_name):
        if backend_name not in self.mapping:
            raise LookupError('Unknown backend: %s' % backend_name)

        self.backend_name = backend_name

        for line in self.mapping[self.backend_name].get('LO'):
            self._set_LO(
                int(line['Board']),
                int(line['REFfreq']),
                int(line['LOfreq']),
                1  # 1: Turn on the LO, 0: turn it off
            )

        for line in self.mapping[self.backend_name].get('BW'):
            self._set_filter(
                int(line['Board']),
                int(line['BandWidth'])
            )
    
    def getInfo(self):
        """Gets the IFDistributor infos, like its name and its hardware
        and firmware versions.
        """
        pass

    def setDefault(self):
        """Sets the IFDistributor to its default values"""
        self.setup(self.attributes['DEFAULT_CONFIG'])

    def getSetup(self):
        return self.backend_name

    def _check_backend(self):
        if not self.backend_name:
            raise LookupError('Select a backend first.')

    def _send_command(self, command):
        """Open a socket and send the given command."""

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.attributes['IP'], int(self.attributes['PORT'])))
        s.sendall(command)

        response = s.recv(1024)
        s.close()
        response = response.strip().split('\n')
        return response

    def _get_board_info(self, board):
        self._check_backend()

        if board not in range(21):
            raise IndexError("Choose a board index between 0 and 20.")

        command = '? %d\n' % board

        response = self._send_command(command)

        if len(response) == 1:
            if response[0] == 'nak':
                return 'nak'

        response = [x.strip() for x in response[1].split(',')]

        status = {}
        for i in range(len(response_schema)):
            status.add(response_schema[i], response[i])
        return response

    def _set_LO(self, board, ref_freq, lo_freq, lo_on):
        self._check_backend()

        if board not in range(21):
            raise IndexError('Choose a board index between 0 and 20.')

        command = 'S %d %d %d %d' % (board, ref_freq, lo_freq, lo_on)
        return self._send_command(command)[0]

    def _set_filter(self, board, bandwidth):
        self._check_backend()

        if board not in [1, 2]:
            raise IndexError("Choose a board index between 1 and 2.")
        elif bandwidth not in range(4):
            raise IndexError("Choose a bandwidth index between 0 and 3.")

        command = 'B %d %d\n' % (board, bandwidth)
        return self._send_command(command)[0]

    def _set_cdb_attributes(self, path):
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO_Servant(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            # logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc
        
        for name in ('IP', 'PORT', 'DEFAULT_CONFIG'):
            try:
                self.attributes[name] = dao.get_field_data(name).strip()
            except cdbErrType.CDBFieldDoesNotExistEx:
                reason = "CDB field %s does not exist" % name
                logger.logWarning(reason)
                exc = ComponentErrorsImpl.ValidationErrorExImpl()
                exc.setReason(reason)
                raise exc

    def _set_cdb_mapping(self, path):
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            # logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc

        try:
            children = ElementTree.fromstring(dao)
            for child in children:
                conf_name, conf_data = _dictify(child)
                self.mapping[conf_name] = conf_data
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exist" % path
            # logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc


def _dictify(r, root=True):
    tag = re.sub(r'\{.*\}', '', r.tag)
    text = r.text.strip()
    if root:
        content = _dictify(r, False)
        return content.pop('ConfigurationName'), content
    if text:
        return text
    d = {}
    for x in r.findall("./*"):
        tag = re.sub(r'\{.*\}', '', x.tag)
        element = _dictify(x, False)
        if tag in ['BW', 'LO']:
            if tag not in d:
                d[tag] = []
            d[tag].append(element)
        else:
            d[tag] = element
    return d
