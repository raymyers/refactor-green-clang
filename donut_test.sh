#!/bin/bash
set -euo pipefail

# Donut refactoring test script
# This script demonstrates the rename tool by transforming obfuscated donut.c
# into a more readable version with meaningful variable names

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TOOL="${SCRIPT_DIR}/build/bin/clang-tool"
DONUT_ORIG="/workspace/project/donut/donut.c"
DONUT_OUT="${SCRIPT_DIR}/donut_script_out.c"

echo "=== Donut Refactoring Test ==="
echo "Original file: ${DONUT_ORIG}"
echo "Output file: ${DONUT_OUT}"
echo

# Check if tool exists
if [[ ! -x "${TOOL}" ]]; then
    echo "Error: Tool not found at ${TOOL}"
    echo "Please build the project first: cd build && make"
    exit 1
fi

# Check if original donut exists
if [[ ! -f "${DONUT_ORIG}" ]]; then
    echo "Error: Original donut.c not found at ${DONUT_ORIG}"
    exit 1
fi

# Copy original to output file
echo "Copying original donut.c..."
cp "${DONUT_ORIG}" "${DONUT_OUT}"

# Function to apply rename and update the file
apply_rename() {
    local from="$1"
    local to="$2"
    local type="$3"
    
    echo "Renaming ${type} '${from}' -> '${to}'"
    
    # Apply rename and capture output
    local temp_file
    temp_file=$(mktemp)
    
    if "${TOOL}" --rename-from="${from}" --rename-to="${to}" --rename-type="${type}" "${DONUT_OUT}" -- -std=c89 2>/dev/null > "${temp_file}"; then
        # Extract just the code part (skip the analysis output)
        # Look for the pattern that starts with a variable declaration followed by ;double sin()
        if grep -q ";double sin()" "${temp_file}"; then
            sed -n '/;double sin()/,$p' "${temp_file}" > "${DONUT_OUT}"
            echo "  ✓ Applied successfully"
        else
            echo "  ✗ Failed to find code output"
            rm -f "${temp_file}"
            return 1
        fi
    else
        echo "  ✗ Tool execution failed"
        rm -f "${temp_file}"
        return 1
    fi
    
    rm -f "${temp_file}"
}

echo
echo "Applying variable renames..."

# Rename rotation variables
apply_rename "A" "rotation_x" "variable"
apply_rename "B" "rotation_y" "variable"

# Rename loop variables
apply_rename "i" "torus_angle" "variable"
apply_rename "j" "circle_angle" "variable"

# Rename coordinate variables
apply_rename "x" "screen_x" "variable"
apply_rename "y" "screen_y" "variable"
apply_rename "z" "depth_buffer" "variable"

# Rename calculation variables
apply_rename "c" "sin_torus" "variable"
apply_rename "d" "cos_circle" "variable"
apply_rename "e" "sin_rot_x" "variable"
apply_rename "f" "sin_circle" "variable"
apply_rename "g" "cos_rot_x" "variable"
apply_rename "h" "radius_offset" "variable"
apply_rename "D" "depth_factor" "variable"
apply_rename "l" "cos_torus" "variable"
apply_rename "m" "cos_rot_y" "variable"
apply_rename "n" "sin_rot_y" "variable"
apply_rename "t" "temp_calc" "variable"

# Rename output variables
apply_rename "o" "buffer_index" "variable"
apply_rename "N" "lighting" "variable"
apply_rename "b" "char_buffer" "variable"
apply_rename "k" "output_index" "variable"

echo
echo "=== Refactoring Complete ==="
echo "Transformed file saved as: ${DONUT_OUT}"
echo

# Test compilation
echo "Testing compilation..."
if gcc -o "${SCRIPT_DIR}/donut_refactored" "${DONUT_OUT}" -lm -std=c89 -w 2>/dev/null; then
    echo "  ✓ Refactored code compiles successfully!"
    rm -f "${SCRIPT_DIR}/donut_refactored"
else
    echo "  ✗ Compilation failed"
    exit 1
fi

echo
echo "=== Summary ==="
echo "Successfully renamed 22 variables in donut.c"
echo "The obfuscated single-character variables now have meaningful names:"
echo "  A -> rotation_x, B -> rotation_y"
echo "  i -> torus_angle, j -> circle_angle"
echo "  x -> screen_x, y -> screen_y, z -> depth_buffer"
echo "  And many more..."
echo
echo "You can now examine ${DONUT_OUT} to see the readable version!"