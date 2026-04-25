from __future__ import annotations

import os
from pathlib import Path

from setuptools import Extension, find_packages, setup


ROOT = Path(__file__).parent.resolve()
MELPE_DIR = ROOT / "melpe"
PACKAGE_DIR = ROOT / "src" / "melpe_artifacts"


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


melpe_sources = sorted(
    rel(path)
    for path in MELPE_DIR.glob("*.c")
    if path.name not in {"encoder.c", "decoder.c"}
)

extra_compile_args = ["/O2"] if os.name == "nt" else ["-O3", "-std=c99"]
define_macros = (
    [("_CRT_SECURE_NO_WARNINGS", "1"), ("_USE_MATH_DEFINES", "1")]
    if os.name == "nt"
    else []
)
libraries = [] if os.name == "nt" else ["m"]

extension = Extension(
    "melpe_artifacts._melpe_native",
    sources=[rel(PACKAGE_DIR / "_melpe_native.c"), *melpe_sources],
    include_dirs=[".", "melpe"],
    define_macros=define_macros,
    extra_compile_args=extra_compile_args,
    libraries=libraries,
    language="c",
)

setup(
    name="melpe-artifacts",
    version="0.1.0",
    description="Cross-platform MELPe artifact simulator for NumPy and PyTorch audio arrays",
    long_description=(ROOT / "README.md").read_text(encoding="utf-8"),
    long_description_content_type="text/markdown",
    license="GPL-3.0-only",
    python_requires=">=3.9",
    package_dir={"": "src"},
    packages=find_packages("src"),
    install_requires=["numpy>=1.23"],
    ext_modules=[extension],
    zip_safe=False,
)
