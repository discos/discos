# test        python functional tests of the component (needs a running ACS)
# unit        C++ unit tests of the libastro wrapper (needs gtest and libastro)

UNIT_INC = -I../include -I$(INTROOT)/include -I$(ACSROOT)/include -I/usr/local/include
UNIT_LIB = -L$(INTROOT)/lib -L$(ACSROOT)/lib -L/usr/local/lib

test:
	find . -type d -name "__pycache__" -exec rm -rf {} +
	python -B -m unittest discover -v

unit: wrapper_test
	./wrapper_test

wrapper_test: wrapper_test.cpp ../src/libastrowrapper.cpp ../include/libastrowrapper.h
	g++ -std=c++17 -Wall -pthread $(UNIT_INC) -o $@ wrapper_test.cpp ../src/libastrowrapper.cpp \
	    $(UNIT_LIB) -lastro -lgtest -lgtest_main -lm

clean:
	rm -f wrapper_test

.PHONY: test unit clean
