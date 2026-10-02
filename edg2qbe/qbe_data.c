#include "qbe_codegen.h"
#include "const_ints.h"
#include "float_pt.h"
#include "il.h"
#include "target.h"
#include "types.h"
#include <limits.h>
#include <stdlib.h>
#include <inttypes.h>

typedef struct { uint64_t offset; unsigned bytes; } FloatLeaf;
typedef struct { FloatLeaf data[4]; size_t len; } FloatLeaves;
static a_boolean thread_local_variable(a_variable_ptr v) {if(is_effective_thread_local(v))return TRUE;
#if MICROSOFT_EXTENSIONS_ALLOWED || THREAD_LOCAL_STORAGE_SPECIFIER_ALLOWED
 if(v->decl_modifiers&DM_THREAD)return TRUE;
#endif
 return FALSE;}
static a_boolean floating_layout(QbeBackend *b,a_type_ptr type,uint64_t offset,FloatLeaves *leaves,QbeMap *active) {
 a_boolean valid=TRUE;a_field_ptr field;size_t i;type=qbe_underlying(type);if(!type)return FALSE;
 if(type->kind==tk_float&&(type->size==4||type->size==8)){if(leaves->len==4)return FALSE;leaves->data[leaves->len].offset=offset;leaves->data[leaves->len++].bytes=(unsigned)type->size;return TRUE;}
 if(!qbe_set_insert(b,active,type))return FALSE;
 if(type->kind==tk_array){a_type_ptr element=type->variant.array.element_type;uint64_t count=type->variant.array.variant.number_of_elements,n;if(type->variant.array.is_variable_size_array||count>4)valid=FALSE;for(n=0;valid&&n<count;++n)valid=floating_layout(b,element,offset+n*qbe_size(element),leaves,active);}
 else if(type->kind==tk_union){FloatLeaves largest={0};unsigned primitive=0;for(field=type->variant.class_struct_union.field_list;valid&&field;field=field->next){FloatLeaves alternative={0};if(field->is_optimized_empty_class||qbe_size(field->type)==0)continue;valid=!field->is_bit_field&&floating_layout(b,field->type,offset+field->offset,&alternative,active);for(i=0;i<alternative.len;++i){if(primitive&&primitive!=alternative.data[i].bytes)valid=FALSE;primitive=alternative.data[i].bytes;}if(alternative.len>largest.len)largest=alternative;}if(largest.len>4-leaves->len)valid=FALSE;else {memcpy(leaves->data+leaves->len,largest.data,largest.len*sizeof(*largest.data));leaves->len+=largest.len;}}
 else if(type->kind==tk_struct||type->kind==tk_class){for(field=type->variant.class_struct_union.field_list;valid&&field;field=field->next){if(field->is_optimized_empty_class||qbe_size(field->type)==0)continue;valid=!field->is_bit_field&&floating_layout(b,field->type,offset+field->offset,leaves,active);}}
 else valid=FALSE;
 qbe_map_remove(b,active,type);for(i=1;i<leaves->len;++i)if(leaves->data[i].bytes!=leaves->data[0].bytes)valid=FALSE;return valid;
}
static void integer_bytes(a_constant_ptr constant,unsigned bytes,unsigned char result[16]) {an_integer_value value=constant->variant.integer_value,mask;unsigned i;if(bytes>16)qbe_fail("integer constant exceeds native 128-bit representation");memset(result,0,16);set_unsigned_integer_value(&mask,255);for(i=0;i<bytes;++i){an_integer_value low=value;a_host_large_integer byte;a_boolean error;and_integer_values(&low,&mask);conv_integer_value_to_host_large_integer(&low,FALSE,&byte,&error);if(error)qbe_fail("cannot extract target integer constant byte");result[targ_little_endian?i:bytes-i-1]=(unsigned char)byte;shift_right_integer_value(&value,8,FALSE,FALSE);}}
static unsigned hex_digit(char c) {if(c>='0'&&c<='9')return (unsigned)(c-'0');if(c>='a'&&c<='f')return (unsigned)(c-'a')+10;qbe_fail("invalid EDG floating representation digit");return 0;}
static void float_bytes(a_constant_ptr constant,unsigned bytes,unsigned char result[16]) {a_type_ptr type=qbe_underlying(constant->type);a_number_buffer buffer;const char *hex;unsigned i;if(bytes!=2&&bytes!=4&&bytes!=8&&bytes!=16)qbe_fail("unsupported target floating-point constant format");if(!type||type->kind!=tk_float)qbe_fail("floating constant has no floating representation type");buffer=fp_to_hex_string(type->variant.float_kind,&constant->variant.float_value);hex=edg_string_cstr(&buffer);if(strlen(hex)!=2*bytes)qbe_fail("target floating representation size mismatch");memset(result,0,16);for(i=0;i<bytes;++i)result[targ_little_endian?bytes-i-1:i]=(unsigned char)((hex_digit(hex[2*i])<<4)|hex_digit(hex[2*i+1]));edg_string_destroy(&buffer);}
static uint64_t scalar_bits(const unsigned char bytes[16],unsigned count) {uint64_t bits=0;unsigned i;for(i=0;i<count;++i)bits|=(uint64_t)bytes[targ_little_endian?i:count-i-1]<<(8*i);return bits;}
a_type_ptr qbe_underlying(a_type_ptr type) {while(type&&type->kind==tk_typeref)type=type->variant.typeref.type;return type;}
a_boolean qbe_aggregate(a_type_ptr type) {type=qbe_underlying(type);if(!type)return FALSE;switch(type->kind){case tk_array:case tk_struct:case tk_class:case tk_union:return TRUE;
#if GNU_VECTOR_TYPES_ALLOWED
 case tk_vector:return TRUE;
#endif
 case tk_integer:case tk_float:return type->size>8;default:return FALSE;}}
