"""Shared paths and the direct in-memory EDG IL to QBE compilation route."""
from pathlib import Path
import os
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build"
FRONTEND = BUILD
TOOLS = FRONTEND / "lib"

def environment():
    """Use only the ordinary host toolchain and runtime search environment."""
    return os.environ.copy()



def run(args, **kwargs):
    return subprocess.run([str(arg) for arg in args], check=True, **kwargs)


def capture(args):
    return run(args, stdout=subprocess.PIPE, text=True).stdout.strip()


def required(path):
    path = Path(path)
    if not path.is_file():
        raise RuntimeError(f"Missing prerequisite {path}; run python3 edg2qbe/qbe/build.py first")
    return path


def command(name):
    result = shutil.which(name)
    if not result:
        raise RuntimeError(f"Missing prerequisite command: {name}")
    return result


def reject_overwrite(source, outputs):
    source = Path(source).resolve()
    paths = [Path(output).resolve() for output in outputs]
    if source in paths or len(set(paths)) != len(paths):
        raise RuntimeError("Input and output artifact paths must all be distinct")


def frontend_options(base=None, language="c++", standard=None):
    """Discover target headers/macros; compiler bootstrap requests C only."""
    if language not in ("c", "c++"):
        raise RuntimeError(f"Unsupported source language: {language}")
    standard = standard or ("c11" if language == "c" else "c++20")
    supported = ("c99", "c11", "c17") if language == "c" else (
        "c++11", "c++14", "c++17", "c++20", "c++23")
    if standard not in supported:
        raise RuntimeError(f"Unsupported {language} standard: {standard}")
    gcc = command("gcc")
    compiler = gcc if language == "c" else command("g++")
    version = capture([compiler, "-dumpfullversion"])
    if not re.fullmatch(r"\d+(?:\.\d+){0,2}", version):
        raise RuntimeError(f"Cannot discover GCC version: {version}")
    parts = [int(part) for part in version.split(".")]
    major, minor, patch = (parts + [0, 0])[:3]
    edg_version = str(major * 10000 + minor * 100 + patch)
    probe = run([compiler, "-std=" + standard, "-E", "-x", language, "-v", "/dev/null"],
                capture_output=True, text=True)
    try:
        search = probe.stderr.split("#include <...> search starts here:\n", 1)[1].split("End of search list.", 1)[0]
    except IndexError as error:
        raise RuntimeError(f"Cannot discover GCC {language} system include search paths") from error
    includes = [line.strip() for line in search.splitlines() if line.strip()]
    base = Path(base) if base is not None else TOOLS / "base-gcc"
    (base / "lib").mkdir(parents=True, exist_ok=True)
    # Probe every invocation: installed headers/macros may change without a
    # compiler version change (e.g. distro package updates).
    macro_args = [ROOT / "util/make_predef_macro_table", "--gcc", gcc]
    macro_args += ["--c-only"] if language == "c" else ["--g++", compiler]
    run(macro_args, cwd=base / "lib")
    options = ["--gcc" if language == "c" else "--g++",
               "--gnu_version", edg_version, "--" + standard,
               "--edg_base_dir", str(base)]
    if language == "c++":
        # Instantiate consumer templates without a host code-generation step.
        options += ["--strict_gnu", "--instantiate", "used"]
    for path in includes:
        options += ["--sys_include", path]
    return options


def selected_toolchain(root=None):
    """Return the one compiler executable and target-program support objects."""
    root = Path(root).resolve() if root is not None else FRONTEND
    return root / "bin/edg-qbe", root / "lib/runtime.o", root / "lib/native.o"


def compile_source(source, output, artifacts=None, emit_qbe=None, toolchain=None,
                   standard=None):
    source, output = Path(source).resolve(), Path(output).resolve()
    required(source)
    artifacts = Path(artifacts).resolve() if artifacts else output.parent / (output.name + ".artifacts")
    qbe = Path(emit_qbe).resolve() if emit_qbe else artifacts / (source.stem + ".qbe")
    assembly = artifacts / (source.stem + ".S")
    reject_overwrite(source, [output, qbe, assembly])
    artifacts.mkdir(parents=True, exist_ok=True)
    output.parent.mkdir(parents=True, exist_ok=True)
    qbe.parent.mkdir(parents=True, exist_ok=True)
    frontend, runtime, native = selected_toolchain(toolchain)
    language = "c" if source.suffix == ".c" else "c++"
    run([required(frontend), "--save-qbe", qbe,
         *frontend_options(artifacts / (source.stem + ".base-gcc"), language, standard),
         "--output", assembly, source])
    # C++ libraries are target-program support, never compiler dependencies.
    linker = command("gcc" if language == "c" else "g++")
    run([linker, "-no-pie", "-pthread", assembly,
         required(runtime), required(native),
         "-latomic", "-lm", "-o", output])
    return output


def cli(main):
    try:
        main()
    except (RuntimeError, OSError, subprocess.SubprocessError) as error:
        raise SystemExit(f"error: {error}")
