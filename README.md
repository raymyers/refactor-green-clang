# clang-tool

[![Build](https://github.com/raymyers/refactor-green-clang/actions/workflows/build.yml/badge.svg)](https://github.com/raymyers/refactor-green-clang/actions/workflows/build.yml)
[![CI](https://github.com/raymyers/refactor-green-clang/actions/workflows/ci.yml/badge.svg)](https://github.com/raymyers/refactor-green-clang/actions/workflows/ci.yml)

clang-tool is a powerful general-purpose rename utility built with Clang's LibTooling[1]. It can analyze C/C++ code to identify functions and variables, and perform precise semantic-aware renaming operations.

## Compatibility

- **Clang/LLVM**: Compatible with Clang 17, 18, and 19
- **Platforms**: Linux (Ubuntu 20.04+), macOS
- **Build System**: CMake 3.10+

## Installation
Install the necessary llvm and clang headers and libraries for you system an run:

```bash
git clone https://github.com/firolino/clang-tool
cd clang-tool
mkdir build
cd build
cmake ..
make
```

## Usage

### Code Analysis Mode (Default)
Run without rename options to analyze code and identify functions and variables:

```bash
bin/clang-tool examples/simple.cc -- -std=c++11
```

This will output:
- User-defined functions vs built-in functions
- All variables found in the code
- The formatted source code

### Rename Mode
Use command line options to perform targeted renames:

```bash
# Rename a function
bin/clang-tool --rename-from=old_function --rename-to=new_function --rename-type=function source.c -- -std=c89

# Rename a variable  
bin/clang-tool --rename-from=old_var --rename-to=new_var --rename-type=variable source.c -- -std=c89
```

### Command Line Options
- `--rename-from=<old_name>` - Name to rename from
- `--rename-to=<new_name>` - Name to rename to  
- `--rename-type=<type>` - Type of entity to rename (`function` or `variable`)

### Example: Refactoring Obfuscated Code
The included `donut_test.sh` script demonstrates how to use the tool to transform obfuscated code with single-character variable names into readable code:

```bash
./donut_test.sh
```

This script renames 22 variables in the famous donut.c, transforming cryptic names like `A`, `B`, `i`, `j` into meaningful names like `rotation_x`, `rotation_y`, `torus_angle`, `circle_angle`.

### Specifiying compiler arguments
There are multiple ways to provide your projects' compiler arguments to a clang tool:

* **Direct**

  As can be seen in the example above, compiler arguments can be placed directly after ```--```.

* **Compilation Database**

  If your project is CMake-based use ```-DCMAKE_EXPORT_COMPILE_COMMANDS=ON``` in your cmake call. It will generate a ```compile_commands.json``` in your build directory containing your compiler arguments etc. With the json file you don't need to provide ```--``` anymore. clang-tool will automatically use your json file.

* **Makefiles**

  If you have an ordinary Makefile based project, use rizsottos awesome tool from https://github.com/rizsotto/Bear and run ```bear make``` on a cleanded project directory. It will generate a ```compile_commands.json``` for you!

# TODOs
Inlude PPCallbacks

# References
[1] http://clang.llvm.org/docs/LibTooling.html

[2] http://clang.llvm.org/docs/LibASTMatchersReference.html

