add_llvm_library(LLVMUCPMDesc
  UCPMAsmBackend.cpp
  UCPMELFObjectWriter.cpp
  UCPMMCAsmInfo.cpp
  UCPMMCCodeEmitter.cpp  
  UCPMMCTargetDesc.cpp
  UCPMInsnLine.cpp
)

add_dependencies(LLVMUCPMDesc UCPMCommonTableGen)
