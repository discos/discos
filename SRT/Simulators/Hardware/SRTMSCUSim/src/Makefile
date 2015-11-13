#*****************************************
#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------
#*****************************************

PY_PACKAGES = SRTMSCUSimImpl
PY_SCRIPTS = srt-mscu-sim

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
	$(RM) *.pyc SRTMSCUSimImpl/*.pyc
	$(RM) ../lib ../bin ../config ../doc ../idl ../include \
		  ../object ../rtai ../test
	$(RM) $(INTROOT)/lib/python/site-packages/SRTMSCUSim*
	$(RM) $(INTROOT)/bin/srt-mscu-*
	@echo " . . . clean done"

install : install_all
	@echo " . . . installation done"

#___oOo___
