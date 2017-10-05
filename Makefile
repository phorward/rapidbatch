#-------------------------------------------------------------------------------
# RapidBATCH Script Language Engine
# Copyright (C) 2008, 2009 by Phorward Software Technologies, Jan Max Meyer
# http://www.phorward-software.com ++ mail@phorward-software.com
#
# Make-File:	Makefile
# Author:		Jan Max Meyer
#-------------------------------------------------------------------------------

include		../../include/Make.inc
include		Make.inc

LIBNAME		=	$(RB_BASE_LIB)
LIBTEST		=	rb6test$(EXEEXT)

PARSER		=	rb6.par
PARSER_OUT	=	rb_comp.parser.c

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
				rb_util.c
				
OBJECTS		=	rb_comp.parser$(OBJEXT) \
				rb_comp.util$(OBJEXT) \
				rb_comp.symtab$(OBJEXT) \
				rb_comp.codegen$(OBJEXT) \
				rb_comp.opt$(OBJEXT) \
				rb_comp.error$(OBJEXT) \
				rb_comp.native$(OBJEXT) \
				rb_comp.main$(OBJEXT) \
				rb_vm.run$(OBJEXT) \
				rb_vm.var$(OBJEXT) \
				rb_vm.var.tool$(OBJEXT) \
				rb_vm.var.stack$(OBJEXT) \
				rb_vm.val$(OBJEXT) \
				rb_vm.util$(OBJEXT) \
				rb_vm.native$(OBJEXT) \
				rb_vm.error$(OBJEXT) \
				rb_vm.dbg$(OBJEXT) \
				rb_string$(OBJEXT) \
				rb_util$(OBJEXT)

LIBS		=	$(DBG_LIB) \
				$(LLIST_LIB) \
				$(XML_LIB) \
				$(PSTRING_LIB)

HEADERS		=	rb_comp.h \
				rb_global.h \
				rb_val.h \
				rb_var.h \
				rb_vm.h \
				rb_native.h
				
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
	@echo
	@echo Building parser...
	$(UNICC) -s -v -w -o $@ $(PARSER)
	
$(PROTOFILE): $(SOURCE) $(HEADERS)
	@echo
	@echo Making prototypes...
	cproto_all "$(SOURCE)" $(INCLUDE_DIR) $@

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

