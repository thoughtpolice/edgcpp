/* Metadata-bearing QBE assembly adapter.  GNU Itanium EH uses absolute
 * LSDA/personality/type pointers; link the resulting code with -no-pie. */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <regex.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
#include "assemble.h"
#include "main.h"

typedef struct Mem Mem;
struct Mem { Mem *prev, *next; max_align_t align; };
static Mem *memory;
static regex_t *compiled_regex[16];
static size_t compiled_count;
static FILE *qbe_input, *qbe_output;
static FILE *assembly_destination;
static jmp_buf assembly_error;
static int assembling;
static void cleanup(void) {
    if (qbe_input) { fclose(qbe_input); qbe_input = NULL; }
    if (qbe_output) { fclose(qbe_output); qbe_output = NULL; }
    if (assembly_destination) { fclose(assembly_destination); assembly_destination = NULL; }
    while (compiled_count) regfree(compiled_regex[--compiled_count]);
    while (memory) { Mem *p = memory; memory = p->next; free(p); }
}
static _Noreturn void fail(const char *fmt, ...) {
    va_list ap; fprintf(stderr, "qbe-assemble: "); va_start(ap, fmt);
    vfprintf(stderr, fmt, ap); va_end(ap); fputc('\n', stderr); cleanup();
    if (assembling) longjmp(assembly_error, 1);
    exit(1);
}
static void *alloc(size_t n) {
    if (n > SIZE_MAX - sizeof(Mem)) fail("allocation size overflow");
    Mem *p = malloc(sizeof(Mem) + (n ? n : 1));
    if (!p) fail("out of memory");
    p->prev = NULL; p->next = memory; if (memory) memory->prev = p; memory = p;
    return p + 1;
}
static void *resize(void *v, size_t n) {
    if (!v) return alloc(n);
    if (n > SIZE_MAX - sizeof(Mem)) fail("allocation size overflow");
    Mem *old = (Mem *)v - 1, *prev = old->prev, *next = old->next;
    Mem *p = realloc(old, sizeof(Mem) + (n ? n : 1));
    if (!p) fail("out of memory");
    if (prev) prev->next = p; else memory = p;
    if (next) next->prev = p;
    return p + 1;
}
static void release(void *v) {
    if (!v) return;
    Mem *p = (Mem *)v-1;
    if (p->prev) p->prev->next = p->next; else memory = p->next;
    if (p->next) p->next->prev = p->prev;
    free(p);
}
static char *slice(const char *s, size_t n) { char *v = alloc(n + 1); memcpy(v, s, n); v[n] = 0; return v; }
static char *fmt(const char *format, ...) {
    va_list ap, cp; va_start(ap, format); va_copy(cp, ap);
    int n = vsnprintf(NULL, 0, format, cp); va_end(cp);
    if (n < 0) fail("formatting failure");
    char *s = alloc((size_t)n + 1); vsnprintf(s, (size_t)n + 1, format, ap); va_end(ap); return s;
}
typedef struct { char *s; size_t n, cap; } Buffer;
static void appendn(Buffer *b, const char *s, size_t n) {
    if (n > SIZE_MAX - b->n - 1) fail("buffer size overflow");
    size_t need = b->n + n + 1;
    if (need > b->cap) { size_t c = b->cap ? b->cap : 256; while (c < need) { if (c > SIZE_MAX / 2) { c = need; break; } c *= 2; } b->s = resize(b->s, c); b->cap = c; }
    memcpy(b->s + b->n, s, n); b->n += n; b->s[b->n] = 0;
}
static void append(Buffer *b, const char *s) { appendn(b, s, strlen(s)); }
static void line(Buffer *b, const char *s) { append(b, s); appendn(b, "\n", 1); }
typedef struct { void **v; size_t n, cap; } Vector;
static void push(Vector *v, void *p) {
    if (v->n == v->cap) { if (v->cap > SIZE_MAX / (2 * sizeof(void *))) fail("vector size overflow"); v->cap = v->cap ? v->cap * 2 : 8; v->v = resize(v->v, v->cap * sizeof(void *)); }
    v->v[v->n++] = p;
}
typedef struct { const char *key; void *value; uint64_t hash; } Slot;
typedef struct { Slot *s; size_t n, cap; } Map;
static uint64_t hashstr(const char *s) { uint64_t h = UINT64_C(1469598103934665603); for (; *s; ++s) { h ^= (unsigned char)*s; h *= UINT64_C(1099511628211); } return h; }
static void *get(Map *m, const char *key) {
    if (!m->cap) return NULL;
    uint64_t h = hashstr(key); size_t i = h & (m->cap - 1);
    while (m->s[i].key) { if (m->s[i].hash == h && !strcmp(m->s[i].key, key)) return m->s[i].value; i = (i + 1) & (m->cap - 1); } return NULL;
}
static void put(Map *m, const char *key, void *value) {
    if (!m->cap || m->n + 1 > m->cap / 2) {
        size_t oldcap = m->cap; Slot *old = m->s;
        if (oldcap > SIZE_MAX / (2 * sizeof(Slot))) fail("map size overflow");
        m->cap = oldcap ? oldcap * 2 : 16; m->s = alloc(m->cap * sizeof(Slot)); memset(m->s, 0, m->cap * sizeof(Slot)); m->n = 0;
        for (size_t j = 0; j < oldcap; ++j) if (old[j].key) put(m, old[j].key, old[j].value);
        release(old);
    }
    uint64_t h = hashstr(key); size_t i = h & (m->cap - 1);
    while (m->s[i].key && strcmp(m->s[i].key, key)) i = (i + 1) & (m->cap - 1);
    if (!m->s[i].key) ++m->n;
    m->s[i] = (Slot){key, value, h};
}
/* Escape only bytes that cannot serve directly as a NUL-terminated map key.
 * This is injective even for JSON object keys containing embedded NUL. */
static const char *json_key(const char *s, size_t n) {
    if (!memchr(s,0,n) && !memchr(s,'%',n)) return s;
    Buffer b = {0};
    for (size_t i = 0; i < n; ++i) {
        if (!s[i]) append(&b,"%00"); else if (s[i] == '%') append(&b,"%25"); else appendn(&b,s+i,1);
    }
    return b.s;
}

