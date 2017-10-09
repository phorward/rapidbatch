#-------------------------------------------------------------------------------
# RapidBATCH Script Language Engine
# Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ mail@phorward-software.com
#
# Make-File:	Makefile
# Author:		Jan Max Meyer
#-------------------------------------------------------------------------------

RAPIDBATCH	=	rb6
LIBPHORWARD	=	../phorward/src/libphorward.a

PATHEXT		=	PATH="../unicc:../phorward/run:$(PATH)"

RB_BASE		=	rapidbatch
RB_BASE_LIB	=	$(RB_BASE).a
RB_SDK_H	=	$(RB_BASE).h


PARSER		=	rb6.par
PARSER_BASE	=	rb_comp.parser
PARSER_OUT	= 	$(PARSER_BASE).c

SOURCE		=	$(PARSER_OUT) \
				rb_comp.util.c \
				rb_comp.symtab.c \
				rb_comp.codegen.c \
				rb_comp.opt.c \
				rb_comp.error.c \
				rb_comp.native.c \
				rb_comp.main.c \
				rb_vm.run.c \
				rb_vm.var.c \
				rb_vm.var.tool.c \
				rb_vm.var.stack.c \
				rb_vm.val.c \
				rb_vm.util.c \
				rb_vm.native.c \
				rb_vm.error.c \
				rb_vm.dbg.c \
				rb_string.c \
				rb_util.c \
				xml.c \
				\
				comptime.c \
				getcharat.c \
				getlen.c \
				getpos.c \
				gettok.c \
				replacevar.c \
				string.c \
				trimvar.c \
				\
				_std.c \
				main.c
				
OBJECTS		=	$(patsubst %.c,%.o,$(SOURCE))

HEADERS		=	rb_comp.h \
				rb_global.h \
				rb_val.h \
				rb_var.h \
				rb_vm.h \
				rb_native.h \
				xml.h
				
PROTOFILE	=	rb_proto.h

#-------------------------------------------------------------------------------

all: $(RAPIDBATCH)

$(RAPIDBATCH):  $(OBJECTS) $(LIBPHORWARD)
	$(CC) -o $(RAPIDBATCH) $+
	
$(PARSER_OUT): $(PARSER)
	$(PATHEXT) unicc -s -v -w -o $(PARSER_BASE) $(PARSER)
	
$(PROTOFILE): $(SOURCE) $(HEADERS)
	$(PATHEXT) ./pproto $(SOURCE) >$@

clean:
	rm $(OBJECTS)
	rm $(PARSER_BASE).c
	rm $(PARSER_BASE).h

