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
# orlati  07/12/18  added installation of discosStartup.xml file for acscommandcenter
# orlati  07/12/18  changed in order to comply with new discos bootup procedure

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

ifndef MANAGER_REFERENCE
	MASTER:=$(HOST)
else
	MASTER:=$(shell echo $(MANAGER_REFERENCE) | cut -d: -f 3)
endif

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
#SCRIPTS         = escs escsConsole
#SCRIPTS_L       = addObserver removeObserver

# new bootup scripts (discos):
SCRIPTS=discosup discosdown discosConsole
SCRIPTS_L         = escs escsConsole


#INSTALL_FILES      = ../app-defaults/escsStartup.xml
INSTALL_FILES = ../app-defaults/discosStartup.xml

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

.PHONY: all
.DEFAULT_GOAL:=all

#
# TARGETS
# -------
all:	do_all
	@sed -i 's/MASTERHOST/$(MASTER)/g' ../bin/discosup
	@sed -i 's/MASTERHOST/$(MASTER)/g' ../bin/discosdown
	@echo " . . . 'all' done" 

clean : clean_all 
	@rm -f $(INTROOT)/app-defaults/discosStartup.xml
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
#	@chmod 744 $(INTROOT)/bin/escs
#	@chmod 744 $(INTROOT)/app-defaults/escsStartup.xml
	@sed -i 's/MASTERHOST/$(MASTER)/g' $(INTROOT)/app-defaults/discosStartup.xml
	@chmod 700 $(INTROOT)/app-defaults/discosStartup.xml 
	@chmod 700 $(INTROOT)/bin/discosdown 
	@chmod 700 $(INTROOT)/bin/discosup 
	@echo " . . . installation done"	

#___oOo___
