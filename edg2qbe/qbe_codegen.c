#include "qbe_codegen.h"
#include "il_read.h"
#include "error.h"
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <inttypes.h>

typedef union QbeAllocation QbeAllocation;
union QbeAllocation { struct { QbeAllocation *next, *prev; } links; long double align; void *pointer; };
NORETURN void qbe_fail(const char *message) { str_catastrophe(ec_qbe_backend_error,message); abort(); }
void *qbe_alloc(QbeBackend *b,size_t bytes) {
 QbeAllocation *p;
 if(bytes>SIZE_MAX-sizeof(*p)) qbe_fail("backend allocation size overflow");
 p=malloc(sizeof(*p)+(bytes?bytes:1)); if(!p) qbe_fail("backend allocation failed");
 p->links.prev=NULL;p->links.next=b->allocations_;if(p->links.next)p->links.next->links.prev=p;b->allocations_=p;return p+1;
}
void *qbe_resize(QbeBackend *b,void *old,size_t bytes) {
 QbeAllocation *p,*next,*prev;
 if(!old)return qbe_alloc(b,bytes);
 if(bytes>SIZE_MAX-sizeof(*p))qbe_fail("backend allocation size overflow");
 p=(QbeAllocation*)old-1;next=p->links.next;prev=p->links.prev;
 p=realloc(p,sizeof(*p)+(bytes?bytes:1));if(!p)qbe_fail("backend allocation failed");
 if(prev)prev->links.next=p;else b->allocations_=p;if(next)next->links.prev=p;return p+1;
}
void qbe_free(QbeBackend *b,void *ptr) { QbeAllocation *p;if(!ptr)return;p=(QbeAllocation*)ptr-1;if(p->links.prev)p->links.prev->links.next=p->links.next;else b->allocations_=p->links.next;if(p->links.next)p->links.next->links.prev=p->links.prev;free(p); }
void qbe_vec_reserve(QbeBackend *b,void **data,size_t *cap,size_t count,size_t width) {
 size_t n;if(count<=*cap)return;n=*cap?*cap:8;while(n<count){if(n>SIZE_MAX/2){n=count;break;}n*=2;}if(width&&n>SIZE_MAX/width)qbe_fail("backend vector size overflow");*data=qbe_resize(b,*data,n*width);*cap=n;
}
const char *qbe_string(QbeBackend *b,const char *s,size_t n) {char *p;if(n==SIZE_MAX)qbe_fail("backend string size overflow");p=qbe_alloc(b,n+1);memcpy(p,s,n);p[n]=0;return p;}
static const char *format_va(QbeBackend *b,const char *fmt,va_list args) {va_list copy;int n;char *s;va_copy(copy,args);n=vsnprintf(NULL,0,fmt,copy);va_end(copy);if(n<0)qbe_fail("backend formatting failed");s=qbe_alloc(b,(size_t)n+1);vsnprintf(s,(size_t)n+1,fmt,args);return s;}
const char *qbe_format(QbeBackend *b,const char *fmt,...) {va_list args;const char *s;va_start(args,fmt);s=format_va(b,fmt,args);va_end(args);return s;}
const char *qbe_concat(QbeBackend *b,const char *first,...) {va_list args;size_t n=0,k;const char *s;char *out,*p;va_start(args,first);for(s=first;s;s=va_arg(args,const char*)){k=strlen(s);if(k>SIZE_MAX-n-1)qbe_fail("backend string size overflow");n+=k;}va_end(args);out=qbe_alloc(b,n+1);p=out;va_start(args,first);for(s=first;s;s=va_arg(args,const char*)){k=strlen(s);memcpy(p,s,k);p+=k;}va_end(args);*p=0;return out;}
void qbe_buffer_append(QbeBackend *b,QbeBuffer *v,const char *s) {size_t n=strlen(s);if(n>SIZE_MAX-v->len-1)qbe_fail("backend buffer size overflow");qbe_vec_reserve(b,(void**)&v->data,&v->cap,v->len+n+1,1);memcpy(v->data+v->len,s,n+1);v->len+=n;}
void qbe_buffer_printf(QbeBackend *b,QbeBuffer *v,const char *fmt,...) {va_list args,copy;int n;va_start(args,fmt);va_copy(copy,args);n=vsnprintf(NULL,0,fmt,copy);va_end(copy);if(n<0||(size_t)n>SIZE_MAX-v->len-1)qbe_fail("backend formatting size overflow");qbe_vec_reserve(b,(void**)&v->data,&v->cap,v->len+(size_t)n+1,1);vsnprintf(v->data+v->len,(size_t)n+1,fmt,args);v->len+=(size_t)n;va_end(args);}
const char *qbe_buffer_string(QbeBuffer *v) {return v->data?v->data:"";}
void qbe_buffer_clear(QbeBuffer *v) {v->len=0;if(v->data)v->data[0]=0;}
const char *qbe_json_string(QbeBackend *b,const char *s) {QbeBuffer out={0};const unsigned char *p=(const unsigned char*)s;qbe_buffer_append(b,&out,"\"");for(;*p;++p){switch(*p){case '"':qbe_buffer_append(b,&out,"\\\"");break;case '\\':qbe_buffer_append(b,&out,"\\\\");break;case '\b':qbe_buffer_append(b,&out,"\\b");break;case '\f':qbe_buffer_append(b,&out,"\\f");break;case '\n':qbe_buffer_append(b,&out,"\\n");break;case '\r':qbe_buffer_append(b,&out,"\\r");break;case '\t':qbe_buffer_append(b,&out,"\\t");break;default:if(*p<32)qbe_buffer_printf(b,&out,"\\u00%02x",*p);else {char c[2]={(char)*p,0};qbe_buffer_append(b,&out,c);}}}qbe_buffer_append(b,&out,"\"");return out.data;}
static uint64_t map_hash(const QbeMap *m,const void *key) {uint64_t h;if(m->string_keys){const unsigned char *s=key;h=UINT64_C(1469598103934665603);while(*s){h^=*s++;h*=UINT64_C(1099511628211);}return h;}h=(uintptr_t)key;h^=h>>30;h*=UINT64_C(0xbf58476d1ce4e5b9);h^=h>>27;h*=UINT64_C(0x94d049bb133111eb);return h^(h>>31);}
static a_boolean map_equal(const QbeMap *m,const void *a,const void *b) {return m->string_keys?strcmp(a,b)==0:a==b;}
static size_t map_slot(const QbeMap *m,const void *key) {size_t s=(size_t)map_hash(m,key)&(m->slot_cap-1);while(m->slots[s]&&!map_equal(m,m->data[m->slots[s]-1].key,key))s=(s+1)&(m->slot_cap-1);return s;}
static void map_rehash(QbeBackend *b,QbeMap *m,size_t count) {size_t i; m->slots=qbe_resize(b,m->slots,count*sizeof(*m->slots));m->slot_cap=count;memset(m->slots,0,count*sizeof(*m->slots));for(i=0;i<m->len;++i)m->slots[map_slot(m,m->data[i].key)]=i+1;}
static QbeMapEntry *map_find(const QbeMap *m,const void *key) {size_t i;if(!m->slot_cap)return NULL;i=m->slots[map_slot(m,key)];return i?&m->data[i-1]:NULL;}
void *qbe_map_get(const QbeMap *m,const void *key) {QbeMapEntry *p=map_find(m,key);return p?p->value:NULL;}
a_boolean qbe_map_has(const QbeMap *m,const void *key) {return map_find(m,key)!=NULL;}
a_boolean qbe_map_put(QbeBackend *b,QbeMap *m,const void *key,const void *value) {QbeMapEntry *p=map_find(m,key);size_t s;if(p){p->value=(void*)value;return FALSE;}if(!m->slot_cap||m->len+1>m->slot_cap/2){if(m->slot_cap>SIZE_MAX/2/sizeof(size_t))qbe_fail("backend map size overflow");map_rehash(b,m,m->slot_cap?m->slot_cap*2:16);}qbe_vec_reserve(b,(void**)&m->data,&m->cap,m->len+1,sizeof(*m->data));s=map_slot(m,key);p=&m->data[m->len];memset(p,0,sizeof(*p));p->key=key;p->value=(void*)value;m->slots[s]=++m->len;return TRUE;}
a_boolean qbe_set_insert(QbeBackend *b,QbeMap *m,const void *key) {return qbe_map_put(b,m,key,(void*)(uintptr_t)1);}
void qbe_map_remove(QbeBackend *b,QbeMap *m,const void *key) {
 size_t hole,scan,home,index,mask,moved_slot;(void)b;
 if(!m->slot_cap)return;hole=map_slot(m,key);if(!m->slots[hole])return;
 index=m->slots[hole]-1;mask=m->slot_cap-1;
 if(index+1<m->len){moved_slot=map_slot(m,m->data[m->len-1].key);m->data[index]=m->data[m->len-1];m->slots[moved_slot]=index+1;}
 --m->len;scan=(hole+1)&mask;
 while(m->slots[scan]){home=(size_t)map_hash(m,m->data[m->slots[scan]-1].key)&mask;if(((scan-home)&mask)>=((scan-hole)&mask)){m->slots[hole]=m->slots[scan];hole=scan;}scan=(scan+1)&mask;}
 m->slots[hole]=0;
}
void qbe_map_clear(QbeMap *m) {m->len=0;if(m->slots)memset(m->slots,0,m->slot_cap*sizeof(*m->slots));}
void qbe_map_free(QbeBackend *b,QbeMap *m) {a_boolean strings=m->string_keys;qbe_free(b,m->data);qbe_free(b,m->slots);memset(m,0,sizeof(*m));m->string_keys=strings;}
void qbe_alias_set(QbeBackend *b,const char *name,QbeAlias alias) {qbe_map_put(b,&b->aliases_,name,NULL);map_find(&b->aliases_,name)->alias=alias;}
static int order_compare(const void *a,const void *b) {const QbeMapEntry *x=*(QbeMapEntry*const*)a,*y=*(QbeMapEntry*const*)b;return strcmp(x->key,y->key);}
QbeMapEntry **qbe_map_order(QbeBackend *b,QbeMap *m) {size_t i;QbeMapEntry **a=qbe_alloc(b,m->len*sizeof(*a));for(i=0;i<m->len;++i)a[i]=&m->data[i];qsort(a,m->len,sizeof(*a),order_compare);return a;}
void qbe_backend_init(QbeBackend *b,FILE *output) {memset(b,0,sizeof(*b));b->output_=output;b->symbols_.string_keys=b->symbolNames_.string_keys=b->weak_.string_keys=b->hidden_.string_keys=b->protected_.string_keys=b->defined_.string_keys=b->aliases_.string_keys=TRUE;b->moduleAssembly_=b->currentLabel_=b->currentFunctionSymbol_=b->nativePacket_=b->nativeResult_=b->inlineReturnLabel_=b->inlineReturnSlot_="";b->overridingResult_=qbe_void_value();b->nextEH_=1;}
void qbe_backend_destroy(QbeBackend *b) {QbeAllocation *p,*next;qbe_free_function_eh(b);p=b->allocations_;while(p){next=p->links.next;free(p);p=next;}memset(b,0,sizeof(*b));}
const char *qbe_temporary(QbeBackend *b) {return qbe_format(b,"%%v%u",b->nextValue_++);}
const char *qbe_new_label(QbeBackend *b,const char *prefix) {return qbe_format(b,"@%s%u",prefix,b->nextLabel_++);}
void qbe_line(QbeBackend *b,const char *op) {qbe_buffer_printf(b,&b->body_,"\t%s\n",op);}
const char *qbe_emit(QbeBackend *b,const char *type,const char *op) {const char *result;if(!*type){qbe_line(b,op);return "";}result=qbe_temporary(b);qbe_buffer_printf(b,&b->body_,"\t%s =%s %s\n",result,type,op);return result;}
void qbe_label(QbeBackend *b,const char *name) {b->currentLabel_=*name=='@'?name:qbe_format(b,"@%s",name);qbe_buffer_printf(b,&b->body_,"%s\n",b->currentLabel_);b->terminated_=FALSE;}
void qbe_jump(QbeBackend *b,const char *name) {if(!b->terminated_){qbe_buffer_printf(b,&b->body_,"\tjmp %s%s\n",*name=='@'?"":"@",name);b->terminated_=TRUE;}}
void qbe_branch(QbeBackend *b,const char *condition,const char *yes,const char *no) {if(b->terminated_)qbe_fail("branch in a terminated block");qbe_buffer_printf(b,&b->body_,"\tjnz %s, %s, %s\n",condition,yes,no);b->terminated_=TRUE;}
const char *qbe_offset(QbeBackend *b,const char *address,int64_t displacement) {return displacement?qbe_emit(b,"l",qbe_format(b,"add %s, %" PRId64,address,displacement)):address;}
const char *qbe_allocate(QbeBackend *b,uint64_t bytes,unsigned align) {const char *result,*bumped,*aligned;unsigned native;uint64_t extra,total;if(!align||(align&(align-1)))qbe_fail("invalid object alignment");native=align<=4?4:align<=8?8:16;extra=align>16?align-1:0;if(bytes>UINT64_MAX-extra)qbe_fail("object allocation size overflow");total=bytes+extra;result=qbe_temporary(b);qbe_buffer_printf(b,&b->entry_,"\t%s =l alloc%u %" PRIu64 "\n",result,native,total?total:1);if(align<=16)return result;bumped=qbe_temporary(b);aligned=qbe_temporary(b);qbe_buffer_printf(b,&b->entry_,"\t%s =l add %s, %" PRIu64 "\n\t%s =l and %s, -%u\n",bumped,result,extra,aligned,bumped,align);return aligned;}

