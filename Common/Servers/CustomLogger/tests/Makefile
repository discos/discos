# CPP UNIT TESTING SETUP
#--------------
GTEST_HOME=/usr/local/include/gtest
GMOCK_HOME=/usr/local/include/gmock
GTEST_LIBS=gtest gtest_main

USER_INC=-I$(GTEST_HOME) -I$(GMOCK_HOME)
# END OF CPP UNIT TESTING SETUP
#---------------------

# DEFINE YOUR CPP UNIT TEST EXECUTABLES HERE as:
#
# EXECTUABLES_L = unittest
# unittest_OBJECTS = unittest
# unittest_LIBS = $(GTEST_LIBS) <ComponentNameImpl>

EXECUTABLES_L = unittest
unittest_OBJECTS = unittest
unittest_LIBS = $(GTEST_LIBS) CustomLoggerImpl
unittest_LDFLAGS = -lstdc++ -lpthread

# END OF CUSTOMIZATION
# do not edit below this line
#----------------------------

CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

MAKEDIRTMP := $(shell searchFile include/acsMakefile)
ifneq ($(MAKEDIRTMP),\#error\#)
   MAKEDIR := $(MAKEDIRTMP)/include
   include $(MAKEDIR)/acsMakefile
endif

# TEST TARGETS
#TODO: unittest(2) discover pyunit

do_unit: all
	@echo "running cpp unit tests"
	../bin/unittest --gtest_output=xml:results/cppunittest.xml

do_pyunit:
	@echo "running python unit tests"
	nosetests -v pyunit

do_functional:
	@echo "running python functional tests"
	mkdir -p /tmp/events
	rm -f /tmp/events/*	
	nosetests -v functional
	rm -f /tmp/events/*	

do_external:
	@echo "running python external tests"
	nosetests -v external

clean_test:
	rm -f results/*.xml
	rm -f functional/*.pyc
	rm -f pyunit/*.pyc
	rm -f external/*.pyc

unit: do_unit
	@echo " . . . 'unit' done"

pyunit: do_pyunit
	@echo " . . . 'pyunit' done"

functional: do_functional
	@echo " . . . 'functional' done"

external: do_external
	@echo " . . . 'external' done"

# TARGETS
# -------
all:	do_all
	@echo " . . . 'all' done" 

clean : clean_all clean_test
	@echo " . . . clean done"

clean_dist : clean_all clean_dist_all clean_test
	@echo " . . . clean_dist done"

man   : do_man 
	@echo " . . . man page(s) done"

install : install_all
	@echo " . . . installation done"

