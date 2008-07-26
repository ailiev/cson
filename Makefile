#
# Simple library to parse and access a JSON-like mini-language for representing
# structures.
#
# Copyright (C) 2003-2007, Alexander Iliev <sasho@cs.dartmouth.edu> and
# Sean W. Smith <sws@cs.dartmouth.edu>
#
# All rights reserved.
#
# This code is released under a BSD license.
# Please see LICENSE.txt for the full license and disclaimers.
#

include config.make

include $(SHARED_DIR)/utils.make
include $(SHARED_DIR)/common.make
include $(SHARED_DIR)/shared-targets.make

PATH += :$(TOOLS_DIR)/usr/bin

TARGET_LIBS = libjson.$(LIBEXT)
TARGET_EXES = get-path
TARGETS= $(TARGET_LIBS) $(TARGET_EXES)

all: bnfc $(TARGETS)


get-path :: LDLIBFILES += -ljson
get-path : test-get-path.o

CPPFLAGS += -I.. -I../pir

BNFC_ARTIFACTS = Absyn Printer Skeleton Lexer Parser

#vpath %.C $(BNFCDIR)
#vpath %.o $(BNFCDIR)
LIBSRCS = get-path.cc $(bnfc_cpp_files)
# $(patsubst %,%.C,$(BNFC_ARTIFACTS))
TESTSRCS = $(wildcard test-*.cc)

CPPFLAGS += -I.

LIBDIRS		+= $(DIST_LIB) . ../common
LDLIBFILES	+= -lcommon


# FIXME: duplicated from sfdl-compiler makefile.
ifdef TOOLS_DIR
#HAPPYFLAGS += --template=$(TOOLS_DIR)/usr/share/happy-1.16
#ALEXFLAGS += --template=$(TOOLS_DIR)/usr/share/alex-2.1.0
endif

# The bnfc-generated files go in here.
# This dir contains only generated files and can be removed.
BNFCDIR = bnfc/Json

# require BNFC 2.3b or newer.
# produces both C++ and Haskell code.
bnfc: bnfc_pre bnfc_cpp bnfc_haskell

bnfc_pre:
	mkdir -p $(BNFCDIR)

bnfc_cpp_files=$(patsubst %,$(BNFCDIR)/%.C,$(BNFC_ARTIFACTS))
bnfc_cpp: $(bnfc_cpp_files)
$(bnfc_cpp_files): bnfc/Json.cf
	(cd $(BNFCDIR) && bnfc -m -cpp_stl ../Json.cf)
	$(MAKE) -C $(BNFCDIR) Lexer.C Parser.C Json.ps
	ps2pdf $(BNFCDIR)/Json.ps $(BNFCDIR)/Doc.pdf

bnfc_haskell_files=$(patsubst %,$(BNFCDIR)/%.hs,$(BNFC_ARTIFACTS))
bnfc_haskell: $(bnfc_haskell_files)
# Explicit calls to happy and alex are so we can pass them custom flags.
# Otherwise the generated Makefile.haskell can invoke them too.
$(bnfc_haskell_files): bnfc/Json.cf
	(cd bnfc && bnfc -haskell -m -d Json.cf && mv Makefile Json/Makefile.haskell)
	happy $(HAPPYFLAGS) -gca $(BNFCDIR)/Par.y
	alex $(ALEXFLAGS) -g $(BNFCDIR)/Lex.x


install: install_libs install_exes

# make sure we do not try to install bnfc!
install_libs: $(TARGET_LIBS)
	$(INSTALL) $^ $(LEEDS_LIB)
install_exes: $(TARGET_EXES)
	$(INSTALL) $^ $(LEEDS_BIN)


libjson.so: $(LIBOBJS); $(CXXLINK)
libjson.a: $(LIBOBJS);  $(AR_CMD)


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)

include $(SHARED_DIR)/footer.make


.PHONY: bnfc bnfc_haskell bnfc_cpp bnfc_pre
