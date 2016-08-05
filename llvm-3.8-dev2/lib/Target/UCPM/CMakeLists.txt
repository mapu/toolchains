set(LLVM_TARGET_DEFINITIONS UCPM.td)

tablegen(LLVM UCPMGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM UCPMGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM UCPMGenDisassemblerTables.inc -gen-disassembler)

tablegen(LLVM UCPMGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM UCPMGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM UCPMGenSubtargetInfo.inc -gen-subtarget)

tablegen(LLVM UCPMGenDFAPacketizer.inc -gen-dfa-packetizer)
add_public_tablegen_target(UCPMCommonTableGen)


add_llvm_target(UCPMCodeGen
		UCPMSubtarget.cpp
)

add_dependencies(LLVMUCPMCodeGen intrinsics_gen)

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
