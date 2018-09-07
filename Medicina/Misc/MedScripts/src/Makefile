#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.7 2011-02-12 11:29:00 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  24/01/09  created
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

#
# additional include and library search paths
#USER_INC = 
#USER_LIB = 

#
# Python stuff (public and local)
# script setupkkc, setupccc and medsubr are deprecated and listed for removal
# ----------------------------
PY_SCRIPTS         = 
PY_SCRIPTS_L       = setupkkc setupccc medsubr

PY_MODULES         =
PY_MODULES_L       =

PY_PACKAGES        =
PY_PACKAGES_L      =
pppppp_MODULES	   =

#scripts addObserver and removeObserver removed from repository control as the are strictly related to the server machines configuration
SCRIPTS         = escs escsConsole
SCRIPTS_L       = addObserver removeObserver

INSTALL_FILES      = ../app-defaults/escsStartup.xml

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
	@chmod 744 $(INTROOT)/bin/escs
	@chmod 744 $(INTROOT)/app-defaults/escsStartup.xml

	@chmod 700 $(INTROOT)/bin/discosdown 
	@chmod 700 $(INTROOT)/bin/discosup 	
	
	@echo " . . . installation done"


#___oOo___
