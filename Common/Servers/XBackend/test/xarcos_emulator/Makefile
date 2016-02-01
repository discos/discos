OBJS=	Adc.o \
	ArcosEmulator.o \
	CpldInterface.o \
	Dbbc.o \
	Emulator.o \
	FftSpectrometer.o \
	IntThread.o \
	Semaphore.o \
	SingleSpectrometer.o \
	SocketConnection.o \
	SocketServer.o \
	Spectrometer.o \
	Thread.o \
	TpThread.o

CC=g++
 
CPPFLAGS= -Wall -g -c

TARGET= emulator

all:	$(TARGET)

emulator:	Emulator.o $(OBJS)
	$(CC) -g -o emulator $(OBJS)  -pthread

clean:
	\rm *.o $(TARGET)
