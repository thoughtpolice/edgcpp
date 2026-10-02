#ifndef EDG_QBE_CODEGEN_H
#define EDG_QBE_CODEGEN_H
#include "fe_common.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct QbeBackend QbeBackend;
typedef struct { a_type_ptr type; const char *operand; } QbeValue;
typedef struct { const char *target; a_boolean exported; } QbeAlias;
typedef struct { const char *type, *operand; } QbeRawArg;
typedef struct { const char *first, *second; } QbeStringPair;
typedef struct { a_type_ptr type; uint64_t offset; } QbeFloating;
typedef struct { a_boolean indirect; unsigned align; uint64_t bytes; QbeFloating floating[4]; size_t floating_count; } QbeNativeClass;
typedef struct { const char *name, *body; uint64_t stack; a_boolean exported; } QbeNativeEntry;
#define QBE_VEC(T) struct { T *data; size_t len, cap; }
typedef QBE_VEC(const char *) QbeStringVec;
typedef QBE_VEC(QbeValue) QbeValueVec;
typedef QBE_VEC(unsigned) QbeUnsignedVec;
typedef struct { unsigned id, landingpad; } QbeEHSite;
typedef struct { size_t after; QbeUnsignedVec allowed; int selector; } QbeEHSpecification;
typedef QBE_VEC(QbeEHSpecification) QbeEHSpecificationVec;
typedef struct { unsigned id; QbeUnsignedVec catches; a_boolean cleanup; QbeEHSpecificationVec specifications; } QbeEHHandler;
typedef struct { const char *name, *personality; QbeStringVec types; QBE_VEC(QbeEHSite) sites; QBE_VEC(QbeEHHandler) landingpads; } QbeEHFunction;
typedef struct QbeEHState QbeEHState;
typedef struct { char *data; size_t len, cap; } QbeBuffer;
/* Hash lookup is independent of insertion order; metadata explicitly sorts keys. */
typedef struct { const void *key; void *value; QbeAlias alias; } QbeMapEntry;
typedef struct { QbeMapEntry *data; size_t len, cap; size_t *slots; size_t slot_cap; a_boolean string_keys; } QbeMap;
struct QbeBackend {
 FILE *output_; QbeBuffer body_, entry_, moduleAssemblyBuffer_;
 QbeMap symbols_, types_, symbolNames_, variables_, constants_, unnamedVariables_, unnamedRoutines_, emittedScopes_, labels_;
 QbeStringVec functionDefinitions_, typeDefinitions_, dataDefinitions_;
 QbeMap weak_, hidden_, protected_, defined_, aliases_;
 QBE_VEC(QbeEHFunction) ehFunctions_;
 const char *moduleAssembly_, *currentLabel_, *currentFunctionSymbol_;
 a_routine_ptr routine_; a_scope_ptr scope_; a_variable_ptr resultParameter_;
 QBE_VEC(a_type_ptr) syntheticTypes_; QbeMap integerTypes_; a_type_ptr pointerType_;
 const char *nativePacket_, *nativeResult_; unsigned nativeNamedGP_, nativeNamedFP_; uint64_t nativeNamedStack_;
 const char *persistentStackFloor_;
 QBE_VEC(QbeNativeEntry) nativeEntries_; QbeValue overridingResult_;
 a_routine_ptr inlineForwardRoutine_; const char *inlineReturnLabel_, *inlineReturnSlot_; a_type_ptr inlineReturnType_;
 QBE_VEC(QbeStringPair) loops_; QbeValueVec statementResults_; QbeStringVec statementResultDestinations_;
 a_boolean terminated_; unsigned nextSymbol_, nextValue_, nextLabel_, nextEH_; QbeEHState *eh_;
 void *allocations_;
};
NORETURN void qbe_fail(const char *message);
void *qbe_alloc(QbeBackend *, size_t);
void *qbe_resize(QbeBackend *, void *, size_t);
void qbe_free(QbeBackend *, void *);
const char *qbe_string(QbeBackend *, const char *, size_t);
const char *qbe_format(QbeBackend *, const char *, ...);
const char *qbe_concat(QbeBackend *, const char *, ...);
const char *qbe_json_string(QbeBackend *, const char *);
void qbe_vec_reserve(QbeBackend *, void **, size_t *, size_t, size_t);
#define qbe_vec_push(b,v,x) do { __typeof__((v)->data[0]) qbe_push_value_ = (x); qbe_vec_reserve((b),(void **)&(v)->data,&(v)->cap,(v)->len+1,sizeof(*(v)->data)); (v)->data[(v)->len++] = qbe_push_value_; } while (0)
#define qbe_string_vec_push(b,v,x) qbe_vec_push(b,v,x)
#define qbe_value_vec_push(b,v,x) qbe_vec_push(b,v,x)
#define qbe_unsigned_vec_push(b,v,x) qbe_vec_push(b,v,x)
#define qbe_vec_clear(v) ((v)->len=0)
#define qbe_vec_free(b,v) (qbe_free((b),(v)->data), (v)->data=NULL,(v)->len=(v)->cap=0)
void qbe_buffer_append(QbeBackend *, QbeBuffer *, const char *);
void qbe_buffer_printf(QbeBackend *, QbeBuffer *, const char *, ...);
const char *qbe_buffer_string(QbeBuffer *);
void qbe_buffer_clear(QbeBuffer *);
void *qbe_map_get(const QbeMap *, const void *);
a_boolean qbe_map_has(const QbeMap *, const void *);
a_boolean qbe_map_put(QbeBackend *, QbeMap *, const void *, const void *);
a_boolean qbe_set_insert(QbeBackend *, QbeMap *, const void *);
void qbe_map_remove(QbeBackend *, QbeMap *, const void *);
void qbe_map_clear(QbeMap *);
void qbe_map_free(QbeBackend *, QbeMap *);
void qbe_alias_set(QbeBackend *, const char *, QbeAlias);
QbeMapEntry **qbe_map_order(QbeBackend *, QbeMap *);
static inline QbeValue qbe_value(a_type_ptr t,const char *s) { QbeValue v={t,s};return v; }
static inline QbeValue qbe_void_value(void) { return qbe_value(NULL,""); }
void qbe_backend_init(QbeBackend *, FILE *);
void qbe_backend_destroy(QbeBackend *);
void qbe_module(QbeBackend *);
a_type_ptr qbe_underlying(a_type_ptr);
a_boolean qbe_aggregate(a_type_ptr);
a_boolean qbe_signed_integer(a_type_ptr);
a_boolean qbe_narrow_float(a_type_ptr);
unsigned qbe_width(a_type_ptr);
uint64_t qbe_size(a_type_ptr);
unsigned qbe_alignment(a_type_ptr);
const char *qbe_base_type(QbeBackend *,a_type_ptr);
const char *qbe_abi_type(QbeBackend *,a_type_ptr);
const char *qbe_symbol(QbeBackend *,const char *);
const char *qbe_routine_symbol(QbeBackend *,a_routine_ptr);
const char *qbe_variable_symbol(QbeBackend *,a_variable_ptr);
const char *qbe_rtti(QbeBackend *,a_type_ptr);
QbeValue qbe_constant(QbeBackend *,a_constant_ptr);
const char *qbe_var_address(QbeBackend *,a_variable_ptr);
const char *qbe_temporary(QbeBackend *);
const char *qbe_new_label(QbeBackend *,const char *);
const char *qbe_emit(QbeBackend *,const char *,const char *);
void qbe_line(QbeBackend *,const char *);
void qbe_label(QbeBackend *,const char *);
void qbe_jump(QbeBackend *,const char *);
void qbe_branch(QbeBackend *,const char *,const char *,const char *);
const char *qbe_offset(QbeBackend *,const char *,int64_t);
const char *qbe_allocate(QbeBackend *,uint64_t,unsigned);
/* VLA cells use the ordinary per-function variable map: pointer, saved SP. */
const char *qbe_vla_cell(QbeBackend *,a_variable_ptr);
void qbe_vla_allocate(QbeBackend *,a_variable_ptr);
void qbe_vla_deallocate(QbeBackend *,a_variable_ptr);
void qbe_keep_stack(QbeBackend *);
const char *qbe_runtime_size(QbeBackend *,a_type_ptr);
QbeValue qbe_load(QbeBackend *,a_type_ptr,const char *);
void qbe_store(QbeBackend *,a_type_ptr,QbeValue,const char *);
void qbe_copy(QbeBackend *,const char *,const char *,uint64_t);
void qbe_zero(QbeBackend *,const char *,uint64_t);
QbeValue qbe_cast(QbeBackend *,QbeValue,a_type_ptr);
QbeValue qbe_runtime(QbeBackend *,const char *,a_type_ptr,size_t,const QbeValue *,a_boolean);
const char *qbe_runtime_raw(QbeBackend *,const char *,const char *,size_t,const QbeRawArg *);
QbeValue qbe_emit_call(QbeBackend *,a_routine_ptr,a_type_ptr,const char *,size_t,const QbeValue *,a_boolean);
QbeValue qbe_overriding_result(QbeBackend *);
void qbe_return_value(QbeBackend *,QbeValue);
void qbe_va_start(QbeBackend *,const char *);
QbeValue qbe_va_arg(QbeBackend *,a_type_ptr,const char *);
QbeNativeClass qbe_native_class(QbeBackend *,a_type_ptr);
void qbe_add_module_assembly(QbeBackend *,const char *);
a_type_ptr qbe_integer_type(QbeBackend *,unsigned,a_boolean);
a_type_ptr qbe_pointer_type(QbeBackend *);
QbeValue qbe_expression(QbeBackend *,an_expr_node_ptr);
const char *qbe_address(QbeBackend *,an_expr_node_ptr);
QbeValue qbe_operation(QbeBackend *,an_expr_node_ptr);
void qbe_initialize(QbeBackend *,a_dynamic_init_ptr,const char *,a_type_ptr);
a_boolean qbe_builtin(QbeBackend *,a_routine_ptr,size_t,const QbeValue *,QbeValue *);
const char *qbe_truth(QbeBackend *,QbeValue);
void qbe_statement(QbeBackend *,a_statement_ptr);
void qbe_statement_list(QbeBackend *,a_statement_ptr);
void qbe_begin_function_eh(QbeBackend *,a_routine_ptr,a_scope_ptr);
void qbe_finish_function_eh(QbeBackend *);
void qbe_free_function_eh(QbeBackend *);
void qbe_begin_inline_eh(QbeBackend *);
void qbe_finish_inline_eh(QbeBackend *);
unsigned qbe_begin_invoke(QbeBackend *);
void qbe_end_invoke(QbeBackend *,unsigned);
QbeValue qbe_throw_expression(QbeBackend *,an_expr_node_ptr);
QbeValue qbe_eh_construct(QbeBackend *,an_expr_node_ptr);
QbeValue qbe_lifetime_expression(QbeBackend *,an_expr_node_ptr);
const char *qbe_lifetime_address(QbeBackend *,an_expr_node_ptr);
void qbe_initialized(QbeBackend *,a_dynamic_init_ptr);
void qbe_emit_routine(QbeBackend *,a_routine_ptr);
void qbe_emit_variable(QbeBackend *,a_variable_ptr);
void qbe_emit_scope(QbeBackend *,a_scope_ptr);
const char *qbe_data(QbeBackend *,a_constant_ptr,a_type_ptr,const char *,unsigned,a_boolean,a_boolean);
#endif
