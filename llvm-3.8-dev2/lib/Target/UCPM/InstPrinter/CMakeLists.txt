include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. ${CMAKE_CURRENT_SOURCE_DIR}/.. )
add_llvm_library(LLVMUCPMAsmPrinter 
  UCPMInstPrinter.cpp
)

add_dependencies(LLVMUCPMAsmPrinter UCPMCommonTableGen)
