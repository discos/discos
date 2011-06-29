#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.2 2007-09-17 14:47:59 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  30/03/07  created
#

#*******************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#*******************************************************************************
# REMARKS
#    None
#------------------------------------------------------------------------

ACSERRDEF= ComponentErrors

ERRXMLFILE= ../idl/ComponentErrors.xml

#
# MODULE CODE DESCRIPTION:
# ------------------------
# As a general rule:  public file are "cleaned" and "installed"  
#                     local (_L) are not "installed".



#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
#>>>>> END OF standard rules

TMP := $(shell searchFile /tmp/tmp.txt)
ifeq ($(TMP),\#error\#)
PERM:=$(shell stat --format=%a $(ERRXMLFILE))
else 
PERM=$(shell cat /tmp/tmp.txt) 
endif

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

install : xmlpatch_pre install_all
	@chmod $(PERM) $(ERRXMLFILE)
	@echo "Patch applied" 
	@rm -rf /tmp/tmp.txt
	@echo " . . . installation done"

xmlpatch_pre :
	@echo "File permissions: " $(PERM)
	@chmod a+w $(ERRXMLFILE)
	@echo $(PERM) > /tmp/tmp.txt


#___oOo___
