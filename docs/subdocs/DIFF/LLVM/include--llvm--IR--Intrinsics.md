```diff
--- ../../llvm-3.8.0-r267675//include/llvm/IR/Intrinsics.td	2017-12-02 11:28:51.724019686 +0800
+++ /home/xiesl/Downloads/llvm-r267675/include/llvm/IR/Intrinsics.td	2017-11-30 08:41:16.397465869 +0800
@@ -686,6 +686,3 @@
 include "llvm/IR/IntrinsicsBPF.td"
 include "llvm/IR/IntrinsicsSystemZ.td"
 include "llvm/IR/IntrinsicsWebAssembly.td"
-// ARCH_MAPU MaPU intrinsics
-include "llvm/IR/IntrinsicsMSPU.td"
-// ARCH_MAPU MaPU intrinsics end
```
