#*****************************************
#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------
#*****************************************

# On-Line Database Files
# ----------------------
CDB_SCHEMAS = MinorServo

# ----------------------------
# Libraries (public and local)
# ----------------------------
LIBRARIES = WPServoImpl DerotatorImpl MinorServoBossImpl

WPServoImpl_OBJECTS = WPServoImpl WPServoSocket WPServoTalker RequestDispatcher WPStatusUpdater SocketListener utils libCom WPUtils
WPServoImpl_LIBS = MinorServoStubs IRALibrary ComponentErrors SRTMinorServoLibrary MinorServoErrors

DerotatorImpl_OBJECTS = DerotatorImpl sensorSocket icdSocket utils
DerotatorImpl_LIBS = MinorServoStubs IRALibrary ComponentErrors MinorServoErrors

MinorServoBossImpl_OBJECTS = MinorServoBossImpl utils SetupThread TrackingThread ScanThread MSBossPublisher libCom
MinorServoBossImpl_LIBS = MinorServoStubs MinorServoBossStubs SRTMinorServoLibrary ManagmentDefinitionsStubs ManagementErrors \
						  IRALibrary ComponentErrors MinorServoErrors acsnc

# ----------------------------------------------------------------------
# List of all possible C-sources (used to create automatic dependencies)
# ----------------------------------------------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))


# -----------------
# Include Standards
# -----------------

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif


# TARGETS
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	$(RM) *~ ../include/*~ ../idl/*~ ../*~ ../../*~ core
	$(RM) ../doc/html
	$(RM) tmp.txt acsexmplbeans.jar ../doc/abeans.log
	@echo " . . . clean done"

clean_dist : clean clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	# cp ../doc/html/group__ACSEXMPLDOC.html ../doc/html/main.html
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
