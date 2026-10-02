#!/usr/bin/env python3
"""Build three complete native self stages and compare stage2/stage3 bytes.

Only stage0 uses host C code generation. Self stages share one work directory
so source, include, output and linker paths are identical. Each stage retains its
own .qbe/.S/.o/log snapshots. Diagnostic stops never report a fixed point.
"""
import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path
import platform
import shlex
import shutil
import subprocess
import sys

from build import SEED_TARGETS, seed_commands, source_inputs, support_commands
from toolchain import (BUILD, FRONTEND, ROOT, capture, cli, command,
                       frontend_options, required, selected_toolchain)

OUTPUTS = ("bin/edg-qbe", "lib/runtime.o", "lib/native.o",
           *("bin/" + name for name in SEED_TARGETS if name != "edg-qbe"))


def digest(path):
    value = hashlib.sha256()
    with Path(path).open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            value.update(block)
    return value.hexdigest()


def key(value):
    return hashlib.sha256(json.dumps(value, sort_keys=True).encode()).hexdigest()


def snapshot(paths):
    """Hash contents, not mtimes; include headers added since the previous run."""
    files = set()
    for path in paths:
        path = Path(path)
        if path.is_dir():
            files.update(p for p in path.rglob("*") if p.is_file())
        else:
            files.add(required(path))
    return {str(p): digest(p) for p in sorted(files)}


def save_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_name(path.name + ".tmp")
    temporary.write_text(json.dumps(value, sort_keys=True, indent=2) + "\n")
    temporary.replace(path)


def logged(args, log, env, cwd=ROOT):
    args = [str(arg) for arg in args]
    log.parent.mkdir(parents=True, exist_ok=True)
    with log.open("w") as stream:
        stream.write("cwd: " + str(cwd) + "\n$ " + shlex.join(args) + "\n")
        stream.flush()
        result = subprocess.run(args, cwd=cwd, env=env, stdout=stream,
                                stderr=subprocess.STDOUT)
    if result.returncode:
        lines = log.read_text(errors="replace").splitlines()
        raise RuntimeError(f"Command failed ({result.returncode}); log: {log}\n"
                           + "\n".join(lines[-35:]))


def copy(source, destination):
    destination.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, destination)


def translate(tokens, includes, base_options, cwd=FRONTEND):
    """Translate semantic GCC flags to options registered in src/cmd_line.c.

    Debug/optimization, dependency emission and host warning policy have no
    frontend equivalent. Unknown semantic flags fail rather than being ignored.
    Compiler implementation units must select an explicit C dialect.
    """
    language, standard = None, None
    options = []
    source = output = None
    i = 1
    while i < len(tokens):
        flag = tokens[i]
        if flag in ("-D", "-U", "-I", "-isystem", "-include"):
            i += 1
            arg = tokens[i]
            option = {"-D": "--define_macro", "-U": "--undefine_macro",
                      "-I": "--include_directory", "-isystem": "--sys_include",
                      "-include": "--preinclude"}[flag]
            if flag in ("-I", "-isystem", "-include"):
                arg = str((cwd / arg).resolve()) if not Path(arg).is_absolute() else arg
                arg = includes.get(arg, arg)
            options += [option, arg]
        elif flag[:2] in ("-D", "-U", "-I"):
            prefix, arg = flag[:2], flag[2:]
            if prefix == "-I":
                arg = str((cwd / arg).resolve()) if not Path(arg).is_absolute() else arg
                arg = includes.get(arg, arg)
            options += [{"-D": "--define_macro", "-U": "--undefine_macro",
                         "-I": "--include_directory"}[prefix], arg]
        elif flag in ("-x", "-o", "-c", "-MF", "-MT", "-MQ"):
            i += 1
            arg = tokens[i]
            if flag == "-x":
                language = arg
            elif flag == "-o":
                output = arg
            elif flag == "-c":
                source = str((cwd / arg).resolve())
        elif flag.startswith("-std="):
            standard = flag.split("=", 1)[1]
            standard = {"gnu99": "c99", "gnu11": "c11", "gnu17": "c17"}.get(standard, standard)
        elif (flag.startswith(("-W", "-g", "-O")) or flag in
              ("-pedantic", "-w", "-MD", "-MMD", "-MP", "-funwind-tables", "-fno-unwind-tables")):
            pass
        else:
            raise RuntimeError(f"Untranslated compiler flag {flag!r} in {shlex.join(tokens)}")
        i += 1
    language = language or ("c" if source and Path(source).suffix == ".c" else None)
    if not source or not output or language != "c":
        raise RuntimeError(f"Unexpected C compile command: {shlex.join(tokens)}")
    if standard not in ("c99", "c11", "c17"):
        raise RuntimeError(f"Compiler unit must specify a supported C standard: {standard}")
    common = [option for option in base_options if option not in ("--c99", "--c11", "--c17")]
    # EDG searches these directories in registration order; GCC -I directories
    # precede system directories even when they occur later on its command line.
    system_start = common.index("--sys_include") if "--sys_include" in common else len(common)
    return {"source": source, "host_object": output, "host_command": tokens,
            "options": [*common[:system_start], "--" + standard, *options, *common[system_start:]]}


