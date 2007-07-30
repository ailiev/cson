# -*- makefile -*-

include config.make

include $(SHARED_DIR)/utils.make
include $(SHARED_DIR)/common.make
include $(SHARED_DIR)/shared-targets.make


TARGETS=libjson.$(LIBEXT)

CPPFLAGS += -I.. -I../pir

vpath %.C bnfc
LIBSRCS = get-path.cc Absyn.C Lexer.C Parser.C Printer.C Skeleton.C
TESTSRCS = $(wildcard test-*.cc)

CPPFLAGS += -I.

LIBDIRS		+= $(DIST_LIB) . ../common
LDLIBFILES	+= -lcommon

all: $(TARGETS)

# require BNFC 2.3b or newer.
# produces both C++ and Haskell code.
bnfc:
	(cd bnfc && bnfc -haskell -m -d Json.cf && mv Makefile Makefile.haskell)
	$(MAKE) -C bnfc -f Makefile.haskell
	(cd bnfc && bnfc -m -cpp_stl Json.cf)
	$(MAKE) -C bnfc Lexer.C Parser.C JSON.ps



install: $(TARGETS)
	$(INSTALL) $^ $(LEEDS_LIB)


libjson.so: $(LIBOBJS); $(CXXLINK)
libjson.a: $(LIBOBJS);  $(AR_CMD)


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)

include $(SHARED_DIR)/footer.make


.PHONY: bnfc
