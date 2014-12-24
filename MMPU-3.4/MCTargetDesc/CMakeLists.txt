add_llvm_library(LLVMMMPUDesc
  MMPUAsmBackend.cpp
  MMPUELFObjectWriter.cpp
  MMPUMCAsmInfo.cpp
  MMPUMCCodeEmitter.cpp  
  MMPUMCTargetDesc.cpp
)

add_dependencies(LLVMMMPUDesc MMPUCommonTableGen)
