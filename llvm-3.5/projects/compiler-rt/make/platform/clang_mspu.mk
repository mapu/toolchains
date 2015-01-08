Description := Static runtime libraries for clang/MSPU.

###

CC := clang
Arch := unknown
Configs :=

Configs += builtins-mspu
Arch.builtins-mspu := mspu

CFLAGS := -Wall -Werror -O3 -fomit-frame-pointer -target mspu
SANITIZER_CFLAGS := -fPIE -fno-builtin -gline-tables-only
CFLAGS.builtins-mspu := -target mspu -Wall -Werror -O0 -g

FUNCTIONS.builtins-mspu := $(CommonFunctions) $(ArchFunctions.mspu)
