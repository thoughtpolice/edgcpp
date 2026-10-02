# edg2qbe: native compiler and bootstrap

## Host requirements

The maintained native configuration requires **Linux AArch64**. Build the
compiler with GCC supporting GNU C11, system C headers, libc/libm, and the host
assembler/linker. Verification uses GCC 15.2.0. Python 3 runs the optional build,
bootstrap, and example drivers; CMake/Ninja are not required.

G++ and matching libstdc++ development headers plus libatomic are needed for the
C++ examples and independent reference comparisons, **not** for building or
self-hosting the compiler. The compiler contains its EDG frontend, native
backend, embedded QBE, and assembly adapter; there is no companion QBE process.

Everything below runs from this `edg2qbe/` directory. This directory can be
copied outside the repository; ordinary builds need no original EDG trees,
downloads, or source generation.

## Build and exercise the seed

```sh
env CXX=/does/not/exist python3 qbe/build.py -j4
python3 qbe/run_examples.py --output-dir build/examples
python3 qbe/test_backend.py -v
```

The seed is `build/bin/edg-qbe`; target-program support is in `build/lib/`.
A direct build without Python is also available:

```sh
gcc -std=gnu11 -D_GNU_SOURCE -Iconfig -Iedg -I. \
  -include config/native.h edg/*.c *.c \
  qbe/vendor/*.c qbe/vendor/amd64/*.c \
  qbe/vendor/arm64/*.c qbe/vendor/rv64/*.c -lm -o edg-qbe
```

## Bootstrap through stage3

```sh
env CXX=/does/not/exist python3 qbe/bootstrap.py \
  --output-dir build/bootstrap -j4
```

The driver builds the GCC C seed (stage0), then stage0 builds stage1, stage1
builds stage2, and stage2 builds stage3. Each self-hosted stage compiles all 123 C
translation units, including the frontend, backend, every embedded QBE target,
utilities, and primitive runtime. Host GCC supplies assembler/linker invocation,
not C/C++ code generation, for stages1–3. Native support assembly is assembled
directly.

The final comparison requires raw byte equality between stage2 and stage3 for
all eight toolchain outputs and every retained `.qbe`, `.S`, and `.o`. Nothing is
stripped or normalized. The driver records source/header/dependency/producer
hashes, deterministic date macros, command logs, and stage completion manifests.
A self-hosted compiler build alone is not a fixed-point proof.

Exercise the final compiler only after bootstrap finishes:

```sh
python3 qbe/run_examples.py --toolchain build/bootstrap/stage3 \
  --output-dir build/bootstrap/stage3/smoke/examples
python3 qbe/test_backend.py --toolchain build/bootstrap/stage3 -v
python3 qbe/compile.py --toolchain build/bootstrap/stage3 --std c++23 \
  examples/library/expected_pipeline.cpp -o build/expected
./build/expected
```

For iteration, refresh the seed before `--skip-stage0-build`. Resume with
`--start-stage 2` or `--start-stage 3` only when the validated preceding stage
matches all current inputs. Do not run staged consumers while bootstrap rewrites
the same binaries. `--unit`, `--list-units`, and early `--stop-after-stage` are
diagnostic modes and do not prove the complete fixed point.

## Accuracy checks and limits

`examples/` contains 23 positive semantic programs, eight invalid-program
rejection cases, and one unsupported coroutine probe. Run
`python3 qbe/run_examples.py --list` for each case's coverage; repeat `--case NAME`
to select cases. Per-case
manifests declare C++20/C++23 mode and exact output. The driver executes a
separate G++ reference, all-native programs, and both mixed-GNU-object directions
for three two-translation-unit ABI cases. It retains command/execution logs,
QBE/assembly/objects, and `results.json`.

Coverage includes concepts and subsumption, constexpr/consteval, explicit-object
members, multidimensional subscripting, forwarding and elision, subobject
reference lifetimes, aggregates and structured bindings, no-unique-address
subobjects, three-way comparisons, standard-library ownership/ranges/expected,
exception guarantees, deterministic concurrency, RTTI and native GNU ABI/EH.
Rejection checks use strict diagnostic modes on both compilers.

This is behavioral coverage, **not full C++ conformance certification**.
Native coroutine suspension is not implemented: the generator runs with G++,
while native compilation must explicitly reject it. Its result is
`UNSUPPORTED`, never `PASS`. Native module support and unexercised features are
not established by these examples. The optional repository-level
`examples/libstdcxx/run.py` uses the original C++-emission toolchain; it is not
the package-local native runner.

## Hardening results

The expanded baseline exposed four implementation defects, now fixed:

* `edg/lower_il.c` checks each alternate lambda entry's own backend eligibility
  before lowering it, rather than incorrectly retesting the enclosing routine.
* `edg/cmd_line.c` enables modern member-selection xvalue semantics in GNU/Clang
  modes, allowing member references to extend their containing temporary's life.
* `qbe_expr.c` pairs lowered aggregate constants with nonempty initializable
  fields, matching EDG's removal of optimized-empty initializers. The strict
  aggregate storage-size cast invariant remains intact.
* `assemble.c` and embedded QBE preserve the allocated exceptional call path:
  callsite-specific unwind continuations execute post-call register restoration
  and phi transfers, with exception payload and edge status in separate frame
  scratch slots. Landing directly in the catch block had passed an alias value
  of 42 as a destructor's `this` pointer.

The GCC C seed and a fresh copied package with no original upstream source trees
each passed 31 cases with one reported unsupported coroutine and zero failures,
covering 53 program executions including G++ references. All nine existing
backend regressions also passed. Both compiler builds used
`CXX=/does/not/exist`. Build warnings from the maintained port remain; these are
not warning-free-build claims.

The complete hardened bootstrap was verified at `build/hardening/bootstrap/`.
Stage2 and stage3 matched all eight outputs and each stage's 123 QBE files,
123 assembly files, and 123 objects byte-for-byte. Their compiler SHA256 is:

```text
44cbb68581b74fb15a5a5e4a7fc949bb2cd4a6cc0443e1b62731dae70e56b680
```

Stage3 independently repeated the 31 passing cases, one unsupported probe, all
53 program executions, and nine backend regressions. The single-source driver
also compiled and ran the C++23 expected pipeline through stage3. Its compiler
ELF needs only libc and libm, not libstdc++.

Retained evidence is under `build/hardening/`: failing-before `baseline/`,
passing `seed-final/results.json` and `stage3-examples/results.json`, copied
distribution `package-only-results.json`, and the complete `bootstrap/` stages.
The original pre-hardening bootstrap is retained separately at `build/bootstrap/`;
use `build/hardening/bootstrap/stage3` to run this verified hardened compiler,
or rerun the documented bootstrap command to refresh the default location.


## Source provenance

`edg/` and `util/` are the checked-in maintained C port. `port/c-port.patch` and
`port/manifest.json` record its pristine baseline and derived hashes. Regeneration
requires the surrounding repository's original `src/` and `util/`; ordinary
builds do not. With those pristine sources present, run
`python3 port/regenerate.py --check` to verify provenance. Embedded QBE changes
are an ordered, independently applicable series listed in `qbe/patches/series`.
