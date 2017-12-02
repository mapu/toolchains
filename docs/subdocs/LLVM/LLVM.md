# Porting LLVM 
Current toolchain depends on llvm, the version is llvm-3.8.0-r267675.
You can checkout the original llvm source code with following command:
```bash
svn co http://llvm.org/svn/llvm-project/llvm/trunk -r 267675 llvm-r267675
```
## Depending libraries
Besides the llvm itself, the MaPU tool chain also rely on other tools as shown in following table:

|   tools   |   version    | description                        | dir in LLVM |
|-----------|--------------|----------------------------------- |-------------|
|  clang    |3.9.0-r267675 |The C/C++ frontend                  |$LLVM/tools  |
|  lldb     |      r267675 |The Debugger                        |$LLVM/tools  |
|compiler-rt|      r267675 |The runtime library for the compiler|$LLVM/projects |

The above depending libraries can be checkout with following command:
```bash
svn co http://llvm.org/svn/llvm-project/cfe/trunk  -r 267675 clang-r267675
svn co http://llvm.org/svn/llvm-project/lldb/trunk -r 267675 lldb-r267675
svn co http://llvm.org/svn/llvm-project/compiler-rt/trunk -r 267675 compiler-rt-r267675
```
