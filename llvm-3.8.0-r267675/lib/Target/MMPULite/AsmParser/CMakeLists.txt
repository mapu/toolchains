include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(Bison)
set(LLVM_TARGET_PARSER MMPULiteInstrParser.y)
bison(LLVM MMPULiteGenInstrParser.bison)
add_public_bison_target(MMPULiteInstrParser)

include(Flex)
set(LLVM_TARGET_LEXER MMPULiteInstrFLexer.l)
flex(LLVM MMPULiteGenInstrFLexer.flex)
add_public_flex_target(MMPULiteInstrFLexer)

add_llvm_library(LLVMMMPULiteAsmParser
  MMPULiteAsmParser.cpp
  MMPULiteScheduler.cpp
  MCFunction.cpp
  )

add_dependencies(LLVMMMPULiteAsmParser MMPULiteCommonTableGen MMPULiteInstrParser 
MMPULiteInstrFLexer)
