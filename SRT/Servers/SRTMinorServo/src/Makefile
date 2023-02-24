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
CDB_SCHEMAS =

# ----------------------------
# Libraries (public and local)
# ----------------------------
LIBRARIES = SRTMinorServoSocket

SRTMinorServoSocket_OBJECTS = SRTMinorServoSocket
SRTMinorServoSocket_CFLAGS = -std=c++17
SRTMinorServoSocket_LIBS = IRALibrary ComponentErrors SRTMinorServoCommandLibrary


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
