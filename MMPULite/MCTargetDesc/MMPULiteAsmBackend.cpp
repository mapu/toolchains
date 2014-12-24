//===-- MMPULiteASMBackend.cpp - MMPULite Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MMPULiteAsmBackend and MMPULiteELFObjectWriter classes.
//
//===----------------------------------------------------------------------===//
//

#include "MMPULiteFixupKinds.h"
#include "MMPULiteMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
	class MMPULiteAsmBackend: public MCAsmBackend {
			uint8_t OSABI;
		public:
			MMPULiteAsmBackend(const Target &T, uint8_t osabi)
					: MCAsmBackend(), OSABI(osabi)
			{
			}

			MCObjectWriter *createObjectWriter(raw_ostream &OS) const
			{
				return createMMPULiteELFObjectWriter(OS, OSABI);
			}

			/// ApplyFixup - Apply the \p Value for given \p Fixup into the provided
			/// data bytes, at the bit-offset specified by the fixup and following the
			/// fixup kind as appropriate.
			/// Data: the start byte related to the encoding section
			/// DataSize : size of the encoding section in bytes
			void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
					uint64_t Value) const
			{
				MCFixupKind Kind = Fixup.getKind();

				// Which byte do we start in the object section
				unsigned offset = Fixup.getOffset();

				// Number of bytes we need to fixup
				//unsigned NumBytes = ( getFixupKindInfo(Kind).TargetSize + 7 ) / 8;

				// Grab current value from 'Data'
				uint64_t insnCodes = 0;
				for (unsigned i = 0; i < 4; ++i, ++offset) {
					insnCodes |=  ((uint64_t)(Data[offset]) & 0xFFu) << (i*8);
				}

				// TargetOffset and TargetSize describe bits
				uint64_t Mask = ((uint64_t)(-1)	>> (64 - getFixupKindInfo(Kind).TargetSize));
				Value &= Mask; // filter all irrelated higher-bits.
				Value <<= getFixupKindInfo(Kind).TargetOffset;

				insnCodes |= Value;

				// Where do we start in the object
				offset = Fixup.getOffset();
				// Write out the fixed up bytes back to the code/data bits.
				for (unsigned i = 0; i < 4; ++i, ++offset) {
					Data[offset] = (char)( (insnCodes >> (i * 8)) & 0xFFu );
				}
			}

			unsigned getNumFixupKinds() const
			{
				return MMPULite::NumTargetFixupKinds;
			}

			const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const
			{
				const static MCFixupKindInfo Infos[MMPULite::NumTargetFixupKinds] = {
						// This table *must* be in the same order with those fixup_*_* kinds
            // defined in MMPULiteFixupKinds.h

						// name    	          bit-offset  bits flags
						{ "fixup_MMPULite_11", /* 8-17 */9,  11,   0},
				};

				if (Kind < FirstTargetFixupKind)
					return MCAsmBackend::getFixupKindInfo(Kind);

				assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() && "Invalid kind!");
				return Infos[Kind - FirstTargetFixupKind];
			}

			/// @name Target Relaxation Interfaces
			/// @{

			/// MayNeedRelaxation - Check whether the given instruction may need
			/// relaxation.
			///
			/// \param Inst - The instruction to test.
			bool mayNeedRelaxation(const MCInst &Inst) const
			{
				return false;
			}

			/// fixupNeedsRelaxation - Target specific predicate for whether a given
			/// fixup requires the associated instruction to be relaxed.
			bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
					const MCRelaxableFragment *DF, const MCAsmLayout &Layout) const
			{
				// FIXME.
				assert(0 && "relaxInstruction() unimplemented");
				return false;
			}

			/// RelaxInstruction - Relax the instruction in the given fragment
			/// to the next wider instruction.
			///
			/// \param Inst - The instruction to relax, which may be the same
			/// as the output.
			/// \param [out] Res On return, the relaxed instruction.
			void relaxInstruction(const MCInst &Inst, MCInst &Res) const
			{
			}

			/// @}

			/// WriteNopData - Write an (optimal) nop sequence of Count bytes
			/// to the given output. If the target cannot generate such a sequence,
			/// it should return an error.
			///
			/// \return - True on success.
			bool writeNopData(uint64_t Count, MCObjectWriter *OW) const
			{
				// Check for a less than instruction size number of bytes
				if (Count % 38) return false;
				//uint64_t NumNops = Count / 4;
				for (uint64_t i = 0; i != Count; ++i)
					OW->Write8(0);
				return true;
			}
	};
// class MMPULiteAsmBackend

}// namespace

// MCAsmBackend
MCAsmBackend *llvm::createMMPULiteAsmBackend(const Target &T,
                                             const MCRegisterInfo &MRI,
                                             StringRef TT,
                                             StringRef CPU) {
	Triple TheTriple(TT);
	uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(TheTriple.getOS());
	return new MMPULiteAsmBackend(T, OSABI);
}
