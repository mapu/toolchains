set(LLVM_TARGET_DEFINITIONS MSPU.td)

tablegen(LLVM MSPUGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MSPUGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MSPUGenDisassemblerTables.inc -gen-disassembler)

tablegen(LLVM MSPUGenMCCodeEmitter.inc -gen-emitter -mc-emitter)
tablegen(LLVM MSPUGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MSPUGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM MSPUGenCallingConv.inc -gen-callingconv)
tablegen(LLVM MSPUGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM MSPUGenEDInfo.inc -gen-enhanced-disassembly-info)

tablegen(LLVM MSPUGenDFAPacketizer.inc -gen-dfa-packetizer)
add_public_tablegen_target(MSPUCommonTableGen)

add_llvm_target(MSPUCodeGen
		MSPUAsmPrinter.cpp
		MSPUInstrInfo.cpp
		MSPUISelLowering.cpp
		MSPUMCInstLower.cpp
		MSPUSelectionDAGInfo.cpp
		MSPUTargetMachine.cpp
		MSPUVLIWPacketizer.cpp
		MSPUFrameLowering.cpp
		MSPUISelDAGToDAG.cpp
		MSPUMachineFunctionInfo.cpp
		MSPURegisterInfo.cpp
		MSPUSubtarget.cpp
		MSPUTargetObjectFile.cpp
)

add_dependencies(LLVMMSPUCodeGen)

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(Disassembler)
add_subdirectory(AsmParser)
