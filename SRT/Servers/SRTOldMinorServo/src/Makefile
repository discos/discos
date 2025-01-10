#*****************************************
#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------
#*****************************************

PY_PACKAGES = 
PY_SCRIPTS = 

# On-Line Database Files
# ----------------------
CDB_SCHEMAS = MinorServo

# ----------------------------
# Libraries (public and local)
# ----------------------------
LIBRARIES = WPServoImpl MinorServoBossImpl

WPServoImpl_OBJECTS = WPServoImpl WPServoSocket WPServoTalker RequestDispatcher \
					  WPStatusUpdater SocketListener utils libCom WPUtils
WPServoImpl_LIBS = MinorServoStubs IRALibrary ComponentErrors \
				   SRTMinorServoLibrary MinorServoErrors

MinorServoBossImpl_OBJECTS = MinorServoBossImpl utils SetupThread ParkThread \
							 TrackingThread ScanThread MSBossPublisher libCom \
							 MSBossConfiguration
MinorServoBossImpl_LIBS = MinorServoStubs MinorServoBossStubs SRTMinorServoLibrary \
						  ManagmentDefinitionsStubs ManagementErrors \
						  IRALibrary ComponentErrors MinorServoErrors acsnc \
						  ParserErrors DiscosVersion AntennaDefinitionsStubs MountStubs \
						  AntennaBossStubs AntennaErrors ActiveSurfaceBossStubs \
						  SRTActiveSurfaceBossStubs MinorServoDefinitionsStubs
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
	$(RM) ../lib/python/site-packages/*
	$(RM) $(INTROOT)/lib/python/site-packages/SRTMinorServoTest
	$(RM) $(INTROOT)/bin/mscu-runserver
	@echo " . . . clean done"

clean_dist : clean clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	# cp ../doc/html/group__ACSEXMPLDOC.html ../doc/html/main.html
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
