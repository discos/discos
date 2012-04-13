CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		XInterfaces.o

LIBS =

TARGET =	XInterfaces

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
