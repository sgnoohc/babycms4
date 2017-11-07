#########################################################################
# Below list your packages

PACKAGES = rooutil CORE COREHelper
TARGETNAME = babycms4

#########################################################################
# Common section
#########################################################################

DIRS := . $(shell find $(PACKAGES) -type d)
GARBAGE_PATTERNS := *.o *~ core .depend .*.cmd *.ko *.mod.c
GARBAGE := $(foreach DIR,$(DIRS),$(addprefix $(DIR)/,$(GARBAGE_PATTERNS)))

SRCS=$(wildcard *.cc)
HDRS=$(SRCS:.cc=.h)
OBJS=$(SRCS:.cc=.o)

ROOTLIBS:= $(shell root-config --libs) -lTMVA -lEG -lGenVector -lXMLIO -lMLP -lTreePlayer -lRooFit -lRooFitCore

all: $(PACKAGES) $(OBJS)
	g++ -o $(TARGETNAME) $(OBJS) $(ROOTLIBS) $(addprefix -L,$(PACKAGES)) $(addprefix -l,$(PACKAGES))

%.o: %.cc
	g++ -Wunused-variable -g -O2 -Wall -fPIC -Wshadow -Woverloaded-virtual $(shell root-config --cflags) -c $< -o $@

$(PACKAGES):
	make -j 15 -C $@
	ln -sf $(notdir $(shell ls $@/*$@.so | grep -v lib)) $@/lib$@.so

cleanall:
	rm -rf $(GARBAGE)

clean:
	rm -rf *.o
	rm -f $(TARGETNAME)

.PHONY: $(PACKAGES)