a_boolean qbe_narrow_float(a_type_ptr type) {type=qbe_underlying(type);if(!type||type->kind!=tk_float||qbe_size(type)!=2)return FALSE;switch(type->variant.float_kind){case fk_float16:case fk_fp16:case fk_std_float16:case fk_std_bfloat16:return TRUE;default:qbe_fail("unsupported native 16-bit floating representation");}return FALSE;}
a_boolean qbe_signed_integer(a_type_ptr type) {type=qbe_underlying(type);return type&&type->kind==tk_integer&&int_kind_is_signed[type->variant.integer.int_kind];}
unsigned qbe_width(a_type_ptr type) {type=qbe_underlying(type);if(!type)return 0;if(type->kind==tk_integer)return (unsigned)integer_value_bit_size_for_type(type);if(qbe_size(type)>UINT_MAX/8)qbe_fail("type width overflow");return (unsigned)(qbe_size(type)*8);}
uint64_t qbe_size(a_type_ptr type) {return type?(uint64_t)size_of_type(type):0;}
unsigned qbe_alignment(a_type_ptr type) {unsigned align=type?(unsigned)alignment_of_type(type):1;return align?align:1;}
const char *qbe_base_type(QbeBackend *b,a_type_ptr type) {type=qbe_underlying(type);if(!type||type->kind==tk_void)return "";if(qbe_aggregate(type))return "l";switch(type->kind){case tk_integer:return qbe_size(type)<=4?"w":"l";case tk_float:if(qbe_narrow_float(type))return "w";if(qbe_size(type)==4)return "s";if(qbe_size(type)==8)return "d";qbe_fail("unsupported native floating scalar type");break;case tk_pointer:case tk_nullptr:case tk_routine:return "l";default:qbe_fail(qbe_format(b,"unlowered or unsupported native QBE type %u",(unsigned)type->kind));}return "";}
static int leaf_compare(const void *a,const void *b) {uint64_t x=((const FloatLeaf*)a)->offset,y=((const FloatLeaf*)b)->offset;return x<y?-1:x>y;}
const char *qbe_abi_type(QbeBackend *b,a_type_ptr type) {a_type_ptr original=type,key;unsigned align=qbe_alignment(type);const char *name;QbeBuffer def={0};type=qbe_underlying(type);if(!qbe_aggregate(type))return qbe_base_type(b,type);key=align==qbe_alignment(type)?type:original;name=qbe_map_get(&b->types_,key);if(name)return name;name=qbe_format(b,":t%zu",b->types_.len);qbe_map_put(b,&b->types_,key,name);qbe_buffer_printf(b,&def,"type %s = align %u { ",name,align);if(type->kind==tk_integer&&qbe_size(type)==16)qbe_buffer_append(b,&def,"l, l");else {FloatLeaves leaves={0};QbeMap active={0};a_boolean hfa=floating_layout(b,type,0,&leaves,&active)&&leaves.len;uint64_t cursor=0;size_t i;qbe_map_free(b,&active);if(hfa){qsort(leaves.data,leaves.len,sizeof(*leaves.data),leaf_compare);for(i=0;i<leaves.len;++i){if(leaves.data[i].offset!=cursor){hfa=FALSE;break;}cursor+=leaves.data[i].bytes;}hfa=hfa&&cursor==qbe_size(type);}if(hfa)qbe_buffer_printf(b,&def,"%s %zu",leaves.data[0].bytes==4?"s":"d",leaves.len);else qbe_buffer_printf(b,&def,"b %" PRIu64,qbe_size(type));}qbe_buffer_append(b,&def," }");qbe_vec_push(b,&b->typeDefinitions_,def.data);return name;}
const char *qbe_symbol(QbeBackend *b,const char *actual) {const char *name;if(*actual=='\1')++actual;if(!*actual)qbe_fail("empty linker symbol name");name=qbe_map_get(&b->symbolNames_,actual);if(name)return name;actual=qbe_string(b,actual,strlen(actual));name=qbe_format(b,"q%u",b->nextSymbol_++);qbe_map_put(b,&b->symbolNames_,actual,name);qbe_map_put(b,&b->symbols_,name,actual);return name;}
const char *qbe_routine_symbol(QbeBackend *b,a_routine_ptr routine) {const char *actual,*name;if(!routine)qbe_fail("null routine symbol");actual=routine->source_corresp.name;
#if GNU_EXTENSIONS_ALLOWED || REDEFINE_EXTNAME_PRAGMA_ENABLED
 if(routine->gnu_extra_info&&routine->gnu_extra_info->asm_name)actual=routine->gnu_extra_info->asm_name;
#endif
 if(actual)name=qbe_symbol(b,actual);else {name=qbe_map_get(&b->unnamedRoutines_,routine);if(!name){name=qbe_symbol(b,qbe_format(b,".L.edg.routine.%u",b->nextSymbol_));qbe_map_put(b,&b->unnamedRoutines_,routine,name);}}
#if GNU_EXTENSIONS_ALLOWED
 if(routine->is_weak||routine->is_weakref)qbe_set_insert(b,&b->weak_,name);
#endif
#if GNU_VISIBILITY_ATTRIBUTE_ALLOWED
 if(routine->ELF_visibility==evk_hidden||routine->ELF_visibility==evk_internal)qbe_set_insert(b,&b->hidden_,name);if(routine->ELF_visibility==evk_protected&&qbe_set_insert(b,&b->protected_,name))qbe_add_module_assembly(b,qbe_format(b,".protected %s",(const char*)qbe_map_get(&b->symbols_,name)));
#endif
 return name;}
