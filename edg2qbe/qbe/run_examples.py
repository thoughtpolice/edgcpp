#!/usr/bin/env python3
"""Check packaged C++ semantics, rejection cases, and mixed GNU ABI programs."""
import argparse
import json
from pathlib import Path
import shlex
import subprocess
from toolchain import BUILD, ROOT, cli, command, frontend_options, required, selected_toolchain


def cases():
    result = []
    for manifest in sorted((ROOT / "examples").rglob("cases.json")):
        result.extend(json.loads(manifest.read_text()))
    return result


def logged(arguments, path, timeout):
    arguments = [str(arg) for arg in arguments]
    path.parent.mkdir(parents=True, exist_ok=True)
    result = subprocess.run(arguments, capture_output=True, text=True, timeout=timeout)
    path.write_text("$ " + shlex.join(arguments) + "\n" + result.stdout + result.stderr)
    return result


def successful(arguments, path, timeout):
    result = logged(arguments, path, timeout)
    if result.returncode:
        raise RuntimeError(f"Command failed ({result.returncode}); log: {path}\n"
                           + "\n".join((result.stdout + result.stderr).splitlines()[-25:]))
    return result


def check_program(binary, case, work, label, timeout):
    result = successful([binary], work / (label + ".run.log"), timeout)
    if result.stdout != case["stdout"] or result.stderr:
        raise RuntimeError(f"{label}: expected stdout {case['stdout']!r} and empty stderr; "
                           f"got stdout {result.stdout!r}, stderr {result.stderr!r}")


