# Test configuration
set(CLANG_TOOL_BINARY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/clang-tool)

# Basic functionality tests
add_test(NAME basic_functionality
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_basic.sh ${CLANG_TOOL_BINARY}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Rename functionality tests
add_test(NAME rename_functionality
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_rename.sh ${CLANG_TOOL_BINARY}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Scope-aware renaming tests
add_test(NAME scope_aware_renaming
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_scope_aware.sh ${CLANG_TOOL_BINARY}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# Line-specific renaming tests
add_test(NAME line_specific_renaming
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test_line_specific.sh ${CLANG_TOOL_BINARY}
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})