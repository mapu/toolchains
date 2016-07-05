include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(Bison)
set(LLVM_TARGET_PARSER MSPUInstrParser.y)
bison(LLVM MSPUGenInstrParser.bison)
add_public_bison_target(MSPUInstrParser)

include(Ragel)
set(LLVM_TARGET_LEXER MSPUInstrRLexer.rl)
ragel(LLVM MSPUGenInstrRLexer.ragel)
add_public_ragel_target(MSPUInstrRLexer)

add_llvm_library(LLVMMSPUAsmParser
  MSPUAsmParser.cpp
  )

add_dependencies(LLVMMSPUAsmParser MSPUCommonTableGen MSPUInstrParser 
MSPUInstrRLexer)
