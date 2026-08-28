#!/usr/bin/env python3
import os
import re
import subprocess
from pathlib import Path


def get_root_dir():
    return Path(__file__).resolve().parent.parent


def find_best_build_dir(root):
    build_dirs = (
        list(root.glob("build*"))
        + list(root.glob("cmake-build-*"))
        + list(root.glob("out/build/*"))
    )
    best_dir = None
    max_gcno = -1

    for d in build_dirs:
        if d.is_dir() and (d / "CTestTestfile.cmake").exists():
            gcno_count = len(list(d.rglob("*.gcno")))
            if gcno_count > max_gcno:
                max_gcno = gcno_count
                best_dir = d

    if not best_dir:
        for d in build_dirs:
            if d.is_dir() and (d / "CTestTestfile.cmake").exists():
                best_dir = d
                break
    return best_dir


def get_existing_test_coverage(root):
    svg_path = root / "reports" / "test_coverage.svg"
    if svg_path.exists():
        svg_content = svg_path.read_text(encoding="utf-8")
        match = re.search(
            r"<title>test coverage: ([0-9]+(?:\.[0-9]+)?)%</title>", svg_content
        )
        if match:
            return match.group(1)

    readme_path = root / "README.md"
    if readme_path.exists():
        readme_content = readme_path.read_text(encoding="utf-8")
        match = re.search(
            r"https://img\.shields\.io/badge/coverage-([0-9]+(?:\.[0-9]+)?)%25-[a-z]+\.svg",
            readme_content,
        )
        if match:
            return match.group(1)
    return "0"


def get_test_coverage(root, build_dir):
    if not build_dir:
        return get_existing_test_coverage(root)

    gcda_files = list(build_dir.rglob("*.gcda"))
    if not gcda_files:
        try:
            subprocess.run(
                ["ctest", "-C", "Debug"],
                cwd=build_dir,
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
            )
        except (FileNotFoundError, OSError):
            pass

    # Try gcovr first for better Windows (MinGW) support
    try:
        result = subprocess.run(
            ["gcovr", "--print-summary"], cwd=build_dir, capture_output=True, text=True
        )
        match = re.search(r"lines:\s*([0-9]+(?:\.[0-9]+)?)(?:%)?", result.stdout)
        if match and "(0 out of 0)" not in result.stdout:
            return match.group(1)
    except (FileNotFoundError, OSError):
        pass

    try:
        result = subprocess.run(
            ["ctest", "-C", "Debug", "-T", "Coverage"],
            cwd=build_dir,
            capture_output=True,
            text=True,
        )
    except (FileNotFoundError, OSError):
        return get_existing_test_coverage(root)

    match = re.search(
        r"(?:Percentage|Total) Coverage:\s*([0-9]+(?:\.[0-9]+)?)(?:%)?", result.stdout
    )
    if match:
        return match.group(1)

    return get_existing_test_coverage(root)


def get_doc_coverage(root):
    headers = []
    c_str_span_dir = root / "c_str_span"
    exclude_patterns = [
        "_internal.h",
        "_private.h",
        "c_str_span_export.h",
        "c_str_span_stdint.h",
        "c_str_span_stdbool.h",
    ]

    for f in c_str_span_dir.glob("*.h"):
        if not any(excl in f.name for excl in exclude_patterns):
            headers.append(f)

    total_symbols = 0
    doc_symbols = 0

    symbol_re = re.compile(r"^(extern|typedef|#define\s+(AZ_|az_))")

    for header in headers:
        content = header.read_text(encoding="utf-8")

        symbols = 0
        for line in content.splitlines():
            if (
                symbol_re.match(line)
                and "_az_" not in line
                and 'extern "C"' not in line
            ):
                symbols += 1
        total_symbols += symbols

        docs = content.count("/**")
        file_docs = content.count("@file")
        symbol_docs = max(0, docs - file_docs)
        doc_symbols += symbol_docs

    if total_symbols > 0:
        if doc_symbols > total_symbols:
            return "100"
        return str(int(doc_symbols * 100 / total_symbols))
    return "100"


def generate_badge(root, label, value, filename):
    val_num = float(value) if value else 0
    val_int = int(val_num)

    if val_int < 50:
        color = "#e05d44"
        shield_color = "red"
    elif val_int < 80:
        color = "#dfb317"
        shield_color = "yellow"
    else:
        color = "#97ca00"
        shield_color = "brightgreen"

    readme_path = root / "README.md"
    if readme_path.exists():
        readme_content = readme_path.read_text(encoding="utf-8")
        if label == "test coverage":
            readme_content = re.sub(
                r"https://img\.shields\.io/badge/coverage-[0-9]+(?:\.[0-9]+)?%25-[a-z]+\.svg",
                f"https://img.shields.io/badge/coverage-{value}%25-{shield_color}.svg",
                readme_content,
            )
        elif label == "doc coverage":
            readme_content = re.sub(
                r"https://img\.shields\.io/badge/docs-[0-9]+(?:\.[0-9]+)?%25-[a-z]+\.svg",
                f"https://img.shields.io/badge/docs-{value}%25-{shield_color}.svg",
                readme_content,
            )
        readme_path.write_text(readme_content, encoding="utf-8", newline="\n")

    reports_dir = root / "reports"
    reports_dir.mkdir(exist_ok=True)

    label_width = 95
    value_str = f"{value}%"
    value_width = 55 if len(value_str) > 4 else 45

    total_width = label_width + value_width
    label_x = label_width * 5
    value_x = label_width * 10 + value_width * 5

    svg_template = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{total_width}" height="20" role="img" aria-label="{label}: {value_str}">
  <title>{label}: {value_str}</title>
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <clipPath id="r">
    <rect width="{total_width}" height="20" rx="3" fill="#fff"/>
  </clipPath>
  <g clip-path="url(#r)">
    <rect width="{label_width}" height="20" fill="#555"/>
    <rect x="{label_width}" width="{value_width}" height="20" fill="{color}"/>
    <rect width="{total_width}" height="20" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" text-rendering="geometricPrecision" font-size="110">
    <text aria-hidden="true" x="{label_x}" y="150" fill="#010101" fill-opacity=".3" transform="scale(.1)" textLength="{label_width * 10 - 100}">{label}</text>
    <text x="{label_x}" y="140" transform="scale(.1)" fill="#fff" textLength="{label_width * 10 - 100}">{label}</text>
    <text aria-hidden="true" x="{value_x}" y="150" fill="#010101" fill-opacity=".3" transform="scale(.1)" textLength="{value_width * 10 - 100}">{value_str}</text>
    <text x="{value_x}" y="140" transform="scale(.1)" fill="#fff" textLength="{value_width * 10 - 100}">{value_str}</text>
  </g>
</svg>
"""

    (reports_dir / filename).write_text(svg_template, encoding="utf-8", newline="\n")


def main():
    root = get_root_dir()
    best_build = find_best_build_dir(root)

    test_cov = get_test_coverage(root, best_build)
    doc_cov = get_doc_coverage(root)

    generate_badge(root, "test coverage", test_cov, "test_coverage.svg")
    generate_badge(root, "doc coverage", doc_cov, "doc_coverage.svg")


if __name__ == "__main__":
    main()