typedef enum { JNULL, JBOOL, JNUM, JSTR, JARRAY, JOBJECT } Kind;
typedef struct Json Json;
typedef struct { char *key; size_t len; Json *value; } Member;
struct Json { Kind kind; char *text; size_t len; int boolean; Vector items; };
typedef struct { const char *p, *start; unsigned depth; } Parser;
static void jsonerror(Parser *p, const char *why) { fail("invalid metadata JSON at byte %zu: %s", (size_t)(p->p - p->start), why); }
static void space(Parser *p) { while (*p->p == ' ' || *p->p == '\t' || *p->p == '\r' || *p->p == '\n') ++p->p; }
static unsigned hex4(Parser *p) { unsigned v = 0; for (int i = 0; i < 4; ++i) { unsigned char c = (unsigned char)*p->p; unsigned d; if (c >= '0' && c <= '9') d = c - '0'; else if (c >= 'a' && c <= 'f') d = c - 'a' + 10; else if (c >= 'A' && c <= 'F') d = c - 'A' + 10; else { jsonerror(p, "invalid Unicode escape"); return 0; } ++p->p; v = v * 16 + d; } return v; }
static void utf8(Buffer *b, unsigned c) {
    char s[4]; size_t n;
    if (c < 0x80) { s[0] = (char)c; n = 1; }
    else if (c < 0x800) { s[0] = (char)(0xc0 | (c >> 6)); s[1] = (char)(0x80 | (c & 63)); n = 2; }
    else if (c < 0x10000) { s[0] = (char)(0xe0 | (c >> 12)); s[1] = (char)(0x80 | ((c >> 6) & 63)); s[2] = (char)(0x80 | (c & 63)); n = 3; }
    else { s[0] = (char)(0xf0 | (c >> 18)); s[1] = (char)(0x80 | ((c >> 12) & 63)); s[2] = (char)(0x80 | ((c >> 6) & 63)); s[3] = (char)(0x80 | (c & 63)); n = 4; }
    appendn(b, s, n);
}
static Json *jsonstring(Parser *p) {
    Json *j = alloc(sizeof(*j)); memset(j, 0, sizeof(*j)); j->kind = JSTR;
    Buffer b = {0}; ++p->p;
    while (*p->p != '"') {
        unsigned char c = (unsigned char)*p->p++;
        if (c < 32) jsonerror(p, "control byte or unterminated string");
        if (c != '\\') { appendn(&b, (const char *)&c, 1); continue; }
        c = (unsigned char)*p->p++;
        switch (c) {
        case '"': case '\\': case '/': appendn(&b, (const char *)&c, 1); break;
        case 'b': appendn(&b, "\b", 1); break; case 'f': appendn(&b, "\f", 1); break;
        case 'n': appendn(&b, "\n", 1); break; case 'r': appendn(&b, "\r", 1); break; case 't': appendn(&b, "\t", 1); break;
        case 'u': {
            unsigned u = hex4(p);
            if (u >= 0xd800 && u <= 0xdbff) { if (p->p[0] != '\\' || p->p[1] != 'u') jsonerror(p, "unpaired Unicode surrogate"); p->p += 2; unsigned lo = hex4(p); if (lo < 0xdc00 || lo > 0xdfff) jsonerror(p, "invalid Unicode surrogate pair"); u = 0x10000 + ((u - 0xd800) << 10) + lo - 0xdc00; }
            else if (u >= 0xdc00 && u <= 0xdfff) jsonerror(p, "unpaired Unicode surrogate");
            utf8(&b, u); break;
        }
        default: jsonerror(p, "invalid string escape");
        }
    }
    ++p->p; j->text = b.s ? b.s : slice("", 0); j->len = b.n; return j;
}
static Json *jsonvalue(Parser *p) {
    space(p); if (++p->depth > 256) jsonerror(p, "nesting depth exceeds 256");
    if (*p->p == '"') { Json *j = jsonstring(p); --p->depth; return j; }
    Json *j = alloc(sizeof(*j)); memset(j, 0, sizeof(*j));
    if (*p->p == '[' || *p->p == '{') {
        int object = *p->p++ == '{'; j->kind = object ? JOBJECT : JARRAY; space(p);
        char close = object ? '}' : ']';
        Map keys = {0};
        if (*p->p != close) for (;;) {
            if (object) {
                if (*p->p != '"') jsonerror(p, "object key must be a string");
                Json *key = jsonstring(p); space(p); if (*p->p++ != ':') jsonerror(p, "expected colon");
                const char *mapkey = json_key(key->text,key->len);
                if (get(&keys,mapkey)) fail("duplicate metadata key: %s",key->text);
                put(&keys,mapkey,(void *)1);
                Member *m = alloc(sizeof(*m)); *m = (Member){key->text, key->len, jsonvalue(p)}; push(&j->items, m);
            } else push(&j->items, jsonvalue(p));
            space(p); if (*p->p == close) break;
            if (*p->p++ != ',') jsonerror(p, "expected comma");
            space(p);
        }
        ++p->p;
    } else if (!strncmp(p->p, "null", 4)) { j->kind = JNULL; p->p += 4; }
    else if (!strncmp(p->p, "true", 4)) { j->kind = JBOOL; j->boolean = 1; p->p += 4; }
    else if (!strncmp(p->p, "false", 5)) { j->kind = JBOOL; p->p += 5; }
    else {
        const char *start = p->p; if (*p->p == '-') ++p->p;
        if (*p->p == '0') ++p->p;
        else if (*p->p >= '1' && *p->p <= '9') { do ++p->p; while (*p->p >= '0' && *p->p <= '9'); }
        else jsonerror(p, "expected JSON value");
        if (*p->p == '.') { ++p->p; if (!isdigit((unsigned char)*p->p)) jsonerror(p, "expected fractional digit"); while (isdigit((unsigned char)*p->p)) ++p->p; }
        if (*p->p == 'e' || *p->p == 'E') { ++p->p; if (*p->p == '+' || *p->p == '-') ++p->p; if (!isdigit((unsigned char)*p->p)) jsonerror(p, "expected exponent digit"); while (isdigit((unsigned char)*p->p)) ++p->p; }
        j->kind = JNUM; j->len = (size_t)(p->p - start); j->text = slice(start, j->len);
    }
    --p->depth; return j;
}
static Json *field(Json *j, const char *key) {
    if (!j || j->kind != JOBJECT) return NULL;
    size_t n = strlen(key);
    for (size_t i = 0; i < j->items.n; ++i) { Member *m = j->items.v[i]; if (m->len == n && !memcmp(m->key, key, n)) return m->value; } return NULL;
}
static void schema(Json *j, const char *where, const char *const *required, size_t nr, const char *const *optional, size_t no) {
    if (!j || j->kind != JOBJECT) fail("%s: expected an object", where);
    for (size_t i = 0; i < nr; ++i) if (!field(j, required[i])) fail("%s: missing field %s", where, required[i]);
    for (size_t i = 0; i < j->items.n; ++i) {
        Member *m = j->items.v[i]; int found = 0;
        for (size_t k = 0; k < nr; ++k) if (strlen(required[k]) == m->len && !memcmp(required[k], m->key, m->len)) found = 1;
        for (size_t k = 0; k < no; ++k) if (strlen(optional[k]) == m->len && !memcmp(optional[k], m->key, m->len)) found = 1;
        if (!found) fail("%s: unexpected field %s", where, m->key);
    }
}
#define SCHEMA(j,where,...) do { const char *names_[] = {__VA_ARGS__}; schema(j,where,names_,sizeof(names_)/sizeof(names_[0]),NULL,0); } while (0)
static void array(Json *j, const char *where) { if (!j || j->kind != JARRAY) fail("%s must be an array", where); }
static void boolean(Json *j, const char *where) { if (!j || j->kind != JBOOL) fail("%s must be boolean", where); }
static const char *integer(Json *j, const char *where) {
    if (!j || j->kind != JNUM || strpbrk(j->text, ".eE") || (j->text[0] == '-' && strcmp(j->text, "-0"))) fail("%s: expected nonnegative integer", where);
    return !strcmp(j->text, "-0") ? "0" : j->text;
}
static uint64_t u64(Json *j, const char *where) {
    const char *s = integer(j, where); uint64_t v = 0;
    for (; *s; ++s) { unsigned d = (unsigned)(*s - '0'); if (v > (UINT64_MAX - d) / 10) fail("%s exceeds AArch64 address space", where); v = v * 10 + d; } return v;
}
static size_t indexvalue(Json *j, size_t limit, const char *where) {
    uint64_t n = u64(j, where); if (!n || n > limit) fail("%s: invalid type index", where); return (size_t)n;
}

