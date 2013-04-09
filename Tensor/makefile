CC_SOURCES = $(shell ls -R ./*cc ./lib/*/*cc)
CC_OBJECTS = $(CC_SOURCES:.cc=.o)
OBJECTS = $(CC_OBJECTS)

OSNAME := $(shell uname -s)

MACHNAME := $(shell uname -n | sed 's/[0-9].*//')
ifeq ($(OSNAME),Darwin)
	include makefile.osx
endif

# this is what we're building
all: client

# the principal target
client: $(OBJECTS) makefile
	nvcc $(NVCC_CFLAGS) $(NVCC_LFLAGS) -o main -g $(OBJECTS)

$(CC_OBJECTS):
	touch $@
	nvcc $(NVCC_CFLAGS) -c -o $@ $(@:.o=.cc)

# create source documentation
docs:
	doxygen

# clean the installation
clean:
	rm -rfv main ./lib/*/*.o ./*.o doc
