import time
from math import radians
from multiprocessing import Queue
from MinorServo__POA import MinorServoBoss as POA
from Acspy.Servants.CharacteristicComponent import CharacteristicComponent as charcomponent
from Acspy.Servants.ContainerServices import ContainerServices as services
from Acspy.Servants.ComponentLifecycle import ComponentLifecycle as lcycle
from Acspy.Util.BaciHelper import addProperty
from Acspy.Clients.SimpleClient import PySimpleClient
from Acspy.Nc.Supplier import Supplier
from Acspy.Common.TimeHelper import getTimeStamp
from maciErrType import CannotGetComponentEx
from ACSErrTypeCommonImpl import CORBAProblemExImpl
import ACSLog

import Management
import ComponentErrorsImpl
import ComponentErrors
import MinorServoErrors
import MinorServoErrorsImpl
import ManagementErrorsImpl
import ManagementErrors
import socket

from NotoMinorServoBoss.servo import Servo
from NotoMinorServoBoss.devios import stringDevIO

from IRAPy import logger


__copyright__ = "Andrea Orlati <a.orlati@ira.inaf.it>"


class MinorServoImpl(POA, charcomponent, services, lcycle):
	"""
	commands = {
   	# command_name: (method_name, (type_of_arg1, ..., type_of_argN))
        'derotatorSetup': ('setup', (str,)), 
        'derotatorGetActualSetup': ('getActualSetup', ()),
        'derotatorIsReady': ('isReady', ()), 
        'derotatorSetConfiguration': ('setConfiguration', (str,)),
        'derotatorGetConfiguration': ('getConfiguration', ()), 
        'derotatorPark': ('park', ()),
        'derotatorSetOffset': ('setOffset', (float,)),
        'derotatorGetOffset': ('getOffset', ()),
        'derotatorClearOffset': ('clearOffset', ()),
        'derotatorSetRewindingMode': ('setRewindingMode', (str,)),
        'derotatorGetRewindingMode': ('getRewindingMode', ()),
        'derotatorSetAutoRewindingSteps': ('setAutoRewindingSteps', (int,)),
        'derotatorGetAutoRewindingSteps': ('getAutoRewindingSteps', ()),
        'derotatorClearAutoRewindingSteps': ('clearAutoRewindingSteps', ()),
        'derotatorIsUpdating': ('isUpdating', ()),
        'derotatorIsRewinding': ('isRewinding', ()), 
        'derotatorIsRewindingRequired': ('isRewindingRequired', ()), 
        'derotatorRewind': ('rewind', (int,)),
        'derotatorSetPosition': ('_setPositionCmd', (str,)),
        'derotatorGetPosition': ('_getPositionCmd', ()), 
        'derotatorGetCmdPosition': ('_getCmdPositionCmd', ()), 
        'derotatorGetRewindingStep': ('_getRewindingStepCmd', ()),
        'derotatorGetScanInfo': ('_getScanInfoCmd', ()),
        'derotatorGetMaxLimit': ('_getMaxLimitCmd', ()),
        'derotatorGetMinLimit': ('_getMinLimitCmd', ()),
    }
	"""

	def __init__(self):
		charcomponent.__init__(self)
		services.__init__(self)
		self.minorServo = Servo(services())
		self.control = Control()
		self.minorServo.connect()
 
		try:
			self.workingThread=services().getThread(name='Worker',target=MinorServoImpl.worker,args=(self.minorServo,self.control))
			self.workingThread.start()
		except Exception, ex:
			newEx=ComponentErrorsImpl.CanNotStartThreadExImpl( exception=ex, create=1 )
			newEx.log(services().getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx

		try:
			self.supplier=Supplier(MinorServo.MINORSERVO_DATA_CHANNEL)
        	except CORBAProblemExImpl, ex:
			newEx=ComponentErrorsImpl.NotificationChannelErrorExImpl(exception=ex, create=1 )
			newEx.log(services().getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx
		try:
			self.publisherThread=services().getThread(name='Publisher',target=MinorServoImpl.publisher,args=(self.minorServo, self.supplier, self.control))
			self.publisherThread.start()
		except Exception, ex:
			newEx=ComponentErrorsImpl.CanNotStartThreadExImpl( exception=ex, create=1 )
			newEx.log(services().getLogger(),ACSLog.ACS_LOG_DEBUG)
			raise newEx

	def initialize(self):		
		addProperty(self,'actualSetup',devio_ref=stringDevIO(self.minorServo),prop_type="ROstring")
		#motionInfo: Starting, Parking, Not Elevation Tracking, 
      		#Elevation Tracking, Not Ready to Move, ...
		addProperty(self,'motionInfo',devio_ref=stringDevIO(self.minorServo),prop_type="ROstring")
		"""
		The property below are not implemented as it seems current python component does not support enum properties
		"""
		#addProperty(self,'ready',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'starting',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'asConfiguration',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'elevationTrack',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'elevationTrack',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'tracking',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'scanActive',devio_ref=boolDevIO(self.minorServo))
		#addProperty(self,'scanning',devio_ref=boolDevIO(self.minorServo))


	def cleanUp(self):
		try:
			self.control.stop=True
			self.workingThread.join(timeout=5)
			if self.statusThread.isAlive():
				services().getLogger().logWarnig('working thread still alive and properly closed')
			self.supplier.disconnect()
		except Exception,ex:
			pass
		finally:
			self.control.stop=False


	"""
	Set the elevation tracking flag to "ON" or "OFF"
	:param value "ON" or "OFF"
	:throw MinorServoErrors::MinorServoErrorsEx if the input is different from "ON" or "OFF"
	:throw MinorServoErrors::StatusErrorEx if the servo is differrently configured
	"""
	def setElevationTracking(self,value):
		cmd=value.upper()
		if cmd=="ON":
			pass
		elif cmd=="OFF":
			pass
		else:
			exc=MinorServoErrors.ConfigurationError()
			raise exc.getMinorServoErrorsEx()

	"""	
	Not supported command in Noto
	"""   
	def setASConfiguration(self,value):
		pass


	"""
	Should the minor servo position change with the antenna elevation?
	:return true if the elevation tracking is enabled
   """
	def isElevationTrackingEn(self):
		pass
	
	"""
	Is the minor servo position following the antenna elevation?
	:return true if the minor servo position is following the antenna elevation
	"""
	def isElevationTracking():
		pass
	
	"""
	Is the system on focus?
	:return true if the minor servos are following the commanded positions
	"""
	def isTracking(self):
		#To be implemented, presently we always return true
		return True
		
	"""
	Is the system starting?
	:return true if the system is starting
	"""   
	def isStarting(self):
		#to be implemented
		pass

	"""
	Is the system using the active surface configuration (polynomial to use when te AS is active)?
   :return true if we are using the configuration with the AS active.
	"""
	def isASConfiguration(self):
		#this is not implmented in Noto servo system, in principle it is always used
		return True

	"""
   Is the system parking?
   :return true if the system is parking
	"""
	def isParking(self):
		#to be implemented
		pass
		
	"""
	Is the system ready?
	System ready means the system is configured (a setup is done). The system could be
	ready but out of focus, for instance, when we are performing a scan. If the system
	is ready but out of fucus, isTracking() returns false.
	:return true if the system is ready (ready to move)
	"""
	def isReady(self):
		#to be implemented
		pass
		
	"""
   Is the system performing a scan?
   :return true if the system is performing a scan
	"""
	def isScanning(self):
		#This is not presently implemented, always false
		return False
		
	"""
	Is the system performing a scan or waiting for another scan?
	:return true if the system is performing a scan or waiting for another scan
	"""
	def isScanActive(self):
		#This is not presently implemented, always False
		return False

	"""
	Return the actual configuration
	"""
	def getActualSetup(self):
		#to be implemented
		return ""

	"""
	Return the commanded configuration
	"""
	def getCommandedSetup(self):
		#to be implemented
		return ""

	"""
	Return the central position of the axis involved in the scan
	raises (MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx);
	"""
	def getCentralScanPosition(self):
		#Not implemented, return always zero
		return 0.0

	"""
   Start the scan of one axis of the MinorServo target.
   :param starting_time the time the scan will start or 0 if the scan is required to start immediately (ACS::Time)
	:param scan structure containing the description of the scan to be executed (MinorServoScan)
	:param antennaInfo auxiliary information from the antenna (Antenna::TRunTimeParameters)
	:raises (MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx);
 	"""
	def startScan(self,starting_time,scan,antennaInfo):
		#TO be implemented
		pass

	"""
	Closes the current scan and force the minor servo system to its normal behaviour
	:param timeToStop stores the expected epoch in which the scan will be stopped (closed). It could be zero meaning the scan will be closed immediately (ACS::Time)
	:raises (ComponentErrors::ComponentErrorsEx, ReceiversErrors::ReceiversErrorsEx)
	"""
	def closeScan(timeToStop):
		#To be implemented
		pass

	"""
   Check if it is possible to execute a scan along a given axis
   :param starting_time the time the scan will start or 0 if the scan is required to start immediately (ACS::Time)
   :param scan structure containing the description of the scan to be executed (MinorServoScan)
   :param antennaInfo auxiliary information from the antenna (Antenna::TRunTimeParameters)
   :param runTime auxiliary information computed at run time by the subsystem (TRunTimeParameters)
	: raises ((MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx)
	"""
	def checkScan(self,starting_time,scan,antennaInfo,runtime):
		#to be implemented
		return False

	"""
   Clear the user offset of a servo (or all servos)
   :param servo a string: the servo name or "ALL" to clear the user offset of all servos
	raises (MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx)
	"""
	def clearUserOffset(self,servo):
		# to be implemented
		pass
 
	"""
	Set the user offset of the servo
	:param axis_code the axis code (SRP_TZ, GFR_RZ, ecc.)
	:param double offset 
	:raises (ManagementError::ConfigurationErrorEx,MinorServoErrors::OperationNotPermittedEx)	
	"""
	def setUserOffset(self,axis_code,offset):
		#to be implemented
		pass

	"""
	Return the user offset of the system, by following the order axes of getAxesInfo()
   :return offset the user offset of the system (ACS::doyubleSeq)
	:raises (MinorServoErrors::MinorServoErrorsEx, ComponentErrors::ComponentErrorsEx)
	"""
	def getUserOffset(self):
		#return ACS::doubleSeq, to be implemented
		pass 
 
	"""
	Clear the system offset of a servo (or all servos)
	:param servo a string: the servo name or "ALL" to clear the system offset of all servos
	:raises MinorServoErrors::OperationNotPermittedEx
	"""
	def clearSystemOffset(slef,servo):
		#to be implemented
		pass
 
	"""
	Set the system offset of the servo
	:param axis_code the axis code (SRP_TZ, GFR_RZ, ecc.)
	:param double offset 
	:raises (MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx)
	"""
	def setSystemOffset(self,axis_code,offset):
		#to be implemented
		pass

	"""
	Return the system offset, by following the order axes of getAxesInfo()
	:return offset the system offset (ACS::doubleSeq)
	:raises (MinorServoErrors::MinorServoErrorsEx,ComponentErrors::ComponentErrorsEx)
	"""
	def getSystemOffset(self):
		#to be implemented
		pass
 
	"""
	Return the active axes names and related units
	:param axes a sequence of active axes (ACS::stringSeq). For instance, for the KKC (secondary) configuration: 
         * ("X", "Y", "Z1", "Z2", "Z3")
         * For the XXP (primary) configuration: 
         * ("YP", "ZP")
	:param units a sequence of strings, each one is the unit of the corresponding axis (ACS::stringSeq).
         * For instance: ("mm", "mm", "mm", "degree", "degree", "degree", "mm")
	:raises MinorServoErrors::ConfigurationErrorEx
	"""
	def getAxesInfo(self,axes,units):
		#to be implemented
		pass

	""" 
	Return the code of the axis involved in the scan (string)
	"""
	def getScanAxis(self):
		#to be implemented
		pass
 
	"""
	Return the positions of the active axes (ACS::doubleSeq)
	:param time the time related to the position we want to retrieve (ACS::Time)
	:return a sequence of positions, in the same order of the axes parameter of getAxesInfo()
	:raises MinorServoErrors::ConfigurationErrorEx if the system is not configured
	"""
	def getAxesPosition(self,time):
		#Not implemented
		pass    

	def setup(self, code):
		try:
			self.minorServo.setup(code)
			logger.logNotice('minor servo being configured: %s' %code)
		except ManagementErrorsImpl.ConfigurationErrorExImpl,ex:
			raise ex

	"""
	This method can be called in order to disable the MinorServo refresh.
	:throw CORBA::SystemException 
	"""
	def msOff(self):
		pass
		
	"""
	This method can be called in order to enable the Minor Servo refresh.
	:throw CORBA::SystemException 
	"""
	def msOn(self):
		pass


		"""
		self.commandedSetup = code.upper()
        logger.logNotice('starting the derotator %s setup' %self.commandedSetup)
        try:
            self.cdbconf.setup(self.commandedSetup)
            deviceName = self.cdbconf.getAttribute('DerotatorName')
            device = self.client.getComponent(deviceName)
        except CannotGetComponentEx, ex:
            reason = "cannot get the %s component: %s" %(deviceName, ex.message)
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

        try:
            observatoryName = self.cdbconf.getAttribute('ObservatoryName')
            observatory = self.client.getComponent(observatoryName)
            lat_obj = observatory._get_latitude()
            latitude_dec, compl = lat_obj.get_sync()
            latitude = radians(latitude_dec)
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logWarning('cannot get the site information: %s' %reason)
            latitude = radians(float(self.cdbconf.getAttribute('Latitude')))
            logger.logWarning('setting the default latitude value: %.2f radians' %latitude)
        finally:
            siteInfo = {'latitude': latitude}

        try:
            sourceName = self.cdbconf.getAttribute('CoordinateSourceName')
            source = self.client.getComponent(sourceName)
        except Exception:
            logger.logWarning('cannot get the %s component' %sourceName)
            source = None

        try:
            self.positioner.setup(
                    siteInfo, 
                    source, 
                    device, 
                    float(self.cdbconf.getAttribute('SetupPosition')))
                
            self.setConfiguration(self.cdbconf.getAttribute('DefaultConfiguration'))
            self.setRewindingMode(self.cdbconf.getAttribute('DefaultRewindingMode'))
            self.actualSetup = self.commandedSetup
            logger.logNotice('derotator %s setup done' %self.commandedSetup)
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except ComponentErrors.ComponentErrorsEx, ex:
            data_list = ex.errorTrace.data # A list
            reason = data_list[0].value if data_list else 'component error'
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', reason)
            raise exc.getComponentErrorsEx()

    def park(self):
        logger.logNotice('parking the derotator')
        try:
            if self.positioner.isReady():
                parkPosition = float(self.cdbconf.getAttribute('ParkPosition'))
                self.positioner.park(parkPosition)
            else:
                logger.logWarning('positioner not ready: a setup() is required')
                self.positioner.park()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()
        finally:
            self._setDefaultSetup()
        logger.logNotice('derotator parked')


    def getPosition(self):
        try:
            return self.positioner.getPosition()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position")
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def getMaxLimit(self):
        try:
            return self.positioner.getMaxLimit()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def getMinLimit(self):
        try:
            return self.positioner.getMinLimit()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def getPositionFromHistory(self, t):
        try:
            return self.positioner.getPositionFromHistory(t)
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except (DerotatorErrors.CommunicationErrorEx, ComponentErrors.ComponentErrorsEx), ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason("Cannot get the derotator position at the time %s" %s)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def getCmdPosition(self):
        try:
            return self.positioner.getCmdPosition()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def setPosition(self, position):
        try:
            self.positioner.setPosition(position)
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()
	"""
	"""
    def _setPositionCmd(self, position):
        #Wrap setPosition() in order to strip the `d` at the end of the string
        str_value = position.strip().rstrip('d')
        self.setPosition(float(str_value))
 

    def _getPositionCmd(self):
        #Wrap getPosition() in order to add the `d` at the end of the string
        return '%.4fd' %self.getPosition()
 

    def _getCmdPositionCmd(self):
        #Wrap getCmdPosition() in order to add the `d` at the end of the string
        return '%.4fd' %self.getCmdPosition()
 

    def _getMaxLimitCmd(self):
        #Wrap getMaxLimit() in order to add the `d` at the end of the string
        return '%.4fd' %self.getMaxLimit()
 

    def _getMinLimitCmd(self):
        #Wrap getMinLimit() in order to add the `d` at the end of the string
        return '%.4fd' %self.getMinLimit()
 

    def _getRewindingStepCmd(self):
        #Wrap getRewindingStep() in order to add the `d` at the end of the string
        return '%sd' %self.getRewindingStep()
    

    def _getScanInfoCmd(self):
        #Wrap getScanAxis() in order to show a human representation
        res = ''
        info = self.getScanInfo()
        res += 'axis: %s\n' %info.axis
        res += 'sector: %s\n' %info.sector
        res += 'Pis: %.4f\n' %info.iStaticPos
        res += 'Pip: %.4f\n' %info.iParallacticPos
        res += 'Pdp: %.4f\n' %info.dParallacticPos
        res += 'rewindingOffset: %.4f' %info.rewindingOffset
        return res
        
    def getScanInfo(self):
        return ScanInfo(**self.positioner.getScanInfo())

    def startUpdating(self, axis, sector, az, el, ra, dec):
        logger.logNotice('starting the derotator position updating')
        try:
            self.positioner.startUpdating(axis, sector, az, el, ra, dec)
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl(ex.message)
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def stopUpdating(self):
        logger.logNotice('stopping the derotator position updating')
        try:
            self.positioner.stopUpdating()
        except PositionerError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()
        logger.logNotice('derotator position updating stopped')


    def setAutoRewindingSteps(self, steps):
        try:
            self.positioner.setAutoRewindingSteps(steps)
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()


    def getAutoRewindingSteps(self):
        return self.positioner.getAutoRewindingSteps()


    def clearAutoRewindingSteps(self):
        return self.positioner.clearAutoRewindingSteps()


    def rewind(self, steps):
        try:
            return self.positioner.rewind(steps)
        except (PositionerError, NotAllowedError), ex:
            reason = "cannot rewind the derotator: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def isRewinding(self):
        return self.positioner.isRewinding()


    def isRewindingRequired(self):
        return self.positioner.isRewindingRequired()


    def getRewindingStep(self):
        try:
            return self.positioner.getRewindingStep()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def isConfigured(self):
        # Add the isReady() constraint because the Receiver boss checks if
        # the DewarPositioner is configured instead of checking if it is Ready
        return self.positioner.isConfigured() and self.isReady()


    def isReady(self):
        try:
            return self.positioner.isReady()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is ready: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isSlewing(self):
        try:
            return self.positioner.isSlewing()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is slewing: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isTracking(self):
        try:
            return self.positioner.isTracking()
        except DerotatorErrors.CommunicationErrorEx, ex:
            reason = "cannot known if the derotator is tracking: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def isUpdating(self):
        return self.positioner.isUpdating()

    def setOffset(self, offset):
        try:
            self.positioner.setOffset(offset)
        except (PositionerError, NotAllowedError), ex:
            reason = "cannot set the derotator offset: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()

    def clearOffset(self):
        try:
            self.positioner.clearOffset()
        except PositionerError, ex:
            reason = "cannot set the derotator offset: %s" %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.OperationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx()
        except NotAllowedError, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.NotAllowedExImpl()
            exc.setReason(ex.message)
            raise exc.getComponentErrorsEx()
        except Exception, ex:
            logger.logError(ex.message)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', ex.message)
            raise exc.getComponentErrorsEx()


    def getOffset(self):
        return self.positioner.getOffset()


    def getActualSetup(self):
        return self.actualSetup


    def getCommandedSetup(self):
        return self.commandedSetup


    def setRewindingMode(self, mode):
        try:
            mode = mode.upper().strip()
            self.positioner.setRewindingMode(mode)
        except PositionerError, ex:
            reason = 'cannot set the rewinding mode: %s' %ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.ValidationErrorExImpl()
            exc.setReason(reason)
            raise exc.getComponentErrorsEx() # wrong system input


    def setConfiguration(self, confCode):
        if self.positioner.isUpdating():
            self.positioner.stopUpdating()
        # The unit of time is `seconds`
        max_wait_time = 2 
        counter = 0
        step = 0.2
        while self.positioner.isUpdating():
            self.positioner.stopUpdating()
            time.sleep(step)
            counter += step
            if counter > max_wait_time:
                reason = "cannot stop the position updating"
                logger.logError(reason)
                exc = ComponentErrorsImpl.OperationErrorExImpl()
                exc.setReason(reason)
                raise exc.getComponentErrorsEx()
        try:
            self.positioner.control.clearScanInfo()
            self.cdbconf.setConfiguration(confCode.upper())
        except Exception, ex:
            reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
            logger.logError(reason)
            exc = ComponentErrorsImpl.UnexpectedExImpl()
            exc.setData('Reason', reason)
            raise exc.getComponentErrorsEx()


    def getManagementStatus(self):
        return self.control.mngStatus

    def getConfiguration(self):
        return self.positioner.getConfiguration()

    def getRewindingMode(self):
        return self.positioner.getRewindingMode()

    def _setDefaultSetup(self):
        self.actualSetup = 'none'
        self.commandedSetup = ''

    @staticmethod
    def publisher(positioner, supplier, control, sleep_time=1):
        error = False
        while True:
            if control.stop:
                break
            else:
                try:
                    status = [bool(int(item)) for item in positioner.getStatus()]
                    failure, warning, slewing, updating, tracking, ready = status

                    if failure:
                        control.mngStatus = Management.MNG_FAILURE
                    elif warning or not positioner.isSetup():
                        control.mngStatus = Management.MNG_WARNING
                    else:
                        control.mngStatus = Management.MNG_OK

                    event = Receivers.DewarPositionerDataBlock(
                            getTimeStamp().value,
                            ready,
                            tracking, 
                            updating,
                            slewing,
                            control.mngStatus
                    )
                    supplier.publishEvent(simple_data=event)
                    error = False
                except Exception, ex:
                    reason = ex.getReason() if hasattr(ex, 'getReason') else ex.message
                    if not error:
                        logger.logError('cannot publish the status: %s' %reason)
                        error = True
                finally:
                    time.sleep(sleep_time)

    def command(self, cmd):
        #Execute the command cmd
        
        This method returns a tuple (success, answer), where success is a boolean
        that indicates the command is executed correctly (success = True) or not
        (success = False). The argument answer is a string that represents the
        error message in case of error, the value returned from the method in
        case it returns a non Null object, an empty string in case it returns None.
        
        try:
            command, args_str = cmd.split('=') if '=' in cmd else (cmd, '')
            command = command.strip()
            args = [item.strip() for item in args_str.split(',')]
        except ValueError:
            success = False 
            answer = 'Error - invalid command: maybe there are too many symbols of ='
        except Exception, ex:
            success = False 
            answer = ex.message
        else:
            success = True

        if not success:
            logger.logError(answer)
            return (success, answer)

        if command not in DewarPositionerImpl.commands:
            success = False
            answer = 'Error - command %s does not exist' %command
            logger.logError(answer)
            return (success, answer)
        else:
            # For instance:
            # >>> DewarPositionerImpl.commands['foo']
            # ('setup', (str, str, float))
            # >>> method_name, types = DewarPositionerImpl.commands[command]
            # >>> method_name
            # 'setup'
            # >>> types
            # (str, str, float)
            method_name, types = DewarPositionerImpl.commands[command]
            # If we expect some arguments but there is not
            if types and not any(args):
                success = False
                answer = 'Error - missing arguments, type help(%s) for details' %command
                logger.logError(answer)
                return (success, answer)

            try:
                method = getattr(self, method_name)
            except AttributeError, ex:
                success = False
                answer = "Error - %s has no attribute %s" %(self, method_name)
                logger.logError(answer)
                return (success, answer)
            try:
                result = method(*[type_(arg) for (arg, type_) in zip(args, types)])
                # For instance:
                # >>> args = ('1', 'python', '3.50')
                # >>> types = (int, str, float)
                # >>> [type_(arg) for (arg, type_) in zip(args, types)]
                # [1, 'python', 3.5]
                # >>> def foo(a, b, c):
                # ...     print a, b, c
                # ... 
                # >>> foo(*[type_(arg) for (arg, type_) in zip(args, types)])
                # 1 python 3.5
                answer = '' if result is None else str(result)
                success = True
            except (ValueError, TypeError), ex:
                success = False
                answer = 'Error - wrong parameter usage.\nType help(%s) for details' %command
                logger.logError('%s\n%s' %(ex.message, answer))
                return (success, answer)
            except ComponentErrors.ComponentErrorsEx, ex:
                success = False
                data_list = ex.errorTrace.data # A list
                reason = data_list[0].value if data_list else 'component error'
                answer = 'Error - %s' %reason
                logger.logError(answer)
                return (success, answer)
            except Exception, ex:
                success = False
                msg = ex.message if ex.message else 'unexpected exception'
                answer = 'Error - %s' %(ex.getReason() if hasattr(ex, 'getReason') else msg)
                logger.logError(answer)
                return (success, answer)

        logger.logInfo('command %s executed' %cmd)
        return (success, answer)
"""
	@staticmethod
	def worker(servo, control, sleep_time=1):
		while True:
			try:
				servo.updatePosition()
			except Exception, ex:
				newEx=ComponentErrorsImpl.OperationErrorExImpl(exception=ex,create=1)
				newEx.log(services().getLogger(),ACSLog.ACS_LOG_ERROR)
			finally:
				time.sleep(sleep_time)
	"""
	The contenet of the event structure is:
	ACS::Time timeMark;
	boolean tracking;
	boolean starting;
	boolean parking;
	boolean parked;
	Management::TSystemStatus status;
	Presently I have to apply static values, i.e. tracking=TRUE, starting=TRUE, parking=False, Parked=False, status=Management.MNG_OK	
	"""
	@staticmethod
	def publisher(servo, supplier, control, sleep_time=1):
		while True:
			if control.stop:
				break
			else:
				try:	
					event=MinorServo.MinorServoDataBlock(getTimeStamp().value,True,True,False,False,Management.MNG_OK)	
					supplier.publishEvent(simple_data=event)
				except Exception, ex:
					newEx=ComponentErrorsImpl.NotificationChannelErrorExImpl(exception=ex, create=1 )
					newEx.log(services().getLogger(),ACSLog.ACS_LOG_WARNING)
		                finally:
					time.sleep(sleep_time)


class Control(object):
    def __init__(self):
        self.stop = False

