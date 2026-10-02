#!/usr/bin/env python3
"""Recreate checked-in QBE sources from the verified archive and local patch."""
import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import tarfile
import tempfile

ROOT = Path(__file__).resolve().parent.parent
URL = "https://c9x.me/compile/release/qbe-1.3.tar.xz"
SHA256 = "d587905d620dc5e1d2bfa7c2cc642b9b837aa89a3188c6e37b53d756cf66e320"
ARCHES = {"amd64", "arm64", "rv64"}


def selected(path):
    return ((path.suffix in {".c", ".h"} and
             (len(path.parts) == 1 or
              (len(path.parts) == 2 and path.parts[0] in ARCHES))) or
            str(path) in {"LICENSE", "README"})


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, default=ROOT / "build/qbe-1.3.tar.xz")
    parser.add_argument("--output", type=Path, default=ROOT / "qbe/vendor")
    parser.add_argument("--check", action="store_true",
                        help="verify checked-in sources without modifying them")
    args = parser.parse_args()
    content = args.archive.read_bytes()
    if hashlib.sha256(content).hexdigest() != SHA256:
        raise SystemExit(f"SHA256 mismatch: {args.archive}")
    patch_directory = ROOT / "qbe/patches"
    series = [line.strip() for line in (patch_directory / "series").read_text().splitlines()
              if line.strip() and not line.lstrip().startswith("#")]
    if (not series or len(series) != len(set(series)) or
            any(Path(name).name != name for name in series)):
        raise SystemExit("Invalid QBE patch series")
    patches = [patch_directory / name for name in series]
    with tempfile.TemporaryDirectory(prefix="edg-qbe-vendor-") as directory:
        source = Path(directory)
        with tarfile.open(args.archive) as archive:
            for member in archive:
                path = Path(member.name)
                if (path.is_absolute() or ".." in path.parts or
                        not path.parts or path.parts[0] != "qbe-1.3" or
                        member.issym() or member.islnk()):
                    raise SystemExit(f"Unsafe archive entry: {member.name}")
                relative = Path(*path.parts[1:])
                if not member.isfile() or not selected(relative):
                    continue
                target = source / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                with archive.extractfile(member) as stream:
                    target.write_bytes(stream.read())
        command = ["git", "apply", "--binary", "--whitespace=nowarn"]
        for patch in patches:
            subprocess.run([*command, "--check", str(patch)], cwd=source, check=True)
            subprocess.run([*command, str(patch)], cwd=source, check=True)
        patch_provenance = "".join(
            f"  {hashlib.sha256(patch.read_bytes()).hexdigest()}  {patch.name}\n"
            for patch in patches)
        provenance = ("QBE 1.3\n" + URL + "\nArchive SHA256: " + SHA256 +
                      "\nPatch series:\n" + patch_provenance +
                      "Regenerate: python3 edg2qbe/qbe/update_vendor.py --archive edg2qbe/build/qbe-1.3.tar.xz\n"
                      "Local changes: AArch64 CFI, ABI, TLS, dynamic-stack and stack-offset fixes;\n"
                      "GVN width/type preservation, QBE-only symbol namespace and in-process FILE-stream entry point.\n"
                      "Post-1.3 fixes: fa19d3c974dae37cf335781300d0e4791be0d87c,\n"
                      "80d745c842b4dafe02a69a6eb15e4f8ff72d42fa, and\n"
                      "e786f06032fefa2e3790d6b1c9e31ed138f475a6 from https://c9x.me/git/qbe.git.\n"
                      "Ordinary builds consume these sources directly; no regeneration is needed.\n")
        (source / "UPSTREAM").write_text(provenance)
        expected = {p.relative_to(source) for p in source.rglob("*") if p.is_file()}
        if not args.check:
            args.output.mkdir(parents=True, exist_ok=True)
        existing = {p.relative_to(args.output) for p in args.output.rglob("*") if p.is_file()}
        unexpected = existing - expected
        if unexpected:
            raise SystemExit("Output contains unexpected files; remove obsolete vendor files explicitly: " +
                             ", ".join(map(str, sorted(unexpected))))
        if args.check:
            if existing != expected:
                raise SystemExit("Checked-in QBE source inventory mismatch")
            for relative in sorted(expected):
                if (source / relative).read_bytes() != (args.output / relative).read_bytes():
                    raise SystemExit("Checked-in QBE source mismatch: " + str(relative))
            print("Verified checked-in QBE sources against the pinned archive and patch series")
        else:
            for relative in sorted(expected):
                target = args.output / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                shutil.copyfile(source / relative, target)


if __name__ == "__main__":
    main()
