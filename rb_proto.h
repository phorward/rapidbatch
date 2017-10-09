/* rb_comp.parser.c */
#if 1838 == 0
#else
void rb_comp_current_srcpos( srcpos* pos );
srcpos* rb_comp_cur_pos( void );
#endif
int rb_parse( rb_pcb* pcb );

/* rb_comp.util.c */
symbol* rb_comp_get_symbol( char* ident, symtype type, boolean auto_create );
long rb_comp_get_var_count_of_scope( symbol* begin );
symbol* rb_comp_proc_header( char* ident, boolean is_function, plist* parameters );
void rb_comp_proc_complete( symbol* proc, symbol* last_param, SCOPE_INFO* prev_scope, vm_addr jmp );
void rb_comp_proc_return( symbol* proc );
int rb_comp_proc_call_perform( struct proc_call* pc, boolean free_call );
void rb_comp_backpatch_proc_calls( symbol* proc );
void rb_comp_proc_call( symbol* proc, boolean as_function, plist* params, vm_addr begin );
void rb_comp_cache_address( ADDR_CACHE* cache, vm_addr addr );
boolean rb_comp_region_branch( boolean cont );
void rb_comp_region_close( REGION* prev, vm_addr break_target, vm_addr cont_target );
symbol* rb_comp_get_label( char* ident, boolean at_definition );
void rb_comp_label_call( char* ident );
void rb_comp_label_call_perform( LBL_CALL* lc );
void rb_comp_backpatch_label_calls( symbol* label );
void rb_comp_optimize_operator( PARAM* ret, PARAM* operand1, PARAM* operand2 );
void rb_comp_push_constant( char* constval );

/* rb_comp.symtab.c */
symbol* rb_symtab_find( char* name, symtype type );
symbol* rb_symtab_find_in_scope( symbol* begin, char* name, symtype type );
symbol* rb_symtab_new( char* name, symtype type );
symbol* rb_symtab_engage_scope( symbol* begin, symbol* engage );
symbol* rb_symtab_drop_scope( symbol* begin, boolean drop_sym );
int rb_symtab_scope_pos( symbol* begin, symbol* find );
void rb_symtab_dump( FILE* stream );
void rb_symtab_drop_all( boolean keep_native );

/* rb_comp.codegen.c */
vm_addr rb_comp_next_addr( vm_prog* prog, vm_code instr );
void rb_comp_patch_jmp( vm_prog* prog, vm_code instr, vm_addr call );
vm_addr rb_comp_cur_addr( vm_prog* prog );
boolean rb_comp_patch( vm_code* code, int instr, void* parm );
vm_addr rb_comp_gen( vm_prog* prog, vm_code instr, void* parm );

/* rb_comp.opt.c */
int rb_comp_opt_expr( vm_prog* prog, vm_addr begin_opt, boolean with_return );
boolean rb_comp_opt_test( vm_code* code, vm_addr code_cnt );

/* rb_comp.error.c */
void rb_comp_error( srcpos* pos, char* id, ... );

/* rb_comp.native.c */
int rb_add_native_function( char* name, rb_native_func fct, boolean isfunc, char* parmdef, int run_at );
int rb_add_native_var( char* name, rb_native_var get, rb_native_var set );
int rb_add_native_const( char* name, char* value, boolean dup_val );

/* rb_comp.main.c */
int rb_comp_compile( char* filename, char* src );
void rb_comp_reset( boolean keep_natives );
void rb_run( void );

/* rb_vm.run.c */
boolean rb_vm_stack_push( vm_stack* stack, vm_stackitem* item );
boolean rb_vm_stack_pop( vm_stack* stack, vm_stackitem* item );
vm_stackitem* rb_vm_stack_access( vm_stack* stack, vm_addr offset );
boolean rb_vm_stack_put( vm_stack* stack, vm_stackitem* item, vm_addr offset );
void rb_vm_stack_drop( vm_stack* stack, vm_addr count );
void rb_vm_item_free( vm_stackitem* item );
void rb_vm_item_memdup( vm_stackitem* item );
void rb_vm_stack_dump( char* file, int line, vm_stack* stack );
int rb_vm_nice_type( vm_stackitem* first, vm_stackitem* last );
int rb_vm_run( vm_stack* stack, vm_code* code, vm_addr code_cnt );

/* rb_vm.var.c */
vm_var* rb_vm_var_get_idx( vm_var* var, vm_val* idx, boolean alloc );
status rb_vm_var_set( vm_var* var, vm_val* value, boolean no_dup );
status rb_vm_var_get( vm_val* value, vm_var* var, boolean no_dup );
void rb_vm_var_free( vm_var* var );
void rb_vm_var_dump( vm_var* var, int tabs, vm_addr cnt );

/* rb_vm.var.tool.c */
vm_var* rb_vm_var_resolve( vm_var* var, vm_val* idx, int idx_depth, boolean allocate );
vm_var* rb_vm_var_dim( vm_var* var, vm_val* idx, int idx_depth, boolean allocate );
boolean rb_vm_var_copy( vm_var* dest, vm_var* src );
vm_var* rb_vm_make_var( vm_stackitem* item );

