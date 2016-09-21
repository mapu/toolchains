include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(Bison)
set(LLVM_TARGET_PARSER UCPSInstrParser.y)
bison(LLVM UCPSGenInstrParser.bison)
add_public_bison_target(UCPSInstrParser)

include(Ragel)
set(LLVM_TARGET_LEXER UCPSInstrRLexer.rl)
ragel(LLVM UCPSGenInstrRLexer.ragel)
add_public_ragel_target(UCPSInstrRLexer)

add_llvm_library(LLVMUCPSAsmParser
  UCPSAsmParser.cpp
  )

add_dependencies(LLVMUCPSAsmParser UCPSCommonTableGen UCPSInstrParser 
UCPSInstrRLexer)
