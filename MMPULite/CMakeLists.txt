set(LLVM_TARGET_DEFINITIONS MMPULite.td)

tablegen(LLVM MMPULiteGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MMPULiteGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MMPULiteGenDisassemblerTables.inc -gen-disassembler)

tablegen(LLVM MMPULiteGenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM MMPULiteGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MMPULiteGenSubtargetInfo.inc -gen-subtarget)

tablegen(LLVM MMPULiteGenDFAPacketizer.inc -gen-dfa-packetizer)
add_public_tablegen_target(MMPULiteCommonTableGen)


add_llvm_target(MMPULiteCodeGen
		MMPULiteSubtarget.cpp
)

add_dependencies(LLVMMMPULiteCodeGen intrinsics_gen)

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
