#*****************************************
#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------
#*****************************************

PY_PACKAGES = SRTDerotatorSim
PY_SCRIPTS = 

# -----------------
# Include Standards
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
	$(RM) *.pyc SRTDerotatorSim/*.pyc
	$(RM) ../lib ../bin ../config ../doc ../idl ../include \
		  ../object ../rtai ../test
	$(RM) $(INTROOT)/lib/python/site-packages/SRTDerotatorSim*
	@echo " . . . clean done"

install : install_all
	@echo " . . . installation done"

#___oOo___
