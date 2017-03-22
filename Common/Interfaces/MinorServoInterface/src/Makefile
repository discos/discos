#**************************************************************************************
# E.S.O. - ACS project
#
# Makefile of MinorServo IDL
#
#--------------------------------------------------------------------------------------
# Marco Buttu (mbuttu@oa-cagliari.inaf.it)
#     Created: Mon Oct 12 12:08:05 CEST 2009
#     Last modified: Mon Nov  9 11:26:11 CET 2009
#--------------------------------------------------------------------------------------
#
#**************************************************************************************

#**************************************************************************************
# This Makefile follows VLT Standards (see Makefile(5) for more).
#**************************************************************************************

MAKE_PDF = ON

# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

#
# IDL FILES
#
IDL_FILES = MinorServo MinorServoBoss

#
# INCLUDE STANDARDS
# -----------------

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif


# TARGETS
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	$(RM) *~ ../include/*~ ../idl/*~ ../*~ ../../*~ core
	$(RM) ../doc/html
	$(RM) tmp.txt acsexmplbeans.jar ../doc/abeans.log
	@echo " . . . clean done"

clean_dist : clean clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	# cp ../doc/html/group__ACSEXMPLDOC.html ../doc/html/main.html
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

#___oOo___
