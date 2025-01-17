# The following lines will look for the libZMQPublisher in the
# $INTROOT/lib path instead of looking in the $INTROOT/lib/python/site-packages
# path. This is a workaround needed since the said library is a C++ shared
# library and it does not get automatically installed in the correct folder.
# By adding this workaround we're also able to rename the library with the
# module name ZMQPublisher, trimming the 'lib' header. I
# suggest to use this approach whenever a Python wrapper is needed.
import os
import sys
sys.path.append(os.path.join(os.environ['INTROOT'], 'lib'))
from libPyZMQPublisher import PyZMQPublisher, DEFAULT_ADDRESS, DEFAULT_PORT
sys.path.remove(os.path.join(os.environ['INTROOT'], 'lib'))
del os
del sys

# Add the actual 'publish' public method to the ZMQPublisher class.
import json
def publish(self, dictionary: dict):
    self.__publish(json.dumps(dictionary))

PyZMQPublisher.publish = publish


# This class acts as a class factory. This is a workaround in order to make use
# of Python optional arguments for a constructor method (or a class factory
# method, like '__new__'), since this is not possible to achieve with boost_python
# The '__new__' method simply returns an object of type PyZMQPublisher,
# boost_python wrapper of the actual c++ ZMQPublisher class.
class ZMQPublisher:

    def __new__(cls, topic: str, address: str=DEFAULT_ADDRESS, port: int=DEFAULT_PORT):
        return PyZMQPublisher(topic, address, port) 


__all__ = ['ZMQPublisher']
