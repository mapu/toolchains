# LLVM source code structure
The MaPU assembler is based on LLVM Machine Code layer, which is LLVM MC in short. 
Current assembler version is llvm-3.8.0-r267675.
You can checkout the original llvm source code with following command:
```bash
svn co http://llvm.org/svn/llvm-project/llvm/trunk -r 267675 llvm-r267675
```
Besides the llvm itself, the MaPU tool chain also rely on other tools as shown in following table:

|   tools   |   version    |   source code  | description                       | dir in LLVM |
|-----------|--------------|----------------|-------------                      |-------------|
|  clang    |   3.9.0      |                |The C/C++ frontend                 |$LLVM/tools  |
|  lldb     |              |                |The Debugger                       |$LLVM/tools  |
|compiler-rt|              |                |The runtime library for the compiler|$LLVM/projects |
