#!/usr/bin/env python3
"""Differential, reproducible YARPGen execution against a selected native compiler."""
import argparse
from collections import Counter
from concurrent.futures import ThreadPoolExecutor, as_completed
import hashlib
import json
import os
from pathlib import Path
import random
import re
import shlex
import shutil
import signal
import subprocess
import time

from toolchain import command, frontend_options, required, selected_toolchain


LANGUAGES = {
    "c": (("driver.c", "func.c", "init.h"), "c11", "gcc"),
    "c++": (("driver.cpp", "func.cpp", "init.h"), "c++20", "g++"),
}


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def save(path, value):
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(json.dumps(value, indent=2) + "\n")
    temporary.replace(path)


class Failure(Exception):
    def __init__(self, status, detail):
        self.status, self.detail = status, detail


def execute(directory, label, args, timeout):
    args = [str(arg) for arg in args]
    start = time.monotonic()
    with subprocess.Popen(args, cwd=directory, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, start_new_session=True) as process:
        timed_out = False
        try:
            stdout, stderr = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            os.killpg(process.pid, signal.SIGKILL)
            stdout, stderr = process.communicate()
    record = dict(command=args, returncode=process.returncode, timeout=timed_out,
                  seconds=time.monotonic() - start,
                  stdout=stdout.decode(errors="replace"),
                  stderr=stderr.decode(errors="replace"))
    save(directory / (label + ".json"), record)
    (directory / (label + ".log")).write_text(
        "$ " + shlex.join(args) + "\n" + record["stdout"] + record["stderr"])
    return record


def checked(directory, label, args, timeout, status):
    result = execute(directory, label, args, timeout)
    if result["timeout"]:
        raise Failure(status + "_TIMEOUT", label)
    if result["returncode"]:
        raise Failure(status, label)
    return result


