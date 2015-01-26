#*******************************************************************************
# PPPPPPPP
#
# "@(#) $Id: Makefile,v 1.4 2010-08-13 06:32:24 bartolini Exp $"
#
# Makefile of ........
#
# who        when      what
# --------   --------  ----------------------------------------------
# bartolini  23/01/14  created
#
#
# Python stuff (public and local)
# ----------------------------
PY_SCRIPTS         = getTemplateForTest
PY_MODULES         = gettemplatefortest

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

clean_templates:
	rm -rf $(INTROOT)/templates/test
	@echo " . . . templates removed from introot"

clean : clean_all clean_templates
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all clean_templates
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install_templates:
	cp -r ../templates/test $(INTROOT)/templates
	@echo " . . . templates installed"

install : install_all install_templates
	@echo " . . . installation done"


#___oOo___
