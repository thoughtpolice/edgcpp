#!/usr/bin/env python3
"""Build QBE's IR frontend directly and exercise vendored optimizer regressions."""
import re
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parent.parent
VENDOR = ROOT / "qbe/vendor"
FIXTURES = ROOT / "qbe/tests/qbe_ir"


def run(arguments, **options):
    return subprocess.run([str(value) for value in arguments], check=True, **options)


class DirectQBE(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(prefix="edg-direct-qbe-")
        cls.work = Path(cls.temporary.name)
        entry = cls.work / "entry.c"
        entry.write_text(
            "extern int il2qbe_qbe_main(int, char **);\n"
            "int main(int argc, char **argv) { return il2qbe_qbe_main(argc, argv); }\n"
        )
        sources = sorted(VENDOR.glob("*.c"))
        for architecture in ("amd64", "arm64", "rv64"):
            sources.extend(sorted((VENDOR / architecture).glob("*.c")))
        cls.qbe = cls.work / "qbe"
        run(["gcc", "-std=gnu11", "-O2", "-D_GNU_SOURCE=1", entry,
             *sources, "-o", cls.qbe])

    @classmethod
    def tearDownClass(cls):
        cls.temporary.cleanup()

    def compile(self, fixture):
        assembly = self.work / (fixture.stem + ".S")
        run([self.qbe, "-t", "arm64", "-o", assembly, fixture])
        run(["as", "-o", assembly.with_suffix(".o"), assembly])
        return assembly

    def test_shared_phi_width_analysis(self):
        assembly = self.compile(FIXTURES / "shared_phi_bool.ssa")
        executable = self.work / "shared-phi-bool"
        run(["gcc", "-no-pie", assembly, "-o", executable])
        result = subprocess.run([executable], capture_output=True, text=True, timeout=10)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_phi_replacement_preserves_integer_width(self):
        fixture = FIXTURES / "mixed_width_phi.ssa"
        assembly = self.compile(fixture)
        result = run([self.qbe, "-d", "G", "-t", "arm64", "-o", assembly, fixture],
                     capture_output=True, text=True)
        classes = {}
        for line in result.stderr.splitlines():
            definition = re.match(r"\s*(%\S+) =([wlsd])(?:\s|$)", line)
            if definition:
                classes[definition.group(1)] = definition.group(2)
            returned = re.match(r"\s*ret([wl]) (%\S+)$", line)
            if returned and returned.group(2) in classes:
                self.assertEqual(classes[returned.group(2)], returned.group(1), line)


if __name__ == "__main__":
    unittest.main()
