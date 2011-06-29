# Makefile of Receivers
#
#-----------------------------------------------------------------------
# Marco Buttu (mbuttu@oa-cagliari.inaf.it)
#------------------------------------------------------------------------

# Libraries (public and local)
# ----------------------------

CDB_SCHEMAS = SRTKBandMF

LIBRARIES       = SRTKBandMFReceiverImpl

SRTKBandMFReceiverImpl_OBJECTS = SRTKBandMFReceiverImpl ReceiverSocket utils Talk
SRTKBandMFReceiverImpl_LIBS = ReceiversDefinitionsStubs CommonReceiverInterfaceStubs GenericReceiverStubs \
						   SRTKBandMFStubs IRALibrary ComponentErrors \
						   ReceiversErrors ManagementErrors


# List of all possible C-sources (used to create automatic dependencies)
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))


# INCLUDE STANDARDS
MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif


# TARGETS
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all 
	$(RM) *~ ../include/*~ ../idl ../*~ ../../*~ \
		../bin ../lib ../rtai ../object
	$(RM) ../doc/html
	$(RM) tmp.txt acsexmplbeans.jar ../doc/abeans.log
	@echo " . . . clean done"

clean_dist : clean clean_dist_all 
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

