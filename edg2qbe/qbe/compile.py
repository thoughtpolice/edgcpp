#!/usr/bin/env python3
"""Compile C or modern C++ directly through in-memory EDG IL to native QBE."""
import argparse
from toolchain import cli, compile_source


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", help="C (.c) or C++ source file parsed by EDG")
    parser.add_argument("-o", "--output", required=True, help="native executable output")
    parser.add_argument("--emit-qbe", help="write QBE IL to this path instead of the artifact directory")
    parser.add_argument("--artifacts-dir", help="retain .qbe and .S here (default: OUTPUT.artifacts)")
    parser.add_argument("--toolchain", help="staged toolchain root (bin/ and lib/)")
    parser.add_argument("--std", choices=("c99", "c11", "c17", "c++11", "c++14",
                                          "c++17", "c++20", "c++23"),
                        help="source dialect (default: C11 or C++20)")
    args = parser.parse_args()
    compile_source(args.source, args.output, args.artifacts_dir, args.emit_qbe,
                   toolchain=args.toolchain, standard=args.std)


if __name__ == "__main__":
    cli(main)
