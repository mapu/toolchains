add_llvm_library(LLVMUCPSAsmPrinter 
  UCPSInstPrinter.cpp
)

add_dependencies(LLVMUCPSAsmPrinter UCPSCommonTableGen)
