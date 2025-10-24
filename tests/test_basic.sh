#!/bin/bash

# Basic functionality test
# Usage: test_basic.sh <clang-tool-binary>

set -e

CLANG_TOOL="$1"
if [ -z "$CLANG_TOOL" ]; then
    echo "Usage: $0 <clang-tool-binary>"
    exit 1
fi

echo "=== Basic Functionality Test ==="

# Test with simple example
echo "Testing function call transformation..."
output=$("$CLANG_TOOL" ../examples/simple.cc -- -std=c++11)
echo "$output"

# Check if transformation occurred
if echo "$output" | grep -q "fn_print()"; then
    echo "✅ Basic transformation successful"
else
    echo "❌ Basic transformation failed"
    exit 1
fi

echo "=== Basic functionality test passed ==="