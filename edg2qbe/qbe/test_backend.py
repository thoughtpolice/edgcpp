#!/usr/bin/env python3
"""Check native GNU exceptions, wide values, aggregates and variadic interoperability."""
import argparse
from pathlib import Path
import subprocess
import errno
import tempfile
import sys
import unittest
from toolchain import (ROOT, command, compile_source, environment,
                       frontend_options, run, selected_toolchain)

TOOLCHAIN = None


class CControlFlow(unittest.TestCase):
    def test_c_conditions_and_independent_local_statics(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-c-conditions-") as directory:
            work = Path(directory)
            frontend, runtime, native = selected_toolchain(TOOLCHAIN)
            qbe, assembly = (work / ("conditions." + suffix) for suffix in ("qbe", "S"))
            run([frontend, "--save-qbe", qbe,
                 *frontend_options(work / "base-gcc", language="c"),
                 "--output", assembly, ROOT / "qbe/tests/c_conditions.c"])
            binary = work / "conditions"
            run([command("gcc"), "-no-pie", assembly, runtime, native, "-latomic", "-lm", "-o", binary])
            result = subprocess.run([binary], env=environment(), capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


class FunctionDesignators(unittest.TestCase):
    def test_explicit_function_pointer_dereference(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-designator-") as directory:
            binary = Path(directory) / "callback"
            compile_source(ROOT / "qbe/tests/function_designator.cpp", binary, toolchain=TOOLCHAIN)
            result = subprocess.run([binary], env=environment(), capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


class CrossUnitODR(unittest.TestCase):
    def test_local_class_constructor_destructor_aliases(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-odr-") as directory:
            work = Path(directory)
            frontend, runtime, native = selected_toolchain(TOOLCHAIN)
            options = frontend_options(work / "base-gcc")
            objects = []
            for name, definitions in (("main", []), ("peer", ["--define_macro", "QBE_ODR_OTHER_TU=1"])):
                qbe, assembly, obj = (work / (name + "." + suffix) for suffix in ("qbe", "S", "o"))
                run([frontend, "--save-qbe", qbe, *options, *definitions,
                     "--output", assembly, ROOT / "qbe/tests/local_odr.cpp"])
                run([command("as"), "-o", obj, assembly])
                objects.append(obj)
            binary = work / "local-odr"
            run([command("g++"), "-no-pie", *objects, runtime, native, "-latomic", "-o", binary])
            result = subprocess.run([binary], env=environment(), capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)


class ExceptionABI(unittest.TestCase):
    def test_cleanup_and_rethrow(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-eh-") as directory:
            binary = Path(directory) / "exceptions"
            compile_source(ROOT / "qbe/tests/exceptions.cpp", binary, toolchain=TOOLCHAIN)
            result = subprocess.run([binary], env=environment(), capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(result.stdout, "score=31 live=0 destroyed=9\n")
            self.assertEqual(result.stderr, "")


class NativeABI(unittest.TestCase):
    def test_native_values_and_gcc_interoperability(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-abi-") as directory:
            work = Path(directory)
            reference = work / "reference.o"
            run([command("g++"), "-std=c++20", "-O2", "-c",
                 ROOT / "qbe/tests/native_abi_reference.cpp", "-o", reference])
            source = ROOT / "qbe/tests/native_abi.cpp"
            qbe, assembly = (work / ("native_abi." + suffix) for suffix in ("qbe", "S"))
            frontend, runtime, native = selected_toolchain(TOOLCHAIN)
            run([frontend, "--save-qbe", qbe, *frontend_options(work / "base-gcc"),
                 "--output", assembly, source])
            binary = work / "native_abi"
            run([command("g++"), "-no-pie", "-pthread", assembly, reference,
                 runtime, native, "-latomic", "-o", binary])
            result = subprocess.run([binary], env=environment(), capture_output=True, text=True, timeout=30)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(result.stderr, "")


class DirectOutputSafety(unittest.TestCase):
    def make_hardlink(self, destination, source):
        try:
            destination.hardlink_to(source)
        except OSError as error:
            if error.errno in (errno.EPERM, errno.EACCES, errno.ENOSYS,
                               errno.EOPNOTSUPP, errno.EXDEV):
                self.skipTest(f"Hardlinks unavailable: {error}")
            raise

    def check_rejected_output(self, work, source, output, content):
        frontend, _, _ = selected_toolchain(TOOLCHAIN)
        result = subprocess.run(
            [frontend, *frontend_options(work / "base-gcc"),
             "--output", output, source],
            env=environment(), capture_output=True, text=True, timeout=30)
        self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(source.read_bytes(), content)
        self.assertEqual(output.read_bytes(), content)

    def test_same_path_preserves_source(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-output-") as directory:
            work = Path(directory)
            source = work / "input.cpp"
            content = b"int main() { return 0; }\n"
            source.write_bytes(content)
            self.check_rejected_output(work, source, source, content)

    def test_hardlink_preserves_source(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-output-") as directory:
            work = Path(directory)
            source, output = work / "input.cpp", work / "output.qbe"
            content = b"int main() { return 0; }\n"
            source.write_bytes(content)
            self.make_hardlink(output, source)
            self.check_rejected_output(work, source, output, content)

    def test_saved_qbe_preserves_source(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-saved-source-") as directory:
            work = Path(directory)
            source, assembly = work / "input.cpp", work / "output.S"
            content = b"int main() { return 0; }\n"
            source.write_bytes(content)
            saved = work / "saved.qbe"
            self.make_hardlink(saved, source)
            frontend, _, _ = selected_toolchain(TOOLCHAIN)
            result = subprocess.run(
                [frontend, "--save-qbe", saved, *frontend_options(work / "base-gcc"),
                 "--output", assembly, source],
                capture_output=True, text=True, timeout=30)
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(source.read_bytes(), content)
            self.assertFalse(assembly.exists())

    def test_saved_qbe_preserves_assembly_output(self):
        with tempfile.TemporaryDirectory(prefix="edg-qbe-saved-output-") as directory:
            work = Path(directory)
            source, assembly = work / "input.cpp", work / "output.S"
            source.write_text("int main() { return 0; }\n")
            content = b"existing assembly output\n"
            assembly.write_bytes(content)
            saved = work / "saved.qbe"
            self.make_hardlink(saved, assembly)
            frontend, _, _ = selected_toolchain(TOOLCHAIN)
            result = subprocess.run(
                [frontend, "--save-qbe", saved, *frontend_options(work / "base-gcc"),
                 "--output", assembly, source],
                capture_output=True, text=True, timeout=30)
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
            self.assertEqual(assembly.read_bytes(), content)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--toolchain", type=Path, help="staged toolchain root (bin/ and lib/)")
    args, remaining = parser.parse_known_args()
    TOOLCHAIN = args.toolchain
    unittest.main(argv=[sys.argv[0], *remaining])