static regex_t rx_short, rx_symbol, rx_marker, rx_label, rx_call, rx_block, rx_branch, rx_terminal, rx_anycall, rx_type, rx_begin, rx_word, rx_lpad, rx_callprefix;
static regex_t rx_stacksave, rx_stackrestore;
static void compile(regex_t *r, const char *s) {
    if (regcomp(r,s,REG_EXTENDED)) fail("cannot initialize assembly parser");
    compiled_regex[compiled_count++] = r;
}
static void regex_init(void) {
    compile(&rx_short, "^[A-Za-z_][A-Za-z0-9_.$]*$"); compile(&rx_symbol, "^[A-Za-z_.$][A-Za-z0-9_.$]*$");
    compile(&rx_marker, "^__edg_qbe_(guard|begin|edge|lpad)_([0-9]+)$");
    compile(&rx_label, "^([A-Za-z_.$][A-Za-z0-9_.$]*):[[:space:]]*$");
    compile(&rx_call, "^[[:space:]]*bl[[:space:]]+([^[:space:]]+)[[:space:]]*$");
    compile(&rx_block, "^\\.Lbb[0-9]+$");
    compile(&rx_branch, "^[[:space:]]*(b(\\.?(eq|ne|cs|cc|mi|pl|vs|vc|hi|ls|ge|lt|gt|le))?)[[:space:]]+(\\.Lbb[0-9]+)[[:space:]]*$");
    compile(&rx_terminal, "^[[:space:]]*(ret|brk|br)([^A-Za-z0-9_]|$)");
    compile(&rx_anycall, "^[[:space:]]*blr?[[:space:]]");
    compile(&rx_type, "^[[:space:]]*\\.type[[:space:]]+([^,[:space:]]+),[[:space:]]*[@%]function[[:space:]]*$");
    compile(&rx_begin, "^[[:space:]]*call[[:space:]]*$");
    compile(&rx_word, "^[[:space:]]*%[A-Za-z0-9_.$]+[[:space:]]*=w[[:space:]]+call[[:space:]]*$");
    compile(&rx_lpad, "^[[:space:]]*%[A-Za-z0-9_.$]+[[:space:]]*=:[A-Za-z0-9_.$]+[[:space:]]+call[[:space:]]*$");
    compile(&rx_callprefix, "(^|[^A-Za-z0-9_])call[[:space:]]*$");
    compile(&rx_stacksave, "^[[:space:]]*%[A-Za-z0-9_.$]+[[:space:]]*=l[[:space:]]+call[[:space:]]+\\$__edg_qbe_stack_save\\(\\)[[:space:]]*$");
    compile(&rx_stackrestore, "^[[:space:]]*call[[:space:]]+\\$__edg_qbe_stack_restore\\(l[[:space:]]+%[A-Za-z0-9_.$]+\\)[[:space:]]*$");
}
static int matches(regex_t *r, const char *s) { return regexec(r, s, 0, NULL, 0) == 0; }
static char *capture(regex_t *r, const char *s, size_t group) { regmatch_t m[8]; if (regexec(r, s, 8, m, 0)) return NULL; return slice(s + m[group].rm_so, (size_t)(m[group].rm_eo - m[group].rm_so)); }
static int tokenchar(unsigned char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.' || c == '$'; }
static int shortstart(unsigned char c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_'; }
static const char *shortname(Json *j, Map *symbols, const char *where) {
    if (!j || j->kind != JSTR || strlen(j->text) != j->len || !matches(&rx_short, j->text)) fail("%s: invalid short name", where);
    if (!get(symbols, j->text)) fail("%s: missing symbol mapping for %s", where, j->text);
    return j->text;
}
static char *trim(char *s) { while (isspace((unsigned char)*s)) ++s; size_t n = strlen(s); while (n && isspace((unsigned char)s[n-1])) --n; return slice(s, n); }
static Vector splitlines(const char *s) {
    Vector v = {0}; const char *p = s, *start = s;
    while (*p) { if (*p == '\n' || *p == '\r' || *p == '\v' || *p == '\f' || (unsigned char)*p == 0x1c || (unsigned char)*p == 0x1d || (unsigned char)*p == 0x1e || ((unsigned char)p[0] == 0xc2 && (unsigned char)p[1] == 0x85) || ((unsigned char)p[0] == 0xe2 && (unsigned char)p[1] == 0x80 && ((unsigned char)p[2] == 0xa8 || (unsigned char)p[2] == 0xa9))) {
        push(&v, slice(start, (size_t)(p-start))); if (*p == '\r' && p[1] == '\n') p += 2; else if ((unsigned char)*p == 0xc2) p += 2; else if ((unsigned char)*p == 0xe2) p += 3; else ++p; start = p;
    } else ++p; }
    if (p != start) push(&v, slice(start, (size_t)(p-start)));
    return v;
}

typedef struct Function Function;
typedef struct Pad Pad;
typedef struct Site Site;
typedef struct { Json *json; size_t after; const char *selector; Json *allowed; } Specification;
struct Pad { Json *json; const char *id; Json *catches; int cleanup, live; Vector specifications; const char *block; };
struct Site { const char *id; Pad *pad; int live; };
struct Function { const char *name, *personality; Json *types; Vector pads, sites, ordered; Map padmap, sitemap; int live; };
typedef struct { const char *kind, *id, *key; Function *function; Pad *pad; Site *site; size_t il, native, seen; int live; } Marker;
typedef struct { Json *json, *symbols_json, *weak, *hidden, *aliases, *entries; Map symbols, markers, functions; Vector markerlist, eh; const char *module_asm; } Metadata;
static const char *canonical_id(const char *s) { while (s[0] == '0' && s[1]) ++s; return s; }
static Marker *marker_get(Metadata *m, const char *kind, const char *id) { return get(&m->markers, fmt("%s:%s", kind, canonical_id(id))); }
static void marker_add(Metadata *m, const char *kind, const char *id, Function *f, Pad *pad, Site *site) {
    Marker *mk = alloc(sizeof(*mk)); memset(mk, 0, sizeof(*mk)); mk->kind = kind; mk->id = id; mk->key = fmt("%s:%s", kind, id); mk->function = f; mk->pad = pad; mk->site = site; put(&m->markers, mk->key, mk); push(&m->markerlist, mk);
}
static Marker *native_marker(Metadata *m, const char *s, const char **kind, const char **id) {
    char *target = capture(&rx_call, s, 1); if (!target || !matches(&rx_marker, target)) return NULL;
    *kind = capture(&rx_marker, target, 1); *id = canonical_id(capture(&rx_marker, target, 2));
    Marker *mk = marker_get(m, *kind, *id); if (!mk) fail("unexpected native EH marker: %s", target); return mk;
}
static void attr_validate(Json *values, Metadata *m, const char *name) {
    array(values, name); Map seen = {0}; for (size_t i = 0; i < values->items.n; ++i) { const char *s = shortname(values->items.v[i], &m->symbols, name); if (get(&seen, s)) fail("duplicate %s symbol", name); put(&seen, s, (void *)1); }
}
static Metadata parse_metadata(const char *source) {
    Metadata m = {0}; Vector lines = splitlines(source); const char *comment = NULL;
    for (size_t i = 0; i < lines.n; ++i) { char *s = lines.v[i]; while (isspace((unsigned char)*s)) ++s; if (!strncmp(s, "# EDG_QBE_META", 14)) { if (comment) fail("expected exactly one EDG_QBE_META comment"); comment = s; } }
    if (!comment) fail("expected exactly one EDG_QBE_META comment");
    if (strncmp(comment, "# EDG_QBE_META ", 15)) fail("malformed metadata comment");
    Parser p = {comment + 15, comment + 15, 0}; m.json = jsonvalue(&p); space(&p); if (*p.p) jsonerror(&p, "trailing data");
    const char *required[] = {"symbols","weak","hidden","aliases","eh"}, *optional[] = {"module_asm","native_entries"};
    schema(m.json, "metadata", required, 5, optional, 2);
    Json *module = field(m.json, "module_asm"); if (module && (module->kind != JSTR || strlen(module->text) != module->len)) fail("module_asm must be an assembly string without NUL bytes"); m.module_asm = module ? module->text : "";
    m.symbols_json = field(m.json, "symbols"); if (m.symbols_json->kind != JOBJECT) fail("symbols must be an object"); Map actual = {0};
    for (size_t i = 0; i < m.symbols_json->items.n; ++i) {
        Member *member = m.symbols_json->items.v[i]; Json *j = member->value;
        if (strlen(member->key) != member->len || !matches(&rx_short, member->key) || !strncmp(member->key, "__edg_qbe_", 10)) fail("invalid or reserved short symbol: %s", member->key);
        if (j->kind != JSTR || !j->len || strlen(j->text) != j->len) fail("invalid actual symbol for %s", member->key);
        for (size_t k = 0; k < j->len; ++k) if ((unsigned char)j->text[k] < 32) fail("invalid actual symbol for %s", member->key);
        if (get(&actual, j->text)) fail("duplicate actual symbol mappings");
        put(&actual, j->text, (void *)1); put(&m.symbols, member->key, j->text);
    }
    m.weak = field(m.json, "weak"); m.hidden = field(m.json, "hidden"); attr_validate(m.weak, &m, "weak"); attr_validate(m.hidden, &m, "hidden");
    m.aliases = field(m.json, "aliases"); if (m.aliases->kind != JOBJECT) fail("aliases must be an object"); Map aliases = {0};
    for (size_t i = 0; i < m.aliases->items.n; ++i) {
        Member *a = m.aliases->items.v[i]; Json key = {0}; key.kind = JSTR; key.text = a->key; key.len = a->len; shortname(&key, &m.symbols, "alias");
        SCHEMA(a->value,"alias","target","exported"); shortname(field(a->value,"target"), &m.symbols,"alias target"); boolean(field(a->value,"exported"),"alias exported"); put(&aliases,a->key,a->value);
    }
    for (size_t i = 0; i < m.aliases->items.n; ++i) {
        Member *a = m.aliases->items.v[i]; Map seen = {0}; put(&seen,a->key,(void *)1); const char *target = field(a->value,"target")->text; Json *next;
        while ((next = get(&aliases,target))) { if (get(&seen,target)) fail("cyclic aliases"); put(&seen,target,(void *)1); target = field(next,"target")->text; }
    }
    m.entries = field(m.json,"native_entries"); Map names = {0}, bodies = {0};
    if (m.entries) { array(m.entries,"native_entries"); for (size_t i = 0; i < m.entries->items.n; ++i) {
        Json *e = m.entries->items.v[i]; const char *req[] = {"name","body","stack"}, *opt[] = {"exported"}; schema(e,"native entry",req,3,opt,1);
        Json *ex = field(e,"exported"); if (ex) boolean(ex,"native entry exported");
        const char *name = shortname(field(e,"name"),&m.symbols,"native entry name"), *body = shortname(field(e,"body"),&m.symbols,"native entry body"); u64(field(e,"stack"),"native entry stack size");
        if (!strcmp(name,body) || get(&names,name) || get(&bodies,body)) fail("duplicate or self-referencing native entry");
        put(&names,name,(void *)1); put(&bodies,body,(void *)1);
    } for (size_t i = 0; i < m.entries->items.n; ++i) if (get(&bodies,field(m.entries->items.v[i],"name")->text)) fail("native entry body is also a native entry"); }
    Json *eh = field(m.json,"eh"); array(eh,"eh"); Map ids = {0};
    for (size_t i = 0; i < eh->items.n; ++i) {
        Json *j = eh->items.v[i]; SCHEMA(j,"EH function","name","personality","types","sites","landingpads");
        Function *f = alloc(sizeof(*f)); memset(f,0,sizeof(*f)); f->name = shortname(field(j,"name"),&m.symbols,"EH function"); f->personality = shortname(field(j,"personality"),&m.symbols,"personality");
        if (get(&m.functions,f->name)) fail("duplicate EH function: %s",f->name);
        put(&m.functions,f->name,f); push(&m.eh,f);
        f->types = field(j,"types"); array(f->types,"types"); for (size_t k = 0; k < f->types->items.n; ++k) { Json *t = f->types->items.v[k]; if (t->kind != JNULL) shortname(t,&m.symbols,"RTTI"); }
        Json *pads = field(j,"landingpads"); array(pads,"landingpads"); size_t offset = 0;
        for (size_t k = 0; k < pads->items.n; ++k) {
            Json *pj = pads->items.v[k]; const char *req[] = {"id","catches","cleanup"}, *opt[] = {"specifications"}; schema(pj,"landingpad",req,3,opt,1);
            Pad *pad = alloc(sizeof(*pad)); memset(pad,0,sizeof(*pad)); pad->json = pj; pad->id = integer(field(pj,"id"),"landingpad id");
            if (get(&ids,pad->id)) fail("duplicate EH id: %s",pad->id);
            put(&ids,pad->id,(void *)1); put(&f->padmap,pad->id,pad); push(&f->pads,pad);
            Json *cl = field(pj,"cleanup"); boolean(cl,"cleanup"); pad->cleanup = cl->boolean; pad->catches = field(pj,"catches"); array(pad->catches,"catches");
            for (size_t z = 0; z < pad->catches->items.n; ++z) indexvalue(pad->catches->items.v[z],f->types->items.n,f->name);
            Json *specs = field(pj,"specifications"); size_t previous = 0;
            if (specs) { array(specs,"specifications"); for (size_t z = 0; z < specs->items.n; ++z) {
                Json *sj = specs->items.v[z]; SCHEMA(sj,"specification","after","allowed","selector");
                Specification *sp = alloc(sizeof(*sp)); sp->json = sj; uint64_t after = u64(field(sj,"after"),"specification position");
                if (after < previous || after > pad->catches->items.n) fail("specification positions must be ordered within catch actions");
                sp->after = (size_t)after; previous = sp->after;
                sp->allowed = field(sj,"allowed"); array(sp->allowed,"specification allowed types"); Map allowed = {0};
                Json *selector = field(sj,"selector"); sp->selector = fmt("-%zu",offset+1);
                if (!selector || selector->kind != JNUM || strcmp(selector->text,sp->selector)) fail("specification selector does not match GNU filter-list offset");
                if (offset == SIZE_MAX) fail("specification offset overflow");
                ++offset;
                for (size_t t = 0; t < sp->allowed->items.n; ++t) {
                    Json *ix = sp->allowed->items.v[t]; size_t n = indexvalue(ix,f->types->items.n,f->name);
                    if (((Json *)f->types->items.v[n-1])->kind == JNULL) fail("%s: invalid specification RTTI index",f->name);
                    const char *key = integer(ix,"specification RTTI"); if (get(&allowed,key)) fail("duplicate specification RTTI"); put(&allowed,key,(void *)1);
                    size_t bytes = 1; while (n >= 128) { n >>= 7; ++bytes; } if (offset > SIZE_MAX-bytes) fail("specification offset overflow"); offset += bytes;
                }
                push(&pad->specifications,sp);
            } }
            if (!pad->cleanup && !pad->catches->items.n && !pad->specifications.n) fail("%s: landingpad has no actions",f->name);
            marker_add(&m,"lpad",pad->id,f,pad,NULL);
        }
        Json *sites = field(j,"sites"); array(sites,"sites"); if (!sites->items.n) fail("%s: empty EH function",f->name); Map referenced = {0};
        for (size_t k = 0; k < sites->items.n; ++k) {
            Json *sj = sites->items.v[k]; SCHEMA(sj,"invoke site","id","landingpad"); Site *site = alloc(sizeof(*site)); memset(site,0,sizeof(*site)); site->id = integer(field(sj,"id"),"invoke id");
            if (get(&ids,site->id)) fail("duplicate EH id: %s",site->id);
            put(&ids,site->id,(void *)1);
            const char *target = integer(field(sj,"landingpad"),"invoke landingpad"); site->pad = get(&f->padmap,target); if (!site->pad) fail("%s: unknown landingpad %s",f->name,target); put(&referenced,target,(void *)1);
            push(&f->sites,site); put(&f->sitemap,site->id,site); marker_add(&m,"guard",site->id,f,NULL,site); marker_add(&m,"begin",site->id,f,NULL,site); marker_add(&m,"edge",site->id,f,NULL,site);
        }
        if (referenced.n != f->pads.n) fail("%s: unreferenced landingpad",f->name);
    }
    Buffer code = {0};
    for (const char *s = source; *s;) {
        if (*s == '"') { ++s; while (*s && *s != '"') { if (*s == '\\' && s[1]) s += 2; else ++s; } if (*s) ++s; }
        else if (*s == '#') { while (*s && *s != '\n') ++s; }
        else { appendn(&code,s,1); ++s; }
    }
    if (!code.s) append(&code,"");
    for (const char *s = code.s; *s; ++s) if (*s == '$' && shortstart((unsigned char)s[1])) {
        const char *end = s+2; while (tokenchar((unsigned char)*end)) ++end; char *name = slice(s+1,(size_t)(end-s-1));
        if (strncmp(name,"__edg_qbe_",10)) { if (!get(&m.symbols,name)) fail("QBE global: missing symbol mapping for %s",name); }
        s = end-1;
    }
    for (const char *s = code.s; *s; ++s) if (*s == '$' && !strncmp(s+1,"__edg_qbe_",10)) {
        const char *end = s+11; while (isalnum((unsigned char)*end) || *end == '_') ++end; char *name = slice(s+1,(size_t)(end-s-1));
        if (!strcmp(name,"__edg_qbe_stack_save") || !strcmp(name,"__edg_qbe_stack_restore")) {
            const char *start = s, *finish = end;
            while (start > code.s && start[-1] != '\n') --start;
            while (*finish && *finish != '\n') ++finish;
            regex_t *r = !strcmp(name,"__edg_qbe_stack_save") ? &rx_stacksave : &rx_stackrestore;
            if (!matches(r,slice(start,(size_t)(finish-start)))) fail("invalid stack intrinsic: %s",name);
            continue;
        }
        if (!matches(&rx_marker,name)) fail("unsupported marker: %s",name);
        const char *kind = capture(&rx_marker,name,1), *id = canonical_id(capture(&rx_marker,name,2)); Marker *mk = marker_get(&m,kind,id);
        const char *tail = end; while (isspace((unsigned char)*tail)) ++tail; if (strncmp(tail,"()",2)) fail("marker is not a no-argument call: %s",name);
        const char *start = s; while (start > code.s && start[-1] != '\n') --start; char *prefix = slice(start,(size_t)(s-start));
        if (!matches(&rx_callprefix,prefix)) fail("marker is not a call: %s",name);
        regex_t *r = !strcmp(kind,"begin") ? &rx_begin : !strcmp(kind,"lpad") ? &rx_lpad : &rx_word;
        if (!matches(r,prefix)) fail("%s marker has invalid return type",kind);
        if (!mk) fail("IL EH markers do not match metadata");
        ++mk->il;
    }
    for (size_t i = 0; i < m.markerlist.n; ++i) if (((Marker *)m.markerlist.v[i])->il != 1) fail("IL EH markers do not match metadata");
    return m;
}

typedef struct { const char *name, *function; Vector successors; int terminal, barrier; size_t visited; } Block;
static void successor(Block *b, const char *name) {
    for (size_t i = 0; i < b->successors.n; ++i) if (!strcmp(b->successors.v[i],name)) return;
    push(&b->successors,(void *)name);
}
static void landing_blocks(Vector *lines, Metadata *m, Map *function_names) {
    const char *current = NULL, *candidate = NULL; Block *block = NULL;
    Map blocks = {0}, pads = {0}, guards = {0}, edges = {0}; Vector blocklist = {0};
    for (size_t i = 0; i < lines->n; ++i) {
        char *s = lines->v[i], *label = capture(&rx_label,s,1), *stripped = trim(s);
        if (label && get(function_names,label)) candidate = label;
        Function *f = current ? get(&m->functions,current) : NULL;
        if (!strcmp(stripped,".cfi_startproc")) { current = candidate; block = NULL; }
        else if (!strcmp(stripped,".cfi_endproc")) {
            if (f && f->live && (!block || !block->terminal)) fail("%s: native function can fall through into landing trampolines",current);
            current = candidate = NULL; block = NULL;
        } else if (f && f->live) {
            if (label && matches(&rx_block,label)) {
                if (get(&blocks,label)) fail("duplicate native basic block: %s",label);
                if (block && !block->terminal) successor(block,label);
                block = alloc(sizeof(*block)); memset(block,0,sizeof(*block)); block->name = label; block->function = current;
                put(&blocks,label,block); push(&blocklist,block); continue;
            }
            const char *kind = NULL, *id = NULL; Marker *mk = native_marker(m,s,&kind,&id);
            if (mk) {
                if (!block) fail("%s: EH marker lacks a native basic-block label",current);
                if (!strcmp(kind,"lpad")) {
                    if (get(&pads,id) || block->barrier || block->successors.n || block->terminal) fail("%s: landingpad %s is not at native block entry",current,id);
                    put(&pads,id,block);
                } else if (!strcmp(kind,"guard") || !strcmp(kind,"edge")) {
                    Map *origins = !strcmp(kind,"guard") ? &guards : &edges;
                    if (get(origins,id)) fail("duplicate native invoke %s: %s",kind,id);
                    put(origins,id,block);
                }
                block->barrier = 1;
            } else if (block) {
                char *branch = capture(&rx_branch,s,1);
                if (branch) { successor(block,capture(&rx_branch,s,4)); if (!strcmp(branch,"b")) block->terminal = 1; }
                else if (matches(&rx_terminal,s)) block->terminal = 1;
                if (matches(&rx_anycall,s)) block->barrier = 1;
            }
        }
    }
    for (size_t i = 0; i < blocklist.n; ++i) {
        Block *b = blocklist.v[i];
        for (size_t k = 0; k < b->successors.n; ++k) {
            Block *next = get(&blocks,b->successors.v[k]);
            if (!next || strcmp(next->function,b->function)) fail("%s: native branch leaves its EH function",b->name);
        }
    }
    size_t visit = 0;
    for (size_t i = 0; i < m->eh.n; ++i) {
        Function *f = m->eh.v[i]; if (!f->live) continue;
        for (size_t k = 0; k < f->pads.n; ++k) {
            Pad *pad = f->pads.v[k]; if (!pad->live) continue;
            Block *b = get(&pads,pad->id);
            if (!b || strcmp(b->function,f->name)) fail("%s: missing native landingpad %s",f->name,pad->id);
            pad->block = b->name;
        }
        for (size_t k = 0; k < f->sites.n; ++k) {
            Site *site = f->sites.v[k]; if (!site->live) continue;
            for (unsigned which = 0; which < 2; ++which) {
                const char *kind = which ? "edge" : "guard"; Block *origin = get(which ? &edges : &guards,site->id);
                if (!origin || strcmp(origin->function,f->name)) fail("%s: missing native invoke %s %s",f->name,kind,site->id);
                Vector pending = {0}; for (size_t z = 0; z < origin->successors.n; ++z) push(&pending,origin->successors.v[z]);
                ++visit; int found = 0;
                while (pending.n) {
                    const char *name = pending.v[--pending.n];
                    if (!strcmp(name,site->pad->block)) { found = 1; break; }
                    Block *b = get(&blocks,name); if (b->visited == visit) continue; b->visited = visit;
                    if (!b->barrier && b->successors.n == 1) push(&pending,b->successors.v[0]);
                }
                if (!found) fail("%s: invoke %s %s does not enter landingpad %s",f->name,site->id,kind,site->pad->id);
            }
        }
    }
}
static void lsda_row(Buffer *out, const char *name, const char *start, const char *end, const char *landing, const char *action) {
    line(out,fmt("\t.uleb128 %s-%s",start,name)); line(out,fmt("\t.uleb128 %s-%s",end,start));
    line(out,fmt("\t.uleb128 %s",landing)); line(out,fmt("\t.uleb128 %s",action));
}
static void lsda_filter(Buffer *out, const char *prefix, Pad *pad, size_t index, const char *filter, int more) {
    const char *label = fmt("%s_Action%s_%zu",prefix,pad->id,index);
    line(out,fmt("%s:",label)); line(out,fmt("\t.sleb128 %s",filter)); line(out,fmt("%s_Next:",label));
    line(out,more ? "\t.sleb128 1" : "\t.sleb128 0");
}
static void emit_lsda(Buffer *out, Function *f) {
    const char *prefix = fmt(".LQBE_%s",f->name); int has_types = f->types->items.n != 0;
    for (size_t i = 0; i < f->pads.n; ++i) if (((Pad *)f->pads.v[i])->specifications.n) has_types = 1;
    line(out,"\t.section .gcc_except_table,\"a\",@progbits"); line(out,"\t.p2align 3"); line(out,fmt("%s_LSDA:",prefix)); line(out,"\t.byte 255");
    line(out,has_types ? "\t.byte 0" : "\t.byte 255");
    if (has_types) { line(out,fmt("\t.uleb128 %s_Types-%s_TypeOffset",prefix,prefix)); line(out,fmt("%s_TypeOffset:",prefix)); }
    line(out,"\t.byte 1"); line(out,fmt("\t.uleb128 %s_CallEnd-%s_CallStart",prefix,prefix)); line(out,fmt("%s_CallStart:",prefix));
    const char *previous = f->name;
    for (size_t i = 0; i < f->ordered.n; ++i) {
        Site *site = f->ordered.v[i]; Pad *pad = site->pad;
        const char *begin = fmt(".LQBECallBegin%s",site->id), *end = fmt(".LQBECallEnd%s",site->id);
        lsda_row(out,f->name,previous,begin,"0","0");
        const char *action = pad->catches->items.n || pad->specifications.n ? fmt("%s_Action%s_0-%s_Actions+1",prefix,pad->id,prefix) : "0";
        lsda_row(out,f->name,begin,end,fmt(".LQBELanding%s-%s",site->id,f->name),action); previous = end;
    }
    lsda_row(out,f->name,previous,fmt("%s_End",prefix),"0","0");
    line(out,fmt("%s_CallEnd:",prefix)); line(out,fmt("%s_Actions:",prefix));
    for (size_t i = 0; i < f->pads.n; ++i) {
        Pad *pad = f->pads.v[i]; if (!pad->live || (!pad->catches->items.n && !pad->specifications.n)) continue;
        size_t total = pad->catches->items.n + pad->specifications.n + (size_t)pad->cleanup, index = 0, spindex = 0;
        for (size_t position = 0; position <= pad->catches->items.n; ++position) {
            while (spindex < pad->specifications.n && ((Specification *)pad->specifications.v[spindex])->after == position) {
                Specification *sp = pad->specifications.v[spindex++]; lsda_filter(out,prefix,pad,index,sp->selector,index+1 < total); ++index;
            }
            if (position < pad->catches->items.n) { Json *c = pad->catches->items.v[position]; lsda_filter(out,prefix,pad,index,c->text,index+1 < total); ++index; }
        }
        if (pad->cleanup) lsda_filter(out,prefix,pad,index,"0",0);
    }
    if (has_types) {
        line(out,"\t.p2align 3");
        for (size_t i = f->types->items.n; i; --i) { Json *t = f->types->items.v[i-1]; line(out,fmt("\t.quad %s",t->kind == JNULL ? "0" : t->text)); }
        line(out,fmt("%s_Types:",prefix));
        /* Keep dead pads' filter lists: selectors encode original byte offsets. */
        for (size_t i = 0; i < f->pads.n; ++i) {
            Pad *pad = f->pads.v[i];
            for (size_t k = 0; k < pad->specifications.n; ++k) {
                Specification *sp = pad->specifications.v[k];
                for (size_t z = 0; z < sp->allowed->items.n; ++z) line(out,fmt("\t.uleb128 %s",((Json *)sp->allowed->items.v[z])->text));
                line(out,"\t.uleb128 0");
            }
        }
    }
    line(out,"");
}
static void emit_native_entry(Buffer *out, Json *e) {
    const char *name = field(e,"name")->text, *body = field(e,"body")->text; uint64_t stack = u64(field(e,"stack"),"native entry stack size");
    line(out,".text"); line(out,".p2align 2"); Json *ex = field(e,"exported");
    if (!ex || ex->boolean) line(out,fmt(".global %s",name));
    line(out,fmt(".type %s, %%function",name)); line(out,fmt("%s:",name)); line(out,".cfi_startproc");
    line(out,fmt("\tadrp x16, %s",body)); line(out,fmt("\tadd x16, x16, :lo12:%s",body)); line(out,fmt("\tmovz x17, #%" PRIu64,stack & 65535));
    for (unsigned shift = 16; shift <= 48; shift += 16) { uint64_t part = (stack >> shift) & 65535; if (part) line(out,fmt("\tmovk x17, #%" PRIu64 ", lsl #%u",part,shift)); }
    line(out,"\tb __edg_qbe_enter_native"); line(out,".cfi_endproc"); line(out,fmt(".size %s, .-%s",name,name));
}
static char *asm_symbol(const char *name) {
    if (matches(&rx_symbol,name)) return (char *)name;
    Buffer b = {0}; append(&b,"\"");
    for (const char *s = name; *s; ++s) { if (*s == '\\' || *s == '"') append(&b,"\\"); appendn(&b,s,1); }
    append(&b,"\""); return b.s;
}
static char *replace_symbols(const char *text, Map *symbols) {
    Buffer b = {0};
    for (const char *p = text; *p;) {
        const char *end = p+1;
        if (shortstart((unsigned char)*p) || *p == '.' || *p == '$') {
            while (tokenchar((unsigned char)*end)) ++end;
            char *token = slice(p,(size_t)(end-p)); const char *actual = get(symbols,token);
            append(&b,actual ? asm_symbol(actual) : token); p = end; continue;
        }
        if (*p == '/' && p[1] == '/') { end = p+2; while (*end && *end != '\n') ++end; }
        else if (*p == '"') {
            while (*end && *end != '"') { if (*end == '\\' && end[1] && end[1] != '\n') end += 2; else if (*end == '\\') break; else ++end; }
            if (*end == '"') ++end; else end = p+1;
        }
        appendn(&b,p,(size_t)(end-p)); p = end;
    }
    if (!b.s) append(&b,"");
    return b.s;
}
static char *adapt(const char *assembly, Metadata *m) {
    Vector lines = splitlines(assembly); Map function_names = {0}, definitions = {0};
    for (size_t i = 0; i < lines.n; ++i) {
        const char *s = lines.v[i]; char *name = capture(&rx_type,s,1);
        if (name) { put(&function_names,name,(void *)1); put(&definitions,name,(void *)1); }
        name = capture(&rx_label,s,1); if (name && matches(&rx_short,name)) put(&definitions,name,(void *)1);
    }
    if (m->entries) for (size_t i = 0; i < m->entries->items.n; ++i) {
        Json *e = m->entries->items.v[i]; const char *name = field(e,"name")->text, *body = field(e,"body")->text;
        if (get(&definitions,name) || field(m->aliases,name)) fail("native entry already defined: %s",name);
        if (!get(&function_names,body)) fail("native entry has undefined QBE body: %s",body);
        put(&definitions,name,(void *)1);
    }
    for (size_t i = 0; i < m->aliases->items.n; ++i) { Member *a = m->aliases->items.v[i]; if (get(&definitions,a->key)) fail("alias also defined: %s",a->key); }
    for (size_t i = 0; i < lines.n; ++i) {
        const char *kind = NULL, *id = NULL; Marker *mk = native_marker(m,lines.v[i],&kind,&id); if (mk) ++mk->native;
    }
    for (size_t i = 0; i < m->eh.n; ++i) {
        Function *f = m->eh.v[i]; if (!get(&function_names,f->name)) fail("EH metadata refers to undefined function");
        for (size_t k = 0; k < f->sites.n; ++k) {
            Site *site = f->sites.v[k];
            Marker *guard = marker_get(m,"guard",site->id), *begin = marker_get(m,"begin",site->id), *edge = marker_get(m,"edge",site->id);
            if (!guard->native && !begin->native && !edge->native) continue;
            if (guard->native != 1 || begin->native != 1 || edge->native != 1) fail("native invoke was only partially removed: %s",site->id);
            site->live = site->pad->live = f->live = guard->live = begin->live = edge->live = 1;
        }
        for (size_t k = 0; k < f->pads.n; ++k) {
            Pad *pad = f->pads.v[k]; if (!pad->live) continue;
            Marker *mk = marker_get(m,"lpad",pad->id); if (mk->native != 1) fail("missing native landingpad: %s",pad->id); mk->live = 1;
        }
    }
    for (size_t i = 0; i < m->markerlist.n; ++i) { Marker *mk = m->markerlist.v[i]; if (mk->native != (size_t)mk->live) fail("native EH markers remain without reachable invoke sites"); }
    landing_blocks(&lines,m,&function_names);
    Buffer out = {0}; const char *current = NULL, *candidate = NULL; Site *active = NULL; unsigned calls = 0; Map starts = {0};
    for (size_t i = 0; i < lines.n; ++i) {
        char *s = lines.v[i], *label = capture(&rx_label,s,1), *stripped = trim(s);
        if (label && get(&function_names,label)) candidate = label;
        Function *f = current ? get(&m->functions,current) : NULL;
        if (!strcmp(stripped,".cfi_startproc")) {
            if (current || !candidate) fail("invalid native CFI function boundary");
            current = candidate; put(&starts,current,(void *)1); f = get(&m->functions,current); line(&out,s);
            if (f && f->live) { line(&out,fmt("\t.cfi_personality 0, %s",f->personality)); line(&out,fmt("\t.cfi_lsda 0, .LQBE_%s_LSDA",current)); }
            continue;
        }
        if (!strcmp(stripped,".cfi_endproc")) {
            if (!current || active) fail("unterminated EH invoke or CFI function");
            if (f && f->live) {
                /* Enter at the real call's return boundary, not at the pad:
                   allocation can restore live values before the edge marker
                   and insert phi transfers on its exceptional successor. */
                for (size_t k = 0; k < f->sites.n; ++k) {
                    Site *site = f->sites.v[k]; if (!site->live) continue;
                    line(&out,fmt(".LQBELanding%s:",site->id));
                    line(&out,"\tstp\tx0, x1, [x29, #16]");
                    line(&out,"\tmov\tw0, #1");
                    line(&out,"\tstr\tw0, [x29, #32]");
                    /* GP call results are undefined on unwind. Do not replay
                       copies with the unwinder payload in the result regs.
                       Exceptional-live values survived the ordinary call's
                       clobbers in callee-saved registers or spill slots. */
                    line(&out,"\tmov\tx0, #0"); line(&out,"\tmov\tx1, #0");
                    line(&out,fmt("\tb\t.LQBECallReturn%s",site->id));
                }
                line(&out,fmt(".LQBE_%s_End:",current));
            }
            current = candidate = NULL;
        }
        const char *kind = NULL, *id = NULL; Marker *mk = native_marker(m,s,&kind,&id);
        if (mk) {
            if (!mk->live || !current || strcmp(mk->function->name,current)) fail("unexpected assembly EH marker: %s %s",kind,id);
            if (++mk->seen != 1) fail("duplicate assembly EH marker: %s %s",kind,id);
            if (!strcmp(kind,"guard")) { if (active) fail("invoke guard inside call range"); line(&out,"\tmov\tw0, #0"); }
            else if (!strcmp(kind,"begin")) { if (active) fail("nested invoke marker"); active = mk->site; calls = 0; line(&out,"\tstr\twzr, [x29, #32]"); }
            else if (!strcmp(kind,"edge")) {
                if (active != mk->site || calls != 1) fail("invoke must bracket exactly one native call");
                active = NULL;
                push(&mk->function->ordered,mk->site); line(&out,"\tldr\tw0, [x29, #32]");
            } else { if (active) fail("landingpad inside invoke"); line(&out,"\tldp\tx0, x1, [x29, #16]"); }
            continue;
        }
        if (strstr(s,"__edg_qbe_")) fail("unrecognized residual marker: %s",stripped);
        if (active && matches(&rx_anycall,s)) {
            if (++calls != 1) fail("invoke must bracket exactly one native call");
            line(&out,fmt(".LQBECallBegin%s:",active->id));
            line(&out,s);
            line(&out,fmt(".LQBECallEnd%s:",active->id));
            /* This label precedes every allocated post-call transfer, even
               those placed before the edge's call-shaped clobber barrier. */
            line(&out,fmt(".LQBECallReturn%s:",active->id));
        } else line(&out,s);
    }
    if (current) fail("unterminated native CFI");
    for (size_t i = 0; i < function_names.cap; ++i) if (function_names.s[i].key && !get(&starts,function_names.s[i].key)) fail("QBE backend lacks required AArch64 CFI patch");
    for (size_t i = 0; i < m->markerlist.n; ++i) { Marker *mk = m->markerlist.v[i]; if (mk->seen != (size_t)mk->live) fail("assembly EH markers do not match metadata"); }
    for (size_t i = 0; i < m->eh.n; ++i) { Function *f = m->eh.v[i]; if (f->live) emit_lsda(&out,f); }
    if (m->entries) for (size_t i = 0; i < m->entries->items.n; ++i) emit_native_entry(&out,m->entries->items.v[i]);
    Map weak = {0};
    for (size_t i = 0; i < m->weak->items.n; ++i) { Json *j = m->weak->items.v[i]; line(&out,fmt("\t.weak %s",j->text)); put(&weak,j->text,(void *)1); }
    for (size_t i = 0; i < m->hidden->items.n; ++i) line(&out,fmt("\t.hidden %s",((Json *)m->hidden->items.v[i])->text));
    for (size_t i = 0; i < m->aliases->items.n; ++i) {
        Member *a = m->aliases->items.v[i]; if (field(a->value,"exported")->boolean && !get(&weak,a->key)) line(&out,fmt("\t.global %s",a->key));
        line(&out,fmt("\t.set %s, %s",a->key,field(a->value,"target")->text));
    }
    /* Python's join adds one newline even when the complete assembly is empty. */
    if (!out.s) line(&out,"");
    char *result = replace_symbols(out.s,&m->symbols);
    if (*m->module_asm) { Buffer final = {0}; append(&final,result); line(&final,m->module_asm); result = final.s; }
    return result;
}

static void validate_utf8(const char *s, size_t n, const char *where) {
    for (size_t i = 0; i < n;) {
        unsigned char c = (unsigned char)s[i++]; if (!c) fail("%s contains a NUL byte",where); if (c < 128) continue;
        unsigned count, value, minimum;
        if (c >= 0xc2 && c <= 0xdf) { count = 1; value = c & 31; minimum = 0x80; }
        else if (c >= 0xe0 && c <= 0xef) { count = 2; value = c & 15; minimum = 0x800; }
        else if (c >= 0xf0 && c <= 0xf4) { count = 3; value = c & 7; minimum = 0x10000; }
        else fail("%s is not UTF-8",where);
        if (count > n-i) fail("%s has truncated UTF-8",where);
        while (count--) { c = (unsigned char)s[i++]; if ((c & 0xc0) != 0x80) fail("%s is not UTF-8",where); value = (value << 6) | (c & 63); }
        if (value < minimum || value > 0x10ffff || (value >= 0xd800 && value <= 0xdfff)) fail("%s is not UTF-8",where);
    }
}
static char *read_stream(FILE *f, const char *where) {
    Buffer b = {0}; char chunk[8192]; size_t n;
    while ((n = fread(chunk,1,sizeof(chunk),f))) appendn(&b,chunk,n);
    if (ferror(f)) fail("cannot read %s: %s",where,strerror(errno));
    if (!b.s) append(&b,"");
    validate_utf8(b.s,b.n,where);
    /* Match Python text streams' universal-newline decoding. */
    size_t writepos = 0;
    for (size_t i = 0; i < b.n; ++i) {
        if (b.s[i] == '\r') { b.s[writepos++] = '\n'; if (i+1 < b.n && b.s[i+1] == '\n') ++i; }
        else b.s[writepos++] = b.s[i];
    }
    b.s[writepos] = 0; return b.s;
}
static char *run_qbe(char *source) {
    qbe_input = fmemopen(source,strlen(source),"r"); qbe_output = tmpfile();
    if (!qbe_input || !qbe_output) fail("cannot create QBE streams: %s",strerror(errno));
    if (il2qbe_qbe_compile(qbe_input,qbe_output,"arm64","<frontend IL>"))
        fail("embedded QBE compilation failed");
    if (fseek(qbe_output,0,SEEK_SET)) fail("cannot rewind QBE assembly: %s",strerror(errno));
    char *assembly = read_stream(qbe_output,"QBE assembly");
    fclose(qbe_input); qbe_input = NULL;
    fclose(qbe_output); qbe_output = NULL;
    return assembly;
}
static int assemble_stream(FILE *input, FILE *output, const char *output_name) {
    if (setjmp(assembly_error)) { assembling = 0; return 1; }
    assembling = 1;
    regex_init();
    char *source = read_stream(input,"QBE input");
    Metadata m = parse_metadata(source);
    char *result = adapt(run_qbe(source),&m);
    /* CLI output is opened only after input has been consumed and compilation
     * succeeds, preserving the adapter's same-file and failure behavior. */
    if (!output) {
        output = !strcmp(output_name,"-") ? stdout : fopen(output_name,"wb");
        if (!output) fail("cannot open %s: %s",output_name,strerror(errno));
        if (output != stdout) assembly_destination = output;
    }
    size_t n = strlen(result);
    if (fwrite(result,1,n,output) != n || fflush(output))
        fail("cannot write assembly: %s",strerror(errno));
    if (assembly_destination) {
        FILE *dest = assembly_destination; assembly_destination = NULL;
        if (fclose(dest)) fail("cannot close assembly output: %s",strerror(errno));
    }
    cleanup(); assembling = 0; return 0;
}
int il2qbe_assemble(FILE *input, FILE *output) {
    return assemble_stream(input,output,NULL);
}
static void usage(FILE *f) { fprintf(f,"usage: edg-qbe --assemble input.qbe -o output.S\n"); }
int il2qbe_assemble_main(int argc, char **argv) {
    const char *input = NULL, *output = NULL; int positional = 0;
    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        if (!positional && (!strcmp(arg,"-h") || !strcmp(arg,"--help"))) { usage(stdout); return 0; }
        if (!positional && !strcmp(arg,"--")) { positional = 1; continue; }
        if (!positional && (!strcmp(arg,"-o") || !strcmp(arg,"--output"))) {
            if (++i == argc) fail("missing argument for %s",arg);
            output = argv[i]; continue;
        }
        if (!positional && !strncmp(arg,"--output=",9)) { output = arg+9; continue; }
        if (!positional && arg[0] == '-' && arg[1] == 'o' && arg[2]) { output = arg+2; continue; }
        if (!positional && arg[0] == '-' && arg[1]) fail("unknown option: %s",arg);
        if (input) fail("unexpected argument: %s",arg);
        input = arg;
    }
    if (!input || !output) { usage(stderr); return 1; }
    FILE *source = !strcmp(input,"-") ? stdin : fopen(input,"rb");
    if (!source) fail("cannot open %s: %s",input,strerror(errno));
    int status = assemble_stream(source,NULL,output);
    if (source != stdin && fclose(source)) status = 1;
    return status;
}
