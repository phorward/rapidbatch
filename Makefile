#-------------------------------------------------------------------------------
# RapidBATCH Script Language Engine
# Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ mail@phorward-software.com
#
# Make-File:	Makefile
# Author:		Jan Max Meyer
#-------------------------------------------------------------------------------

PATHEXT		=	PATH="../unicc:../phorward/run:$(PATH)"

RB_BASE		=	rapidbatch
RB_BASE_LIB	=	$(RB_BASE).a
RB_SDK_H	=	$(RB_BASE).h

LIBNAME		=	$(RB_BASE_LIB)
LIBTEST		=	rb6test

PARSER		=	rb6.par
PARSER_BASE	=	rb_comp.parser
PARSER_OUT	= 	$(PARSER_BASE).c

SOURCE		=	rb_comp.parser.c \
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
				xml.c
				
OBJECTS		=	$(patsubst %.c,%.o,$(SOURCE))

LIBS		=	$(DBG_LIB) \
				$(LLIST_LIB) \
				$(XML_LIB) \
				$(PSTRING_LIB)

HEADERS		=	rb_comp.h \
				rb_global.h \
				rb_val.h \
				rb_var.h \
				rb_vm.h \
				rb_native.h \
				xml.h
				
PROTOFILE	=	rb_proto.h

#-------------------------------------------------------------------------------

all: sdk
	@echo
	@echo --- Compilation succeeded! ---
	
$(LIBNAME): $(PROTOFILE) $(PARSER_OUT) $(SOURCE) $(HEADERS) Makefile
	@echo
	@echo Compiling...
	$(CC) $(CLIBOPTS) $(DEBUG) $(SOURCE)
	@echo
	@echo Linking library...
	$(LLIB) $(LIBOPTS)$@ $(OBJECTS)
	
$(LIBTEST): $(PROTOFILE) $(PARSER_OUT) Makefile
	@echo
	@echo Linking executable...
	$(LLINK) $(LINKOPTS)$@ $(OBJECTS) $(LIBS) $(LIBNAME)

$(PARSER_OUT): $(PARSER)
	$(PATHEXT) unicc -s -v -w -o $(PARSER_BASE) $(PARSER)
	
$(PROTOFILE): $(SOURCE) $(HEADERS)
	$(PATHEXT) pproto $(SOURCE) >$@

$(RB_SDK_H): $(SOURCE) $(HEADERS) $(PROTOFILE)
	@echo
	@echo Exporting header for external SDK...
	mk_sdk_h >$(RB_SDK_H)

#$(RB_SDK_H): $(SOURCE) $(HEADERS) $(PROTOFILE)
#	@echo
#	@echo Exporting header for internal SDK...
#	export.sc internal >$(RB_SDK_H)

sdk sdkext: $(LIBNAME) $(RB_SDK_H)
	@echo
	@echo Removing debug informations...
	-strip --strip-unneeded $(LIBNAME)

sdkint: $(RB_SDK_H) $(LIBNAME)

clean:
	-@$(RM) $(RB_SDK_H)
	-@$(RM) $(PARSER_OUT)
	-@$(RM) $(LIBNAME)
	-@$(RM) $(LIBTEST)
	-@$(RM) $(OBJECTS)
	-@$(RM) cscope.out

backup: clean
	-@$(RM) ../rb6lib.tar
	tar cvf ../rb6lib.tar ../rb6lib 

