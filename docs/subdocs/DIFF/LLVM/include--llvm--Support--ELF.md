```diff
--- ../../llvm-3.8.0-r267675//include/llvm/Support/ELF.h	2017-12-02 11:28:51.749019698 +0800
+++ /home/xiesl/Downloads/llvm-r267675/include/llvm/Support/ELF.h	2017-11-30 08:41:16.694465993 +0800
@@ -320,11 +320,6 @@
   // an official value for Lanai. As soon as one is allocated, this enum will be
   // updated to use it.
   EM_LANAI         = 0x8123, // Lanai 32-bit processor
-#ifdef ARCH_MAPU
-  EM_MSPU          = 0x288, // MaPU SPU
-  EM_MMPU          = 0x460, // MaPU MPU
-  EM_UCPM          = 0x461, // UCP MPU
-#endif
 };
 
 // Object file classes.
@@ -619,13 +614,6 @@
 #include "ELFRelocs/WebAssembly.def"
 };
 
-#ifdef ARCH_MAPU
-// ELF Relocation type for MaPU
-enum {
-#include "ELFRelocs/MaPU.def"
-};
-#endif
-
 #undef ELF_RELOC
 
 // Section header.
```
