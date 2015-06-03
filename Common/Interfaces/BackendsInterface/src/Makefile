#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.5 2011-05-31 10:13:22 amelis Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  05/09/08  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
#USER_CFLAGS = 

MAKE_PDF = ON

 

#
# Configuration Database Files
# ----------------------------
CDB_SCHEMAS = GenericBackend

# 
# IDL Files and flags
# 
IDL_FILES = GenericBackend BackendsBoss TotalPower NoiseGenerator DigitalXCorrelator Holography DBBC DFBInterface Roach
IDL_TAO_FLAGS =
USER_IDL =

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

#
# INCLUDE STANDARDS
# -----------------

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

#
# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@cp ../images/Backends.png $(INTROOT)/bitmaps/
	@chmod 644 $(INTROOT)/bitmaps/Backends.png
	@echo " . . . installation done"


#___oOo___
