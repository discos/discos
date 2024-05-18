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

PY_SCRIPTS = _cover _airblade

# On-Line Database Files
# ----------------------
CDB_SCHEMAS = SRTMinorServoBoss SRTMinorServo SRTMinorServoSocketConfiguration SRTMinorServoProperties SRTMinorServoLookupTable

# ----------------------------
# Libraries (public and local)
# ----------------------------
LIBRARIES = SRTGenericMinorServoImpl SRTProgramTrackMinorServoImpl SRTMinorServoBossImpl

USER_CFLAGS =

SRTMinorServoBossImpl_OBJECTS = SRTMinorServoBossCore SRTMinorServoSetupThread SRTMinorServoParkThread SRTMinorServoTrackingThread SRTMinorServoScanThread SRTMinorServoBossImpl SRTMinorServoStatusThread
SRTMinorServoBossImpl_LIBS = IRALibrary SRTMinorServoBossStubs SRTMinorServoStubs ComponentErrors MinorServoErrors ManagementErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary AntennaBossStubs SRTMinorServoSocketLibrary ParserErrors DiscosVersion acsnc
SRTMinorServoBossImpl_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoBossCore_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoStatusThread_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoSetupThread_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoParkThread_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoTrackingThread_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoScanThread_CFLAGS = -std=c++17 -fconcepts

SRTBaseMinorServoImpl_CFLAGS = -std=c++17 -fconcepts
SRTGenericMinorServoImpl_OBJECTS = SRTGenericMinorServoImpl SRTBaseMinorServoImpl
SRTGenericMinorServoImpl_LIBS = IRALibrary SRTMinorServoStubs MinorServoErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary SRTMinorServoSocketLibrary DiscosVersion
SRTGenericMinorServoImpl_CFLAGS = -std=c++17 -fconcepts
SRTProgramTrackMinorServoImpl_OBJECTS = SRTProgramTrackMinorServoImpl SRTBaseMinorServoImpl
SRTProgramTrackMinorServoImpl_LIBS = IRALibrary SRTMinorServoStubs MinorServoErrors MinorServoDefinitionsStubs SRTMinorServoCommandLibrary SRTMinorServoSocketLibrary DiscosVersion
SRTProgramTrackMinorServoImpl_CFLAGS = -std=c++17 -fconcepts


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
