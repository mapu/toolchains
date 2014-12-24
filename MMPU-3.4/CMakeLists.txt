set(LLVM_TARGET_DEFINITIONS MMPU.td)

tablegen(LLVM MMPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MMPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MMPUGenDisassemblerTables.inc -gen-disassembler)

tablegen(LLVM MMPUGenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM MMPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MMPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM MMPUGenCallingConv.inc -gen-callingconv)
tablegen(LLVM MMPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM MMPUGenEDInfo.inc -gen-enhanced-disassembly-info)

tablegen(LLVM MMPUGenDFAPacketizer.inc -gen-dfa-packetizer)
add_public_tablegen_target(MMPUCommonTableGen)

add_llvm_target(MMPUCodeGen
		MMPUInstrInfo.cpp
		MMPUISelLowering.cpp
		MMPUMCInstLower.cpp
		MMPUSelectionDAGInfo.cpp
		MMPUTargetMachine.cpp
		MMPUFrameLowering.cpp
		MMPUISelDAGToDAG.cpp
		MMPUMachineFunctionInfo.cpp
		MMPURegisterInfo.cpp
		MMPUSubtarget.cpp
)

add_dependencies(LLVMMMPUCodeGen)

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
