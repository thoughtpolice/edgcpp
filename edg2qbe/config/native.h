#ifndef IL2QBE_NATIVE_CONFIG_H
#define IL2QBE_NATIVE_CONFIG_H

/* The ordinary build needs no generated headers or build-system definitions. */
#define USE_CMAKE_DEFINES 1
#define EDG_MAIN edg_main
#define DO_IL_LOWERING 1
#define DO_FULL_PORTABLE_EH_LOWERING 0
#define GENERATE_EH_TABLES 0
#define INSTANTIATE_EXTERN_INLINE 0
#define IL_SHOULD_BE_WRITTEN_TO_FILE 0
#define BACK_END_IS_C_GEN_BE 0
#define BACK_END_IS_CP_GEN_BE 0
#define BACK_END_SHOULD_BE_CALLED 1
#define BACK_END_IS_QBE 1

#endif
