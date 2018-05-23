# Authors:
#    Giuseppe Carboni <gcarboni@oa-cagliari.inaf.it>

from Receivers__POA import SRTIFDistributor
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as cc
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl
from threading import Timer
from IRAPy import logger
from IFDistributor.devios import GenericDevIO

from xml.etree import ElementTree
from Acspy.Util import ACSCorba

import socket
import Receivers 
import ComponentErrorsImpl
import ComponentErrors
import cdbErrType
import re
import IFDParser


class SRTIFDistributorImpl(SRTIFDistributor, cc, services, lcycle):
 
    ampl = 0
    attenuation_step = 0.125

    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.configuration_name = ''
        self.attributes = {}
        self.mapping = {}

        self.LO_board = None
        self.freq = 0
        self.lock = 0

    def initialize(self):
        self._set_cdb_attributes('alma/RECEIVERS/SRTIFDistributor')
        self._set_cdb_mapping('alma/DataBlock/SRTIFDistributor/Mapping')

        addProperty(self, 'frequency', devio_ref=GenericDevIO(self, 'freq'))
        addProperty(self, 'amplitude', devio_ref=GenericDevIO(self, 'ampl'))
        addProperty(self, 'isLocked', devio_ref=GenericDevIO(self, 'lock'))

    def cleanUp(self):
        pass

    def setup(self, configuration_name):
        if configuration_name not in self.mapping:
            raise LookupError('Unknown configuration: %s' % configuration_name)

        self.configuration_name = configuration_name

        configuration = self.mapping[configuration_name]

        # LO
        LO_conf = configuration.get('LO')
        if int(LO_conf['Board']) != self._LO_board():
            raise IndexError(
                'Wrong LO board selected. Please, fix the CDB.'
            )
        if LO_conf['Enable'] == '0':
            self.rfoff()
        else:
            self._set_LO(int(LO_conf['Frequency']), 1)

        # BW
        for line in self.mapping[self.configuration_name].get('BW'):
            self._set_filter(
                int(line['Board']),
                int(line['Bandwidth'])
            )

        # ATT
        for line in self.mapping[self.configuration_name].get('ATT'):
            for channel_conf in line['ChannelConfiguration']:
                self._set_att(
                    int(line['Board']),
                    int(channel_conf['Channel']),
                    float(channel_conf['Attenuation']),
                )

        # INPUT
        for line in self.mapping[self.configuration_name].get('INPUT'):
            self._set_input(
                int(line['Board']),
                int(line['Conversion'])
            )

        self._is_configured()

    def setDefault(self):
        """Sets the IFDistributor to its default values"""
        self.setup(self.attributes['DEFAULT_CONFIG'])

    def getSetup(self):
        return self.configuration_name

    def get(self, *_):
        raise NotImplementedError

    def set(self, *_):
        raise NotImplementedError

    def rfoff(self):
        self._LO_board()
        self._set_LO(self.freq, 0)

    def rfon(self):
        self._LO_board()
        self._set_LO(self.freq, 1)

    def _set_LO(self, lo_freq, lo_on):
        command = (
            'S %d %d %d %d\n' %
            (
                self._LO_board(),
                int(self.attributes['REF_FREQ']),
                lo_freq,
                lo_on
            )
        )
        return self._send_command(command)[0]

    def _set_filter(self, board, bandwidth):
        status = self._get_board_status(board)
        if status['TYPE'] not in [0, 1]:
            raise IndexError('The selected board does not accept command `B`.')
        elif bandwidth not in range(4):
            raise ValueError('Choose a bandwidth between 0 and 3.')

        command = 'B %d %d\n' % (board, bandwidth)
        return self._send_command(command)[0]

    def _set_att(self, board, channel, attenuation):
        status = self._get_board_status(board)
        if status['TYPE'] != 5:
            raise IndexError('The selected board does not accept command `A`.')
        elif channel not in range(4):
            raise IndexError('Choose a channel between 0 and 3.')
        elif attenuation < 0 or attenuation > 31.5:
            raise ValueError('Choose an attenuation between 0 and 31.5dB.')

        command = 'A %d %d %.3f\n' % (board, channel, attenuation)
        return self._send_command(command)[0]

    def _set_input(self, board, conversion):
        status = self._get_board_status(board)
        if status['TYPE'] != 1:
            raise IndexError('The selected board does not accept command `I`.')
        elif conversion not in [0, 1]:
            raise ValueError('Choose a conversion value between 0 and 1.')

        command = 'I %d %d\n' % (board, conversion)
        return self._send_command(command)[0]

    def _check_configuration(self):
        if not self.configuration_name:
            raise LookupError('Select a configuration first.')

    def _LO_board(self):
        if self.LO_board is None:
            for board in range(21):
                status = self._get_board_status(board)
                if status['TYPE'] == 2:
                    self.LO_board = board
                    self.freq = status['FREQ']
                    self.lock = status['LOCK']
                    break

        if self.LO_board is not None:
            return self.LO_board
        else:
            raise ValueError('No LO boards detected.')

    def _get_board_status(self, board):
        self._check_configuration()

        if board not in range(21):
            raise IndexError("Choose a board index between 0 and 20.")

        command = '? %d\n' % board

        response = self._send_command(command)

        if len(response) == 1:
            return response[0]

        response = [int(x.strip()) for x in response[1].split(',')]

        status = IFDParser.parse(response)
        return status

    def _send_command(self, command):
        """Open a socket and send the given command."""

        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((self.attributes['IP'], int(self.attributes['PORT'])))
        except socket.error:
            raise socket.error('Could not reach the device!')

        s.sendall(command)

        response = s.recv(1024)
        s.close()
        response = response.strip().split('\n')
        return response

    def _is_configured(self):
        t = Timer(int(self.attributes['CYCLE_TIME']), self._is_configured)
        t.daemon = True
        t.start()

        LO_status = self._get_board_status(self._LO_board())
        LO_conf = self.mapping[self.configuration_name].get('LO')

        if LO_status['ENABLED'] != int(LO_conf['Enable']):
            raise Exception(
                'Local oscillator status is %d, configuration is %d.'
                % (LO_status['ENABLED'], int(LO_conf['Enable']))
            )
        if LO_status['ENABLED'] == 1:
            if LO_status['REF_FREQ'] != int(self.attributes['REF_FREQ']):
                raise Exception(
                    'Reference frequency of LO is %d, configuration is %d.'
                    % (LO_status['REF_FREQ'], int(self.attributes['REF_FREQ']))
                )
            if LO_status['FREQ'] != int(LO_conf['Frequency']):
                raise Exception(
                    'LO current frequency is %d, configuration is %d.'
                    % (LO_status['FREQ'], int(LO_conf['Frequency']))
                )
            if LO_status['ERR'] != 0:
                raise Exception('Local oscillator error.')
            self.lock = LO_status['LOCK']

        for line in self.mapping[self.configuration_name].get('BW'):
            BW_status = self._get_board_status(int(line['Board']))
            if BW_status['BANDWIDTH'] != int(line['Bandwidth']):
                raise Exception(
                    'Board %d bandwidth is %d, configuration is %d.' %
                    (
                        int(line['Board']),
                        BW_status['BANDWIDTH'],
                        int(line['Bandwidth'])
                    )
                )

        for line in self.mapping[self.configuration_name].get('ATT'):
            board = int(line['Board'])
            ATT_status = self._get_board_status(board)['ATT']
            for configuration in line['ChannelConfiguration']:
                channel = int(configuration['Channel'])
                conf_att = float(configuration['Attenuation'])
                status_att = float(ATT_status[channel]) * self.attenuation_step
                if conf_att != status_att:
                    raise Exception(
                        (
                            'Board %d, channel %d attenuation is %.3f, '
                            + 'configuration is %.3f.'
                        )
                        % (board, channel, status_att, conf_att)
                    )

        for line in self.mapping[self.configuration_name].get('INPUT'):
            INPUT_status = self._get_board_status(int(line['Board']))
            if INPUT_status['INPUT_CONV'] != int(line['Conversion']):
                raise Exception(
                    'Board %d conversion is %d, configuration is %d.' %
                    (
                        int(line['Board']),
                        INPUT_status['INPUT_CONV'],
                        int(line['Conversion'])
                    )
                )

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
        
        for name in ('IP', 'PORT', 'REF_FREQ', 'DEFAULT_CONFIG', 'CYCLE_TIME'):
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
        if tag in ['BW', 'ATT', 'ChannelConfiguration', 'INPUT']:
            if tag not in d:
                d[tag] = []
            d[tag].append(element)
        else:
            d[tag] = element
    return d
