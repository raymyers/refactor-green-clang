#!/bin/bash

# Rename functionality test
# Usage: test_rename.sh <clang-tool-binary>

set -e

CLANG_TOOL="$1"
if [ -z "$CLANG_TOOL" ]; then
    echo "Usage: $0 <clang-tool-binary>"
    exit 1
fi

echo "=== Rename Functionality Test ==="

# Test 1: Variable renaming
echo "Testing variable renaming..."
cat > test_rename_var.cc << 'EOF'
int oldVariable = 42;
int main() {
    return oldVariable;
}
EOF

output=$("$CLANG_TOOL" rename oldVariable newVariable test_rename_var.cc -- -std=c++11)
echo "Variable rename output:"
echo "$output"

if echo "$output" | grep -q "int newVariable = 42" && echo "$output" | grep -q "return newVariable"; then
    echo "✅ Variable renaming successful"
else
    echo "❌ Variable renaming failed"
    exit 1
fi

# Test 2: Function renaming
echo "Testing function renaming..."
cat > test_rename_func.cc << 'EOF'
void oldFunction() {}
int main() {
    oldFunction();
    return 0;
}
EOF

output=$("$CLANG_TOOL" rename oldFunction newFunction test_rename_func.cc -- -std=c++11)
echo "Function rename output:"
echo "$output"

if echo "$output" | grep -q "void newFunction()" && echo "$output" | grep -q "newFunction()"; then
    echo "✅ Function renaming successful"
else
    echo "❌ Function renaming failed"
    exit 1
fi

# Test 3: Parameter renaming
echo "Testing parameter renaming..."
cat > test_rename_param.cc << 'EOF'
int func(int oldParam) {
    return oldParam * 2;
}
EOF

output=$("$CLANG_TOOL" rename oldParam newParam test_rename_param.cc -- -std=c++11)
echo "Parameter rename output:"
echo "$output"

if echo "$output" | grep -q "int func(int newParam)" && echo "$output" | grep -q "return newParam"; then
    echo "✅ Parameter renaming successful"
else
    echo "❌ Parameter renaming failed"
    exit 1
fi

# Test 4: Member variable renaming
echo "Testing member variable renaming..."
cat > test_rename_member.cc << 'EOF'
class TestClass {
public:
    int memberVar;
    void setVar(int val) { memberVar = val; }
    int getVar() { return memberVar; }
};
EOF

output=$("$CLANG_TOOL" rename memberVar dataField test_rename_member.cc -- -std=c++11)
echo "Member variable rename output:"
echo "$output"

if echo "$output" | grep -q "int dataField;" && echo "$output" | grep -q "dataField = val" && echo "$output" | grep -q "return dataField"; then
    echo "✅ Member variable renaming successful"
else
    echo "❌ Member variable renaming failed"
    exit 1
fi

# Cleanup
rm -f test_rename_*.cc

echo "=== Rename functionality test passed ==="