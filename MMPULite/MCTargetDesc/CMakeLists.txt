add_llvm_library(LLVMMMPULiteDesc
  MMPULiteAsmBackend.cpp
  MMPULiteELFObjectWriter.cpp
  MMPULiteMCAsmInfo.cpp
  MMPULiteMCCodeEmitter.cpp  
  MMPULiteMCTargetDesc.cpp
  MMPULiteInsnLine.cpp
  MMPULiteELFRelocationInfo.cpp
  MMPULiteTargetStreamer.cpp
)

add_dependencies(LLVMMMPULiteDesc MMPULiteCommonTableGen)
