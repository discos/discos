#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.4 2010-10-14 13:09:44 spoppi Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# spoppi  02/12/09  created
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
#USER_LIB = 	-lACE 
	


#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
EXECUTABLES     =
EXECUTABLES_L   = 
#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = HolographyImpl 
LIBRARIES_L     =

#
# <brief description of xxxxx program>
HolographyImpl_OBJECTS   =HolographyImpl 					SenderThread    Configuration
#xxxxx_LDFLAGS   =
HolographyImpl_LIBS =   ComponentErrors \
			AntennaErrors \
			AntennaBossStubs \
			AntennaDefinitionsStubs\
			BackendsErrors  \
			HolographyStubs\
			GenericBackendStubs\
			ManagmentDefinitionsStubs\
			ACSBulkDataError\
			ReceiversDefinitionsStubs \
			BackendsDefinitionsStubs\
			bulkDataStubs \
			bulkDataSenderStubs \
			bulkDataReceiverStubs\
			ACSBulkDataError\
			ACSErrTypeFPGACommunication \
			ACSErrTypeFPGAConnection \
			IRALibrary\
			DigitalXCorrelatorStubs \
			ManagementErrors \
			ClientErrors

DigitalXCorrelatorImpl_LIBS    = ACSErrTypeFPGAConnection ACSErrTypeFPGACommunication


#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =


#
# <brief description of lllll library>
lllll_OBJECTS   =

#
# Scripts (public and local)
# ----------------------------
SCRIPTS         =
SCRIPTS_L       =

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     =
TCL_SCRIPTS_L   =

#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         =
PY_SCRIPTS_L       =

PY_MODULES         = corr2serial
PY_MODULES_L       =

PY_PACKAGES        = DigitalXCorrelatorSimImpl DigitalXCorrelatorImpl serial
PY_PACKAGES_L      =
pppppp_MODULES	   =

#
# <brief description of tttttt tcl-script>
tttttt_OBJECTS  =
tttttt_TCLSH    = 
tttttt_LIBS     = 

#
# TCL libraries (public and local)
# ------------------------------
TCL_LIBRARIES   =
TCL_LIBRARIES_L =

#
# <brief description of tttlll library>
tttlll_OBJECTS  = 

#
# Configuration Database Files
# ----------------------------
# CDB_SCHEMAS = Holography DigitalXCorrelator

# 
# IDL Files and flags
# 
#IDL_FILES = Holography
TAO_IDLFLAGS =
USER_IDL =
#
# Jarfiles and their directories
#
JARFILES= 
jjj_DIRS=
jjj_EXTRAS= 
#
# java sources in Jarfile on/off
DEBUG= 
#
# ACS XmlIdl generation on/off
#
XML_IDL= 
#
# Java Component Helper Classes generation on/off
#
COMPONENT_HELPERS=
#
# Java Entity Classes generation on/off
#
XSDBIND=
#
# Schema Config files for the above
# #
XSDBIND_INCLUDE=
# man pages to be done
# --------------------
MANSECTIONS =
MAN1 =
MAN3 =
MAN5 =
MAN7 =
MAN8 =

#
# local man pages
# ---------------
MANl =

#
# ASCII file to be converted into Framemaker-MIF
# --------------------
ASCII_TO_MIF = 

#
# other files to be installed
#----------------------------
INSTALL_FILES =

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
