#*******************************************************************************
#
# "@(#) $Id: Makefile,v 1.1 2011-03-24 16:47:39 m.buttu Exp $"
#
# Makefile of MinorServo's Libraries
#
# who       when      what
# --------  --------  ----------------------------------------------
# almamgr  28/07/05  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

#
# user definable C-compilation flags
USER_CFLAGS = -Wall

#
# additional include and library search paths
# USER_INC = /usr/local/include
# USER_LIB =

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

xxxxx_OBJECTS   =
xxxxx_CFLAGS    =
xxxxx_LIBS      =

#
# <brief description of xxxxx program>
xxxxx_OBJECTS   =
xxxxx_LDFLAGS   =
xxxxx_LIBS      =

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        = SRTMinorServoCommandLibrary.h SRTMinorServoAnswerMap.h SRTMinorServoSocket.h SRTMinorServoTestingSocket.h SRTMinorServoUtils.h #hexlib.h

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       = SRTMinorServoLibrary SRTMinorServoCommandLibrary SRTMinorServoSocketLibrary PySRTMinorServoCommandLibrary
LIBRARIES_L     =
SRTMinorServoLibrary_OBJECTS = hexlib
SRTMinorServoLibrary_LIBS = gsl gslcblas m
SRTMinorServoCommandLibrary_OBJECTS = SRTMinorServoCommandLibrary
SRTMinorServoCommandLibrary_CFLAGS = -std=c++17 -fconcepts
SRTMinorServoCommandLibrary_LIBS = pthread IRALibrary
PySRTMinorServoCommandLibrary_OBJECTS = PySRTMinorServoCommandLibrary
PySRTMinorServoCommandLibrary_CFLAGS = -std=c++17 -fconcepts
PySRTMinorServoCommandLibrary_LIBS = SRTMinorServoCommandLibrary boost_python3
SRTMinorServoSocketLibrary_OBJECTS = SRTMinorServoSocket
SRTMinorServoSocketLibrary_LIBS = IRALibrary ComponentErrors MinorServoErrors SRTMinorServoCommandLibrary
SRTMinorServoSocket_CFLAGS = -std=c++17 -fconcepts

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

PY_MODULES         =
PY_MODULES_L       =

PY_PACKAGES        = #SRTMinorServoCommandLibrary
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
CDB_SCHEMAS = 

# 
# IDL Files and flags
# 
IDL_FILES =
IDL_TAO_FLAGS =
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
#
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
