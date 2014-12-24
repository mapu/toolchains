add_llvm_library(LLVMMMPUAsmPrinter 
  MMPUInstPrinter.cpp
)

add_dependencies(LLVMMMPUInstPrinter MMPUCommonTableGen)
