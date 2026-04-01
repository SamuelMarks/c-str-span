#!/bin/bash

# shellcheck disable=SC2236
if [ ! -z "${BASH_VERSION+x}" ]; then
  # shellcheck disable=SC3028 disable=SC3054
  this_file="${BASH_SOURCE[0]}"
  # shellcheck disable=SC3040
  set -o pipefail
elif [ ! -z "${ZSH_VERSION+x}" ]; then
  # shellcheck disable=SC2296
  this_file="${(%):-%x}"
  # shellcheck disable=SC3040
  set -o pipefail
else
  this_file="${0}"
fi
ROOT="$( cd "$( dirname -- "${this_file}" )" && pwd )"

# 1. Test Coverage
BUILD_DIRS=($(find "${ROOT}" -maxdepth 2 -type d \( -name "build*" -o -name "cmake-build-*" \)))
BEST_BUILD_DIR=""
MAX_GCNO=0

for dir in "${BUILD_DIRS[@]}"; do
    if [ -f "${dir}/CTestTestfile.cmake" ]; then
        GCNO_COUNT=$(find "${dir}" -name "*.gcno" | wc -l)
        if [ "${GCNO_COUNT}" -gt "${MAX_GCNO}" ]; then
            MAX_GCNO="${GCNO_COUNT}"
            BEST_BUILD_DIR="${dir}"
        fi
    fi
done

BUILD_DIR="${BEST_BUILD_DIR}"
# Fallback to any build dir if no .gcno found
if [ -z "${BUILD_DIR}" ]; then
    for dir in "${BUILD_DIRS[@]}"; do
        if [ -f "${dir}/CTestTestfile.cmake" ]; then
            BUILD_DIR="${dir}"
            break
        fi
    done
fi

TEST_COVERAGE="0%"
if [ -n "${BUILD_DIR}" ]; then
    # Try to get coverage. If no .gcda files, run tests first.
    if ! find "${BUILD_DIR}" -name "*.gcda" | grep -q "."; then
        (cd "${BUILD_DIR}" && ctest -C 'Debug' 2>/dev/null)
    fi
    COVERAGE_OUTPUT="$(cd "${BUILD_DIR}" && ctest -C 'Debug' -T 'Coverage' 2>/dev/null)"
    PCT=$(echo "${COVERAGE_OUTPUT}" | grep -E "Percentage Coverage:|Total Coverage:" | grep -oE "[0-9]+(\.[0-9]+)?%" | head -n 1)
    if [ -n "${PCT}" ]; then
        TEST_COVERAGE="${PCT}"
    else
        PCT_VAL=$(echo "${COVERAGE_OUTPUT}" | awk '/Percentage Coverage:/ {print $NF} /Total Coverage:/ {print $NF}')
        if [[ "${PCT_VAL}" =~ [0-9] ]]; then
            if [[ ! "${PCT_VAL}" =~ %$ ]]; then PCT_VAL="${PCT_VAL}%"; fi
            TEST_COVERAGE="${PCT_VAL}"
        fi
    fi
fi

# 2. Doc Coverage
PUBLIC_HEADERS=($(ls "${ROOT}/c_str_span/"*.h | grep -vE "_internal.h|_private.h|c_str_span_export.h|c_str_span_stdint.h|c_str_span_stdbool.h"))
TOTAL_SYMBOLS=0
DOC_SYMBOLS=0

for header in "${PUBLIC_HEADERS[@]}"; do
    # Heuristic for public symbols:
    # - Function declarations (extern ... az_...)
    # - Macro definitions (#define AZ_... or #define az_...)
    # - Typedefs (typedef ... az_... or typedef struct ... az_...)
    # Excluding internal ones starting with _az_
    
    SYMBOLS=$(grep -E "^(extern|typedef|#define (AZ_|az_))" "${header}" | grep -vE "_az_|extern \"C\"" | wc -l)
    TOTAL_SYMBOLS=$((TOTAL_SYMBOLS + SYMBOLS))
    
    # Heuristic for documented symbols: count how many of these symbols have a /** block above them.
    # This is hard with grep, so we'll just use the /** vs symbols ratio as a proxy if it's less than 1.
    DOCS=$(grep -c "/\*\*" "${header}")
    FILE_DOCS=$(grep -E "/\*\*|@file" "${header}" | grep -c "@file")
    SYMBOL_DOCS=$((DOCS - FILE_DOCS))
    if [ "${SYMBOL_DOCS}" -lt 0 ]; then SYMBOL_DOCS=0; fi
    DOC_SYMBOLS=$((DOC_SYMBOLS + SYMBOL_DOCS))
