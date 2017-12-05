```diff
--- ../../llvm-3.8.0-r267675//lib/Target/LLVMBuild.txt	2017-12-02 11:28:52.002019818 +0800
+++ /home/xiesl/Downloads/llvm-r267675/lib/Target/LLVMBuild.txt	2017-11-30 08:40:47.157453534 +0800
@@ -36,10 +36,6 @@
  WebAssembly
  X86
  XCore
- MSPU
- MMPULite
- UCPM
-; ARCH_MAPU
 
 ; This is a special group whose required libraries are extended (by llvm-build)
 ; with the best execution engine (the native JIT, if available, or the
```