def run_case(seed, previous, config):
    directory = config["output"] / str(seed)
    directory.mkdir()
    result = dict(seed=seed, status="INCOMPLETE")
    start = time.monotonic()
    try:
        if previous:
            for name in config["sources"]:
                shutil.copy2(previous / name, directory / name)
        else:
            generator_args = [config["generator"], "--std=" + config["language"],
                              "--seed=" + str(seed),
                              "--check-algo=" + config["check_algorithm"],
                              "--allow-ub-in-dc=none", "--max-array-dims=4"]
            if config["mutation"] != "none":
                mutation_seed = (seed ^ 0x9E3779B9) & 0xFFFFFFFF or 1
                generator_args += ["--mutate=" + config["mutation"],
                                   "--mutation-seed=" + str(mutation_seed)]
            generator_args += [*config["generator_options"],
                               "--out-dir=" + str(directory)]
            checked(directory, "generate", generator_args,
                    config["compile_timeout"], "GENERATOR_FAILURE")
        result["sources"] = {name: digest(directory / name) for name in config["sources"]}
        reference = []
        for level in ("0", "2"):
            executable = directory / ("reference-O" + level)
            checked(directory, "gcc-O" + level, [config["reference_compiler"],
                    "-std=" + config["standard"], "-O" + level, "-pthread",
                    *config["translation_units"], "-o", executable],
                    config["compile_timeout"], "REFERENCE_COMPILE_FAILURE")
            output = checked(directory, "reference-O" + level, [executable],
                             config["run_timeout"], "REFERENCE_RUNTIME_FAILURE")
            if not re.fullmatch(r"[0-9]+\n", output["stdout"]) or output["stderr"]:
                raise Failure("REFERENCE_INVALID_OUTPUT", "reference-O" + level)
            reference.append(output["stdout"])
        result["expected"] = reference[0]
        if reference[0] != reference[1]:
            raise Failure("REFERENCE_DISAGREEMENT", "reference -O0 and -O2 differ")
        objects = []
        for stem in ("driver", "func"):
            assembly = directory / (stem + ".S")
            obj = directory / (stem + ".o")
            checked(directory, "native-" + stem, [config["frontend"], "--save-qbe",
                    directory / (stem + ".qbe"), *config["options"],
                    "--output", assembly, directory / (stem + config["suffix"])],
                    config["compile_timeout"], "NATIVE_COMPILE_FAILURE")
            checked(directory, "assemble-" + stem, [config["as"], "-o", obj, assembly],
                    config["compile_timeout"], "ASSEMBLY_FAILURE")
            objects.append(obj)
        executable = directory / "native"
        checked(directory, "link", [config["reference_compiler"], "-no-pie", "-pthread",
                *objects, config["runtime"], config["native"], "-latomic", "-lm",
                "-o", executable], config["compile_timeout"], "LINK_FAILURE")
        output = checked(directory, "execute-native", [executable], config["run_timeout"],
                         "NATIVE_RUNTIME_FAILURE")
        result["actual"] = output["stdout"]
        if output["stdout"] != reference[0] or output["stderr"]:
            raise Failure("WRONG_CODE", "native output differs from both references")
        result["status"] = "PASS"
    except Failure as error:
        result.update(status=error.status, detail=error.detail)
    except Exception as error:
        result.update(status="HARNESS_FAILURE", detail=repr(error))
    result["seconds"] = time.monotonic() - start
    save(directory / "case.json", result)
    if result["status"] == "PASS" and not config["keep"]:
        for name in ("driver.S", "func.S", "driver.qbe", "func.qbe", "driver.o", "func.o",
                     "reference-O0", "reference-O2", "native"):
            (directory / name).unlink(missing_ok=True)
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--generator", type=Path,
                        default=Path.home() / "src/yarpgen/scripts/yarpgen")
    parser.add_argument("--toolchain", type=Path, required=True,
                        help="Explicit stage directory; never silently use the seed compiler")
    parser.add_argument("--output-dir", type=Path, required=True,
                        help="New directory; contains sources, commands, logs and summary")
    parser.add_argument("-j", "--jobs", type=int, default=12)
    parser.add_argument("--language", choices=tuple(LANGUAGES), default="c++",
                        help="Generated source language (default: c++)")
    parser.add_argument("--count", type=int, default=1000)
    parser.add_argument("--campaign-seed", type=int)
    parser.add_argument("--seed", type=int, action="append", help="Replay a generator seed")
    parser.add_argument("--replay", type=Path,
                        help="Reuse all failed cases' retained sources from a previous campaign")
    parser.add_argument("--generator-option", action="append", default=[],
                        help="Additional option passed verbatim to YARPGen; repeat as needed")
    parser.add_argument("--check-algorithm", choices=("hash", "asserts"),
                        default="hash", help="YARPGen result checker (default: hash)")
    parser.add_argument("--mutate", choices=("none", "exprs", "all"), default="none",
                        help="Deterministic per-case YARPGen mutation (default: none)")
    parser.add_argument("--compile-timeout", type=float, default=120)
    parser.add_argument("--run-timeout", type=float, default=30)
    parser.add_argument("--keep-passing-artifacts", action="store_true")
    args = parser.parse_args()
    if args.jobs < 1 or args.count < 1 or min(args.compile_timeout, args.run_timeout) <= 0:
        parser.error("Jobs, count and timeouts must be positive")
    if args.seed and args.replay:
        parser.error("Choose --seed or --replay")
    generator = required(args.generator.resolve())
    sources, standard, reference_name = LANGUAGES[args.language]
    reference_compiler = command(reference_name)
    frontend, runtime, native = map(required, selected_toolchain(args.toolchain))
    output = args.output_dir.resolve()
    output.mkdir(parents=True, exist_ok=False)
    campaign_seed = args.campaign_seed if args.campaign_seed is not None else random.SystemRandom().getrandbits(64)
    previous = {}
    if args.replay:
        previous_root = args.replay.resolve()
        old = json.loads((previous_root / "summary.json").read_text())
        previous = {row["seed"]: previous_root / str(row["seed"]) for row in old["cases"]
                    if row["status"] != "PASS"}
        seeds = sorted(previous)
    elif args.seed:
        seeds = args.seed
    else:
        seeds = random.Random(campaign_seed).sample(range(1, 2**32), args.count)
    if not seeds or len(seeds) != len(set(seeds)) or any(not 0 < s < 2**32 for s in seeds):
        parser.error("Need nonempty, unique positive 32-bit seeds")
    # Freeze the selected compiler/support objects: rebuilding the original stages
    # must not change the producer halfway through a campaign.
    snapshot = output / "toolchain"
    for source, relative in ((frontend, "bin/edg-qbe"), (runtime, "lib/runtime.o"),
                             (native, "lib/native.o")):
        target = snapshot / relative
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, target)
    frontend, runtime, native = selected_toolchain(snapshot)
    options = frontend_options(output / "base-gcc", language=args.language,
                               standard=standard)
    config = dict(output=output, generator=generator,
                  generator_options=args.generator_option, mutation=args.mutate,
                  check_algorithm=args.check_algorithm, frontend=frontend,
                  runtime=runtime, native=native, options=options,
                  language=args.language, standard=standard, sources=sources,
                  translation_units=sources[:2], suffix=Path(sources[0]).suffix,
                  reference_compiler=reference_compiler,
                  **{"as": command("as")},
                  compile_timeout=args.compile_timeout, run_timeout=args.run_timeout,
                  keep=args.keep_passing_artifacts)
    manifest = dict(campaign_seed=campaign_seed, seeds=seeds, jobs=args.jobs,
                    generator=str(generator), generator_options=args.generator_option,
                    mutation=args.mutate, check_algorithm=args.check_algorithm,
                    toolchain=str(args.toolchain.resolve()),
                    replay=str(args.replay.resolve()) if args.replay else None,
                    language=args.language, standard=standard, frontend_options=options,
                    compile_timeout=args.compile_timeout, run_timeout=args.run_timeout,
                    sha256={str(p): digest(p) for p in
                            (generator, frontend, runtime, native)},
                    platform=os.uname()._asdict() if hasattr(os.uname(), "_asdict")
                    else list(os.uname()),
                    versions={name: subprocess.check_output([program, "--version"], text=True)
                              for name, program in
                              (("generator", generator),
                               (reference_name, reference_compiler))})
    save(output / "manifest.json", manifest)
    start = time.monotonic()
    results = []
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {executor.submit(run_case, seed, previous.get(seed), config): seed for seed in seeds}
        for future in as_completed(futures):
            row = future.result()
            results.append(row)
            summary = dict(cases=sorted(results, key=lambda r: r["seed"]),
                           counts=dict(Counter(r["status"] for r in results)),
                           completed=len(results), requested=len(seeds), seconds=time.monotonic() - start)
            save(output / "summary.json", summary)
            print(f"{len(results)}/{len(seeds)} seed={row['seed']} {row['status']} "
                  f"{row['seconds']:.2f}s", flush=True)
    print(json.dumps(summary["counts"], sort_keys=True), flush=True)
    raise SystemExit(0 if all(row["status"] == "PASS" for row in results) else 1)


if __name__ == "__main__":
    main()
