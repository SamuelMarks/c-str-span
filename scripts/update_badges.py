import os
import re
import subprocess
import glob

def find_build_dirs(root):
    build_dirs = []
    for d in os.listdir(root):
        path = os.path.join(root, d)
        if os.path.isdir(path) and (d.startswith("build") or d.startswith("cmake-build")):
            build_dirs.append(path)
    return build_dirs

def get_best_build_dir(build_dirs):
    best_dir = None
    max_gcno = 0

    for d in build_dirs:
        if os.path.exists(os.path.join(d, "CTestTestfile.cmake")):
            gcno_count = 0
            for root_dir, _, files in os.walk(d):
                gcno_count += sum(1 for f in files if f.endswith(".gcno"))
            if gcno_count > max_gcno:
                max_gcno = gcno_count
                best_dir = d

    if not best_dir:
        for d in build_dirs:
            if os.path.exists(os.path.join(d, "CTestTestfile.cmake")):
                best_dir = d
                break

    return best_dir

def get_test_coverage(build_dir):
    if not build_dir:
        return "0%"

    has_gcda = False
    for root_dir, _, files in os.walk(build_dir):
        if any(f.endswith(".gcda") for f in files):
            has_gcda = True
            break

    if not has_gcda:
        subprocess.run(["ctest", "-C", "Debug"], cwd=build_dir, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

    res = subprocess.run(["ctest", "-C", "Debug", "-T", "Coverage"], cwd=build_dir, capture_output=True, text=True)

    match = re.search(r"(?:Percentage Coverage|Total Coverage):\s*([0-9]+(?:\.[0-9]+)?)%", res.stdout)
    if match:
        return f"{match.group(1)}%"

    return "0%"

def get_doc_coverage(root):
    total_symbols = 0
    doc_symbols = 0

    headers = []
    c_str_span_dir = os.path.join(root, "c_str_span")
    if os.path.exists(c_str_span_dir):
        for f in os.listdir(c_str_span_dir):
            if f.endswith(".h") and not re.search(r"_internal\.h|_private\.h|c_str_span_export\.h|c_str_span_stdint\.h|c_str_span_stdbool\.h", f):
                headers.append(os.path.join(c_str_span_dir, f))

    for header in headers:
        with open(header, "r", encoding="utf-8", errors="ignore") as f:
            content = f.read()

        symbols = 0
        for line in content.splitlines():
            if re.match(r"^(extern|typedef|#define (AZ_|az_))", line) and not re.search(r"_az_|extern \"C\"", line):
                symbols += 1
        total_symbols += symbols

        docs = content.count("/**")
        file_docs = len(re.findall(r"/\*\*|@file", content)) and content.count("@file")
        symbol_docs = max(0, docs - file_docs)
        doc_symbols += symbol_docs

    if total_symbols > 0:
        val = min(100, int(doc_symbols * 100 / total_symbols))
        return f"{val}%"
    return "100%"

def generate_badge(label, value, filename, root):
    val_str = re.sub(r"[^\d.]", "", value)
    val_num = float(val_str) if val_str else 0
    val_int = int(val_num)

    color = "#97ca00" # green
    if val_int < 50:
        color = "#e05d44" # red
    elif val_int < 80:
        color = "#dfb317" # yellow

    label_width = 95
    value_width = 45
    if len(value) > 4:
        value_width = 55

    total_width = label_width + value_width
    label_x = label_width * 5
    value_x = label_width * 10 + value_width * 5

    svg = f"""<svg xmlns="http://www.w3.org/2000/svg" width="{total_width}" height="20" role="img" aria-label="{label}: {value}">
  <title>{label}: {value}</title>
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
    <text aria-hidden="true" x="{value_x}" y="150" fill="#010101" fill-opacity=".3" transform="scale(.1)" textLength="{value_width * 10 - 100}">{value}</text>
    <text x="{value_x}" y="140" transform="scale(.1)" fill="#fff" textLength="{value_width * 10 - 100}">{value}</text>
  </g>
</svg>
"""
    reports_dir = os.path.join(root, "reports")
    os.makedirs(reports_dir, exist_ok=True)
    with open(os.path.join(reports_dir, filename), "w", encoding="utf-8") as f:
        f.write(svg)

def main():
    root = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
    build_dirs = find_build_dirs(root)
    best_build_dir = get_best_build_dir(build_dirs)

    test_cov = get_test_coverage(best_build_dir)
    doc_cov = get_doc_coverage(root)

    generate_badge("test coverage", test_cov, "test_coverage.svg", root)
    generate_badge("doc coverage", doc_cov, "doc_coverage.svg", root)

if __name__ == "__main__":
    main()
