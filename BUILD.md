# Build Instructions

This document contains build instructions for building relatively-standard
debug and release builds for the most common operating systems via the official
build.

> [!TIP]
>
> `src/` and `util/` are pristine upstream EDG sources. The checked-in C port,
> native backend, assembly adapter, and embedded QBE sources all live under
> `edg2qbe/`. The compiler builds with one
> `gcc` command, without CMake, Python, source generation, downloads, or a C++
> compiler/runtime.

## Requirements

The standalone compiler currently targets native Linux AArch64. Its build
requires GCC supporting GNU C11, the system C headers/libraries, and the
ordinary host assembler/linker used by GCC. Verification used GCC 15.2.0.

Python 3 is needed only for optional build/test/upgrade automation. CMake 3.19
or later and Ninja are needed only for the optional CMake integration.
C++ development headers and a C++ linker driver are needed for target programs
using the installed C++ standard library and for G++ reference comparisons,
not for building or self-hosting the compiler.

For optional CMake builds, [ninja](https://ninja-build.org/) is recommended
but not required. Consult the CMake
[generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html)
documentation for a list of additional supported build tools.

## Standalone compiler: one gcc command

From inside `edg2qbe/` on Linux AArch64:

```sh
gcc -std=gnu11 -D_GNU_SOURCE \
  -Iconfig -Iedg -I. \
  -include config/native.h \
  edg/*.c *.c \
  qbe/vendor/*.c qbe/vendor/amd64/*.c \
  qbe/vendor/arm64/*.c qbe/vendor/rv64/*.c \
  -lm -o edg-qbe
```

For a statically linked compiler, add `-static` to the same command when
static libc/libm archives are installed. This variant was built and exercised
on Linux AArch64: its ELF has no dynamic section or shared-library dependency.

This produces one executable containing the frontend, IL-to-QBE backend,
QBE optimizer/code generator, and native ABI/exception assembly adapter.
The compiler does not execute another QBE process. Compile a header-free
sample directly, without Python or generated macro tables:

```sh
./edg-qbe --c++20 --clear_flag use_predefined_macro_file --instantiate used \
  --output templates.S examples/il/templates.cpp
gcc -c qbe/runtime.c -o runtime.o
gcc -c qbe/native.S -o native.o
g++ -no-pie templates.S runtime.o native.o -latomic -lm -o templates
./templates
```

The remaining assembler/linker and target-program libraries are not embedded
in the compiler. Header search paths and GNU predefined macro tables can be
supplied through EDG's existing options; the optional Python driver discovers
them for programs using installed system headers.

The complete `edg2qbe/` directory can be copied elsewhere and built without
the surrounding checkout. Its build drivers and bootstrap also use only
package-local compiler sources and support files. Pristine EDG trees are
required only for C-port regeneration. Examples and backend regressions live
inside the package; their native runners need no surrounding checkout.
Other commands below run from the repository root.

Compiler-owned options precede the ordinary EDG options:

* Default: compile a C/C++ source to adapted assembly (`.S`).
* `--save-qbe FILE`: also retain the exact intermediate QBE text.
* `--emit-qbe`: emit raw QBE instead of assembly.
* `--qbe`: run the embedded QBE CLI, for example
  `./edg-qbe --qbe -t arm64 input.qbe -o raw.S`.
* `--assemble`: adapt frontend QBE metadata through embedded QBE, for example
  `./edg-qbe --assemble input.qbe -o final.S`.

### Upstream and local source boundaries

* `src/`, `util/`: byte-for-byte upstream EDG at
  `0ac366374c06e54612ddbac397c74ffa40e5182c`. Never edit these to implement the backend.
* `edg2qbe/edg/`, `edg2qbe/util/`: checked-in derived C sources.
  Upstream EDG uses C++ constructs despite its `.c` filenames; it cannot be
  compiled unchanged in C mode.
* `edg2qbe/port/c-port.patch`: the explicit C-port adaptations.
  `manifest.json` records pristine/derived file inventories, hashes, modes,
  and the patch digest. The native backend is not part of this patch.
* `edg2qbe/*.c`, `edg2qbe/*.h`: native backend, compiler entry, and assembly adapter.
* `edg2qbe/config/`: checked-in native configuration and generated error tables.
* `edg2qbe/qbe/vendor/`: pinned QBE core and all three architecture backends, with
  its license and upgrade provenance. `edg2qbe/qbe/qbe-1.3.patch` isolates the local
  code-generation, symbol-namespace, and callable-entry adaptations.

EDG's C-generating backend can translate and self-regenerate the original
C++ frontend, but its output is not a host-ABI-neutral C distribution. Even
with GNU-specific generation disabled, translation can fold `sizeof(void *)`
to `8` and a pointer/long aggregate's size to `16`; compiling that output for
ILP32 does not recompute the correct `4` and `8`. The experimental frontend
snapshot also expands host libc declarations and ABI-specific types.
The maintained C-port patch is therefore retained. The supported distribution
remains the Linux AArch64/GNU C11 configuration described above, not a claim
that one generated snapshot works with arbitrary C compilers and host ABIs.

Normal builds consume these files directly. Maintenance checks reproduce
the derived trees without modifying them:

```sh
python3 edg2qbe/port/regenerate.py --check
python3 edg2qbe/qbe/update_vendor.py --check --archive edg2qbe/build/qbe-1.3.tar.xz
```

To upgrade EDG, replace the pristine trees from the selected upstream commit,
rebase `c-port.patch` and resolve its conflicts, then explicitly record and
regenerate from that baseline:

```sh
python3 edg2qbe/port/regenerate.py --refresh-manifest --baseline UPSTREAM_COMMIT
```

The upgrade command verifies the pristine files against the named Git commit
and requires the patch to apply before publishing a new manifest/C tree.
Ordinary regeneration rejects unrecorded upstream or patch changes.
After changing error definitions, regenerate the checked-in error headers:

```sh
gcc -std=gnu11 -Iedg2qbe/edg edg2qbe/util/mk_errinfo.c -o /tmp/edg-mk-errinfo
/tmp/edg-mk-errinfo edg2qbe/edg/error_msg.txt edg2qbe/edg/error_tag.txt \
  edg2qbe/config/err_codes.h edg2qbe/config/err_data.h
```

After a QBE upgrade, update its pinned archive URL/SHA256, rebase the QBE patch,
and run `edg2qbe/qbe/update_vendor.py --archive PATH`. Neither maintenance tool silently
accepts an unrecorded source release. Rebuild and rerun the examples, regression
checks, and complete self-bootstrap after either upgrade.


## Optional CMake integration

CMake remains available for the frontend/IL tools; the standalone compiler
does not require it. The C port is verified with the Linux AArch64 configuration
below. Other upstream presets have not been validated with this port.


## Linux AArch64: frontend and Intermediate Language

For a native Linux AArch64 build with GCC, use the `linux-aarch64-gcc`
macro configuration.  The `linux-gcc-debug` preset uses x86-64 settings,
including libquadmath and 80-bit `long double`; it is not the native
AArch64 configuration.

The compiler implementation is built with the C compiler. For the native
QBE compiler and its verified C-only build, use the next section.

```sh
cmake -S . -B build/gcc-aarch64 -G Ninja \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_C_COMPILER=gcc \
  -DEDG_MACRO_CONF=linux-aarch64-gcc \
  -DEDG_BUILD_LIB=OFF
cmake --build build/gcc-aarch64 --target cpfe cdisp -j 4
build/gcc-aarch64/bin/cpfe --version
```

This configuration uses binary128 `long double` and EDG's internal
floating-point string conversion routines, without libquadmath.
`EDG_BUILD_LIB=OFF` skips the EDG runtime-library build and its `EDG_BASE`
requirement; neither is needed to parse these samples into IL.

### Parse the samples

The header-free programs in `edg2qbe/examples/il/` exercise control flow and
floating-point constants (`control_flow.cpp`), C++20 concepts, templates,
constant evaluation and generic lambdas (`templates.cpp`), and inheritance,
virtual dispatch and exception handling (`classes.cpp`).

Run from the repository root:

```sh
mkdir -p build/gcc-aarch64/il
for name in control_flow templates classes; do
  build/gcc-aarch64/bin/cpfe \
    --c++20 --clear_flag use_predefined_macro_file --instantiate used \
    --no_il_lowering \
    --output "build/gcc-aarch64/il/$name.cil" \
    "edg2qbe/examples/il/$name.cpp" || exit 1
  build/gcc-aarch64/bin/cdisp "build/gcc-aarch64/il/$name.cil" \
    > "build/gcc-aarch64/il/$name.il.txt" || exit 1
done
```

`--clear_flag use_predefined_macro_file` disables the driver-supplied
`lib/predefined_macros.txt` lookup for these standalone, header-free samples.
It does not configure system headers or a C++ standard library.
`--no_il_lowering` preserves the high-level C++ IL and suppresses the
C-generating back end.  `--instantiate used` emits the used template
definitions.  The `.cil` files are binary IL; `cdisp` reads them back into
memory and displays their declarations, types, statements and expressions.
The dumps report `il_has_C_semantics: FALSE` and include separate memory
regions for function bodies.

Alternatively, add `--il_display` to the `cpfe` command to display the IL
directly on standard output.  Build `cpfe` and `cdisp` with matching macro
configurations: binary IL is configuration-dependent, not a portable
interchange format.  See `doc/source/il.rst` and `src/il_def.h` for the
IL model and data structures.

### Emit and run programs using installed libstdc++

The `cpfe-cp` target uses the C++-generating back end.  It parses the original
source and included headers into EDG IL, then emits C++ source with the
headers expanded.  G++ compiles and links that emitted source into native
executables using the installed libstdc++; no EDG runtime library is needed.

After configuring the AArch64 build above:

```sh
cmake --build build/gcc-aarch64 --target cpfe-cp -j 4
python3 edg2qbe/examples/libstdcxx/run.py
```

The runner discovers the installed G++ version and include search paths,
generates `base/lib/predefined_macros.txt` using
`util/make_predef_macro_table`, and passes these settings to EDG:

* `--g++ --gnu_version 150200 --gen_c_gnu_version 150200` for the tested
  G++ 15.2.0 installation (the version is discovered, not hard-coded).
* `--c++20` and `--sys_include` for each installed compiler include directory.
* `--edg_base_dir` for the generated predefined-macro table.
* `--gen_c_file_name` for the emitted C++ source and `--output` for binary IL.

Unlike the header-free IL recipe, this keeps the predefined-macro file
enabled and runs the back end.  EDG itself preprocesses and parses the
installed headers; the input is not preprocessed by G++.

Each emitted source is compiled with `g++ -std=c++20 -O2 -pthread`.  The
runner executes both the emitted program and a directly compiled G++
reference, requiring exit status zero and the exact expected output:

```text
containers:      sorted=1,2,3,4 sum=10 edg=2 gcc=1
lifetimes:       edg: unwind live=0 answer=42 expired=1
ranges_threads:  squares=56 async=57 thread=7
```

These exercise containers, algorithms, strings and streams; virtual
dispatch, smart pointers and exception unwinding; and C++20 ranges,
atomics, threads and asynchronous futures.  All three passed with the
installed GCC 15 libstdc++ headers.  Their executables were also verified
to load `/usr/lib/aarch64-linux-gnu/libstdc++.so.6`.

Artifacts are left in `build/gcc-aarch64/libstdcxx/`:

* `<name>.edg.cpp`: EDG-emitted C++ source.
* `<name>.cil`: binary IL from the C++-generating configuration.
* `<name>`: executable compiled from EDG's emitted source.
* `<name>.gcc`: directly compiled reference executable.
* `base/lib/predefined_macros.txt`: the installed compiler's macro table.

These are end-to-end checks for the listed features, not a claim that every
libstdc++ header or language mode is supported.

### Compile EDG IL in memory through QBE

The optional `edg2qbe/qbe/` drivers compile the packaged examples using the same
standalone executable and checked-in [QBE 1.3](https://c9x.me/compile/releases.html)
sources. [QBE's IL reference](https://c9x.me/compile/doc/il.html) describes the
retained `.qbe` format.

The compiler pipeline is:

```text
original C/C++ source and supplied system headers
  -> one edg-qbe process:
       EDG parsing and lowering
       native QBE emission from resident IL
       embedded QBE arm64 optimizer/code generator
       native ABI and GNU exception assembly adaptation
  -> .S (optionally retained .qbe)
  -> host assembler/linker + target-program libraries
  -> executable
```

No LLVM, Clang, reconstructed C++, external QBE executable, or external
compiler-generated program bodies are used. GCC in C mode builds the seed.
C++ header paths and predefined macros are discovered only when compiling
C++ target programs; their libstdc++ linkage is separate from the compiler build.
G++ reference programs are compiled separately, solely to compare behavior.

`edg2qbe/qbe/build.py` is optional automation around direct GCC commands. It needs
Python 3 and GCC, but does not run CMake, Make, download QBE, or generate sources.
The C++ examples additionally need G++, libstdc++ development headers, and
libatomic. Verification used GCC/G++ 15.2.0.

```sh
env CXX=/does/not/exist python3 edg2qbe/qbe/build.py
python3 edg2qbe/qbe/run_examples.py
python3 edg2qbe/qbe/test_backend.py -v

python3 edg2qbe/qbe/compile.py edg2qbe/examples/libstdcxx/containers.cpp \
  -o edg2qbe/build/containers \
  --emit-qbe edg2qbe/build/containers.qbe
```

### Modern C++ accuracy checks

All examples live under `edg2qbe/examples/`, including the six previously
working IL/libstdc++ programs. Package-local `cases.json` manifests specify
the language version, exact output, source translation units, and semantic
coverage. The native runner compiles and executes a separate G++ reference
before compiling the same source through EDG/QBE; it never substitutes the
reference object for native code generation.

The suite contains 23 positive programs, eight invalid-program rejection
cases, and one explicitly unsupported coroutine probe:

* C++20 concepts/subsumption, requires expressions, folds, perfect forwarding,
  move-only captures, guaranteed elision, complete/member temporary lifetimes,
  designated aggregates, structured bindings, `[[no_unique_address]]`, and
  strong/partial/unordered three-way comparisons.
* C++23 `if consteval`, structural class template arguments, explicit-object
  member functions, multidimensional subscripting, and `std::expected`.
* Optional/variant, lazy ranges and projections, spans/string views, bit/byte
  operations, vector exception guarantees, shared/weak ownership, over-aligned
  objects, and deterministic thread synchronization.
* Three two-translation-unit ABI programs covering diamond RTTI, covariant
  returns, secondary-base destruction, typed/pointer exceptions and rethrow,
  nontrivial ownership transfers, member pointers, and shared inline state.
  Each runs as all-G++, all-EDG, and both mixed-object directions.
* Invalid constraints, runtime immediate-function calls, narrowing, deleted
  copies, ambiguous constrained overloads, access violations, invalid constant
  evaluation, and out-of-order designators. Both compilers must reject them;
  strict diagnostic modes are matched.

```sh
python3 edg2qbe/qbe/run_examples.py --list
python3 edg2qbe/qbe/run_examples.py --case modern/value_categories \
  --case interop/exceptions --output-dir edg2qbe/build/selected-examples
python3 edg2qbe/qbe/compile.py --std c++23 \
  edg2qbe/examples/library/expected_pipeline.cpp -o edg2qbe/build/expected
```

The runner retains native QBE/assembly/objects, reference objects, command and
execution logs, and `results.json`. It checks exact stdout, empty stderr, and
successful execution. Unsupported probes have a separate `UNSUPPORTED`
status, never `PASS`; their G++ behavior is still checked and native compilation
must explicitly reject them without emitting assembly.

**Limits:** these are behavioral checks, not a complete C++ conformance suite.
Native coroutines remain unsupported: the backend lacks suspension/frame
transformation, persistent live-value storage, suspended-state cleanup, and GNU
coroutine-handle builtins. The retained generator exercises real suspension,
ownership, RAII, and exception behavior with G++, while EDG reports
`native coroutine suspension lowering is not implemented`.

The hardened GCC C seed and a fresh package copied outside the repository
each passed 31 cases, reported one unsupported coroutine, and had zero failures.
Each run executed 53 programs including independent G++ references and all
mixed-ABI variants; all nine existing backend regressions also passed.
Compiler builds used `CXX=/does/not/exist`. The changes fix alternate lambda
entry lowering, modern member-subobject lifetime extension, lowered aggregate
initializer traversal over optimized-empty fields, and unwind continuations
that bypassed QBE register/phi transfers. See `edg2qbe/README.md` for the
package-local bootstrap report and retained capability limits.

To reassemble the containers QBE output:

```sh
edg2qbe/build/bin/edg-qbe --assemble edg2qbe/build/containers.qbe \
  -o edg2qbe/build/containers-from-qbe.S
g++ -no-pie -pthread edg2qbe/build/containers-from-qbe.S \
  edg2qbe/build/lib/runtime.o edg2qbe/build/lib/native.o \
  -latomic -lm -o edg2qbe/build/containers-from-qbe
edg2qbe/build/containers-from-qbe
```

`edg2qbe/build/bin/edg-qbe` contains the entire compiler pipeline. After parsing
and lowering, EDG invokes `back_end()` before releasing the file/function IL
regions. The native configuration sets `IL_SHOULD_BE_WRITTEN_TO_FILE=0`: no
`.cil` or IL reader process is involved. `--output`/`-o` selects the assembly
output; without it, EDG derives a `.S` basename. `--emit-qbe` instead selects
raw QBE output and a `.qbe` default basename.
Output paths that alias the source, including hardlinks, are rejected before
truncation.

The source driver discovers the installed GNU headers/version, generates
a per-compilation predefined-macro table, and requests used template
instantiations. Inline-only GNU header definitions are expanded directly
by the native backend.

The backend implements native type layout, data and relocations, expression
and statement lowering, GNU C++ object ABI, RTTI, and exception cleanup.
Its AAPCS64 classifier and assembly veneers handle binary16 scalars and
homogeneous floating aggregates, wide scalars, aligned aggregates, indirect
results, and variadic register/stack arguments. `edg2qbe/qbe/runtime.c` supplies primitive
bit operations and binary16/bfloat16 conversions with direct rounding; binary128
`long double` and 128-bit integer arithmetic use the installed libgcc primitives
where needed. The runtime itself is self-compilable. `edg2qbe/qbe/native.S` supplies
register-packet call/entry bridges and native atomic compare-exchange operations.

The assembly adapter is C (`edg2qbe/assemble.c`) and calls the resident QBE
engine, rather than a Python code-generation stage or a separate executable.

The QBE patch adds DWARF CFI, the AArch64 x8 indirect-result convention,
and initial-exec addressing for external TLS. The assembly adapter emits
GNU personality/LSDA metadata and exception-entry trampolines. A scratch
pair is reserved only in functions with landingpads. Pre-call exceptional
CFG edges prevent QBE from sinking handler inputs past throwing calls;
post-call exceptional edges preserve those inputs across call clobbers.

All six example executables passed. The three libstdc++ outputs match
the exact outputs above and directly compiled G++ references. The exception
regression checks multiple catch types, catch-all, mismatched-catch cleanup,
rethrow, loop-carried values across unwinding, and exception transport
through `std::async`/`future::get`. The ABI regression checks GCC calls in
both directions, binary128 precision/arithmetic, i128, homogeneous floating
and large aggregates, variadic register/stack arguments, and pointer catches:

```text
score=31 live=0 destroyed=9
native ABI: tf128 i128 HFA aggregates varargs pointer-catch OK
```

The direct compiler regressions also verify C scalar conditions across all
64 register-mask bits, floating and i128 conditions, independent C local statics,
explicit function-pointer dereference, and cross-translation-unit local-class
constructor/destructor and template-static linkage. Same-path and hardlink
output aliases fail without changing the source.
The C regression also checks C11 `_Generic`: the selector and unselected
expressions are not evaluated, and the selected lvalue, address, and function
designator retain their normal semantics. GNU C `nothrow` functions do not
introduce C++ exception-runtime dependencies. Scoped variable-length arrays
reclaim stack storage across loops and `continue`, preserve surrounding VLAs
and persistent `__builtin_alloca` storage, and evaluate bounds once. C++ VLA
cleanup also preserves normal, early-return, and exception unwinding behavior.

Executables and per-program `.qbe` and `.S` files are retained under
`edg2qbe/build/edg-qbe/examples/`. Single-source compilation defaults to
`<executable>.artifacts/`. The EDG IL stays in memory through QBE emission;
only generated QBE text and assembly are written as compiler artifacts.

The current executable target is non-PIE Linux AArch64 with the GNU C++
runtime. These checks establish the exercised language and ABI behavior,
not exhaustive C++ conformance. Unsupported GNU extensions, including
constrained inline assembly, asm-goto, and computed goto, are diagnosed
instead of delegated to another compiler.

### Precompiled headers after the C cutover

Regenerate PCH files with the C-built compiler. The cache signature includes
the C layout version, container callback encoding version, and SHA256 of the
executing compiler image. C++-implementation caches and caches from a different
compiler image are rejected before their saved state is loaded.

PCH state retains the frontend's existing fixed-virtual-address requirement.
Creation and consumption must use matching process-loading environments;
on Linux AArch64, run both with `setarch aarch64 -R` and identical frontend
options. Ordinary ASLR can produce a memory-usage conflict warning and cause
the source to be reparsed instead. This restriction also applies to the prior
C++ compiler; it is not introduced by the C port.

### Bootstrap to a byte-identical native fixed point

```sh
env CXX=/does/not/exist python3 edg2qbe/qbe/bootstrap.py --output-dir edg2qbe/build/bootstrap -j 4
```

The driver builds and snapshots the GCC C seed as `stage0`, then rebuilds
the complete toolchain three times. No C++ compiler or C++ runtime is used to
build the compiler:

```text
GCC C -> stage0 edg-qbe + utilities + target-program support objects
stage0 edg-qbe -> stage1
stage1 edg-qbe -> stage2
stage2 edg-qbe -> stage3
```

Each self-hosted stage compiles 123 C translation units: 75 frontend units,
eight `edg2qbe` backend/entry/adapter units, all 32 QBE units (including amd64,
arm64, and rv64), seven utility units, and `edg2qbe/qbe/runtime.c`. The compiler and
utilities use GNU C11; the primitive runtime uses C99. Native ABI support
assembly is assembled directly. GCC performs no C or C++ code generation for
stage1, stage2, or stage3: only the host assembler/linker and fixed system
libraries remain dependencies. Each compiler unit is lowered and assembled by one
resident `edg-qbe` process, with its exact QBE intermediate retained.
Checked-in configuration/error headers and installed GNU C headers/macros
are shared inputs, not regenerated by the self-hosted stages.

The driver retains each stage's `.qbe`, `.S`, `.o`, command logs, and complete
toolchain under `edg2qbe/build/bootstrap/stageN/`. All three self-hosted builds use
the same working paths and link order. `SOURCE_DATE_EPOCH` defaults to `946684800`
(2000-01-01 UTC); predefined date/time macros and macro-table metadata are
fixed to that epoch. Source/header, producer, dependency, configuration, and
output hashes validate cached units.

The final comparison reads the complete unmodified ELF files. It does not
strip debug information, remove build IDs, normalize bytes, or patch outputs.
The hardened Linux AArch64 fixed point with GCC C 15.2.0 was verified under
`edg2qbe/build/hardening/bootstrap/` and produced these shared stage2/stage3
SHA256 values:

| Output | Stage2 = stage3 SHA256 |
| --- | --- |
| `bin/edg-qbe` | `44cbb68581b74fb15a5a5e4a7fc949bb2cd4a6cc0443e1b62731dae70e56b680` |
| `lib/runtime.o` | `f58a6d5a186e73b88faaffdb74f33379e1a9176682ad7442c155c3826470cd99` |
| `lib/native.o` | `724f17b4ed8e6b6a8ea57f0a7d5f6b0259740f835debad92714b1a2135c2bb58` |
| `bin/edg_munch` | `b619cba14a9ed65df485195379ffd960a9e70b8040f751cdfad25b66b653ad47` |
| `bin/edg_prelink` | `60ed1b9dd33218dec8c4fa94cb8ec325a7611787528fb17cb03036f7a14d14e8` |
| `bin/edg_decode` | `00c9d995f177a12ecd5cd76b2bd54bbfe64094455f3b4cd467931237b91d740c` |
| `bin/mk_errinfo` | `e62a19bca1c470edb5b46ba4891750ab89737e0196b98a6cb51a25b60778191f` |
| `bin/dettarg` | `9e2c8dc23fcf072b27732b85c0122dcb9594b14da4db851a89938fb0c6ef48cf` |

All eight complete outputs matched. The retained 123 `.qbe` files, 123 assembly
files, and 123 native objects also matched byte-for-byte between stage2 and stage3.

The hardened stage3 passed all 31 supported/rejection cases and all nine
backend regressions, with one separately reported unsupported coroutine and
zero failures. Its example run executed 53 programs, including reference and
mixed-ABI variants. The single-source driver also compiled and ran the C++23
expected pipeline through stage3. `readelf -d` lists only libc and libm as the
compiler's shared-library dependencies.

The new failing-before and passing-after artifacts are under
`edg2qbe/build/hardening/`; the old `edg2qbe/build/bootstrap/` remains the
pre-hardening baseline. Select the verified hardened compiler with
`--toolchain edg2qbe/build/hardening/bootstrap/stage3`, or rerun the bootstrap
command above to refresh the default output location.


The following additional checks preceded the modern-C++ hardening run:
The package was also copied outside the repository, then built with the
static wildcard GCC command and its Python build driver. All nine packaged
regressions passed there. Its static compiler generated runnable C and C++
programs with `PATH=/nonexistent` and no surrounding EDG checkout.
Earlier checks of the seed, stage1, and stage2 passed all six examples and all
nine backend regression tests. The stage2 single-source driver compiled and ran
the containers example successfully. The seed and stage2 both genuinely
reused a PCH containing concepts, templates, and a constrained generic lambda,
then compiled and ran the resulting program. The wildcard-built compiler and
stage2 also compiled and ran header-free programs with `PATH=/nonexistent`,
without a companion QBE executable or build tools.
Stage2 also passed a C++ VLA smoke checking repeated stack restoration and
RAII destruction on both return and exception paths.
`readelf -d` reported only libc and libm dependencies for all six stage2
executables, with no C++ runtime.

Stage3 passed all six examples and all nine backend regressions, including
native exceptions, cleanup/rethrow, and GCC ABI interoperability. Its compiler
depends only on libc and libm, not a C++ runtime. A fresh package-only GCC C
build with `CXX=/does/not/exist` passed the nine packaged regressions and
generated a runnable concepts/generic-lambda program with `PATH=/nonexistent`.
Cached `--start-stage 3 --skip-stage0-build` resume also completed and repeated
the full stage2/stage3 byte comparison successfully.

Run programs with a staged standalone compiler and its target-program support objects:

```sh
python3 edg2qbe/qbe/run_examples.py --toolchain edg2qbe/build/bootstrap/stage1 \
  --output-dir edg2qbe/build/bootstrap/stage1/smoke/examples
python3 edg2qbe/qbe/test_backend.py --toolchain edg2qbe/build/bootstrap/stage1 -v
python3 edg2qbe/qbe/run_examples.py --toolchain edg2qbe/build/bootstrap/stage2 \
  --output-dir edg2qbe/build/bootstrap/stage2/smoke/examples
python3 edg2qbe/qbe/test_backend.py --toolchain edg2qbe/build/bootstrap/stage2 -v
python3 edg2qbe/qbe/run_examples.py --toolchain edg2qbe/build/bootstrap/stage3 \
  --output-dir edg2qbe/build/bootstrap/stage3/smoke/examples
python3 edg2qbe/qbe/test_backend.py --toolchain edg2qbe/build/bootstrap/stage3 -v

python3 edg2qbe/qbe/compile.py --toolchain edg2qbe/build/bootstrap/stage2 \
  edg2qbe/examples/libstdcxx/containers.cpp \
  -o edg2qbe/build/bootstrap/stage2/smoke/containers
edg2qbe/build/bootstrap/stage2/smoke/containers
```

For iteration, `--skip-stage0-build` uses an already refreshed seed.
`--start-stage 2` validates the complete stage1 producer; `--start-stage 3`
validates stage2. The recorded configuration, output and translation-unit
hashes, and preceding producer outputs must match before resuming.
The default endpoint is stage3, followed by a complete raw stage2/stage3
comparison. Build/bootstrap outputs live inside `edg2qbe/build/` and are
excluded from compiler input snapshots.
`--list-units`, `--unit SOURCE_SUFFIX`, and `--stop-after-stage 0`, `1`, or `2`
are diagnostic commands: partial builds and earlier stops never report the
required stage3 fixed point.
