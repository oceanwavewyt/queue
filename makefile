CC:=g++
CXX:=g++
DEBUG=DEBUG
OUTPUTNAME:=libqueue.so
OUTPATH=output
INCLUDES += -I/usr/include/ -I./include/

$(shell CC=$(CC) CXX=$(CXX) TARGET_OS=$(TARGET_OS) \
    ./build_detect_platform build_config.mk ./)
include build_config.mk

LDFLAGS +=-shared -Wl,-soname -Wl,$(OUTPUTNAME)  -lpthread -ltcmalloc
 
CPPFLAGS += -g -fPIC -pg -Wall
OS := $(shell uname -s)
IS_DARWIN := $(shell echo $(OS)|grep -i Darwin)
ifdef IS_DARWIN
LDFLAGS += -D_DARWIN
endif

objects := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
objects += $(patsubst util/%.cpp,util/%.o,$(wildcard util/*.cpp))

.PHONY: all clean
all: $(OUTPUTNAME)


%.o:%.cpp
	$(CXX) $(CPPFLAGS) -c $< -o $@ $(INCLUDES) -D$(DEBUG)

clean:
	-rm -f $(OUTPUTNAME) $(objects)

$(OUTPUTNAME):$(objects)
	$(CC) $(LDFLAGS) $(objects) -o $(OUTPUTNAME) 