const char *qbe_variable_symbol(QbeBackend *b,a_variable_ptr variable) {const char *actual,*name;if(!variable)qbe_fail("null variable symbol");actual=variable->source_corresp.name;if(variable->source_corresp.name_linkage==nlk_none&&variable->source_corresp.is_local_to_function
#if NEED_NAME_MANGLING
 &&!variable->source_corresp.name_has_been_mangled
#endif
 )actual=NULL;
#if GNU_EXTENSIONS_ALLOWED || REDEFINE_EXTNAME_PRAGMA_ENABLED
 if(variable->asm_name_is_valid&&variable->asm_name_or_reg.name)actual=variable->asm_name_or_reg.name;
#endif
 if(actual)name=qbe_symbol(b,actual);else {name=qbe_map_get(&b->unnamedVariables_,variable);if(!name){name=qbe_symbol(b,qbe_format(b,".L.edg.variable.%u",b->nextSymbol_));qbe_map_put(b,&b->unnamedVariables_,variable,name);}}
#if GNU_EXTENSIONS_ALLOWED
 if(variable->is_weak||variable->is_weakref)qbe_set_insert(b,&b->weak_,name);
#endif
#if DO_IL_LOWERING
 if(variable->comdat_group)qbe_set_insert(b,&b->weak_,name);
#endif
#if GNU_VISIBILITY_ATTRIBUTE_ALLOWED
 if(variable->ELF_visibility==evk_hidden||variable->ELF_visibility==evk_internal)qbe_set_insert(b,&b->hidden_,name);if(variable->ELF_visibility==evk_protected&&qbe_set_insert(b,&b->protected_,name))qbe_add_module_assembly(b,qbe_format(b,".protected %s",(const char*)qbe_map_get(&b->symbols_,name)));
#endif
 return name;}
