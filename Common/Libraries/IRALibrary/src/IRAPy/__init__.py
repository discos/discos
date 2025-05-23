"""IRA python package for acs system
When importing this package a new logger automatically replaces the ACS one. 
You can access the logger as IRAPy.logger or by using import as:
C{from IRAPy import logger}

list of modules:
  - customlogging: custom logging functionalities to replace standard logging
  - bsqueue: BoundedSortedQueue class implements a priority queue structure
  - Connection: Connection class implements a contextmanager for a socket

"""
from . import customlogging
import ACSLog
from .Connection import Connection

#Some comments required here. The custom logger mechanism is not working in python. 
#do the way to separate the system logs to the ones do be shown to the user is to use different
#logging channels.   
logger = customlogging.getLogger("IRA_SERVICE_LOGGER")
userLogger = customlogging.getLogger("IRA_CUSTOM_LOGGER")
userLogger.isUserLogger=True

customlogging.Log.setCapacity(1)
customlogging.Log.setBatchSize(1)
