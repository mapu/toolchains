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
## Adding the new target
We need to add a new target represent in LLVM so the tool can recongnize our architecture. 
Through the toolchain, we name our architecture **mspu**, which means **m**aPU **s**calar **p**rocessing **u**nit.
We then can specify our target in the command line:
```bash
llvm-mc -target mspu
```
We need to modify following files to make the targe effective:
1. [include/llvm/ADT/Triple.h](../DIFF/LLVM/include--llvm--ADT--Triple.md): This is used for **A**utoconfig **D**etecting **T**riple. 
2. [lib/Support/Triple.cpp](../DIFF/LLVM/lib--Support--Triple.md): This is used for command line parsing target
3. [include/llvm/Support/ELF.h](../DIFF/LLVM/include--llvm--Support--ELF.md): Define the EM number for MaPU elf file.
4. [include/llvm/Object/ELFObjectFile.h](../DIFF/LLVM/include--llvm--Object--ELFObjectFile.md): Return the correct object file type according to the EM

[//]: # (4. [include/llvm/Support/ELFRelocs/MaPU.def](../DIFF/LLVM/include--llvm-Support--ELFRelocs--MaPU.md): Define the Relocation types in MSPU.)

After modifying these files, the LLVM should recognize **mspu** architecture and generates **mspu** ELF files. 
Next we will add simple assembly instructons to the LLVM.

[\<------ \[Table of Content\]]({{site.url}}/index) <span style="float:right">  [\[Assembler\] ------>](Assembler)  </span>
