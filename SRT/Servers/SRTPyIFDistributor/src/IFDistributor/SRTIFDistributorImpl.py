# Author:
#    Giuseppe Carboni <giuseppe.carboni@inaf.it>

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
import cdbErrType
import re
import IFDParser

class SRTIFDistributorImpl(SRTIFDistributor, cc, services, lcycle):
 
    def __init__(self):
        cc.__init__(self)
        services.__init__(self)
        self.configuration_name = ''
        self.configuration = {}
        self.attributes = {}
        self.configurations = {}

        self.LO_board = None
        self.freq = 0
        self.lock = 0
        self.ampl = 0.0
        self.t = None

    def initialize(self):
        self._set_cdb_attributes('alma/RECEIVERS/SRTIFDistributor')
        self._set_cdb_configuration('alma/DataBlock/SRTIFDistributor/Configuration')

        addProperty(self, 'frequency', devio_ref=GenericDevIO(self, 'freq'))
        addProperty(self, 'amplitude', devio_ref=GenericDevIO(self, 'ampl'))
        addProperty(self, 'isLocked', devio_ref=GenericDevIO(self, 'lock'))

        self.setDefault()

    def cleanUp(self):
        pass

    def setup(self, configuration_name):
        """This method performs a complete setup of the device by receiving the
        desired configuration name as a string and using the corresponding CDB
        configuration. If the desired configuration is unknown it raises and
        logs an exception.

        :param configuration_name: the desired device configuration."""
        if configuration_name not in self.configurations:
            reason = (
                'Unknown IFDistributor configuration: %s'
                % configuration_name
            )
            logger.logError(reason)
            exc = ComponentErrorsImpl.CouldntGetAttributeExImpl()
            exc.setData('reason', reason)
            raise exc.getComponentErrorsEx()

        self.configuration_name = configuration_name
        self.configuration = self.configurations[configuration_name]

        # LO
        LO_conf = self.configuration.get('LO')
        if not LO_conf:
            reason = (
                'Local oscillator configuration not present. Aborting setup.'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValidationErrorExImpl(reason)

        if int(LO_conf['Board']) != self._LO_board():
            reason = (
                'Wrong board selected for local oscillator. '
                + 'Please, fix the board index in CDB.'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

        if LO_conf['Enable'] == '0':
            self.rfoff()
        else:
            self._set_LO(int(LO_conf['Frequency']), 1)

        # BW
        for line in (self.configuration.get('BW') or []):
            self._set_filter(
                int(line['Board']),
                int(line['Bandwidth'])
            )

        # ATT
        for line in (self.configuration.get('ATT') or []):
            for channel_conf in line['ChannelConfiguration']:
                self._set_att(
                    int(line['Board']),
                    int(channel_conf['Channel']),
                    float(channel_conf['Attenuation']),
                )

        # INPUT
        for line in (self.configuration.get('INPUT') or []):
            self._set_input(
                int(line['Board']),
                int(line['Conversion'])
            )

        if not self.t:  # Check if a control timer has already been created
            self._is_configured()

    def setDefault(self):
        """Sets the IFDistributor to its default values."""
        self.setup(self.attributes['DEFAULT_CONFIG'])

    def getSetup(self):
        """If present, returns the current configuration name, otherwise it
        returns an empty string."""
        return self.configuration_name

    def get(self):
        """Returns the current amplitude and frequency
        of the local oscillator."""
        return self.ampl, self.freq

    def set(self, _, frequency):
        """Sets the frequency and turn on the local oscillator.
        :param _: placeholder for amplitude. Since the amplitude of the local
            oscillator cannot be set (it's 1 when the local oscillator is
            enabled or 0 when the local oscillator is disabled), it is simply
            ignored.
        :param frequency: the frequency to which the local oscillator will be
            set.
        """
        self._set_LO(frequency, 1)

    def rfoff(self):
        """Turns off the local oscillator."""
        self._LO_board()
        self._set_LO(self.freq, 0)

    def rfon(self):
        """Turns on the local oscillator."""
        self._LO_board()
        self._set_LO(self.freq, 1)

    def _set_LO(self, lo_freq, lo_on):
        """This method is used to issue commands to the local oscillator
        board (type 2).

        :param lo_freq: the frequency to which the local oscillator will be
            set.
        :param lo_on: a boolean flag that enables or disables the local
            oscillator.
        """
        command = (
            'S %d %d %d %d\n' %
            (
                self._LO_board(),
                int(self.attributes['REF_FREQ']),
                lo_freq,
                lo_on
            )
        )

        response = self._send_command(command)[0]

        if response == 'nak':
            reason = (
                ('Command `%s` has not been accepted. ' % command[:-1])
                + 'Check sent values for correctness.'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

        if self.configuration:
            self.configuration['LO']['Frequency'] = str(lo_freq)
            self.configuration['LO']['Enable'] = str(lo_on)

    def _set_filter(self, board, bandwidth):
        """This method is used to issue commands to the set the filter
        bandwidth to type 0 and 1 boards.

        :param board: the board index to which the command will be sent.
        :param bandwidth: an integer that indicates the desired bandwidth
            of the filter. 0: narrow, 1: medium, 2: large, 3: all bandwidth.
            Any other value will force the device to answer with a nak.
        """
        status = self._get_board_status(board)
        if status['TYPE'] not in [0, 1]:
            reason = 'Bandwidth cannot be set in board %d.' % board
            logger.logError(reason)
            raise ComponentErrorsImpl.OperationErrorExImpl(reason)

        command = 'B %d %d\n' % (board, bandwidth)

        response = self._send_command(command)[0]
        if response == 'nak':
            reason = (
                'Command `set bandwidth` has not been accepted. '
                + 'Make sure to send a proper bandwidth value.'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

    def _set_att(self, board, channel, attenuation):
        """This method is used to issue commands to set the
        board attenuations to type 5 boards.

        :param board: the board index to which the command will be sent.
        :param channel: the board channel whose attenuation will be set.
            Accepted channels are 0, 1, 2 and 3. Any other value will force
            the device to answer with a nak.
        :param attenuation: the attenuation to set to the given channel.
            It is a floating point number between 0 and 31.5dB. Any value
            outside this range will be rejected with a nak.
        """
        status = self._get_board_status(board)
        if status['TYPE'] != 5:
            reason = 'Attenuation cannot be set in board %d.' % board
            logger.logError(reason)
            raise ComponentErrorsImpl.OperationErrorExImpl(reason)

        command = 'A %d %d %.3f\n' % (board, channel, attenuation)

        response = self._send_command(command)[0]
        if response == 'nak':
            reason = (
                'Command `set attenuation` has not been accepted. '
                + 'Make sure to send a proper channel '
                + 'and/or attenuation value(s).'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

    def _set_input(self, board, conversion):
        """This method is used to issue commands to set the input conversion on
        type 1 boards.

        :param board: the board index to which the command will be sent.
        :param conversion: a boolean flag indicating whether the input
            conversion switch should be enabled.
        """
        status = self._get_board_status(board)
        if status['TYPE'] != 1:
            reason = 'Input conversion cannot be set in board %d.' % board
            logger.logError(reason)
            raise ComponentErrorsImpl.OperationErrorExImpl(reason)

        command = 'I %d %d\n' % (board, conversion)

        response = self._send_command(command)[0]
        if response == 'nak':
            reason = (
                'Command `set input conversion` has not been accepted. '
                + 'Make sure to send a proper input conversion value (0 or 1)'
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

    def _LO_board(self):
        """This method checks and returns which board is a local oscillator
        board (type 2). It should be called to get the local oscillator board
        index instead of directly accessing the attribute."""
        if self.LO_board is None:
            for board in range(int(self.attributes['N_BOARDS'])):
                status = self._get_board_status(board)
                if status['TYPE'] == 2:
                    self.LO_board = board
                    self.freq = status['FREQ']
                    self.lock = status['LOCK']
                    self.ampl = float(self.lock)
                    break

        if self.LO_board is not None:
            return self.LO_board
        else:
            reason = 'No local oscillator boards detected.'
            logger.logError(reason)
            raise ComponentErrorsImpl.UnexpectedExImpl(reason)

    def _get_board_status(self, board):
        """This method is used to retrieve the status of a given board. It
        makes use of the IFDParser module to conveniently parse the status
        message retrieved from the device.

        :param board: the desired board index.
        """
        if board not in range(int(self.attributes['N_BOARDS'])):
            reason = 'Invalid board index %d.' % board
            reason += (
                'Choose a board index between 0 and %d.'
                % int(self.attributes['N_BOARDS'])
            )
            logger.logError(reason)
            raise ComponentErrorsImpl.ValueOutofRangeExImpl(reason)

        command = '? %d\n' % board

        response = self._send_command(command)

        if len(response) == 1:
            return response[0]

        response = [int(x.strip()) for x in response[1].split(',')]

        status = IFDParser.parse(response)
        return status

    def _send_command(self, command):
        """This method opens a socket and send the given command.

        :param command: the command to be sent to the device.
        """
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.connect((self.attributes['IP'], int(self.attributes['PORT'])))
        except socket.error:
            reason = 'Could not reach the device!'
            logger.logError(reason)
            raise ComponentErrorsImpl.SocketErrorExImpl(reason)

        s.sendall(command)

        response = s.recv(1024)
        s.close()
        response = response.strip().split('\n')
        return response

    def _is_configured(self):
        """This method checks if the device is configured accordingly to the
        configuration issued in the setup process. It is meant to be called
        only once after the setup procedure, it will then spawn a timer that
        will call again this method with a period defined in the component
        configuration attributes (CYCLE_TIME)."""
        self.t = Timer(int(self.attributes['CYCLE_TIME']), self._is_configured)
        self.t.daemon = True
        self.t.start()

        configuration = self.configuration

        LO_conf = configuration.get('LO')
        LO_status = self._get_board_status(self._LO_board())

        self.freq = LO_status['FREQ']
        self.lock = LO_status['LOCK']
        self.ampl = float(self.lock)

        if LO_status['ENABLED'] != int(LO_conf['Enable']):
            logger.logWarning(
                'Local oscillator is %s, but it should be %s.' %
                (
                    ('enabled' if LO_status['ENABLED'] else 'disabled'),
                    ('enabled' if int(LO_conf['Enable']) else 'disabled')
                )
            )
        if LO_status['ENABLED']:
            if LO_status['REF_FREQ'] != int(self.attributes['REF_FREQ']):
                logger.logWarning(
                    'Wrong reference frequency of local oscillator.'
                )
            if LO_status['FREQ'] != int(LO_conf['Frequency']):
                logger.logWarning('Wrong frequency of local oscillator.')
            if LO_status['ERR'] != 0:
                logger.logWarning('Local oscillator error.')

        for line in (configuration.get('BW') or []):
            BW_status = self._get_board_status(int(line['Board']))
            if BW_status['BANDWIDTH'] != int(line['Bandwidth']):
                logger.logWarning(
                    'Board %d bandwidth not configured correctly.'
                    % int(line['Board'])
                )

        for line in (configuration.get('ATT') or []):
            board = int(line['Board'])
            ATT_status = self._get_board_status(board)['ATT']
            for channel_config in line['ChannelConfiguration']:
                channel = int(channel_config['Channel'])
                config_att = float(channel_config['Attenuation'])
                if int(config_att * 2) != ATT_status[channel]:
                    logger.logWarning(
                        ('Board %d, channel %d ' % (board, channel))
                        + 'attenuation not configured correctly.'
                    )

        for line in (configuration.get('INPUT') or []):
            INPUT_status = self._get_board_status(int(line['Board']))
            if INPUT_status['INPUT_CONV'] != int(line['Conversion']):
                logger.logWarning(
                    'Board %d input conversion not configured correctly.'
                    % int(line['Board'])
                )

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
            raise ComponentErrorsImpl.CouldntGetAttributeExImpl(reason)
        
        attributes = [
            'IP',
            'PORT',
            'REF_FREQ',
            'DEFAULT_CONFIG',
            'CYCLE_TIME',
            'N_BOARDS'
        ]

        for name in attributes:
            try:
                self.attributes[name] = dao.get_field_data(name).strip()
            except cdbErrType.CDBFieldDoesNotExistEx:
                reason = "CDB field %s does not exist" % name
                logger.logError(reason)
                raise ComponentErrorsImpl.CouldntGetAttributeExImpl(reason)

    def _set_cdb_configuration(self, path):
        """This method reads the possible device configurations from the CDB
        and conveniently stores them in a dictionary for further use and
        comparison.

        :param path: the CDB path of the device configurations file.
        """
        try:
            dal = ACSCorba.cdb()
            dao = dal.get_DAO(path)
        except cdbErrType.CDBRecordDoesNotExistEx:
            reason = "CDB record %s does not exists" % path
            logger.logError(reason)
            raise ComponentErrorsImpl.CouldntGetAttributeExImpl(reason)

        children = ElementTree.fromstring(dao)
        for child in children:
            conf_name, conf_data = _dictify(child)
            self.configurations[conf_name] = conf_data


def _dictify(node, root=True):
    """This function reads the XML string retrieved from the CDB and
    recursively iterates through each node of the various configurations.
    It finally returns a dictionary with all the configuration parameters.

    :param node: the XML node to parse
    :param root: boolean identifying if the current node is the root node.
    """
    tag = re.sub(r'\{.*\}', '', node.tag)
    text = node.text.strip()
    if root:
        content = _dictify(node, False)
        return content.pop('ConfigurationName'), content
    if text:
        return text
    d = {}
    for x in node.findall("./*"):
        tag = re.sub(r'\{.*\}', '', x.tag)
        element = _dictify(x, False)
        if tag in ['BW', 'ATT', 'ChannelConfiguration', 'INPUT']:
            if tag not in d:
                d[tag] = []
            d[tag].append(element)
        else:
            d[tag] = element
    return d
