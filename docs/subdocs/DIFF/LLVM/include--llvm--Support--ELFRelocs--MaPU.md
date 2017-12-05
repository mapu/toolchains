```diff
--- ../../llvm-3.8.0-r267675//include/llvm/Support/ELFRelocs/MaPU.def	2017-12-02 11:28:51.750019698 +0800
+++ /home/xiesl/Downloads/llvm-r267675/include/llvm/Support/ELFRelocs/MaPU.def	1970-01-01 08:00:00.000000000 +0800
@@ -1,28 +0,0 @@
-
-#ifndef ELF_RELOC
-#error "ELF_RELOC must be defined"
-#endif
-
-ELF_RELOC(R_MAPU_None,         0)
-
-// MSPU relocations
-ELF_RELOC(R_MSPU_PC17,         1)   // PC-relative address, bits 21-5
-ELF_RELOC(R_MSPU_PC28,         2)   // PC-relative address, bits 27-0
-ELF_RELOC(R_MSPU_11_B5,        3)   // 11 bits relo, bits 15-5
-ELF_RELOC(R_MSPU_11_B10,       4)   // 11 bits relo, bits 20-10
-ELF_RELOC(R_MSPU_17,           5)   // PC-irrelative address, bits 21-5
-ELF_RELOC(R_MSPU_28,           6)   // 28-bit extension, at bit 27-0
-// for data sections
-// pc relative
-ELF_RELOC(R_MSPU_PC_8,         7)   // 1 byte, 8 bits
-ELF_RELOC(R_MSPU_PC_16,        8)   // 2 bytes, 16 bits
-ELF_RELOC(R_MSPU_PC_32,        9)   // 32 bits
-ELF_RELOC(R_MSPU_PC_64,        10)  // 64 bits
-// pc irrelative
-ELF_RELOC(R_MSPU_8,            11)  // 1 byte, 8 bits
-ELF_RELOC(R_MSPU_16,           12)  // 2 bytes, 16 bits
-ELF_RELOC(R_MSPU_32,           13)  // 32 bits
-ELF_RELOC(R_MSPU_64,           14)  // 64 bits
-
-// MMPU relocations
-ELF_RELOC(R_MMPU_11,           1)  // PC-irrelative address
```
