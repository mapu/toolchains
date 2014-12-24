include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVMMMPUAsmParser
  MMPUAsmParser.cpp
  )

add_dependencies(LLVMMMPUAsmParser MMPUCommonTableGen)
