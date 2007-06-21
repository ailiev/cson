include ../pir/utils.make

LIBSRCS = Absyn.C Lexer.C Parser.C Printer.C Skeleton.C

CPPFLAGS += -I.

all: app

app: $(LIBOBJS) get-path.o
	$(CXXLINK)
