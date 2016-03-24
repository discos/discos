#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.1 2008-10-02 15:41:57 a.orlati Exp $"
#
# Makefile of ........
#
# who       when      what
# --------  --------  ----------------------------------------------
# andrea  02/10/08  created
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

MAKE_PDF = ON

ACSERRDEF=BackendsErrors

ERRXMLFILE= ../idl/BackendsErrors.xml

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

install : install_all
	@chmod $(PERM) $(ERRXMLFILE)
	@echo "Patch applied" 
	@rm -rf /tmp/tmp.txt
	@echo " . . . installation done"

xmlpatch_pre :
	@echo "File permissions: " $(PERM)
	@chmod a+w $(ERRXMLFILE)
	@echo $(PERM) > /tmp/tmp.txt


#___oOo___
