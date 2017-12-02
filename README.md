# Welcome to the MaPU toolchain!
This repository contains the complete toolchain used for the first MaPU chip, including assembler, linker, compiler, simulator, debugger and necessary linux port and bootloader.

The toolchain is stable now, but changes may be committed for adding new examples and translating the documentations.
## Requirement

The toolchain is fully tested on CentOS 7.0.1, and we are short of hands to test it on other Linux distributions.

## Quick start

### Compilation
Clone this repository and execute the INSTALL.sh script to install all the packages. It takes about 10 mins on an Intel Xeon E5 server.
```bash
./INSTALL.sh <install_dir> # The install_dir should be an absolute path.
```
### Environment Setup
The MaPU toolchain relies on an environment variable *MAPU_HOME* which should point to the install directory. Before using the toolchain, this environment variable should be set.
```bash
export MAPU_HOME= <install_dir>
```
For convenience, you can add the executable directory to the system PATH 
```bash
export PATH=$PATH:$MAPU_HOME/apc/bin
export PATH=$PATH:$MAPU_HOME/simulator/apc
```
### Installation Verification
After setting up the environment, the toolchain should be good to go. There is a simple case *examples/1.Hello_MaPU* under the source tree which can verify the installation. The source code is as following:
```c
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("--------------------------------\n");
  printf(">> Hello MaPU!\n");
  printf("--------------------------------\n");
  return  0;
}
```
Type *make* command in this directory will output followings as expected:
```bash
**** REAL SIMULATION ****
info: Entering event queue @ 0.  Starting simulation...
--------------------------------
>> Hello MaPU!
--------------------------------
hack: be nice to actually delete the event here
Exiting @ tick 21392000 because target called exit()
```
# The repository structure
|  files        | short descriptions        | document              | 
|  -------------|---------------------------|-----------------------|
|  INSTALL.sh   |A script to compile and install all MaPU software, including simulator, compiler, liner, etc.|                       |
|  MaPUSim      |Source code for simulator |                       |
|  README.md    |README                     |                       |
|  deplibs      |                           |                       |
|  docs         |                           |                       |
|  examples     |                           |                       |
|  gold-2.25    |Linker                     |                       |
|  llvm-3.8     |Toolchains including compiler and assembler| docs/subdocs/LLVM/LLVM.md           |
|  newlib       |                           |                       |
|  openocd-0.5  |                           |                       |
|  ragel-6.8    |                           |                       |

# TODO ( please add anything you want to do )
1. A simplified toolchain version that only supports SPU
2. Translate the SPU instruciton set architecture
3. Detailed explaination of what has changed to the original code of following tools.
    1. The assembler/disassembler
    2. The linker
    3. The compiler
    4. The gem5 simulator
    5. The newlib
    6. The openocd 
    7. Other related tools.
4. Maybe update to the latest version
5. Rock the architecture ..
