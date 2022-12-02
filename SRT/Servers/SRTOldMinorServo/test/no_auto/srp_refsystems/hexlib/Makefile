CC	=	g++
CFL	=	-W -lgsl -lgslcblas -lm

default:	all

all:	virtual2real real2virtual

virtual2real:	virtual2real.c hexlib.o
	$(CC) $(CFL) virtual2real.c hexlib.o -o virtual2real

real2virtual:	real2virtual.c hexlib.o
	$(CC) $(CFL) real2virtual.c hexlib.o -o real2virtual

clean:
	rm -f *.o
	rm -f *~
	rm -f virtual2real real2virtual


