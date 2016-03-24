#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id$"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# mbartolini  19/11/15  created
#

USER_LIB = -lstdc++

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = command.hpp protocol.hpp DiscosBackendProtocol

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = DiscosBackendProtocolLib
LIBRARIES_L     =

#
# <brief description of lllll library>
DiscosBackendProtocolLib_OBJECTS   = protocol command
DiscosBackendProtocolLib_LIBS      =


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

ifdef ACSROOT
    MAKEDIR  = $(shell if [ -f $(INTROOT)/include/acsMakefile ];  then \
                     echo $(INTROOT)/include; \
	           else \
	             echo $(ACSROOT)/include; \
		   fi;)
    include $(MAKEDIR)/acsMakefile
else
    MAKEDIR  = $(shell if [ -f $(INTROOT)/include/vltMakefile ];  then \
                     echo $(INTROOT)/include; \
	           else \
	             echo $(VLTROOT)/include; \
		   fi;)
    include $(MAKEDIR)/vltMakefile
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
	@echo " . . . installation done"


#___oOo___