def units_from_build(inputs, base_options):
    """Expand the exact one-command seed source graph into self-built units."""
    commands = seed_commands()
    recorded = json.loads(required(FRONTEND / "seed-commands.json").read_text())
    current = {str(output): arguments for output, arguments in commands.items()}
    if recorded != current:
        raise RuntimeError("Seed commands differ from current inputs; refresh edg2qbe/qbe/build.py")
    save_json(inputs / "seed-commands.json", current)
    generated = ROOT / "config"
    retained = inputs / "includes"
    if retained.exists():
        shutil.rmtree(retained)
    shutil.copytree(generated, retained)
    include_map = {str(generated): str(retained)}
    include_map.update({str(path): str(retained / path.name)
                        for path in generated.iterdir() if path.is_file()})
    units, links = [], {}
    for output, arguments in commands.items():
        target = output.name
        name = "bin/" + target
        flags, link_arguments = [arguments[0]], []
        sources = []
        i = 1
        while i < len(arguments):
            arg = arguments[i]
            if arg == "-o":
                i += 1
                if arguments[i] != str(output):
                    raise RuntimeError(f"Unexpected seed output: {arguments[i]}")
                link_arguments += ["-o", name]
            elif arg.startswith("-l"):
                link_arguments.append(arg)
            elif arg.endswith(".c") and Path(arg).is_file():
                sources.append(arg)
                unit_id = target + "/" + str(Path(arg).relative_to(ROOT))
                link_arguments.append({"unit": unit_id})
            else:
                flags.append(arg)
            i += 1
        if not sources:
            raise RuntimeError(f"No C sources in seed command for {target}")
        for source in sources:
            unit_id = target + "/" + str(Path(source).relative_to(ROOT))
            object_path = FRONTEND / "objects" / (unit_id + ".o")
            unit = translate([*flags, "-c", source, "-o", str(object_path)],
                             include_map, base_options, cwd=ROOT)
            unit["id"] = unit_id
            units.append(unit)
        links[name] = {"driver": arguments[0], "arguments": link_arguments}
    if len({unit["id"] for unit in units}) != len(units):
        raise RuntimeError("Duplicate source identities in the seed build")
    save_json(inputs / "links.json", links)
    return units, links




def artifact_hashes(stage, units):
    return {f"units/{unit['id']}{suffix}": digest(required(stage / f"units/{unit['id']}{suffix}"))
            for unit in units for suffix in (".qbe", ".S", ".o")}


def c_options(base_options, standard="c99"):
    """Select a C dialect without discovering any C++ compiler or headers."""
    if "--gcc" not in base_options:
        raise RuntimeError("Bootstrap options must come from C-only GCC discovery")
    return [option for option in base_options if option not in ("--c99", "--c11", "--c17")] + ["--" + standard]


def cache_hit(stamp, cache_key, outputs):
    if not stamp.is_file() or not all(path.is_file() for path in outputs):
        return False
    try:
        old = json.loads(stamp.read_text())
        return old["key"] == cache_key and old["outputs"] == [digest(path) for path in outputs]
    except (ValueError, KeyError):
        return False


