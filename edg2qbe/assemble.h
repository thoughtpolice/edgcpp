#ifndef IL2QBE_ASSEMBLE_H
#define IL2QBE_ASSEMBLE_H
#include <stdio.h>

/* Adapt metadata-bearing QBE to Linux AArch64 assembly in-process.
 * Serial, non-reentrant; caller owns streams. Errors are diagnosed to stderr
 * and return nonzero; successful output is flushed but not closed. */
int il2qbe_assemble(FILE *input, FILE *output);
int il2qbe_assemble_main(int argc, char **argv);
#endif
