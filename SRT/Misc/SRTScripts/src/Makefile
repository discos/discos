#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.1 2010-08-12 10:03:15 c.migoni Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# almamgr  03/04/09  created
# orlati   28/06/18  nuraghe script moved in local install. It has been deprecated
# orlati   29/06/18  added discosStartup.xml file for acscommandcenter

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
#USER_INC = -I/usr/local/include/gsl
#USER_LIB = -lgsl \
		   -lgslcblas

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".

#
# C programs (public and local)
# -----------------------------
#EXECUTABLES     = SRTActiveSurfaceActuatorsPositions
EXECUTABLES     = 
EXECUTABLES_L   = 

#
# <brief description of xxxxx program>
#SRTActiveSurfaceActuatorsPositions_OBJECTS   =	SRTActiveSurfaceActuatorsPositions
#xxxxx_LDFLAGS   =
#SRTActiveSurfaceActuatorsPositions_LIBS      =

#
# special compilation flags for single c sources
#yyyyy_CFLAGS   = 

#
# Includes (.h) files (public only)
# ---------------------------------
INCLUDES        =

#
# Libraries (public and local)
# ----------------------------
LIBRARIES       =
LIBRARIES_L     =

#
# <brief description of lllll library>
lllll_OBJECTS   =

#
# Scripts (public and local)
# ----------------------------
SCRIPTS         =  discosup discosdown discosConsole
SCRIPTS_L       =  nuragheConsole nuraghe

#
# TCL scripts (public and local)
# ------------------------------
TCL_SCRIPTS     =
TCL_SCRIPTS_L   =

#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         = receiversmonitor.py
PY_SCRIPTS_L       =

PY_MODULES         =
PY_MODULES_L       =

PY_PACKAGES        =
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
#INSTALL_FILES = ../app-defaults/nuragheStartup.xml
INSTALL_FILES = ../app-defaults/discosStartup.xml ../app-defaults/simulationStartup.xml

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
	@rm -f $(INTROOT)/app-defaults/simulationStartup.xml
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
#	@chmod 764 $(INTROOT)/bin/nuraghe
#	@chmod 764 $(INTROOT)/app-defaults/nuragheStartup.xml
	@sed -i 's/MASTERHOST/$(MASTER)/g' $(INTROOT)/app-defaults/discosStartup.xml
	@chmod 700 $(INTROOT)/app-defaults/discosStartup.xml
	@sed -i 's/MASTERHOST/$(MASTER)/g' $(INTROOT)/app-defaults/simulationStartup.xml
	@chmod 700 $(INTROOT)/app-defaults/simulationStartup.xml
	@chmod 700 $(INTROOT)/bin/discosdown 
	@chmod 700 $(INTROOT)/bin/discosup 
	@chmod 700 $(INTROOT)/bin/receiversmonitor.py
	@echo " . . . installation done"


#___oOo___