const char *qbe_rtti(QbeBackend *b,a_type_ptr type) {type=qbe_underlying(type);if(!type||!type->typeinfo_var)qbe_fail("missing lowered GNU typeinfo variable");qbe_emit_variable(b,type->typeinfo_var);return qbe_variable_symbol(b,type->typeinfo_var);}
QbeValue qbe_constant(QbeBackend *b,a_constant_ptr value) {a_type_ptr type;const char *base,*name;unsigned char bytes[16];if(!value)qbe_fail("null native constant");type=value->type;if(value->kind==ck_void)return qbe_value(type,"");if(value->kind==ck_address){switch(value->variant.address.kind){case abk_routine:base=qbe_format(b,"$%s",qbe_routine_symbol(b,value->variant.address.variant.routine));break;case abk_variable:base=qbe_var_address(b,value->variant.address.variant.variable);break;case abk_constant:case abk_temporary:{a_constant_ptr object=value->variant.address.variant.constant;name=qbe_map_get(&b->constants_,object);if(!name){name=qbe_symbol(b,qbe_format(b,".L.edg.constant.%u",b->nextSymbol_));qbe_map_put(b,&b->constants_,object,name);qbe_vec_push(b,&b->dataDefinitions_,qbe_data(b,object,object->type,name,qbe_alignment(object->type),FALSE,FALSE));}base=qbe_format(b,"$%s",name);break;}case abk_typeid:base=qbe_format(b,"$%s",qbe_rtti(b,value->variant.address.variant.type));break;default:qbe_fail(qbe_format(b,"unsupported native address constant base %u",(unsigned)value->variant.address.kind));}if(value->variant.address.offset)base=qbe_offset(b,base,value->variant.address.offset);return qbe_value(type,base);}if(!qbe_aggregate(type)&&value->kind==ck_integer){unsigned bits=qbe_width(type),n=(unsigned)qbe_size(type);uint64_t raw;integer_bytes(value,n,bytes);raw=scalar_bits(bytes,n);if(qbe_signed_integer(type)&&bits&&bits<64&&(raw&(UINT64_C(1)<<(bits-1))))raw|=~((UINT64_C(1)<<bits)-1);return qbe_value(type,qbe_format(b,"%" PRIu64,raw));}if(!qbe_aggregate(type)&&value->kind==ck_float){unsigned n=(unsigned)qbe_size(type);float_bytes(value,n,bytes);return qbe_value(type,qbe_format(b,"%" PRIu64,scalar_bits(bytes,n)));}if(value->kind!=ck_integer&&value->kind!=ck_float&&value->kind!=ck_string&&value->kind!=ck_aggregate)qbe_fail(qbe_format(b,"unsupported native constant kind %u",(unsigned)value->kind));name=qbe_map_get(&b->constants_,value);if(!name){name=qbe_symbol(b,qbe_format(b,".L.edg.constant.%u",b->nextSymbol_));qbe_map_put(b,&b->constants_,value,name);qbe_vec_push(b,&b->dataDefinitions_,qbe_data(b,value,type,name,qbe_alignment(type),FALSE,FALSE));}return qbe_value(type,qbe_format(b,"$%s",name));}
/* Sparse ordered treap: zeros occupy no storage, and lookup/update remain
   logarithmic without scanning large zero-initialized target objects. */
