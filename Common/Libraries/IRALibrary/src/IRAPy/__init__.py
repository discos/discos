"""
IRA python package for acs system
When importing this package a new logger automatically replaces the ACS one. 
You can access the logger as IRAPy.logger or by using import as:
from IRAPy import logger

list of modules: 
    * customlogging - custom logging functionalities to replace standard logging
    * bsqueue - BoundedSortedQueue class implements a priority queue structure
"""
import customlogging
logger = customlogging.getLogger("IRA_CUSTOM_LOGGER")
