#ifndef IL2QBE_MAIN_H
#define IL2QBE_MAIN_H
#include <stdio.h>

extern int il2qbe_emit_qbe_only;
extern const char *il2qbe_saved_qbe_path;
int il2qbe_qbe_main(int argc, char **argv);
/* Serial entry point. Leaves caller-owned streams open; diagnostics use stderr.
 * Returns zero on success, nonzero on input/output or QBE parse errors. */
int il2qbe_qbe_compile(FILE *input, FILE *output, const char *target,
                       const char *name);
#endif