const char *qbe_vla_cell(QbeBackend *b, a_variable_ptr variable) {
 if(!variable || !variable->is_vla)qbe_fail("VLA storage requested for a non-VLA variable");
 const char *cell=qbe_map_get(&b->variables_,variable);
 if(!cell){cell=qbe_allocate(b,16,8);qbe_map_put(b,&b->variables_,variable,cell);}
 return cell;
}
const char *qbe_runtime_size(QbeBackend *b, a_type_ptr type) {
 a_type_ptr t=qbe_underlying(type);
 const char *count,*element;
 a_vla_dimension_ptr dimension;
 a_type_ptr component=t;
 while(component && component->kind==tk_array && !component->variant.array.is_variable_size_array)
  component=qbe_underlying(component->variant.array.element_type);
 if(!component || component->kind!=tk_array)return qbe_format(b,"%" PRIu64,qbe_size(type));
 element=qbe_runtime_size(b,t->variant.array.element_type);
 if(!t->variant.array.is_variable_size_array)
  count=qbe_format(b,"%" PRIu64,(uint64_t)t->variant.array.variant.number_of_elements);
 else if(t->variant.array.has_assoc_vla_dimension){
  a_scope_ptr scope=scope_for_routine(b->routine_);
  for(dimension=scope->vla_dimensions;dimension;dimension=dimension->next)
   if(dimension->type==t)break;
  if(!dimension)qbe_fail("missing native VLA dimension");
  while(dimension->original_dimension)dimension=dimension->original_dimension;
  if(!dimension->dimension_variable)qbe_fail("native VLA dimension has no saved bound");
  count=qbe_cast(b,qbe_load(b,dimension->dimension_variable->type,
                           qbe_var_address(b,dimension->dimension_variable)),
                 qbe_integer_type(b,64,TRUE)).operand;
 }else{
  count=qbe_cast(b,qbe_expression(b,t->variant.array.variant.element_count_expr),
                 qbe_integer_type(b,64,TRUE)).operand;
 }
 if(!strcmp(element,"1"))return count;
 if(!strcmp(count,"1"))return element;
 return qbe_emit(b,"l",qbe_format(b,"mul %s, %s",count,element));
}
void qbe_vla_allocate(QbeBackend *b, a_variable_ptr variable) {
 const char *cell=qbe_vla_cell(b,variable),*bytes,*saved,*pointer;
 unsigned align=qbe_alignment(variable->type);
 bytes=qbe_runtime_size(b,variable->type);
 /* These reserved pseudo-calls are emitted as SP moves by patched QBE.
    Fixed locals, spills, and EH scratch remain addressed relative to x29. */
 saved=qbe_emit(b,"l","call $__edg_qbe_stack_save()");
 qbe_line(b,qbe_format(b,"storel %s, %s",saved,qbe_offset(b,cell,8)));
 if(align>16)bytes=qbe_emit(b,"l",qbe_format(b,"add %s, %u",bytes,align-1));
 pointer=qbe_emit(b,"l",qbe_format(b,"alloc16 %s",bytes));
 if(align>16){
  pointer=qbe_emit(b,"l",qbe_format(b,"add %s, %u",pointer,align-1));
  pointer=qbe_emit(b,"l",qbe_format(b,"and %s, -%u",pointer,align));
 }
 qbe_line(b,qbe_format(b,"storel %s, %s",pointer,cell));
}
static const char *persistent_stack_floor(QbeBackend *b) {
 if(!b->persistentStackFloor_){
  b->persistentStackFloor_=qbe_allocate(b,8,8);
  qbe_buffer_printf(b,&b->entry_,"\tstorel -1, %s\n",b->persistentStackFloor_);
 }
 return b->persistentStackFloor_;
}
void qbe_keep_stack(QbeBackend *b) {
 const char *cell=persistent_stack_floor(b);
 const char *sp=qbe_emit(b,"l","call $__edg_qbe_stack_save()");
 qbe_line(b,qbe_format(b,"storel %s, %s",sp,cell));
}
void qbe_vla_deallocate(QbeBackend *b, a_variable_ptr variable) {
 const char *saved=qbe_emit(b,"l",qbe_format(b,"loadl %s",qbe_offset(b,qbe_vla_cell(b,variable),8)));
 /* An alloca made after a VLA must remain live until function return.
    Keep its low-water mark; later VLA scopes still reclaim below that mark. */
 const char *floor=qbe_emit(b,"l",qbe_format(b,"loadl %s",persistent_stack_floor(b)));
 const char *less=qbe_emit(b,"w",qbe_format(b,"cultl %s, %s",saved,floor));
 const char *mask=qbe_emit(b,"l",qbe_format(b,"extuw %s",less));
 mask=qbe_emit(b,"l",qbe_format(b,"sub 0, %s",mask));
 const char *delta=qbe_emit(b,"l",qbe_format(b,"xor %s, %s",saved,floor));
 delta=qbe_emit(b,"l",qbe_format(b,"and %s, %s",delta,mask));
 saved=qbe_emit(b,"l",qbe_format(b,"xor %s, %s",floor,delta));
 qbe_line(b,qbe_format(b,"call $__edg_qbe_stack_restore(l %s)",saved));
}
QbeValue qbe_load(QbeBackend *b,a_type_ptr type,const char *address) {a_type_ptr t=qbe_underlying(type);const char *op;uint64_t n;if(!t)qbe_fail("load without a type");if(t->kind==tk_routine||qbe_aggregate(t)||qbe_size(t)>8)return qbe_value(type,address);n=qbe_size(t);if(qbe_narrow_float(t))op="loaduh";else if(t->kind==tk_float)op=n==4?"loads":"loadd";else if(n==1)op=qbe_signed_integer(t)?"loadsb":"loadub";else if(n==2)op=qbe_signed_integer(t)?"loadsh":"loaduh";else if(n==4)op="loadw";else if(n==8)op="loadl";else qbe_fail(qbe_format(b,"unsupported scalar load width %" PRIu64,n));return qbe_value(type,qbe_emit(b,qbe_base_type(b,type),qbe_format(b,"%s %s",op,address)));}
void qbe_store(QbeBackend *b,a_type_ptr type,QbeValue value,const char *address) {a_type_ptr t=qbe_underlying(type);QbeValue converted;const char *op;uint64_t n;if(!t)qbe_fail("store without a type");converted=qbe_cast(b,value,type);n=qbe_size(t);if(qbe_aggregate(t)||n>8){qbe_copy(b,address,converted.operand,n);return;}if(qbe_narrow_float(t))op="storeh";else if(t->kind==tk_float)op=n==4?"stores":"stored";else if(n==1)op="storeb";else if(n==2)op="storeh";else if(n==4)op="storew";else if(n==8)op="storel";else qbe_fail(qbe_format(b,"unsupported scalar store width %" PRIu64,n));qbe_line(b,qbe_format(b,"%s %s, %s",op,converted.operand,address));}
void qbe_copy(QbeBackend *b,const char *destination,const char *source,uint64_t bytes) {if(bytes&&strcmp(destination,source))qbe_line(b,qbe_format(b,"blit %s, %s, %" PRIu64,source,destination,bytes));}
void qbe_zero(QbeBackend *b,const char *destination,uint64_t bytes) {uint64_t pos=0;unsigned step;if(bytes>32){QbeRawArg a[3]={{"l",destination},{"w","0"},{"l",qbe_format(b,"%" PRIu64,bytes)}};qbe_runtime_raw(b,"memset","l",3,a);return;}for(step=8;step;step/=2)while(bytes-pos>=step){qbe_line(b,qbe_format(b,"%s 0, %s",step==8?"storel":step==4?"storew":step==2?"storeh":"storeb",qbe_offset(b,destination,(int64_t)pos)));pos+=step;}}
const char *qbe_runtime_raw(QbeBackend *b,const char *name,const char *result,size_t argc,const QbeRawArg *args) {QbeBuffer call={0};size_t i;const char *value;qbe_buffer_printf(b,&call,"call %s%s(",*name=='$'?"":"$",*name=='$'?name:qbe_symbol(b,name));for(i=0;i<argc;++i)qbe_buffer_printf(b,&call,"%s%s %s",i?", ":"",args[i].type,args[i].operand);qbe_buffer_append(b,&call,")");value=qbe_emit(b,result,qbe_buffer_string(&call));qbe_free(b,call.data);return value;}
QbeValue qbe_runtime(QbeBackend *b,const char *name,a_type_ptr result,size_t argc,const QbeValue *args,a_boolean may_throw) {a_type void_type={0},routine_type={0};a_routine_type_supplement supplement={0};a_param_type *params=NULL;size_t i;QbeValue value;void_type.kind=tk_void;routine_type.kind=tk_routine;routine_type.variant.routine.return_type=result?result:&void_type;routine_type.variant.routine.extra_info=&supplement;supplement.prototyped=TRUE;if(argc){if(argc>SIZE_MAX/sizeof(*params))qbe_fail("runtime argument count overflow");params=qbe_alloc(b,argc*sizeof(*params));memset(params,0,argc*sizeof(*params));for(i=0;i<argc;++i){params[i].type=args[i].type;params[i].next=i+1<argc?&params[i+1]:NULL;}}supplement.param_type_list=params;value=qbe_emit_call(b,NULL,&routine_type,qbe_format(b,"$%s",qbe_symbol(b,name)),argc,args,may_throw);qbe_free(b,params);if(!result)value.type=NULL;return value;}
void qbe_add_module_assembly(QbeBackend *b,const char *assembly) {qbe_buffer_append(b,&b->moduleAssemblyBuffer_,assembly);if(b->moduleAssemblyBuffer_.len&&b->moduleAssemblyBuffer_.data[b->moduleAssemblyBuffer_.len-1]!='\n')qbe_buffer_append(b,&b->moduleAssemblyBuffer_,"\n");b->moduleAssembly_=qbe_buffer_string(&b->moduleAssemblyBuffer_);}
void qbe_va_start(QbeBackend *b,const char *address) {
 uint64_t original_sp;const char *stack;a_type_ptr ptr,i32;
 if(!*b->nativePacket_){qbe_line(b,qbe_format(b,"vastart %s",address));return;}
 ptr=qbe_pointer_type(b);i32=qbe_integer_type(b,32,TRUE);original_sp=208+((b->nativeNamedStack_+15)&~UINT64_C(15));
 stack=qbe_load(b,ptr,qbe_offset(b,b->nativePacket_,(int64_t)original_sp)).operand;
 qbe_store(b,ptr,qbe_value(ptr,qbe_offset(b,stack,(int64_t)b->nativeNamedStack_)),address);
 qbe_store(b,ptr,qbe_value(ptr,qbe_offset(b,b->nativePacket_,80)),qbe_offset(b,address,8));
 qbe_store(b,ptr,qbe_value(ptr,qbe_offset(b,b->nativePacket_,208)),qbe_offset(b,address,16));
 qbe_store(b,i32,qbe_value(i32,qbe_format(b,"%d",-(int)(8-(b->nativeNamedGP_<8?b->nativeNamedGP_:8))*8)),qbe_offset(b,address,24));
 qbe_store(b,i32,qbe_value(i32,qbe_format(b,"%d",-(int)(8-(b->nativeNamedFP_<8?b->nativeNamedFP_:8))*16)),qbe_offset(b,address,28));
}
QbeValue qbe_va_arg(QbeBackend *b,a_type_ptr type,const char *address) {
 QbeNativeClass c=qbe_native_class(b,type);uint64_t passed=c.indirect?8:c.bytes;
 a_boolean floating=c.floating_count&&!c.indirect;unsigned slots=floating?(unsigned)c.floating_count:(unsigned)((passed+7)/8),stride=floating?16:8,field=floating?28:24,stack_align;
 const char *result,*reg_try,*reg_read,*stack_read,*done,*cursor,*next,*top,*saved,*stack;size_t i;a_type_ptr ptr=qbe_pointer_type(b),i32=qbe_integer_type(b,32,TRUE);
 if(!passed)return qbe_value(type,qbe_allocate(b,qbe_size(type),qbe_alignment(type)));
 result=qbe_allocate(b,qbe_size(type),qbe_alignment(type));reg_try=qbe_new_label(b,"va_reg");reg_read=qbe_new_label(b,"va_saved");stack_read=qbe_new_label(b,"va_stack");done=qbe_new_label(b,"va_done");
 cursor=qbe_load(b,i32,qbe_offset(b,address,field)).operand;
 qbe_branch(b,qbe_emit(b,"w",qbe_format(b,"csltw %s, 0",cursor)),reg_try,stack_read);qbe_label(b,reg_try);
 if(!floating&&!c.indirect&&c.align>=16)cursor=qbe_emit(b,"w",qbe_format(b,"and %s, -16",qbe_emit(b,"w",qbe_format(b,"add %s, 15",cursor))));
 next=qbe_emit(b,"w",qbe_format(b,"add %s, %u",cursor,slots*stride));qbe_store(b,i32,qbe_value(i32,next),qbe_offset(b,address,field));
 qbe_branch(b,qbe_emit(b,"w",qbe_format(b,"cslew %s, 0",next)),reg_read,stack_read);qbe_label(b,reg_read);
 top=qbe_load(b,ptr,qbe_offset(b,address,floating?16:8)).operand;saved=qbe_emit(b,"l",qbe_format(b,"add %s, %s",top,qbe_emit(b,"l",qbe_format(b,"extsw %s",cursor))));
 if(c.indirect)qbe_copy(b,result,qbe_load(b,ptr,saved).operand,qbe_size(type));
 else if(floating){for(i=0;i<c.floating_count;++i)qbe_copy(b,qbe_offset(b,result,(int64_t)c.floating[i].offset),qbe_offset(b,saved,(int64_t)i*16),qbe_size(c.floating[i].type));}
 else qbe_copy(b,result,saved,qbe_size(type));
 qbe_jump(b,done);qbe_label(b,stack_read);stack=qbe_load(b,ptr,address).operand;stack_align=c.indirect?8:c.align<8?8:c.align>16?16:c.align;
 if(stack_align>8)stack=qbe_emit(b,"l",qbe_format(b,"and %s, -%u",qbe_emit(b,"l",qbe_format(b,"add %s, %u",stack,stack_align-1)),stack_align));
 qbe_copy(b,result,c.indirect?qbe_load(b,ptr,stack).operand:stack,qbe_size(type));qbe_store(b,ptr,qbe_value(ptr,qbe_offset(b,stack,(int64_t)((passed+7)&~UINT64_C(7)))),address);
 qbe_jump(b,done);qbe_label(b,done);return qbe_load(b,type,result);
}
void qbe_emit_scope(QbeBackend *b,a_scope_ptr scope) {
 a_scope_ptr saved,s;a_variable_ptr v;a_routine_ptr r;an_asm_entry_ptr a;a_namespace_ptr n;a_type_ptr t;
 if(!scope||!qbe_set_insert(b,&b->emittedScopes_,scope))return;
 if(scope->kind==sck_function&&ignore_routine_in_back_end(scope->variant.routine.ptr))return;
 if(scope->kind==sck_class_struct_union&&ignore_type_in_back_end(scope->variant.assoc_type))return;
 saved=b->scope_;b->scope_=scope;
 for(v=scope->variables;v;v=v->next)if(!ignore_variable_in_back_end(v)
#if MAINTAIN_NEEDED_FLAGS
 &&needed_flag_is_set(&v->source_corresp)
#endif
 )qbe_emit_variable(b,v);
 b->scope_=saved;
 for(r=scope->routines;r;r=r->next)if(!ignore_routine_in_back_end(r))qbe_emit_routine(b,r);
 for(a=scope->asm_entries;a;a=a->next){size_t length;if(!a->asm_string||a->asm_string->kind!=ck_string)qbe_fail("invalid module assembly constant");length=a->asm_string->variant.string.length;if(length&&a->asm_string->variant.string.value[length-1]==0)--length;qbe_add_module_assembly(b,qbe_string(b,a->asm_string->variant.string.value,length));}
 for(s=scope->scopes;s;s=s->next)qbe_emit_scope(b,s);
 for(n=scope->namespaces;n;n=n->next)if(!n->is_namespace_alias)qbe_emit_scope(b,n->variant.assoc_scope);
 for(t=scope->types;t;t=t->next)if((t->kind==tk_class||t->kind==tk_struct||t->kind==tk_union)&&!ignore_type_in_back_end(t)&&t->variant.class_struct_union.extra_info)qbe_emit_scope(b,t->variant.class_struct_union.extra_info->assoc_scope);
}
static void output_set(QbeBackend *b,QbeMap *m) {size_t i;QbeMapEntry **order=qbe_map_order(b,m);for(i=0;i<m->len;++i)fprintf(b->output_,"%s%s",i?",":"",qbe_json_string(b,order[i]->key));qbe_free(b,order);}
void qbe_module(QbeBackend *b) {
 a_memory_region_number region;size_t i,j,k,l;QbeMapEntry **order;FILE *out=b->output_;
 qbe_emit_scope(b,il_header.primary_scope);
 for(region=FILE_SCOPE_REGION_NUMBER+1;region<=highest_used_region_number;++region)qbe_emit_scope(b,il_header.region_scope_entry[region]);
#if SEPARATE_ROUTINES_FOR_FILE_SCOPE_DYNAMIC_INITS
 {a_routine_list_entry_ptr entry;for(entry=il_header.file_scope_dynamic_init_routines;entry;entry=entry->next){const char *section=".init_array";qbe_emit_routine(b,entry->routine);
#if GNU_INIT_PRIORITY_ATTRIBUTE_ALLOWED
 if(entry->routine->init_priority)section=qbe_format(b,".init_array.%05u",(unsigned)entry->routine->init_priority);
#endif
 qbe_add_module_assembly(b,qbe_format(b,".section %s,\"aw\",%%init_array\n.p2align 3\n.xword %s",section,(const char*)qbe_map_get(&b->symbols_,qbe_routine_symbol(b,entry->routine))));}}
#endif
 for(i=0;i<b->typeDefinitions_.len;++i)fprintf(out,"%s\n",b->typeDefinitions_.data[i]);
 for(i=0;i<b->dataDefinitions_.len;++i)fprintf(out,"%s\n",b->dataDefinitions_.data[i]);
 for(i=0;i<b->functionDefinitions_.len;++i)fprintf(out,"%s\n",b->functionDefinitions_.data[i]);
 fputs("# EDG_QBE_META {\"symbols\":{",out);order=qbe_map_order(b,&b->symbols_);
 for(i=0;i<b->symbols_.len;++i)fprintf(out,"%s%s:%s",i?",":"",qbe_json_string(b,order[i]->key),qbe_json_string(b,order[i]->value));qbe_free(b,order);
 fputs("},\"weak\":[",out);output_set(b,&b->weak_);fputs("],\"hidden\":[",out);output_set(b,&b->hidden_);fputs("],\"aliases\":{",out);order=qbe_map_order(b,&b->aliases_);
 for(i=0;i<b->aliases_.len;++i)fprintf(out,"%s%s:{\"target\":%s,\"exported\":%s}",i?",":"",qbe_json_string(b,order[i]->key),qbe_json_string(b,order[i]->alias.target),order[i]->alias.exported?"true":"false");qbe_free(b,order);
 fputs("},\"eh\":[",out);
 for(i=0;i<b->ehFunctions_.len;++i){QbeEHFunction *f=&b->ehFunctions_.data[i];fprintf(out,"%s{\"name\":%s,\"personality\":%s,\"types\":[",i?",":"",qbe_json_string(b,f->name),qbe_json_string(b,f->personality));
 for(j=0;j<f->types.len;++j)fprintf(out,"%s%s",j?",":"",*f->types.data[j]?qbe_json_string(b,f->types.data[j]):"null");
 fputs("],\"sites\":[",out);for(j=0;j<f->sites.len;++j)fprintf(out,"%s{\"id\":%u,\"landingpad\":%u}",j?",":"",f->sites.data[j].id,f->sites.data[j].landingpad);
 fputs("],\"landingpads\":[",out);for(j=0;j<f->landingpads.len;++j){QbeEHHandler *p=&f->landingpads.data[j];fprintf(out,"%s{\"id\":%u,\"cleanup\":%s,\"catches\":[",j?",":"",p->id,p->cleanup?"true":"false");for(k=0;k<p->catches.len;++k)fprintf(out,"%s%u",k?",":"",p->catches.data[k]);fputc(']',out);
 if(p->specifications.len){fputs(",\"specifications\":[",out);for(k=0;k<p->specifications.len;++k){QbeEHSpecification *s=&p->specifications.data[k];fprintf(out,"%s{\"after\":%zu,\"selector\":%d,\"allowed\":[",k?",":"",s->after,s->selector);for(l=0;l<s->allowed.len;++l)fprintf(out,"%s%u",l?",":"",s->allowed.data[l]);fputs("]}",out);}fputc(']',out);}fputc('}',out);}fputs("]}",out);}
 fputs("],\"native_entries\":[",out);for(i=0;i<b->nativeEntries_.len;++i){QbeNativeEntry *e=&b->nativeEntries_.data[i];fprintf(out,"%s{\"name\":%s,\"body\":%s,\"stack\":%" PRIu64 ",\"exported\":%s}",i?",":"",qbe_json_string(b,e->name),qbe_json_string(b,e->body),e->stack,e->exported?"true":"false");}
 fprintf(out,"],\"module_asm\":%s}\n",qbe_json_string(b,b->moduleAssembly_));
}
