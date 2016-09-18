include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(Bison)
set(LLVM_TARGET_PARSER UCPSInstrParser.y)
bison(LLVM UCPSGenInstrParser.bison)
add_public_bison_target(UCPSInstrParser)

include(Flex)
set(LLVM_TARGET_LEXER UCPSInstrFLexer.l)
flex(LLVM UCPSGenInstrFLexer.flex)
add_public_flex_target(UCPSInstrFLexer)

add_llvm_library(LLVMUCPSAsmParser
  UCPSAsmParser.cpp
  UCPSScheduler.cpp
  MCFunction.cpp
  )

add_dependencies(LLVMUCPSAsmParser UCPSCommonTableGen UCPSInstrParser 
UCPSInstrFLexer)
