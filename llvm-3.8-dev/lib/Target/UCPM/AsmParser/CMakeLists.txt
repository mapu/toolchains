include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. )

include(Bison)
set(LLVM_TARGET_PARSER UCPMInstrParser.y)
bison(LLVM UCPMGenInstrParser.bison)
add_public_bison_target(UCPMInstrParser)

include(Flex)
set(LLVM_TARGET_LEXER UCPMInstrFLexer.l)
flex(LLVM UCPMGenInstrFLexer.flex)
add_public_flex_target(UCPMInstrFLexer)

add_llvm_library(LLVMUCPMAsmParser
  UCPMAsmParser.cpp
  UCPMScheduler.cpp
  MCFunction.cpp
  )

add_dependencies(LLVMUCPMAsmParser UCPMCommonTableGen UCPMInstrParser 
UCPMInstrFLexer)
