#*******************************************************************************
# 
#
# "@(#) $Id: Makefile,v 1.7 2010-12-06 12:48:26 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  12/10/06  created
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
USER_LIB =      -lACE \
		 		-lTAO \
				-lTAO_DsLogAdmin \
				-lTAO_CosNaming \
				-lTAO_IORTable \
				-lTAO_PortableServer \
				-lTAO_Svc_Utils \
				-lTAO_CosTrading \
                -lTAO_CosNotification \
				-lTAO_DynamicAny \
				-lTAO_IFR_Client \
                -lTAO_CosProperty \
				-lacsutil \
				-lcdb \
				-llogging \
				-lacscomponent \
				-lbaci \
				-lmaci \
				-lmaciClient \
				-lacserr \
				-lm \
				-lloki \
				-lncurses

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     = _tui_MedicinaMountTextClient
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
_tui_MedicinaMountTextClient_OBJECTS = MedicinaMountTextClient
_tui_MedicinaMountTextClient_LIBS = acsnc MountStubs MedicinaMountStubs IRALibrary \
TextWindowLibrary ComponentErrors AntennaErrors ManagementErrors ClientErrors AntennaDefinitionsStubs ManagmentDefinitionsStubs

SCRIPTS         = mountTui

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
	@echo " . . . installation done"


#___oOo___
