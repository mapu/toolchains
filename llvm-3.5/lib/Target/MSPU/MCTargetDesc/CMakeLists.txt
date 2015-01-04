add_llvm_library(LLVMMSPUDesc
  MSPUAsmBackend.cpp
  MSPUELFObjectWriter.cpp
  MSPUMCAsmInfo.cpp
  MSPUMCCodeEmitter.cpp  
  MSPUMCTargetDesc.cpp
  MSPUMCInst.cpp
)

add_dependencies(LLVMMSPUDesc MSPUCommonTableGen)
