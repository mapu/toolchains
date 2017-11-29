add_llvm_library(LLVMMSPUAsmPrinter 
  MSPUInstPrinter.cpp
)

add_dependencies(LLVMMSPUAsmPrinter MSPUCommonTableGen)
