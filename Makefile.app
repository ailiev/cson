# -*- makefile -*-

include ../pir/utils.make
include ../pir/common.make


# the code/ directory
CPPFLAGS += -I.. -I../pir

LIBSRCS = Absyn.C Lexer.C Parser.C Printer.C Skeleton.C

CPPFLAGS += -I.

LIBDIRS		+= $(LEEDS_LIB) . ../common
LDLIBFILES	+= -lcommon

all: app

app: $(LIBOBJS) get-path.o
	$(CXXLINK)

include ../pir/footer.make
