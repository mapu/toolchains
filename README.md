# Welcome to the MaPU toolchain!
This repostior contains the complete toolchain used for the first MaPU chip, including assembler, linker, compiler, simulator, debugger and necessary linux port and bootloader.

The toolchain is up to date, but we are adding the examples and translating the documentations.
## Rerequest

The toolchain is fully tested in CentOS 7.0.1, we can't guarantee other distributions yet.

## Quick start

### Compile
Clone this repository and execute the INSTALL.sh script will install all the toolchain. About 10 mins in Intel Xeon E5 server
```bash
./INSTALL.sh <install_dir> # The install_dir should be an absolute path.
```
### Setup the environment 
The MaPU toolchain relies on an environment variable *MAPU_HOME* which should point to the install directory. Before using the toolchain, we should setup this environment variable.
```bash
export MAPU_HOME= <install_dir>
```
For convenience, we can also add the executable directory to the system PATH 
```bash
export PATH=$PATH:$MAPU_HOME/apc/bin
export PATH=$PATH:$MAPU_HOME/simulator/apc
```
### Run the simple test
The *examples/1.Hello_MaPU* is the simplest case we can run. The source code is as following:
```c
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("--------------------------------\n");
  printf(">> Hello MaPU!\n");
  printf("--------------------------------\n");
  return  0;
}
```
Type *make* command in this directory will outputs followings as expected:
```bash
**** REAL SIMULATION ****
info: Entering event queue @ 0.  Starting simulation...
--------------------------------
>> Hello MaPU!
--------------------------------
hack: be nice to actually delete the event here
Exiting @ tick 21392000 because target called exit()
```
