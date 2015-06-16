include_directories( ${CMAKE_CURRENT_BINARY_DIR}/.. 
                     ${CMAKE_CURRENT_SOURCE_DIR}/.. 
                     ${CMAKE_CURRENT_BINARY_DIR}/../../lib/Target/MMPULite
                     ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/Target/MMPULite)
set(LLVM_LINK_COMPONENTS native)

include(Bison)
set(LLVM_TARGET_PARSER MMPUFrontEndParser.y)
bison(MMPUCompiler MMPUGenFrontEndParser.bison)
add_public_bison_target(MMPUFrontEndParser)

include(Flex)
set(LLVM_TARGET_LEXER MMPUFrontEndLexer.l)
flex(MMPUCompiler MMPUGenFrontEndLexer.flex)
add_public_flex_target(MMPUFrontEndLexer)

add_llvm_tool(mmcc
  ../../lib/Target/MMPULite/AsmParser/MMPULiteScheduler.cpp
  ../../lib/Target/MMPULite/AsmParser/MCFunction.cpp
  MMPUDAG.cpp
  MMPUBlock.cpp
  MMPUSymbol.cpp
  MMPUAST.cpp
  MMPUIRGen.cpp
  MMPUCompiler.cpp
  main.cpp
  )

add_dependencies(mmcc MMPUFrontEndParser MMPUFrontEndLexer MMPULiteCommonTableGen)
