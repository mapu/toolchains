```diff
--- ../../llvm-3.8.0-r267675//lib/Support/Triple.cpp	2017-12-05 09:45:08.903706106 +0800
+++ /home/xiesl/Downloads/llvm-r267675/lib/Support/Triple.cpp	2017-11-30 08:40:53.406456172 +0800
@@ -64,11 +64,6 @@
   case shave:       return "shave";
   case wasm32:      return "wasm32";
   case wasm64:      return "wasm64";
-#ifdef ARCH_MAPU
-  case mspu:        return "mspu";
-  case mmpulite:    return "mmpulite";
-  case ucpm:        return "ucpm";
-#endif
   }
 
   llvm_unreachable("Invalid ArchType!");
@@ -136,11 +131,6 @@
   case shave:       return "shave";
   case wasm32:
   case wasm64:      return "wasm";
-#ifdef ARCH_MAPU
-  case mspu:        return "mspu";
-  case mmpulite:    return "mmpulite";
-  case ucpm:        return "ucpm";
-#endif
   }
 }
 
@@ -162,9 +152,6 @@
   case Myriad: return "myriad";
   case AMD: return "amd";
   case Mesa: return "mesa";
-#ifdef ARCH_MAPU
-  case CASIA: return "casia";
-#endif
   }
 
   llvm_unreachable("Invalid VendorType!");
@@ -290,11 +277,6 @@
     .Case("shave", shave)
     .Case("wasm32", wasm32)
     .Case("wasm64", wasm64)
-#ifdef ARCH_MAPU
-    .Case("mspu", mspu)
-    .Case("mmpulite", mmpulite)
-    .Case("ucpm", ucpm)
-#endif
     .Default(UnknownArch);
 }
 
@@ -404,11 +386,6 @@
     .Case("shave", Triple::shave)
     .Case("wasm32", Triple::wasm32)
     .Case("wasm64", Triple::wasm64)
-#ifdef ARCH_MAPU
-    .Case("mspu", Triple::mspu)
-    .Case("mmpulite", Triple::mmpulite)
-    .Case("ucpm", Triple::ucpm)
-#endif
     .Default(Triple::UnknownArch);
 
   // Some architectures require special parsing logic just to compute the
@@ -440,9 +417,6 @@
     .Case("myriad", Triple::Myriad)
     .Case("amd", Triple::AMD)
     .Case("mesa", Triple::Mesa)
-#ifdef ARCH_MAPU
-    .Case("casia", Triple::CASIA)
-#endif
     .Default(Triple::UnknownVendor);
 }
 
@@ -515,11 +489,6 @@
       .EndsWith("kalimba3", Triple::KalimbaSubArch_v3)
       .EndsWith("kalimba4", Triple::KalimbaSubArch_v4)
       .EndsWith("kalimba5", Triple::KalimbaSubArch_v5)
-#ifdef ARCH_MAPU
-      .EndsWith("mspu_v1", Triple::mspu_v1)
-      .EndsWith("mmpu_v1", Triple::mmpu_v1)
-      .EndsWith("ucpm_v1", Triple::ucpm_v1)
-#endif
       .Default(Triple::NoSubArch);
 
   // ARM sub arch.
@@ -631,11 +600,6 @@
   case Triple::wasm32:
   case Triple::wasm64:
   case Triple::xcore:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     return Triple::ELF;
 
   case Triple::ppc:
@@ -1165,11 +1129,6 @@
   case llvm::Triple::lanai:
   case llvm::Triple::shave:
   case llvm::Triple::wasm32:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     return 32;
 
   case llvm::Triple::aarch64:
@@ -1244,11 +1203,6 @@
   case Triple::lanai:
   case Triple::shave:
   case Triple::wasm32:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     // Already 32-bit.
     break;
 
@@ -1283,11 +1237,6 @@
   case Triple::xcore:
   case Triple::sparcel:
   case Triple::shave:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     T.setArch(UnknownArch);
     break;
 
@@ -1362,11 +1311,6 @@
   // drop any arch suffixes.
   case Triple::arm:
   case Triple::thumb:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     T.setArch(UnknownArch);
     break;
 
@@ -1443,11 +1387,6 @@
   case Triple::x86:
   case Triple::x86_64:
   case Triple::xcore:
-#ifdef ARCH_MAPU
-  case Triple::mspu:
-  case Triple::mmpulite:
-  case Triple::ucpm:
-#endif
     // Already little endian.
     break;
 
```