typedef struct ImageNode ImageNode;
struct ImageNode { ImageNode *left,*right;uint64_t position,priority;const char *symbol;int64_t displacement;unsigned char byte; };
typedef struct { QbeBackend *b;uint64_t length;ImageNode *root;const char *name; } DataImage;
static uint64_t image_priority(uint64_t n) {n+=UINT64_C(0x9e3779b97f4a7c15);n=(n^(n>>30))*UINT64_C(0xbf58476d1ce4e5b9);n=(n^(n>>27))*UINT64_C(0x94d049bb133111eb);return n^(n>>31);}
static ImageNode *image_merge(ImageNode *a,ImageNode *b) {if(!a)return b;if(!b)return a;if(a->priority<b->priority){a->right=image_merge(a->right,b);return a;}b->left=image_merge(a,b->left);return b;}
static void image_split(ImageNode *root,uint64_t key,ImageNode **left,ImageNode **right) {if(!root){*left=*right=NULL;return;}if(root->position<key){*left=root;image_split(root->right,key,&root->right,right);}else {*right=root;image_split(root->left,key,left,&root->left);}}
static void image_free(DataImage *im,ImageNode *p) {if(!p)return;image_free(im,p->left);image_free(im,p->right);qbe_free(im->b,p);}
static ImageNode *image_find(DataImage *im,uint64_t key) {ImageNode *p=im->root;while(p&&p->position!=key)p=key<p->position?p->left:p->right;return p;}
static ImageNode *image_floor(DataImage *im,uint64_t key) {ImageNode *p=im->root,*found=NULL;while(p){if(p->position<=key){found=p;p=p->right;}else p=p->left;}return found;}
static void image_reserve(DataImage *im,uint64_t position,uint64_t count) {if(position>UINT64_MAX-count)qbe_fail("constant data size overflow");if(position+count>im->length)im->length=position+count;}
static void image_clear(DataImage *im,uint64_t position,uint64_t count) {ImageNode *p,*a,*middle,*end;uint64_t limit;image_reserve(im,position,count);limit=position+count;p=image_floor(im,position);if(p&&p->position<position&&p->symbol&&position-p->position<8)qbe_fail("initializer partially overwrites a pointer relocation");if(count){p=image_floor(im,limit-1);if(p&&p->symbol&&limit-p->position<8)qbe_fail("initializer partially overwrites a pointer relocation");}image_split(im->root,position,&a,&middle);image_split(middle,limit,&middle,&end);image_free(im,middle);im->root=image_merge(a,end);}
static ImageNode *image_node(DataImage *im,uint64_t position) {ImageNode *p=image_find(im,position),*a,*z;if(p)return p;p=qbe_alloc(im->b,sizeof(*p));memset(p,0,sizeof(*p));p->position=position;p->priority=image_priority(position);image_split(im->root,position,&a,&z);im->root=image_merge(image_merge(a,p),z);return p;}
static void image_put(DataImage *im,uint64_t position,unsigned char byte) {ImageNode *p;image_reserve(im,position,1);p=image_floor(im,position);if(p&&p->symbol&&position-p->position<8)qbe_fail("overlapping constant data relocation");if(byte){p=image_node(im,position);p->byte=byte;}else image_clear(im,position,1);}
static void image_separator(DataImage *im,QbeBuffer *out,a_boolean *comma)
{
  if (*comma) qbe_buffer_append(im->b, out, ", ");
  *comma = TRUE;
}
static void image_contents_nodes(DataImage *im,ImageNode *p,QbeBuffer *out,
                                 uint64_t *position,a_boolean *comma)
{
  if (!p) return;
  image_contents_nodes(im, p->left, out, position, comma);
  if (p->position < *position)
    qbe_fail("overlapping constant data relocation");
  if (p->position > *position) {
    image_separator(im, out, comma);
    qbe_buffer_printf(im->b, out, "z %" PRIu64, p->position - *position);
  }
  image_separator(im, out, comma);
  if (p->symbol) {
    qbe_buffer_printf(im->b, out, "l $%s", p->symbol);
    if (p->displacement)
      qbe_buffer_printf(im->b, out, " + %" PRId64, p->displacement);
    *position = p->position + 8;
  } else {
    qbe_buffer_printf(im->b, out, "b %u", p->byte);
    *position = p->position + 1;
  }
  image_contents_nodes(im, p->right, out, position, comma);
}
static const char *image_contents(DataImage *im)
{
  QbeBuffer out = {0};
  uint64_t position = 0;
  a_boolean comma = FALSE;
  image_contents_nodes(im, im->root, &out, &position, &comma);
  if (position < im->length) {
    image_separator(im, &out, &comma);
    qbe_buffer_printf(im->b, &out, "z %" PRIu64, im->length - position);
  }
  if (!comma) qbe_buffer_append(im->b, &out, "z 0");
  return out.data;
}
static void image_fill(DataImage *im,a_constant_ptr constant,a_type_ptr target,uint64_t position,a_boolean head_only) {
 QbeBackend *b=im->b;unsigned char bytes[16];target=qbe_underlying(target);if(!constant)return;if(!target)qbe_fail("initializer has no target type");
 if(constant->kind==ck_integer||constant->kind==ck_float){unsigned count=(unsigned)qbe_size(target),i;if(constant->kind==ck_integer)integer_bytes(constant,count,bytes);else float_bytes(constant,count,bytes);image_clear(im,position,count);for(i=0;i<count;++i)image_put(im,position+i,bytes[i]);}
 else if(constant->kind==ck_string){uint64_t count=constant->variant.string.length,i;if(qbe_size(target)&&count>qbe_size(target))count=qbe_size(target);image_clear(im,position,count);for(i=0;i<count;++i)image_put(im,position+i,(unsigned char)constant->variant.string.value[i]);}
 else if(constant->kind==ck_address){const char *base;if(qbe_size(target)!=8)qbe_fail("address relocation requires eight-byte target storage");switch(constant->variant.address.kind){case abk_routine:base=qbe_routine_symbol(b,constant->variant.address.variant.routine);break;case abk_variable:{a_variable_ptr v=constant->variant.address.variant.variable;if(thread_local_variable(v))qbe_fail("static initializer contains dynamic TLS address");if(v->storage_class==sc_auto||v->storage_class==sc_register)qbe_fail("static initializer addresses automatic storage");base=qbe_variable_symbol(b,v);break;}case abk_constant:case abk_temporary:{a_constant_ptr object=constant->variant.address.variant.constant;base=qbe_map_get(&b->constants_,object);if(!base){base=qbe_symbol(b,qbe_format(b,".L.edg.constant.%u",b->nextSymbol_));qbe_map_put(b,&b->constants_,object,base);qbe_vec_push(b,&b->dataDefinitions_,qbe_data(b,object,object->type,base,qbe_alignment(object->type),FALSE,FALSE));}break;}case abk_typeid:base=qbe_rtti(b,constant->variant.address.variant.type);break;default:qbe_fail(qbe_format(b,"unsupported static address relocation base %u",(unsigned)constant->variant.address.kind));}
 image_clear(im,position,8);{ImageNode *p=image_node(im,position);p->symbol=base;p->displacement=constant->variant.address.offset;}}
 else if(constant->kind==ck_aggregate){a_boolean array=target->kind==tk_array,vector=FALSE,record=target->kind==tk_struct||target->kind==tk_class||target->kind==tk_union;a_field_ptr field;a_type_ptr element;a_constant_ptr item;uint64_t cursor=0;
#if GNU_VECTOR_TYPES_ALLOWED
 vector=target->kind==tk_vector;
#endif
 if(!array&&!vector&&!record)qbe_fail("aggregate initializer has nonaggregate target");field=record?next_non_empty_initializable_field(target->variant.class_struct_union.field_list):NULL;element=array?target->variant.array.element_type:NULL;
#if GNU_VECTOR_TYPES_ALLOWED
 if(vector)element=target->variant.vector.element_type;
#endif
 for(item=constant->variant.aggregate.first_constant;item;item=item->next){a_constant_ptr repeated=item;uint64_t count=1;a_boolean omit_tail=FALSE;if(item->kind==ck_designator){if(item->variant.designator.is_generic)qbe_fail("unlowered generic initializer designator");if(item->variant.designator.is_field_designator){if(!record)qbe_fail("field designator in array initializer");field=item->variant.designator.variant.field;}else {if(record)qbe_fail("array designator in record initializer");cursor=item->variant.designator.variant.array_element*qbe_size(element);}continue;}
 if(item->kind==ck_init_repeat){repeated=item->variant.init_repeat.constant;count=item->variant.init_repeat.count;if(!count||!repeated)qbe_fail("unresolved initializer repetition");omit_tail=item->variant.init_repeat.multidimensional_aggr_tail_not_repeated;}
 if(record){if(!field||count!=1)qbe_fail("record initializer exceeds field layout");if(field->is_bit_field){unsigned i;if(repeated->kind!=ck_integer)qbe_fail("noninteger bitfield initializer");integer_bytes(repeated,(field->bit_size+7)/8,bytes);for(i=0;i<field->bit_size;++i){uint64_t bit=field->offset_bit_remainder+i,byte=position+field->offset+bit/8;ImageNode *existing=image_find(im,byte);unsigned char old=existing?existing->byte:0;unsigned mask=1u<<(bit%8);image_put(im,byte,(unsigned char)((old&~mask)|(((bytes[i/8]>>(i%8))&1u)?mask:0)));}}
 else image_fill(im,repeated,field->type,position+field->offset,head_only);field=target->kind==tk_union?NULL:next_non_empty_initializable_field(field->next);}
 else {a_type_ptr repeated_type=qbe_underlying(repeated->type),destination_type=element;uint64_t stride,i;while(qbe_underlying(destination_type)->kind==tk_array&&repeated_type&&repeated_type->kind!=tk_array)destination_type=qbe_underlying(destination_type)->variant.array.element_type;stride=qbe_size(destination_type);if(!stride&&count)qbe_fail("initializer repetition has zero stride");for(i=0;i<count;++i){image_fill(im,repeated,destination_type,position+cursor,head_only||(omit_tail&&i+1<count));cursor+=stride;}}
 if(head_only&&!constant->explicit_braces_on_aggregate&&constant->variant.aggregate.first_constant&&constant->variant.aggregate.first_constant->kind==ck_designator)break;}}
 else qbe_fail(qbe_format(b,"global initializer for %s contains unlowered constant kind %u",(const char*)qbe_map_get(&b->symbols_,im->name),(unsigned)constant->kind));
}
const char *qbe_data(QbeBackend *b,a_constant_ptr value,a_type_ptr type,const char *name,unsigned align,a_boolean exported,a_boolean tls) {DataImage im;const char *contents,*definition;if(!targ_little_endian||targ_char_bit!=8)qbe_fail("native QBE backend requires little-endian eight-bit target bytes");im.b=b;im.length=qbe_size(type);im.root=NULL;im.name=name;image_fill(&im,value,type,0,FALSE);contents=image_contents(&im);definition=qbe_format(b,"%s%sdata $%s = align %u { %s }",exported?"export ":"",tls?"thread ":"",name,align?align:1,contents);image_free(&im,im.root);qbe_free(b,(void*)contents);return definition;}
void qbe_emit_variable(QbeBackend *b,a_variable_ptr variable) {const char *name,*definition,*section="";an_init_kind kind;an_initializer_ptr initializer;a_constant_ptr value=NULL;a_boolean exported;unsigned align;if(!variable)return;name=qbe_variable_symbol(b,variable);
#if GNU_EXTENSIONS_ALLOWED
 if(variable->aliased_variable){QbeAlias alias={qbe_variable_symbol(b,variable->aliased_variable),variable->source_corresp.name_linkage==nlk_external};qbe_alias_set(b,name,alias);return;}
#endif
 if(variable->storage_class==sc_extern||variable->storage_class==sc_auto||variable->storage_class==sc_register||variable->is_parameter)return;if(!qbe_set_insert(b,&b->defined_,name))return;kind=variable->init_kind;initializer=&variable->initializer;if(kind==initk_function_local){if(!b->scope_)qbe_fail("function-local initializer has no owning scope");get_variable_initializer(variable,b->scope_,&kind,&initializer);}if(kind==initk_static)value=initializer->constant;else if(kind==initk_dynamic&&initializer->dynamic&&initializer->dynamic->kind==dik_constant)value=initializer->dynamic->variant.constant.ptr;else if(kind!=initk_none&&kind!=initk_zero&&kind!=initk_dynamic&&kind!=initk_function_local)qbe_fail("unlowered global variable initialization kind");exported=variable->source_corresp.name_linkage==nlk_external;align=qbe_alignment(variable->type);if((unsigned)variable->alignment>align)align=(unsigned)variable->alignment;definition=qbe_data(b,value,variable->type,name,align,exported,thread_local_variable(variable));if(variable->is_immutable&&value&&!thread_local_variable(variable))section=".data.rel.ro";
#if GNU_EXTENSIONS_ALLOWED || MICROSOFT_EXTENSIONS_ALLOWED
 if(variable->section)section=variable->section;
#endif
 if(*section)definition=qbe_format(b,"section %s %s",qbe_json_string(b,section),definition);qbe_vec_push(b,&b->dataDefinitions_,definition);}
