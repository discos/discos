#*****************************************
#-----------------------------------------
# Marco Buttu <mbuttu@oa-cagliari.inaf.it>
#-----------------------------------------
#*****************************************

PY_PACKAGES = acswrapper

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
	$(RM) *.pyc acswrapper/*.pyc
	$(RM) ../lib ../bin ../config ../doc ../idl ../include \
		  ../object ../rtai ../test
	$(RM) $(INTROOT)/lib/python/site-packages/acswrapper*
	@echo " . . . clean done"

install : install_all
	@echo " . . . installation done"

#___oOo___
