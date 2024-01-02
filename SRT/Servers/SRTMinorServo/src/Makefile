#********************************************
#--------------------------------------------
# Giuseppe Carboni <giuseppe.carboni@inaf.it>
#--------------------------------------------
#********************************************
#
# C programs (public and local)
# -----------------------------
EXECUTABLES     =
EXECUTABLES_L   =

# On-Line Database Files
# ----------------------
CDB_SCHEMAS = SRTMinorServoBoss SRTMinorServo SRTMinorServoSocket SRTMinorServoProperties SRTMinorServoLookupTable

# ----------------------------
# Libraries (public and local)
# ----------------------------
LIBRARIES = SRTMinorServoSocket SRTGenericMinorServoImpl SRTProgramTrackMinorServoImpl SRTMinorServoBossImpl

SRTMinorServoSocket_OBJECTS = SRTMinorServoSocket
SRTMinorServoSocket_LIBS = IRALibrary ComponentErrors SRTMinorServoCommandLibrary
SRTMinorServoSocket_CFLAGS = -std=c++17

SRTMinorServoBossImpl_OBJECTS = SRTMinorServoStatusThread SRTMinorServoSetupThread SRTMinorServoParkThread SRTMinorServoTrackingThread SRTMinorServoBossCore SRTMinorServoBossImpl
SRTMinorServoBossImpl_LIBS = IRALibrary SRTMinorServoBossStubs SRTMinorServoStubs ComponentErrors MinorServoErrors ManagementErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary AntennaBossStubs SRTMinorServoSocket ParserErrors DiscosVersion
SRTMinorServoBossImpl_CFLAGS = -std=c++17
SRTMinorServoBossCore_CFLAGS = -std=c++17
SRTMinorServoStatusThread_CFLAGS = -std=c++17
SRTMinorServoSetupThread_CFLAGS = -std=c++17
SRTMinorServoParkThread_CFLAGS = -std=c++17
SRTMinorServoTrackingThread_CFLAGS = -std=c++17
SRTMinorServoTrackingThread_LIBS = IRALibrary

SRTGenericMinorServoImpl_OBJECTS = SRTGenericMinorServoImpl SRTBaseMinorServoImpl
SRTGenericMinorServoImpl_LIBS = IRALibrary SRTMinorServoStubs MinorServoErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary SRTMinorServoSocket DiscosVersion
SRTGenericMinorServoImpl_CFLAGS = -std=c++17

SRTProgramTrackMinorServoImpl_OBJECTS = SRTProgramTrackMinorServoImpl SRTBaseMinorServoImpl
SRTProgramTrackMinorServoImpl_LIBS = IRALibrary SRTMinorServoStubs MinorServoErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary SRTMinorServoSocket DiscosVersion
SRTProgramTrackMinorServoImpl_CFLAGS = -std=c++17

SRTBaseMinorServoImpl_CFLAGS = -std=c++17


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
	@echo " . . . clean done"

clean_dist : clean clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
