add_llvm_library(LLVMUCPSDesc
  UCPSAsmBackend.cpp
  UCPSELFObjectWriter.cpp
  UCPSMCAsmInfo.cpp
  UCPSMCCodeEmitter.cpp  
  UCPSMCTargetDesc.cpp
)

add_dependencies(LLVMUCPSDesc UCPSCommonTableGen)
