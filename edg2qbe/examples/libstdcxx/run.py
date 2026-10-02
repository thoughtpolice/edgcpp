#!/usr/bin/env python3
"""Parse installed libstdc++ with EDG, compile its C++ output, and run it."""

from pathlib import Path
import subprocess


ROOT = Path(__file__).resolve().parents[3]
BUILD = ROOT / "build/gcc-aarch64"
OUTPUT = BUILD / "libstdcxx"
BASE = OUTPUT / "base"
EXPECTED = {
    "containers": "sorted=1,2,3,4 sum=10 edg=2 gcc=1\n",
    "lifetimes": "edg: unwind live=0 answer=42 expired=1\n",
    "ranges_threads": "squares=56 async=57 thread=7\n",
}


def main():
    (BASE / "lib").mkdir(parents=True, exist_ok=True)
    subprocess.run(
        [str(ROOT / "util/make_predef_macro_table"),
         "--gcc", "gcc", "--g++", "g++"],
        cwd=BASE / "lib", check=True,
    )

    version = subprocess.check_output(
        ["g++", "-dumpfullversion"], text=True,
    ).strip()
    major, minor, patch = (int(part) for part in version.split("."))
    edg_version = str(major * 10000 + minor * 100 + patch)
    probe = subprocess.run(
        ["g++", "-std=c++20", "-E", "-x", "c++", "-v", "/dev/null"],
        capture_output=True, text=True, check=True,
    )
    search = probe.stderr.split("#include <...> search starts here:\n", 1)[1]
    includes = search.split("End of search list.", 1)[0].splitlines()
    options = [
        "--g++", "--gnu_version", edg_version,
        "--gen_c_gnu_version", edg_version, "--c++20",
        "--edg_base_dir", str(BASE),
    ]
    for directory in includes:
        options.extend(["--sys_include", directory.strip()])

    print("Using G++ {} and its installed libstdc++ headers".format(version),
          flush=True)
    for name, expected in EXPECTED.items():
        source = Path(__file__).resolve().parent / (name + ".cpp")
        emitted = OUTPUT / (name + ".edg.cpp")
        executable = OUTPUT / name
        reference = OUTPUT / (name + ".gcc")
        subprocess.run(
            [str(BUILD / "bin/cpfe-cp"), *options,
             "--gen_c_file_name", str(emitted),
             "--output", str(OUTPUT / (name + ".cil")), str(source)],
            check=True,
        )
        for input_file, binary in ((emitted, executable), (source, reference)):
            subprocess.run(
                ["g++", "-std=c++20", "-O2", "-pthread",
                 str(input_file), "-o", str(binary)],
                check=True,
            )
            result = subprocess.run(
                [str(binary)], capture_output=True, text=True,
                check=True, timeout=10,
            )
            if result.stdout != expected:
                raise RuntimeError("{}: unexpected output {!r}".format(
                    binary, result.stdout))
        print("{}: EDG emission, emitted executable, and G++ reference PASS\n  {}"
              .format(name, expected.strip()), flush=True)
    print("Artifacts: {}".format(OUTPUT))


if __name__ == "__main__":
    main()
