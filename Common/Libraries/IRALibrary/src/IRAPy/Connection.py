# Author:
#    Giuseppe Carboni, <giuseppe.carboni@inaf.it>

import socket
import time
from contextlib import contextmanager

import ComponentErrorsImpl


class Connection(object):
    """This class implements a contextmanager for a socket.
    Usage example:

    with Connection(('127.0.0.1', 10000)) as s:
        s.sendall('COMMAND\n')
        answer = s.recv(1024)
    
    :param: address, a tuple containing IP address and PORT for the socket
    :param: timeout, connection timeout, in seconds
    """

    def __init__(self, address, timeout=2):
        self.address = address

    def __enter__(self):
        self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        connected = 1
        t0 = time.time()
        while time.time() - t0 < timeout:
            connected = self.s.connect_ex(self.address)
            if connected == 0:
                break
            time.sleep(0.01)
        if connected:
            reason = 'Could not reach the device!'
            from IRAPy import logger
            logger.logError(reason)
            exc = ComponentErrorsImpl.SocketErrorExImpl()
            exc.setData('Reason', reason)
            raise exc
        return self.s

    def __exit__(self, type, value, traceback):
        self.s.close()
