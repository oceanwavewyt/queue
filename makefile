CC:=g++
CXX:=g++
DEBUG=DEBUG
OUTPUTNAME:=bqueue
OUTPATH=output
INCLUDES += -I/usr/include/

LDFLAGS += -lpthread
CPPFLAGS += -g -fPIC -pg -Wall
OS := $(shell uname -s)
IS_DARWIN := $(shell echo $(OS)|grep -i Darwin)
ifdef IS_DARWIN
LDFLAGS += -D_DARWIN
endif

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

.PHONY: all clean
all: $(OUTPUTNAME)


%.o:%.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@ $(INCLUDES) -D$(DEBUG)

clean:
	-rm -f $(OUTPUTNAME) $(objects)

$(OUTPUTNAME):$(objects)
	$(CC) -o $(OUTPUTNAME) $(objects) $(LDFLAGS) -rdynamic