done

if [ "${TOTAL_SYMBOLS}" -gt 0 ]; then
    if [ "${DOC_SYMBOLS}" -gt "${TOTAL_SYMBOLS}" ]; then
        DOC_COVERAGE_VAL=100
    else
        DOC_COVERAGE_VAL=$((DOC_SYMBOLS * 100 / TOTAL_SYMBOLS))
    fi
    DOC_COVERAGE="${DOC_COVERAGE_VAL}%"
else
    DOC_COVERAGE="100%"
fi

mkdir -p "${ROOT}/reports"

# Template for badge
generate_badge() {
    local label=$1
    local value=$2
    local filename=$3
    local color="#97ca00" # green
    
    # Simple color heuristic
    local val_num=$(echo "${value}" | tr -dc '0-9.')
    if [ -z "${val_num}" ]; then val_num=0; fi
    
    local val_int=${val_num%.*}
    if [ -z "${val_int}" ]; then val_int=0; fi
    
    if [ "${val_int}" -lt 50 ]; then
        color="#e05d44" # red
    elif [ "${val_int}" -lt 80 ]; then
        color="#dfb317" # yellow
    fi

    local label_width=95 # Increased for "test coverage"
    local value_width=45
    if [ "${#value}" -gt 4 ]; then value_width=55; fi
    
    local total_width=$((label_width + value_width))
    local label_x=$((label_width * 5))
    local value_x=$((label_width * 10 + value_width * 5))

    cat <<EOF > "${ROOT}/reports/${filename}"
<svg xmlns="http://www.w3.org/2000/svg" width="${total_width}" height="20" role="img" aria-label="${label}: ${value}">
  <title>${label}: ${value}</title>
  <linearGradient id="s" x2="0" y2="100%">
    <stop offset="0" stop-color="#bbb" stop-opacity=".1"/>
    <stop offset="1" stop-opacity=".1"/>
  </linearGradient>
  <clipPath id="r">
    <rect width="${total_width}" height="20" rx="3" fill="#fff"/>
  </clipPath>
  <g clip-path="url(#r)">
    <rect width="${label_width}" height="20" fill="#555"/>
    <rect x="${label_width}" width="${value_width}" height="20" fill="${color}"/>
    <rect width="${total_width}" height="20" fill="url(#s)"/>
  </g>
  <g fill="#fff" text-anchor="middle" font-family="Verdana,Geneva,DejaVu Sans,sans-serif" text-rendering="geometricPrecision" font-size="110">
    <text aria-hidden="true" x="${label_x}" y="150" fill="#010101" fill-opacity=".3" transform="scale(.1)" textLength="$((label_width * 10 - 100))">${label}</text>
    <text x="${label_x}" y="140" transform="scale(.1)" fill="#fff" textLength="$((label_width * 10 - 100))">${label}</text>
    <text aria-hidden="true" x="${value_x}" y="150" fill="#010101" fill-opacity=".3" transform="scale(.1)" textLength="$((value_width * 10 - 100))">${value}</text>
    <text x="${value_x}" y="140" transform="scale(.1)" fill="#fff" textLength="$((value_width * 10 - 100))">${value}</text>
  </g>
</svg>
EOF
}

generate_badge "test coverage" "${TEST_COVERAGE}" "test_coverage.svg"
generate_badge "doc coverage" "${DOC_COVERAGE}" "doc_coverage.svg"
