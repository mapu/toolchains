```diff
--- ../../llvm-3.8.0-r267675//include/llvm/Object/ELFObjectFile.h	2017-12-02 11:28:51.742019695 +0800
+++ /home/xiesl/Downloads/llvm-r267675/include/llvm/Object/ELFObjectFile.h	2017-11-30 08:41:15.517465498 +0800
@@ -838,14 +838,6 @@
       return "ELF32-wasm";
     case ELF::EM_AMDGPU:
       return "ELF32-amdgpu";
-#ifdef ARCH_MAPU
-    case ELF::EM_MSPU:
-      return "ELF32-mspu";
-    case ELF::EM_MMPU:
-      return "ELF32-mmpulite";
-    case ELF::EM_UCPM:
-      return "ELF32-ucpm";
-#endif
     default:
       return "ELF32-unknown";
     }
```