a_type_ptr qbe_integer_type(QbeBackend *b,unsigned bits,a_boolean is_signed) {const void *key=(const void*)(uintptr_t)(bits*2+(is_signed!=FALSE));a_type_ptr type=qbe_map_get(&b->integerTypes_,key);if(type)return type;if(bits!=8&&bits!=16&&bits!=32&&bits!=64&&bits!=128)qbe_fail("unsupported synthetic integer width");type=qbe_alloc(b,sizeof(*type));memset(type,0,sizeof(*type));type->kind=tk_integer;type->variant.integer.int_kind=int_kind_for_bit_size(bits,is_signed);type->size=bits/8;type->alignment=bits==128?16:bits/8;qbe_vec_push(b,&b->syntheticTypes_,type);qbe_map_put(b,&b->integerTypes_,key,type);return type;}
a_type_ptr qbe_pointer_type(QbeBackend *b) {a_type_ptr pointee,type;if(b->pointerType_)return b->pointerType_;pointee=qbe_alloc(b,sizeof(*pointee));memset(pointee,0,sizeof(*pointee));pointee->kind=tk_void;pointee->alignment=1;pointee->incomplete=TRUE;type=qbe_alloc(b,sizeof(*type));memset(type,0,sizeof(*type));type->kind=tk_pointer;type->size=8;type->alignment=8;type->variant.pointer.type=pointee;qbe_vec_push(b,&b->syntheticTypes_,pointee);qbe_vec_push(b,&b->syntheticTypes_,type);b->pointerType_=type;return type;}
