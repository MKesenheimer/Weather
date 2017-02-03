########################################################################
#                          -*- Makefile -*-                            #
########################################################################

COMPILER = clang++ -std=c++11

########################################################################
## Flags
FLAGS   = -g
LDFLAGS =
PREPRO  =
# DEBUG Level
LEVEL = 0
DEBUG = -DDEBUG=$(LEVEL)

### generate directory obj, if not yet existing
$(shell mkdir -p build)

########################################################################
## Paths

WORKINGDIR = $(shell pwd)
PARENTDIR  = $(WORKINGDIR)/..
TOOLS = $(WORKINGDIR)/Tools
OPENNN = $(TOOLS)/OpenNN
RJSON = $(TOOLS)/rapidjson

########################################################################
## search for the files and set paths

vpath %.cpp $(WORKINGDIR)
vpath %.a $(WORKINGDIR)/build
UINCLUDE = $(WORKINGDIR)/include
NNINCLUDE = $(OPENNN)/opennn
RJSONINCLUDE = $(RJSON)/include

########################################################################
## Includes
CXX  = $(COMPILER) $(FLAGS) $(OPT) $(WARN) $(DEBUG) $(PREPRO) -I$(UINCLUDE)
CXX += -I$(NNINCLUDE) -I$(RJSONINCLUDE)
INCLUDE = $(wildcard *.h $(UINCLUDE)/*.h $(NNINCLUDE)/*.h $(RJSONINCLUDE)/*.h)

%.a: %.cpp $(INCLUDE)
	$(CXX) -c -o build/$@ $<

# Libraries
LIB = -lcurl 
LIB += $(TOOLS)/libopennn.a
LIB += $(TOOLS)/libtinyxml2.a

# Frameworks
FRM = 

########################################################################
## Linker files

### USER Files ###
USER = Curl.a Interface.a Functions.a JData.a

########################################################################
## Rules
## type make -j4 [rule] to speed up the compilation

BUILD1 = $(USER) Main.a
BUILD2 = $(USER) Crawler.a

Weather: $(BUILD1)
	  $(CXX) $(patsubst %,build/%,$(BUILD1)) $(LDFLAGS) $(LIB) $(FRM) -o $@

Crawler: $(BUILD2)
	  $(CXX) $(patsubst %,build/%,$(BUILD2)) $(LDFLAGS) $(LIB) $(FRM) -o $@

libopennn.a: 
	cd $(OPENNN) && mkdir -p build
	cd $(OPENNN)/build && cmake .. && make
	cp $(OPENNN)/build/opennn/*.a $(TOOLS)
	cp $(OPENNN)/build/tinyxml2/*.a $(TOOLS)

clean:
	rm -f build/*.a Weather Crawler

clean-all:
	rm -f build/*.a Weather
	cd $(OPENNN) && rm -rf build
	rm $(TOOLS)/*.a

do:
	make && ./Weather

########################################################################
#                       -*- End of Makefile -*-                        #
########################################################################
