#***********************************************************************
#   Makefile of Receivers Errors
#
#   Author: 
#       Marco Buttu, mbuttu@oa-cagliari.inaf.it
#***********************************************************************

MAKE_PDF = ON

ACSERRDEF = ReceiversErrors DerotatorErrors

ERRXMLFILE_1 = ../idl/ReceiversErrors.xml 
ERRXMLFILE_2 = ../idl/DerotatorErrors.xml

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
PERM_1:=$(shell stat --format=%a $(ERRXMLFILE_1))
PERM_2:=$(shell stat --format=%a $(ERRXMLFILE_2))
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
	@chmod $(PERM_1) $(ERRXMLFILE_1)
	@chmod $(PERM_2) $(ERRXMLFILE_2)
	@echo "Patch applied" 
	@rm -rf /tmp/tmp.txt
	@echo " . . . installation done"

xmlpatch_pre :
	@echo "File permissions: " $(PERM_1) $(PERM_2)
	@chmod a+w $(ERRXMLFILE_1)
	@chmod a+w $(ERRXMLFILE_2)
	@echo $(PERM_1) > /tmp/tmp.txt


#___oOo___
