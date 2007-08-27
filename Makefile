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

# FIXME: duplicated from sfdl-compiler makefile.
ifdef TOOLS_DIR
HAPPYFLAGS += --template=$(TOOLS_DIR)/usr/share/happy-1.16
ALEXFLAGS += --template=$(TOOLS_DIR)/usr/share/alex-2.1.0
endif

BNFCDIR = bnfc/Json

# require BNFC 2.3b or newer.
# produces both C++ and Haskell code.
bnfc:
	(cd bnfc && bnfc -haskell -m -d Json.cf && mv Makefile Makefile.haskell)
	happy $(HAPPYFLAGS) -gca $(BNFCDIR)/Par.y
	alex $(ALEXFLAGS) -g $(BNFCDIR)/Lex.x
	(cd bnfc && bnfc -m -cpp_stl Json.cf)
	$(MAKE) -C bnfc Lexer.C Parser.C Json.ps


install: $(TARGETS)
	$(INSTALL) $^ $(LEEDS_LIB)


libjson.so: $(LIBOBJS); $(CXXLINK)
libjson.a: $(LIBOBJS);  $(AR_CMD)


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)

include $(SHARED_DIR)/footer.make


.PHONY: bnfc
