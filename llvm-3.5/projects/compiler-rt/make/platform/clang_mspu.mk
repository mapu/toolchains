Description := Static runtime libraries for clang/MSPU.

###

CC := clang
Arch := unknown
Configs :=

Configs += builtins-mspu
Arch.builtins-mspu := mspu

CFLAGS := -Wall -Werror -O3 -fomit-frame-pointer -target mspu -DARCH_MAPU
SANITIZER_CFLAGS := -fPIE -fno-builtin -gline-tables-only
CFLAGS.builtins-mspu := -target mspu -O3 -Wall -Werror -DARCH_MAPU

FUNCTIONS.builtins-mspu := $(CommonFunctions) $(ArchFunctions.mspu)
