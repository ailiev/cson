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

# need this before including header.make
EXTRA_INCLUDE_DIRS = $(DIST_ROOT)/include

make_incl_dir=$(DIST_ROOT)/make_include/faerieplay/common

include $(make_incl_dir)/utils.make
include $(make_incl_dir)/header.make
include $(make_incl_dir)/shared-targets.make

HEADERS=get-path.h
BNFC_HEADERS=bnfc/Json/Absyn.H bnfc/Json/Parser.H bnfc/Json/Printer.H bnfc/Json/Skeleton.H


TARGET_LIBS = libjson.$(LIBEXT)
TARGET_EXES = get-path
TARGETS= $(TARGET_LIBS) $(TARGET_EXES)

all: init $(TARGETS)

install: init install_libs install_exes install_hdrs

# make sure we do not try to install bnfc!
.PHONY: install_libs install_exes install_hdrs
install_libs: $(TARGET_LIBS)
	$(INSTALL) $^ $(DIST_LIB)
install_exes: $(TARGET_EXES)
	$(INSTALL) $^ $(DIST_BIN)
install_hdrs: $(HEADERS) $(BNFC_HEADERS)
	mkdir -p $(DIST_ROOT)/include/json/bnfc/Json
	$(INSTALL) $(HEADERS) $(DIST_ROOT)/include/json
	$(INSTALL) $(BNFC_HEADERS) $(DIST_ROOT)/include/json/bnfc/Json


PATH += :$(TOOLS_DIR)/usr/bin

get-path :: LDLIBFILES += -ljson
# test-get-path.cc contains the main function; get-path.cc are library
# functions.
get-path : test-get-path.o 

# BNFC produces somewhat different-looking artifacts for C++ and Haskell.
BNFC_ARTIFACTS_CPP = Absyn Printer Skeleton Lexer Parser
BNFC_ARTIFACTS_HS = Abs ErrM Lex Par Print Skel Test

bnfc_cpp_files=$(patsubst %,$(BNFCDIR)/%.C,$(BNFC_ARTIFACTS_CPP))
bnfc_haskell_files=$(patsubst %,$(BNFCDIR)/%.hs,$(BNFC_ARTIFACTS_HS))


#vpath %.C $(BNFCDIR)
#vpath %.o $(BNFCDIR)
LIBSRCS = get-path.cc $(bnfc_cpp_files)
TESTSRCS = $(wildcard test-*.cc)

CPPFLAGS += -I.

# external libraries. they get added into LDLIBS in header.make
LIBDIRS		+= $(DIST_LIB) .
LDLIBFILES	+= -lfaerieplay-common


# duplicated from sfdl-compiler makefile.
ifdef TOOLS_DIR
#HAPPYFLAGS += --template=$(TOOLS_DIR)/usr/share/happy-1.16
#ALEXFLAGS += --template=$(TOOLS_DIR)/usr/share/alex-2.1.0
endif

# The bnfc-generated files go in here.
# This dir contains only generated files and can be removed.
BNFCDIR = bnfc/Json

# require BNFC 2.3b or newer.
# produces both C++ and Haskell code.
bnfc: bnfc_cpp bnfc_haskell

.PHONY: init
init::
	mkdir -p $(BNFCDIR)

#$(info bnfc_cpp_files=$(bnfc_cpp_files))
bnfc_cpp: $(bnfc_cpp_files)
$(bnfc_cpp_files): bnfc/Json.cf
	(cd $(BNFCDIR) && bnfc -m -cpp_stl ../Json.cf)
	ed $(BNFCDIR)/Json.y < add-include.ed
	$(MAKE) -C $(BNFCDIR) Lexer.C Parser.C Json.ps
	ps2pdf $(BNFCDIR)/Json.ps $(BNFCDIR)/Doc.pdf

bnfc_haskell: $(bnfc_haskell_files)
# Explicit calls to happy and alex are so we can pass them custom flags.
# Otherwise the generated Makefile.haskell can invoke them too.
$(bnfc_haskell_files): bnfc/Json.cf
	(cd bnfc && bnfc -haskell -m -d Json.cf && mv Makefile Json/Makefile.haskell)
	happy $(HAPPYFLAGS) -gca $(BNFCDIR)/Par.y
	alex $(ALEXFLAGS) -g $(BNFCDIR)/Lex.x
	cp json.cabal LICENSE.txt bnfc/
	(cd bnfc && cabal install)

bnfc_cpp_objs=$(bnfc_cpp_files:.C=.o)
#$(info bnfc_cpp_objs=$(bnfc_cpp_objs))
#$(info bnfc_cpp_files=$(bnfc_cpp_files))
get-path.o : $(bnfc_cpp_objs)


#$(info LIBOBJS=$(LIBOBJS))
libjson.so: $(LIBOBJS); $(CXXLINK)
libjson.a: $(LIBOBJS);  $(AR_CMD)


tests: $(TESTEXES)

$(TESTEXES): $(LIBOBJS)

# Add this command to the 'clean' step.
CLEAN_HOOK = $(RM) -r $(BNFCDIR)

include $(make_incl_dir)/footer.make

.PHONY: bnfc bnfc_haskell bnfc_cpp
