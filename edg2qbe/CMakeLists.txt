# The upstream src/ and util/ trees are never inputs to the C-only build.
# These optional targets use the same checked-in sources as the gcc recipe.
file(GLOB EDG_C_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/edg/*.c")
file(GLOB IL2QBE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB QBE_C_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/qbe/vendor/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/qbe/vendor/amd64/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/qbe/vendor/arm64/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/qbe/vendor/rv64/*.c")

function(il2qbe_c_target target)
  target_include_directories(${target} PRIVATE
      "${CMAKE_CURRENT_SOURCE_DIR}/config"
      "${CMAKE_CURRENT_SOURCE_DIR}/edg"
      "${CMAKE_CURRENT_SOURCE_DIR}")
  target_compile_definitions(${target} PRIVATE _GNU_SOURCE=1)
  set_target_properties(${target} PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED YES
                        C_EXTENSIONS YES)
  target_link_libraries(${target} PRIVATE m)
endfunction()

add_executable(edg-qbe ${EDG_C_SOURCES} ${IL2QBE_SOURCES} ${QBE_C_SOURCES})
il2qbe_c_target(edg-qbe)
target_compile_options(edg-qbe PRIVATE
    -include "${CMAKE_CURRENT_SOURCE_DIR}/config/native.h")

add_executable(edg_munch util/edg_munch.c)
add_executable(edg_prelink util/edg_prelink.c util/decode.c)
add_executable(edg_decode util/edg_decode.c util/decode.c)
add_executable(mk_errinfo util/mk_errinfo.c)
add_executable(dettarg util/dettarg.c)
foreach(target edg_munch edg_prelink edg_decode mk_errinfo dettarg)
  il2qbe_c_target(${target})
endforeach()

# Existing frontend/display targets remain available without changing upstream.
foreach(target cpfe cpfe-cp)
  add_executable(${target} ${EDG_C_SOURCES})
  il2qbe_c_target(${target})
  configure_definitions_as_values("${target}" "$CACHE{EDG_MACRO_CONF}")
  target_compile_definitions(${target} PRIVATE USE_CMAKE_DEFINES=1
      ${configured_cmakedef_definitions})
endforeach()
set(EDG_DISPLAY_NAMES const_ints debug error fe_init fixed_pt float_pt floating
    host_envir il il_display il_read il_to_str il_walk mem_manage src_seq target types)
set(EDG_DISPLAY_SOURCES)
foreach(name IN LISTS EDG_DISPLAY_NAMES)
  list(APPEND EDG_DISPLAY_SOURCES "edg/${name}.c")
endforeach()
add_executable(cdisp ${EDG_DISPLAY_SOURCES})
il2qbe_c_target(cdisp)
configure_definitions_as_values("cdisp" "$CACHE{EDG_MACRO_CONF}")
target_compile_definitions(cdisp PRIVATE USE_CMAKE_DEFINES=1
    ${configured_cmakedef_definitions})

add_custom_target(cpfe-all DEPENDS edg-qbe cpfe cpfe-cp cdisp)
