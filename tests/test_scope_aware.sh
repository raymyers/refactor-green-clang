#!/bin/bash

# Scope-aware renaming test
# Usage: test_scope_aware.sh <clang-tool-binary>

set -e

CLANG_TOOL="$1"
if [ -z "$CLANG_TOOL" ]; then
    echo "Usage: $0 <clang-tool-binary>"
    exit 1
fi

echo "=== Scope-Aware Renaming Test ==="

# Create test file with multiple scopes
cat > test_scope_enforcement.cc << 'EOF'
void function1() {
    int localVar = 10;
    int result = localVar + 1;
}
void function2() {
    int localVar = 20;
    int result = localVar + 2;
}
class TestClass {
public:
    int memberVar;
    void method() { memberVar = 100; }
};
class AnotherClass {
public:
    int memberVar;
    void method() { memberVar = 200; }
};
int globalVar = 42;
EOF

echo "Test 1: General rename of 'localVar' should only affect first occurrence"
echo "Expected: Only function1's localVar and its reference should be renamed"

output=$("$CLANG_TOOL" rename localVar firstLocal test_scope_enforcement.cc -- -std=c++11)
echo "Output:"
echo "$output"

if echo "$output" | grep -q "int firstLocal = 10" && \
   echo "$output" | grep -q "firstLocal + 1" && \
   echo "$output" | grep -q "int localVar = 20" && \
   echo "$output" | grep -q "localVar + 2"; then
    echo "✅ Test 1 PASSED: Only first localVar renamed"
else
    echo "❌ Test 1 FAILED: Scope-aware localVar renaming failed"
    exit 1
fi

echo "----------------------------------------"

echo "Test 2: General rename of 'memberVar' should only affect first class"
echo "Expected: Only TestClass::memberVar and its references should be renamed"

output=$("$CLANG_TOOL" rename memberVar firstMember test_scope_enforcement.cc -- -std=c++11)
echo "Output:"
echo "$output"

if echo "$output" | grep -q "int firstMember;" && \
   echo "$output" | grep -q "firstMember = 100" && \
   echo "$output" | grep -q "int memberVar;" && \
   echo "$output" | grep -q "memberVar = 200"; then
    echo "✅ Test 2 PASSED: Only first memberVar renamed"
else
    echo "❌ Test 2 FAILED: Scope-aware memberVar renaming failed"
    exit 1
fi

echo "----------------------------------------"

echo "Test 3: General rename of 'globalVar' should work (single occurrence)"

output=$("$CLANG_TOOL" rename globalVar renamedGlobal test_scope_enforcement.cc -- -std=c++11)
echo "Output:"
echo "$output"

if echo "$output" | grep -q "int renamedGlobal = 42"; then
    echo "✅ Test 3 PASSED: globalVar renamed correctly"
else
    echo "❌ Test 3 FAILED: globalVar renaming failed"
    exit 1
fi

# Cleanup
rm -f test_scope_enforcement.cc

echo "=== Scope-aware renaming test passed ==="