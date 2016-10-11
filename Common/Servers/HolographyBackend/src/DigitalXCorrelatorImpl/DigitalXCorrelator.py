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
import ACSErrTypeFPGAConnectionImpl
import ACSErrTypeFPGACommunicationImpl
import ComponentErrorsImpl
import Acspy.Util.ACSCorba
import BackendsErrorsImpl



import datetime
from math import *
import corr2serial # The Giovanni's module defines an high level API to communicate with FPGA

__docformat__ = 'restructuredtext'

# A list of ordered keys of full_res dictionary in save_coeff method
ordered_key_list = ['x_2', 'y_2', 'y90_2', 'xy', 'xy90','modNrm','PwrInX2','PwrInY2','phase_deg']#, 'real', 'imm', 'module', 'phase']

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
#	self.name=ContainerServices.getName(self)

    def initialize(self):
        """Retrieve the CDB parameters."""
#        dal=Acspy.Util.ACSCorba.cdb()
        name=self.getName()
        dal=Acspy.Util.ACSCorba.cdb()

	try:
	    dao=dal.get_DAO_Servant("alma/"+name)
            print self.name
	    self.port = int(dao.get_double("PORT"))
#           self.baudrate = int(dao.get_double("BAUDRATE"))
            self.out_file_name = dao.get_string("OUT_FILE_FP")
#            self.max_attempts =int(dao.get_double("MAX_ATTEMPTS"))
            self.max_attempts =1

#	    self.samples==int(dao.get_double("SAMPLES"))
	    self.port='/dev/ttyr00'
	    self.baudrate=115200
	    self.samples=5000000 
	    self.out_file_name="/home/spoppi/Holography/outfile"
	    
	
	
	except Exception, ex:
            self.getLogger().logError("Error... cannot get attributes from CDB. " + str(e))
            raise ComponentErrorsImpl.CouldntGetAttributeExImpl()
 
        # Connecting to FPGA module
        try:
            err = self.corr.connect(self.port, self.baudrate)

	    self.corr.set_samples(self.samples)
		    
        except Exception:
            self.getLogger().logDebug("Error in initialize: cannot connect to FPGA")
            self.getLogger().logError("Error... cannot connect to FPGA")
            raise ACSErrTypeFPGAConnectionImpl.CannotConnectExImpl()
      
	
    def cleanUp(self):
        self.corr.disconnect()
        self.getLogger().logDebug("CleanUp")
        if not self.out_file.closed:
          
            self.closeFile()
            
        #ComponentLifecycle.cleanUp()
        
    def aboutToAbort(self):
        self.getLogger().logDebug("About to Abort")

        self.corr.disconnect()
        self.out_file.close()
        

    def save_coord(self, azimuth, elevation):
	try:
	
		self.out_file.write("\n")
		self.out_file.write(("#%.4f" %azimuth).ljust(15))
 		self.out_file.write(("%.4f" %elevation).ljust(15))
		self.out_file.write(("%s" %datetime.datetime.now()).ljust(30))
		

	except IOError:
            self.getLogger().logError("Error writing out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()


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

        try:
            if self.corr.isConnected:
                 self.corr.disconnect()
            err = self.corr.connect(self.port, self.baudrate)
	    self.corr.set_samples(self.samples)

        except Exception:
            self.getLogger().logDebug("Error in save_coeff: cannot connect to FPGA")
            self.getLogger().logError("Error... cannot connect to FPGA")
            raise BackendErrorsImpl.NakExImpl()

#       # Running Correlation
        try:
            err = self.corr.run()

        except Exception:
            self.getLogger().logError("Error running correlation.")
            raise BackendErrorsImpl.BackendBusyImpl()

        
        # Getting coefficient    
        try:
            self.corr.getCoeff()

        except Exception:
            raise BackendErrorsImpl.BackendBusyImpl()

#        module = sqrt(self.corr.get_real()**2 + self.corr.get_imm()**2) 
#        phase_tmp = atan2(self.corr.get_imm() ,  self.corr.get_real())
#        DPI= 2*pi
#	phase = (phase_tmp + DPI) % DPI         
        try:
            
            full_res = self.corr.results.copy()
            self.corr.reset()

#            full_res.update({'module': module, 'phase': phase})
#            print full_res
            # Write values (V. 051)
            self.out_file.write("\n")
            self.out_file.write(("%.6f" %degrees(azimuth)).ljust(15))
            self.out_file.write(("%.6f" %degrees(elevation)).ljust(15))
            for item in ordered_key_list:
                self.out_file.write(("%.4f" %full_res[item]).ljust(15))
            self.out_file.write(("%s" %datetime.datetime.now()).ljust(30))
        except IOError:
            self.getLogger().logError("Error writing out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()
        try:	
            self.corr.disconnect()
        except Exception:
            print "disconnect exception"
            raise BackendErrorsImpl.BackendBusyImpl()
        
    def openFile(self,name):
           
       try:
            self.out_file_name=name

            print self.out_file_name
            dt = datetime.datetime.now()
            file_id = "_%04d%02d%02d_%02d%02d%02d" %(dt.year, dt.month, dt.day, dt.hour, dt.minute,dt.second)
            self.out_file = open(self.out_file_name + file_id, 'a', 0)
            self.out_file.write("Azimuth".ljust(15) + "Elevation".ljust(15))
            for item in ordered_key_list:
                self.out_file.write(item.ljust(15))
            self.out_file.write("Timestamp".ljust(30))
            # End 051

            # self.out_file = open(out_file, 'w', 0) # V.05
       except IOError:
            self.getLogger().logDebug("Error in initialize: cannot create out_file")
            self.getLogger().logError("Error creating out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()
    def closeFile(self):
        try:
          if not self.out_file.closed:
            
                 self.out_file.close()
        except IOError:
            self.getLogger().logDebug("Error in initialize: cannot close out_file")
            self.getLogger().logError("Error closing out_file")
            raise ComponentErrorsImpl.FileIOErrorExImpl()
    def reset(self):
        try:
          self.corr.reset()
        except Exception:
            print "reset exception"
            raise BackendErrorsImpl.BackendBusyImpl()
        