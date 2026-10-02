#!/usr/bin/env python3
"""Build the checked-in standalone compiler with ordinary gcc commands."""
import argparse
from concurrent.futures import ThreadPoolExecutor
import hashlib
import json
import platform
from pathlib import Path
from toolchain import BUILD, FRONTEND, ROOT, TOOLS, cli, command, run

SEED_TARGETS = ("edg-qbe", "edg_munch", "edg_prelink", "edg_decode", "mk_errinfo", "dettarg")
UTILITY_SOURCES = {"edg_munch": ("edg_munch.c",),
                   "edg_prelink": ("edg_prelink.c", "decode.c"),
                   "edg_decode": ("edg_decode.c", "decode.c"),
                   "mk_errinfo": ("mk_errinfo.c",), "dettarg": ("dettarg.c",)}


def compiler_sources():
    """The same source groups as the documented shell-wildcard command."""
    patterns = ("edg/*.c", "*.c", "qbe/vendor/*.c",
                "qbe/vendor/amd64/*.c", "qbe/vendor/arm64/*.c", "qbe/vendor/rv64/*.c")
    sources = []
    for pattern in patterns:
        group = sorted(ROOT.glob(pattern))
        if not group:
            raise RuntimeError(f"Missing checked-in compiler source group: {pattern}")
        sources.extend(group)
    return sources


def source_inputs():
    """Checked-in compiler inputs, without generated build/bootstrap outputs."""
    files = {path for pattern in ("*.c", "*.h") for path in ROOT.glob(pattern)}
    for directory in ("edg", "util", "config", "qbe/vendor"):
        files.update(path for path in (ROOT / directory).rglob("*") if path.is_file())
    return sorted(files)


def seed_commands():
    """Expose the exact direct seed command and ordered source inputs."""
    flags = [command("gcc"), "-std=gnu11", "-g", "-D_GNU_SOURCE=1",
             "-I" + str(ROOT / "config"), "-I" + str(ROOT / "edg"),
             "-I" + str(ROOT)]
    commands = {}
    for target in SEED_TARGETS:
        output = FRONTEND / "bin" / target
        if target == "edg-qbe":
            sources = compiler_sources()
            options = ["-include", str(ROOT / "config/native.h")]
        else:
            sources = [ROOT / "util" / name for name in UTILITY_SOURCES[target]]
            options = []
        commands[output] = [*flags, *options, *(str(path) for path in sources),
                            "-lm", "-o", str(output)]
    return commands


def support_commands():
    gcc = command("gcc")
    return {TOOLS / "runtime.o": [gcc, "-std=c99", "-O2", "-fPIC", "-c",
                                  str(ROOT / "qbe/runtime.c"), "-o", str(TOOLS / "runtime.o")],
            TOOLS / "native.o": [gcc, "-c", str(ROOT / "qbe/native.S"),
                                 "-o", str(TOOLS / "native.o")]}


def digest(path):
    value = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def compile_incrementally(output, dependencies, args):
    output.parent.mkdir(parents=True, exist_ok=True)
    stamp = output.with_suffix(output.suffix + ".build-key")
    key = hashlib.sha256((repr(args) + dependencies).encode()).hexdigest()
    expected = [key, digest(output)] if output.is_file() else None
    if expected is None or not stamp.is_file() or stamp.read_text().splitlines() != expected:
        stamp.unlink(missing_ok=True)
        run(args)
        temporary = stamp.with_name(stamp.name + ".tmp")
        temporary.write_text(key + "\n" + digest(output) + "\n")
        temporary.replace(stamp)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("-j", "--jobs", type=int, default=4,
                        help="independent compiler/utility builds in parallel (default: 4)")
    args = parser.parse_args()
    if platform.system() != "Linux" or platform.machine() not in ("aarch64", "arm64"):
        raise RuntimeError("The native compiler configuration requires Linux AArch64")
    if args.jobs < 1:
        raise RuntimeError("--jobs must be positive")
    commands = seed_commands()
    # The one-command compiler build consumes checked-in sources and headers,
    # never CMake, Make, generated files, downloaded archives or external QBE.
    dependencies = "".join(str(path) + digest(path) for path in source_inputs())
    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(compile_incrementally, output, dependencies, arguments)
                   for output, arguments in commands.items()]
        for future in futures:
            future.result()
    FRONTEND.mkdir(parents=True, exist_ok=True)
    (FRONTEND / "seed-commands.json").write_text(json.dumps(
        {str(output): args for output, args in commands.items()}, indent=2) + "\n")
    for output, arguments in support_commands().items():
        source = Path(arguments[arguments.index("-c") + 1])
        compile_incrementally(output, digest(source), arguments)
    print(f"Standalone toolchain ready: {FRONTEND}")


if __name__ == "__main__":
    cli(main)
