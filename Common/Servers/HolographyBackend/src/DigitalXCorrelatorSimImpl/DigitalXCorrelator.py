"""This module difines a component used to carry the Digital Cross Correlator 
system out into ACS.

Classes
=======
DigitalXCorrelator: ACS component used to communicate with
an FPGA system that performs a digital cross-correlation.

Exceptions
==========
The DigitalXCorrelator component raises the following exceptions:

- `ComponentErrorsImpl`: 
    
    * CouldntGetAttributeExImpl: raised when an error occured retrieving attributes 
      from CDB.
    * FileIOErrorExImpl: raised when an error occured writing output file.

- `ACSErrTypeFPGAConnectionImpl`: raised when the connection to FPGA failed
- `ACSErrTypeFPGACommunicationImpl`: raised when an error occured during 
   communication with FPGA

Authors
=======

Digital Cross Correlator System Designers
-----------------------------------------
Giampaolo Serra, gserra@oa-cagliari.inaf.it
Giovanni Busonera, giovannibusonera@gmail.com

ACS Component
-------------
Author: Marco Buttu, mbuttu@oa-cagliari.inaf.it
Last Modified: Thu Sep 24 10:51:30 CEST 2009
"""
import DXC__POA
from Acspy.Servants.ContainerServices import ContainerServices
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle
from Acspy.Servants.ACSComponent import ACSComponent
from Acspy.Common.CDBAccess import CDBaccess
import Acspy.Util.ACSCorba




import ACSErrTypeFPGAConnectionImpl

import ACSErrTypeFPGACommunicationImpl
import ComponentErrorsImpl
import datetime
from math import *
import corr2serial # The Giovanni's module defines an high level API to communicate with FPGA

__docformat__ = 'restructuredtext'

class DigitalXCorrelator(DXC__POA.DigitalXCorrelator, ACSComponent, ContainerServices, ComponentLifecycle):
    """The DigitalXCorrelator ACS component uses the serial port to communicate with
    an FPGA system that perform a digital cross-correlation.
    That system was designed by Giampaolo Serra (INAF, Cagliari) and Giovanni Busonera (CRS4).

    Methods
    =======

    - `__init__`: constructor.
    - `initialize`: called by manager, initializes the component.
    - `cleanUp`: called by manager, releases the component.
    - `save_coeff`: saves to file the cross-correlation coefficients. It take two arguments:
       the target antenna azimuth and elevation.

    Exceptions
    ==========

    - `ComponentErrorsImpl`: 
        
        * CouldntGetAttributeExImpl: raised when an error occured retrieving attributes 
          from CDB.
        * FileIOErrorExImpl: raised when an error occured writing output file.

    - `ACSErrTypeFPGAConnectionImpl`: raised when the connection to FPGA failed
    - `ACSErrTypeFPGACommunicationImpl`: raised when an error occured during communication 
       with FPGA"""
   
    def __init__(self):
        ACSComponent.__init__(self)
        ContainerServices.__init__(self)
        self.corr = corr2serial.Correlator()
        self.counter = 0
#        self.name = self.__class__.__name__
	self.name=ContainerServices.getName(self)
	
	print "component name %s" %self.name

    def initialize(self):
        """Retrieve the CDB parameters."""
#        cdb_obj = CDBaccess()
        dal=Acspy.Util.ACSCorba.cdb()
        try:
		
	    self.getLogger().logDebug("alma/%s" %self.name)
	    dao=dal.get_DAO_Servant("alma/%s" %self.name)
	    print"alma/Metrologi%s" %self.name, self.name
#            cdb_dict = cdb_obj.getElement("alma/%s" %self.name, self.name)[0]
#             cdb_dict = cdb_obj.getElement("alma/%s" %self.name)[0]
	    
            self.port = int(dao.get_double("PORT"))
            self.baudrate = int(dao.get_double("BAUDRATE"))
            self.out_file_name = dao.get_string("OUT_FILE_FP")
	    print "*********out file:%s" %self.out_file_name
            self.max_attempts =int(dao.get_double("MAX_ATTEMPTS"))
        except Exception, ex:
            self.getLogger().logError("Error... cannot get attributes from CDB. " + str(ex))
            raise ComponentErrorsImpl.CouldntGetAttributeExImpl()
        
        # Connecting to FPGA module
         
	self.getLogger().logDebug("Connection")
        
	try:
            self.out_file = open(self.out_file_name, 'w', 0)
        except IOError:
            self.getLogger().logDebug("Error in initialize: cannot create out_file")
            self.getLogger().logError("Error creating out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()
	
    def cleanUp(self):
        self.corr.disconnect()
        self.out_file.close()

    def save_coeff(self, azimuth, elevation):
        """Write cross correlation coefficients to output file.

        Parameters
        ==========

        - `azimuth`: target antenna azimuth.
        - `elevation`: target antenna elevation.

        Exceptions
        ==========

        - `ComponentErrorsImpl.CouldntGetAttributeExImpl`: raised when an error occured 
           retrieving attributes from CDB.
        - `ComponentErrorrsImpl.FileIOErrorExImpl`: raised when an error occured writing 
           output file.
        - `ACSErrTypeFPGAConnectionImpl.CannotConnectExImpl`: raised when the connection 
           to FPGA failed.
        - `ACSErrTypeFPGACommunicationImpl.CannotRunExImpl`: raised when we cannot run 
           the correlation.
        - `ACSErrTypeFPGACommunicationImpl.CannotGetCoefficientsExImpl`: raised when we 
           cannot get the coefficients.
        """

        self.getLogger().logDebug("Save Coeff")        

	
        # Running Correlation
        self.getLogger().logDebug("Running Correlation")
        
#	self.getLogger().logError("Error running correlation.")
	
        err=0
        i = 0
        while err and i < self.max_attempts:
       	    self.getLogger().logDebug("Connecting");
            self.getLogger().logError("Error... cannot connect to FPGA")
        
            i += 1
            self.getLogger().logInfo("Attempt %d failed. Retrying..." %i)
            self.getLogger().logDebug("Running Correlation");
            
            if err and i == self.max_attempts:
           	 self.getLogger().logError("Error... cannot get correlator coefficient in %d attempts" %self.max_attempts)
            
        # Getting coefficient    
        #try:
		
        #except Exception:
            #raise ACSErrTypeFPGACommunicationImpl.CannotGetCoefficientsExImpl()

        module =.1
	phase= 2
 
        try:
            self.counter += 1
            self.out_file.write('#- CORRELATION N.%d -> (%s, %s) @ %s' 
                    %(self.counter, azimuth, elevation, datetime.datetime.now())
            )
           
        except IOError:
            self.getLogger().logError("Error writing out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()

