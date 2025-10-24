#!/bin/bash

# Line-specific renaming test
# Usage: test_line_specific.sh <clang-tool-binary>

set -e

CLANG_TOOL="$1"
if [ -z "$CLANG_TOOL" ]; then
    echo "Usage: $0 <clang-tool-binary>"
    exit 1
fi

echo "=== Line-Specific Renaming Test ==="

# Test 1: Line-specific variable renaming
echo "Testing line-specific variable renaming..."
cat > test_line_var.cc << 'EOF'
int globalVar = 1;
int main() {
    int globalVar = 2;
    return globalVar;
}
EOF

output=$("$CLANG_TOOL" rename globalVar:3 localVar test_line_var.cc -- -std=c++11)
echo "Line-specific variable rename output:"
echo "$output"

if echo "$output" | grep -q "int globalVar = 1" && echo "$output" | grep -q "int localVar = 2" && echo "$output" | grep -q "return localVar"; then
    echo "✅ Line-specific variable renaming successful"
else
    echo "❌ Line-specific variable renaming failed"
    exit 1
fi

# Test 2: Line-specific member variable renaming
echo "Testing line-specific member variable renaming..."
cat > test_line_member.cc << 'EOF'
class TestClass {
public:
    int memberData;
    void setData(int val) { memberData = val; }
    int getData() { return memberData; }
};
EOF

output=$("$CLANG_TOOL" rename memberData:3 dataField test_line_member.cc -- -std=c++11)
echo "Line-specific member variable rename output:"
echo "$output"

if echo "$output" | grep -q "int dataField;" && echo "$output" | grep -q "dataField = val" && echo "$output" | grep -q "return dataField"; then
    echo "✅ Line-specific member variable renaming successful"
else
    echo "❌ Line-specific member variable renaming failed"
    exit 1
fi

# Test 3: Line-specific function renaming
echo "Testing line-specific function renaming..."
cat > test_line_func.cc << 'EOF'
void testFunc() { return; }
void testFunc(int x) { return; }
int main() {
    testFunc();
    testFunc(42);
    return 0;
}
EOF

output=$("$CLANG_TOOL" rename testFunc:2 specificFunc test_line_func.cc -- -std=c++11)
echo "Line-specific function rename output:"
echo "$output"

if echo "$output" | grep -q "void specificFunc(int x)" && echo "$output" | grep -q "void testFunc()" && echo "$output" | grep -q "specificFunc(42)"; then
    echo "✅ Line-specific function renaming successful"
else
    echo "❌ Line-specific function renaming failed"
    exit 1
fi

# Cleanup
rm -f test_line_*.cc

echo "=== Line-specific renaming test passed ==="