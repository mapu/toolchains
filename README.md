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
|  files                                | short descriptions        | document              | 
|  -------------------------------------|---------------------------|-----------------------|
|  HowToUseSimulatorGUI.txt             |                           |                       |
|  INSTALL.sh                           |                           |                       |
|  MaPUSim                              |                           |                       |
|  README.md                            |                           |                       |
|  arm-none-eabi                        |                           |                       |
|  asm-gui                              |                           |                       |
|  benchmarks                           |                           |                       |
|  boot_rom                             |                           |                       |
|  clr_shm.sh                           |                           |                       |
|  deplibs                              |                           |                       |
|  docs                                 |                           |                       |
|  examples                             |                           |                       |
|  gen_image.sh                         |                           |                       |
|  gold-2.25                            |                           |                       |
|  kernel                               |                           |                       |
|  llvm-3.8                             |                           |                       |
|  newlib                               |                           |                       |
|  openocd-0.5                          |                           |                       |
|  pocl                                 |                           |                       |
|  python-nose-1.3.7-1.el7.noarch.rpm   |                           |                       |
|  qtermwidget-master                   |                           |                       |
|  ragel-6.8                            |                           |                       |
|  u-boot                               |                           |                       |