def check_case(case, args):
    work = args.output_dir / case["name"]
    work.mkdir(parents=True, exist_ok=True)
    sources = [required(ROOT / "examples" / source) for source in case["sources"]]
    standard = case["standard"]
    frontend, runtime, native = selected_toolchain(args.toolchain)
    options = frontend_options(work / "base-gcc", standard=standard)
    gcc = command("g++")
    if case.get("reject"):
        assembly = work / "rejected.S"
        assembly.unlink(missing_ok=True)
        generated = logged([required(frontend), *options, "--strict", "--output", assembly, *sources],
                           work / "edg.reject.log", args.compile_timeout)
        reference = logged([gcc, "-std=" + standard, "-pedantic-errors", "-fsyntax-only", *sources],
                           work / "gcc.reject.log", args.compile_timeout)
        if generated.returncode <= 0 or reference.returncode <= 0:
            raise RuntimeError(f"Expected ordinary diagnostic rejection: EDG exit "
                               f"{generated.returncode}, G++ exit {reference.returncode}; logs: {work}")
        if assembly.exists():
            raise RuntimeError(f"Invalid C++ emitted assembly: {assembly}")
        return {"name": case["name"], "standard": standard, "status": "PASS",
                "features": case["features"], "executions": 0, "rejected_by": ["EDG", "G++"]}
    gcc_objects = []
    for index, source in enumerate(sources):
        stem = work / (str(index) + "-" + source.stem)
        reference = Path(str(stem) + ".gcc.o")
        successful([gcc, "-std=" + standard, "-O2", "-pthread", "-c", source, "-o", reference],
                   Path(str(stem) + ".gcc.log"), args.compile_timeout)
        gcc_objects.append(reference)
    reference_binary = work / "gcc"
    successful([gcc, "-no-pie", "-pthread", *gcc_objects, "-lm", "-o", reference_binary],
               work / "gcc.link.log", args.compile_timeout)
    check_program(reference_binary, case, work, "gcc", args.timeout)
    if case.get("unsupported"):
        assembly = work / "unsupported.S"
        assembly.unlink(missing_ok=True)
        result = logged([required(frontend), *options, "--output", assembly, *sources],
                        work / "edg.unsupported.log", args.compile_timeout)
        diagnostic = " ".join((result.stdout + result.stderr).split())
        emitted = assembly.exists() and assembly.stat().st_size != 0
        if result.returncode <= 0 or case["unsupported"] not in diagnostic or emitted:
            raise RuntimeError(f"Expected explicit unsupported-capability rejection; log: {work}")
        return {"name": case["name"], "standard": standard, "status": "UNSUPPORTED",
                "features": case["features"], "executions": 1,
                "reason": case["unsupported"], "variants": ["gcc"]}
    edg_objects = []
    for index, source in enumerate(sources):
        stem = work / (str(index) + "-" + source.stem)
        qbe, assembly, obj = (Path(str(stem) + suffix) for suffix in (".qbe", ".S", ".o"))
        successful([required(frontend), "--save-qbe", qbe, *options,
                    "--output", assembly, source], Path(str(stem) + ".edg.log"), args.compile_timeout)
        successful([command("as"), "-o", obj, assembly],
                   Path(str(stem) + ".as.log"), args.compile_timeout)
        edg_objects.append(obj)
    variants = [("edg", edg_objects)]
    if case.get("mixed"):
        for index in range(len(sources)):
            objects = list(edg_objects)
            objects[index] = gcc_objects[index]
            variants.append((f"gcc-tu{index}", objects))
    for label, objects in variants:
        binary = work / label
        support = [] if label == "gcc" else [required(runtime), required(native), "-latomic"]
        successful([gcc, "-no-pie", "-pthread", *objects, *support, "-lm", "-o", binary],
                   work / (label + ".link.log"), args.compile_timeout)
        check_program(binary, case, work, label, args.timeout)
    return {"name": case["name"], "standard": standard, "status": "PASS",
            "features": case["features"], "executions": len(variants) + 1,
            "variants": ["gcc", *(label for label, _ in variants)]}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output-dir", type=Path, default=BUILD / "edg-qbe/examples",
                        help="executables, references, QBE, assembly, command logs and results.json")
    parser.add_argument("--timeout", type=float, default=30, help="per-executable timeout in seconds")
    parser.add_argument("--compile-timeout", type=float, default=120, help="per-build-command timeout")
    parser.add_argument("--toolchain", type=Path, help="staged toolchain root (bin/ and lib/)")
    parser.add_argument("--case", action="append", help="exact case name; repeat to select multiple cases")
    parser.add_argument("--list", action="store_true", help="list cases and their semantic coverage")
    args = parser.parse_args()
    selected = cases()
    if args.case:
        unknown = set(args.case) - {case["name"] for case in selected}
        if unknown:
            raise RuntimeError("Unknown cases: " + ", ".join(sorted(unknown)))
        selected = [case for case in selected if case["name"] in args.case]
    if args.list:
        for case in selected:
            status = " [UNSUPPORTED: " + case["unsupported"] + "]" if case.get("unsupported") else ""
            print(f"{case['name']} ({case['standard']}){status}: " + "; ".join(case["features"]))
        return
    args.output_dir = args.output_dir.resolve()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    results = []
    for case in selected:
        try:
            result = check_case(case, args)
        except (RuntimeError, OSError, subprocess.SubprocessError) as error:
            result = {"name": case["name"], "standard": case["standard"], "status": "FAIL",
                      "features": case["features"], "error": str(error)}
        results.append(result)
        print(f"{case['name']}: {result['status']}", flush=True)
        if "error" in result:
            print(result["error"], flush=True)
        (args.output_dir / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    failures = [result["name"] for result in results if result["status"] == "FAIL"]
    passed = sum(result["status"] == "PASS" for result in results)
    unsupported = sum(result["status"] == "UNSUPPORTED" for result in results)
    print(f"Cases: {len(results)}; passed: {passed}; unsupported: {unsupported}; failed: {len(failures)}")
    print(f"Artifacts: {args.output_dir}")
    if failures:
        raise RuntimeError("Example failures: " + ", ".join(failures))


if __name__ == "__main__":
    cli(main)