def build_unit(unit, stage, work, producer, inputs_key, env, assembler):
    stem = work / "units" / unit["id"]
    stem.parent.mkdir(parents=True, exist_ok=True)
    retained = stage / "units" / unit["id"]
    suffixes = (".qbe", ".S", ".o")
    outputs = [Path(str(stem) + suffix) for suffix in suffixes]
    saved = [Path(str(retained) + suffix) for suffix in suffixes]
    stamp = Path(str(retained) + ".json")
    cache_key = key([inputs_key, unit])
    if cache_hit(stamp, cache_key, saved):
        for source, target in zip(saved, outputs):
            copy(source, target)
        print(f"{stage.name} {unit['id']}: cached", flush=True)
        return outputs[-1]
    stamp.unlink(missing_ok=True)
    for path in (*outputs, *saved):
        path.unlink(missing_ok=True)
    logs = [Path(str(retained) + "." + phase + ".log") for phase in ("frontend", "assembler")]
    frontend, _, _ = selected_toolchain(producer)
    commands = ([frontend, "--save-qbe", outputs[0], *unit["options"],
                 "--output", outputs[1], unit["source"]],
                [assembler, "-o", outputs[2], outputs[1]])
    try:
        for args, log in zip(commands, logs):
            logged(args, log, env)
    finally:
        for source, target in zip(outputs, saved):
            if source.is_file():
                copy(source, target)
    save_json(stamp, {"key": cache_key, "outputs": [digest(path) for path in saved],
                      "commands": [[str(arg) for arg in args] for args in commands]})
    print(f"{stage.name} {unit['id']}: built", flush=True)
    return outputs[-1]


def build_stage(number, root, units, configuration, env, jobs, selected=None):
    stage, producer, work = root / f"stage{number}", root / f"stage{number - 1}", root / "work"
    for name in OUTPUTS:
        required(producer / name)
    completion = stage / "complete.json"
    completion.unlink(missing_ok=True)
    producer_hashes = {name: digest(producer / name) for name in OUTPUTS}
    inputs_key = key([configuration, producer_hashes])
    active = [unit for unit in units if selected is None or unit["id"] == selected]
    assembler = command("as")
    with ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = [pool.submit(build_unit, unit, stage, work, producer,
                               inputs_key, env, assembler) for unit in active]
        try:
            for future in as_completed(futures):
                future.result()
        except BaseException:
            for future in futures:
                future.cancel()
            raise
        # Link in captured manifest order, never in worker completion order.
        objects = [future.result() for future in futures]
    if selected is not None:
        print(f"Diagnostic stop after {stage.name} unit {selected}; fixed point NOT checked")
        return
    native = work / "lib/native.o"
    native.parent.mkdir(parents=True, exist_ok=True)
    logged([assembler, "-o", native, ROOT / "qbe/native.S"], stage / "native.log", env)
    object_map = {unit["id"]: obj for unit, obj in zip(units, objects)}
    runtime = object_map["support/runtime.c"]
    copy(runtime, work / "lib/runtime.o")
    copy(runtime, stage / "lib/runtime.o")
    copy(native, stage / "lib/native.o")
    for name, link in configuration["links"].items():
        output = work / name
        output.parent.mkdir(parents=True, exist_ok=True)
        arguments = [object_map[arg["unit"]] if isinstance(arg, dict) else
                     output if arg == name else arg for arg in link["arguments"]]
        # Self-generated C objects use the primitive runtime, native ABI support
        # and libatomic. The captured target's ordered link options stay intact.
        args = [command("gcc"), "-no-pie", "-pthread", "-Wl,--build-id=sha1",
                *arguments, work / "lib/runtime.o", native, "-latomic", "-lm"]
        logged(args, stage / (output.name + ".link.log"), env, cwd=work)
        copy(output, stage / name)
    save_json(completion, {"configuration": key(configuration), "producer": producer_hashes,
                          "outputs": {name: digest(stage / name) for name in OUTPUTS},
                          "units": artifact_hashes(stage, units)})
    print(f"{stage.name}: complete toolchain built", flush=True)


def same_bytes(first, second):
    if first.stat().st_size != second.stat().st_size:
        return False
    with first.open("rb") as left, second.open("rb") as right:
        while True:
            block = left.read(1024 * 1024)
            if block != right.read(1024 * 1024):
                return False
            if not block:
                return True


def compare(root, units):
    mismatches = []
    names = [*OUTPUTS, *(f"units/{unit['id']}{suffix}" for unit in units
                        for suffix in (".qbe", ".S", ".o"))]
    for name in names:
        first, second = root / "stage2" / name, root / "stage3" / name
        hashes = digest(required(first)), digest(required(second))
        # Compare whole files directly; no stripping, normalization or rewriting.
        equal = same_bytes(first, second)
        print(f"{name}: stage2 {hashes[0]} stage3 {hashes[1]} {'IDENTICAL' if equal else 'MISMATCH'}")
        if not equal:
            mismatches.append(name)
    if mismatches:
        raise RuntimeError("Bootstrap fixed point failed: " + ", ".join(mismatches))
    print("Bootstrap fixed point: every toolchain output and translation-unit artifact is byte-for-byte identical")


