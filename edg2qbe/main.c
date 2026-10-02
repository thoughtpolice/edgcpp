#include "main.h"
#include "assemble.h"
#include <string.h>

int il2qbe_emit_qbe_only;
const char *il2qbe_saved_qbe_path;
extern int edg_main(int argc, char **argv);

int main(int argc, char **argv)
{
    if (argc > 1 && !strcmp(argv[1], "--qbe"))
        return il2qbe_qbe_main(argc - 1, argv + 1);
    if (argc > 1 && !strcmp(argv[1], "--assemble"))
        return il2qbe_assemble_main(argc - 1, argv + 1);
    while (argc > 1) {
        int consumed;
        if (!strcmp(argv[1], "--emit-qbe")) {
            il2qbe_emit_qbe_only = 1;
            consumed = 1;
        } else if (!strcmp(argv[1], "--save-qbe")) {
            if (argc < 3 || !argv[2][0]) {
                fputs("edg-qbe: --save-qbe requires a path\n", stderr);
                return 1;
            }
            if (il2qbe_saved_qbe_path) {
                fputs("edg-qbe: --save-qbe specified more than once\n", stderr);
                return 1;
            }
            il2qbe_saved_qbe_path = argv[2];
            consumed = 2;
        } else {
            break;
        }
        /* Remove dispatch options, preserving argv[0] for EDG. */
        memmove(argv + 1, argv + 1 + consumed,
                (size_t)(argc - consumed) * sizeof(*argv));
        argc -= consumed;
    }
    if (il2qbe_emit_qbe_only && il2qbe_saved_qbe_path) {
        fputs("edg-qbe: --save-qbe cannot be combined with --emit-qbe\n", stderr);
        return 1;
    }
    return edg_main(argc, argv);
}
