#*******************************************************************************
# Makefile of XEphemAstroLib tests
#
# targets:
#   make all         build the unit and functional (thread safety) tests
#   make unit        run the unit tests
#   make functional  run the functional, multi threaded, tests
#   make tsan        build and run the functional tests under ThreadSanitizer
#                    (needs libtsan; ASLR is disabled through setarch, as
#                    required by recent kernels)
#*******************************************************************************

GTEST_HOME=/usr/local/include/gtest
GMOCK_HOME=/usr/local/include/gmock
GTEST_LIBS=gtest gtest_main

USER_INC=-I$(GTEST_HOME) -I$(GMOCK_HOME)
USER_CFLAGS = -pthread

EXECUTABLES_L = unittest functionaltest

unittest_OBJECTS = unittest
unittest_LIBS = $(GTEST_LIBS) astro
unittest_LDFLAGS = -lstdc++ -lpthread -lm

functionaltest_OBJECTS = functionaltest
functionaltest_LIBS = $(GTEST_LIBS) astro
functionaltest_LDFLAGS = -lstdc++ -lpthread -lm

#
# list of all possible C-sources (used to create automatic dependencies)
# ------------------------------
CSOURCENAMES = \
	$(foreach exe, $(EXECUTABLES) $(EXECUTABLES_L), $($(exe)_OBJECTS)) \
	$(foreach rtos, $(RTAI_MODULES) , $($(rtos)_OBJECTS)) \
	$(foreach lib, $(LIBRARIES) $(LIBRARIES_L), $($(lib)_OBJECTS))

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

# sources of the library, compiled again with the sanitizer instrumentation
LIB_SRC_DIR = ../src
LIB_SOURCES = $(addprefix $(LIB_SRC_DIR)/, $(addsuffix .c, \
    aa_hadec aberration actan airmass anomaly ap_as atlas auxil \
    bdl chap95 chap95_data circum comet constel dbfmt deep deltat earthsat \
    eq_ecl eq_gal formats helio jupmoon libration magdecl marsmoon misc mjd \
    moon mooncolong moonnf nutation obliq parallactic parallax plans plmoon \
    plshadow precess reduce refract rings riset riset_cir satmoon sdp4 sgp4 \
    sphcart sun thetag twobody umoon utc_gst vsop87 vsop87_data))
TSAN_DIR = ../object/tsan

do_unit: all
	@echo "running cpp unit tests"
	mkdir -p results
	../bin/unittest --gtest_output=xml:results/cppunittest.xml

do_functional: all
	@echo "running cpp functional (thread safety) tests"
	mkdir -p results
	../bin/functionaltest --gtest_output=xml:results/cppfunctionaltest.xml

do_tsan:
	@echo "running cpp functional tests under ThreadSanitizer"
	mkdir -p $(TSAN_DIR)
	for f in $(LIB_SOURCES); do \
	    gcc -c -g -O1 -fsanitize=thread -pthread -I../include -o $(TSAN_DIR)/$$(basename $$f .c).o $$f || exit 1; \
	done
	g++ -g -O1 -fsanitize=thread -pthread -I../include $(USER_INC) \
	    -o $(TSAN_DIR)/functionaltest functionaltest.cpp $(TSAN_DIR)/*.o \
	    -L$(INTROOT)/lib -L$(ACSROOT)/lib -lgtest -lgtest_main -lm
	XEPHEM_TEST_REDUCE=20 TSAN_OPTIONS=halt_on_error=1 \
	    setarch $$(uname -m) -R $(TSAN_DIR)/functionaltest

clean_test:
	rm -f results/*.xml
	rm -rf $(TSAN_DIR)

unit: do_unit
	@echo " . . . 'unit' done"

functional: do_functional
	@echo " . . . 'functional' done"

tsan: do_tsan
	@echo " . . . 'tsan' done"

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
