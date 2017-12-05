```diff
--- ../../llvm-3.8.0-r267675//include/llvm/ADT/Triple.h	2017-12-04 13:27:01.083301459 +0800
+++ /home/xiesl/Downloads/llvm-r267675/include/llvm/ADT/Triple.h	2017-11-30 08:41:16.112465749 +0800
@@ -87,11 +87,6 @@
     kalimba,    // Kalimba: generic kalimba
     shave,      // SHAVE: Movidius vector VLIW processors
     lanai,      // Lanai: Lanai 32-bit
-#ifdef ARCH_MAPU
-    mspu,    // mspu: MaPU SPU
-    mmpulite,     // mmpu: MaPU MPU
-    ucpm,    // ucpm: UCP MPU
-#endif
     wasm32,     // WebAssembly with 32-bit pointers
     wasm64,     // WebAssembly with 64-bit pointers
     LastArchType = wasm64
@@ -120,11 +115,6 @@
     KalimbaSubArch_v3,
     KalimbaSubArch_v4,
     KalimbaSubArch_v5
-#ifdef ARCH_MAPU
-    ,mspu_v1    // MaPU v1
-    ,mmpu_v1    // MaPU v1
-    ,ucpm_v1    // UCP v1
-#endif
   };
   enum VendorType {
     UnknownVendor,
@@ -142,9 +132,6 @@
     CSR,
     Myriad,
     AMD,
-#ifdef ARCH_MAPU
-    CASIA,    // IA, CAS
-#endif
     Mesa,
     LastVendorType = Mesa
   };
```