/* rb_vm.var.stack.c */
vm_var* rb_vm_stack_get_var_base( vm_stack* stack );
vm_var* rb_vm_stack_var_resolve( vm_stack* stack, boolean allocate, boolean* get_size );
boolean rb_vm_stack_init_var( vm_stack* stack, vm_var* var, int max, int depth );
status rb_vm_stack_var_push( vm_stack* stack, vm_var* var );

/* rb_vm.val.c */
double rb_vm_get_dbl_val( vm_val* val, boolean convert ) ;
long rb_vm_get_long_val( vm_val* val, boolean convert ) ;
vm_addr rb_vm_get_addr_val( vm_val* val, boolean convert ) ;
char* rb_vm_get_str_val( vm_val* val, boolean convert ) ;
void rb_vm_free_val( vm_val* val ) ;
boolean rb_vm_copy_val( vm_val* dst, vm_val* src, boolean withdup ) ;
void rb_vm_dump_val( FILE* stream, vm_val* val ) ;

/* rb_vm.util.c */
boolean rb_vm_convert_value( vm_val* val, char to_type );
boolean rb_vm_optimal_value( vm_val* val );
char rb_vm_get_parm_size( vm_code instr );

/* rb_vm.native.c */
vm_stackitem* rb_vmwrap_GET_PARAM( vm_stackitem* base, int offset, int max ) ;
vm_var* rb_vmwrap_ITEM_GET_VAR( vm_stackitem* parm ) ;
vm_val* rb_vmwrap_ITEM_GET_VAL( vm_stackitem* parm ) ;
void rb_vmwrap_VAL_SET_ADDR( vm_val* val, vm_addr v ) ;
void rb_vmwrap_VAL_SET_LONG( vm_val* val, long v ) ;
void rb_vmwrap_VAL_SET_DBL( vm_val* val, double v ) ;
void rb_vmwrap_VAL_SET_CSTR( vm_val* val, char* v ) ;
void rb_vmwrap_VAL_SET_STR( vm_val* val, char* v ) ;
char rb_vmwrap_VAL_TYPE( vm_val* val ) ;
vm_val* rb_vmwrap_VAR_VAL_STRUCT( vm_var* var ) ;
char rb_vmwrap_VAR_VAL_TYPE( vm_var* var ) ;

/* rb_vm.error.c */
void rb_vm_error( char* msg );

/* rb_vm.dbg.c */
void rb_vm_dump( FILE* stream, vm_prog* prog, vm_addr begin_at );

/* rb_string.c */
char* rb_str_pool_append( char*** pool, char* insert, boolean do_copy );

/* rb_util.c */
void* rb_memdup( void* ptr, size_t size );
int hash_from_str( char* str, int size );

/* xml.c */
XML_T xml_child( XML_T xml, char* name );
XML_T xml_idx( XML_T xml, int idx );
char* xml_attr( XML_T xml, char* attr );
long xml_int_attr( XML_T xml, char* attr );
double xml_float_attr( XML_T xml, char* attr );
XML_T xml_vget( XML_T xml, va_list ap );
XML_T xml_get( XML_T xml, ... );
char ** xml_pi( XML_T xml, char* target );
char* xml_decode( char* s, char ** ent, char t );
char* xml_str2utf8( char ** s, size_t* len );
void xml_free_attr( char ** attr );
XML_T xml_parse_str( char* s, size_t len );
XML_T xml_parse_fp( FILE* fp );
XML_T xml_parse_file( char* file );
char* xml_ampencode( char* s, size_t len, char ** dst, size_t* dlen, size_t* max, short a );
char* xml_toxml( XML_T xml );
void xml_free( XML_T xml );
char* xml_error( XML_T xml );
XML_T xml_new( char* name );
XML_T xml_insert( XML_T xml, XML_T dest, size_t off );
XML_T xml_add_child( XML_T xml, char* name, size_t off );
XML_T xml_set_txt( XML_T xml, char* txt );
XML_T xml_set_attr( XML_T xml, char* name, char* value );
XML_T xml_set_int_attr( XML_T xml, char* name, long value );
XML_T xml_set_float_attr( XML_T xml, char* name, double value );
XML_T xml_set_flag( XML_T xml, short flag );
int xml_count( XML_T xml );
int xml_count_all( XML_T xml );
XML_T xml_cut( XML_T xml );

/* comptime.c */
RB_FCT( include );
RB_FCT( include_once );

/* getcharat.c */
RB_FCT( getcharat );

/* getlen.c */
RB_FCT( getlen );

/* getpos.c */
RB_FCT( getpos );

/* gettok.c */
RB_FCT( gettok );

/* replacevar.c */
RB_FCT( replacevar );

/* string.c */
RB_FCT( print );

/* trimvar.c */
RB_FCT( trimvar );

/* _std.c */
int rb_register_std( void );

/* main.c */