def check_completed_stage(number, root, units, configuration):
    """Resume only from a complete stage built from current inputs/artifacts."""
    stage = root / f"stage{number}"
    previous = json.loads(required(stage / "complete.json").read_text())
    if previous["configuration"] != key(configuration):
        raise RuntimeError(f"{stage.name} does not match current sources/headers/options; "
                           "rebuild from --start-stage 1")
    if previous["outputs"] != {name: digest(required(stage / name)) for name in OUTPUTS}:
        raise RuntimeError(f"{stage.name} contents differ from its completion manifest")
    if previous.get("units") != artifact_hashes(stage, units):
        raise RuntimeError(f"{stage.name} translation-unit artifacts differ from its completion manifest")
    producer = root / f"stage{number - 1}"
    if previous["producer"] != {name: digest(required(producer / name)) for name in OUTPUTS}:
        raise RuntimeError(f"{stage.name} producer contents changed; rebuild from --start-stage 1")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=BUILD / "bootstrap")
    parser.add_argument("-j", "--jobs", type=int, default=4)
    parser.add_argument("--source-date-epoch", type=int, default=946684800)
    parser.add_argument("--skip-stage0-build", action="store_true",
                        help="copy the existing host-built seed (caller must first refresh edg2qbe/qbe/build.py)")
    parser.add_argument("--start-stage", type=int, choices=(1, 2, 3), default=1,
                        help="resume only if the preceding complete stage matches current inputs/artifacts")
    parser.add_argument("--stop-after-stage", type=int, choices=(0, 1, 2, 3), default=3,
                        help="stop at stage3 and compare stage2/stage3 by default; earlier stops are diagnostic")
    parser.add_argument("--unit", help="compile only one manifest unit (exact ID or unique source suffix); diagnostic only")
    parser.add_argument("--list-units", action="store_true", help="show captured complete source manifest; diagnostic only")
    args = parser.parse_args()
    if platform.system() != "Linux" or platform.machine() not in ("aarch64", "arm64"):
        raise RuntimeError("Bootstrap requires native Linux AArch64")
    if args.jobs < 1 or args.source_date_epoch < 0:
        raise RuntimeError("--jobs must be positive and --source-date-epoch nonnegative")
    if args.stop_after_stage and args.start_stage > args.stop_after_stage:
        raise RuntimeError("--start-stage exceeds --stop-after-stage")
    root = args.output_dir.resolve()
    root.mkdir(parents=True, exist_ok=True)
    env = os.environ.copy()
    env.update(SOURCE_DATE_EPOCH=str(args.source_date_epoch), TZ="UTC", LC_ALL="C")
    # Macro discovery inherits the same deterministic environment.
    os.environ.update(SOURCE_DATE_EPOCH=env["SOURCE_DATE_EPOCH"], TZ="UTC", LC_ALL="C")
    if not args.skip_stage0_build:
        logged([sys.executable, ROOT / "qbe/build.py", "-j", args.jobs], root / "stage0-build.log", env)
    seed = (*selected_toolchain(), *(FRONTEND / "bin" / name for name in SEED_TARGETS
                                    if name != "edg-qbe"))
    for name, source in zip(OUTPUTS, seed):
        copy(required(source), root / "stage0" / name)
    save_json(root / "stage0/complete.json", {"outputs": {name: digest(root / "stage0" / name) for name in OUTPUTS}})
    if args.stop_after_stage == 0:
        print("Diagnostic stop after stage0; fixed point NOT checked")
        return
    inputs = root / "inputs"
    inputs.mkdir(exist_ok=True)
    base_options = frontend_options(inputs / "base-gcc", language="c")
    instant = datetime.fromtimestamp(args.source_date_epoch, timezone.utc)
    date = instant.strftime("%b ") + f"{instant.day:2d}" + instant.strftime(" %Y")
    # EDG constructs builtin dates from wall time rather than SOURCE_DATE_EPOCH.
    # GNU mode permits command-line redefinition (src/macro.c proc_define).
    date_options = ["--define_macro", '__DATE__="' + date + '"',
                    "--define_macro", '__TIME__="' + instant.strftime("%H:%M:%S") + '"',
                    "--define_macro", '__TIMESTAMP__="' + instant.strftime("%a %b ") +
                    f"{instant.day:2d}" + instant.strftime(" %H:%M:%S %Y") + '"']
    seed_units, links = units_from_build(inputs, base_options)
    host_support = {str(output): arguments for output, arguments in support_commands().items()}
    save_json(inputs / "support-commands.json", host_support)
    cflags = c_options(base_options)
    runtime_output = next(output for output in host_support if Path(output).name == "runtime.o")
    runtime_unit = {"id": "support/runtime.c", "source": str(ROOT / "qbe/runtime.c"),
                    "host_object": runtime_output, "host_command": host_support[runtime_output],
                    "options": cflags}
    units = [*seed_units, runtime_unit]
    if len({unit["id"] for unit in units}) != len(units):
        raise RuntimeError("Duplicate source identities in the complete toolchain manifest")
    for unit in units:
        unit["options"] = [*unit["options"], *date_options]
    save_json(inputs / "units.json", units)
    if args.list_units:
        print("\n".join(unit["id"] for unit in units))
        print("Manifest only; fixed point NOT checked")
        return
    selected = None
    if args.unit:
        matches = [unit["id"] for unit in units if unit["id"] == args.unit or unit["id"].endswith("/" + args.unit)]
        if len(matches) != 1:
            raise RuntimeError(f"--unit {args.unit!r} matches {len(matches)} units; use --list-units")
        selected = matches[0]
    include_dirs = [Path(unit["options"][i + 1]) for unit in units
                    for i, value in enumerate(unit["options"])
                    if value in ("--sys_include", "--include_directory", "--preinclude")]
    # Canonicalize nested include roots to avoid hashing the same system tree twice.
    include_dirs = sorted(set(path.resolve() for path in include_dirs))
    include_dirs = [path for path in include_dirs if not any(parent in path.parents for parent in include_dirs)]
    host_files = [Path(command(name)).resolve() for name in ("as", "gcc", "ld")]
    host_files.append(required(capture([command("gcc"), "-print-prog-name=collect2"])).resolve())
    for name in ("libgcc.a", "libgcc_s.so.1", "libc.so", "libc.so.6", "libatomic.so", "libm.so", "libm.so.6",
                 "crt1.o", "crti.o", "crtn.o", "crtbegin.o", "crtend.o"):
        path = Path(capture([command("gcc"), "-print-file-name=" + name]))
        host_files.append(required(path).resolve())
    # Include explicitly selected alternate linkers and file dependencies in
    # resume/cache identity, not just the default GCC linker and runtime.
    for link in links.values():
        for arg in link["arguments"]:
            if isinstance(arg, str) and arg.startswith("-fuse-ld="):
                host_files.append(Path(command("ld." + arg.split("=", 1)[1])).resolve())
            elif isinstance(arg, str) and Path(arg).is_absolute() and Path(arg).is_file():
                host_files.append(Path(arg).resolve())
    # The package root is an include directory, but recursively snapshotting it
    # would also capture the build and bootstrap outputs stored beneath it.
    include_dirs = [path for path in include_dirs if path != ROOT]
    input_files = [*source_inputs(), inputs / "includes",
                   inputs / "base-gcc/lib/predefined_macros.txt",
                   ROOT / "qbe/runtime.c", ROOT / "qbe/native.S",
                   ROOT / "qbe/bootstrap.py", ROOT / "qbe/build.py", ROOT / "qbe/toolchain.py",
                   *include_dirs, *host_files]
    configuration = {"units": units, "links": links, "support_commands": host_support,
                     "files": snapshot(input_files), "epoch": args.source_date_epoch,
                     "environment": {name: env.get(name) for name in
                         ("SOURCE_DATE_EPOCH", "TZ", "LC_ALL", "CPATH", "C_INCLUDE_PATH", "LIBRARY_PATH")},
                     "work": str(root / "work")}
    configuration["linker_specs"] = capture([command("gcc"), "-dumpspecs"])
    save_json(inputs / "configuration.json", configuration)
    if args.start_stage > 1:
        check_completed_stage(args.start_stage - 1, root, units, configuration)
    for number in range(args.start_stage, args.stop_after_stage + 1):
        build_stage(number, root, units, configuration, env, args.jobs, selected)
        if selected:
            return
    if args.stop_after_stage < 3:
        print(f"Diagnostic stop after stage{args.stop_after_stage}; fixed point NOT checked")
        return
    compare(root, units)


if __name__ == "__main__":
    cli(main)
