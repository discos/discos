# Author:
#    Giuseppe Carboni <giuseppe.carboni@inaf.it>

import sys

module = sys.modules[__name__]

common_schema = [
    'SLOT',
    'ADDRESS',
    'TYPE',
]

board_types = []
board_types.append(['_FILTER'])
board_types.append(['_FILTER', '_INPUT'])
for i in range(3):
    board_types.append(['_LO'])
board_types.append(['_LO', '_ATT'])

def _LO(response):
    """This method parses the current status of the local oscillator.

    :param response: the tuple containing the ordered status of the board.
    """
    LO_status = {}

    LO_status['REF_FREQ'] = int(response[3])
    LO_status['FREQ'] = float(response[4])
    LO_status['ENABLED'] = int(bin(response[9])[2:].zfill(8)[4], 2)
    LO_status['ERR'] = int(response[10])
    LO_status['LOCK'] = int(response[11])

    return LO_status

def _INPUT(response):
    """This method parses the current status of the input conversion.

    :param response: the tuple containing the ordered status of the board.
    """
    INPUT_status = {}

    INPUT_status['INPUT_CONV'] = int(bin(response[9])[2:].zfill(8)[5:7], 2) - 1

    return INPUT_status

def _FILTER(response):
    """This method parses the current bandwidth of the filter of the board.

    :param response: the tuple containing the ordered status of the board."""
    FILTER_status = {}

    FILTER_status['BANDWIDTH'] = int(bin(response[9])[2:].zfill(8)[3:5], 2)

    return FILTER_status

def _ATT(response):
    """This method parses the attenuation values of the board, one for each of
    the board channels.

    :param response: the tuple containing the ordered status of the board."""
    ATT_status = {}

    ATT_status['ATT'] = [int(value) for value in response[5:9]]

    return ATT_status

def parse(response):
    """This method conveniently parses the response retrieved from the device
    into a dictionary. Since every board has its characteristics or a
    combination of them, naming the response dictionary keys starting from the
    board type simplifies the subsequent checking procedure.

    :param response: the tuple containing the ordered status of the board."""
    status = {}
    for i in range(len(common_schema)):
        status[common_schema[i]] = int(response[i])
    try:
        pcb_type = board_types[response[2]]
    except IndexError:
        raise ValueError('Unknown PCB type %d.' % response[2])
    for configuration in pcb_type:
        method = getattr(module, configuration)
        status.update(method(response))
    return status
