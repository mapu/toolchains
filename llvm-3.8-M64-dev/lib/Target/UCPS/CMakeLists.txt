set(LLVM_TARGET_DEFINITIONS UCPS.td)

tablegen(LLVM UCPSGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM UCPSGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM UCPSGenDisassemblerTables.inc -gen-disassembler)

tablegen(LLVM UCPSGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM UCPSGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM UCPSGenSubtargetInfo.inc -gen-subtarget)

tablegen(LLVM UCPSGenDFAPacketizer.inc -gen-dfa-packetizer)
add_public_tablegen_target(UCPSCommonTableGen)


add_llvm_target(UCPSCodeGen
		UCPSSubtarget.cpp
)

add_dependencies(LLVMUCPSCodeGen intrinsics_gen)

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
