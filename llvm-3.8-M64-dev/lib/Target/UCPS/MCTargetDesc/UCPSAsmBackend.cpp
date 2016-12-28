//===-- UCPSASMBackend.cpp - UCPS Asm Backend  ----------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the UCPSAsmBackend and UCPSELFObjectWriter classes.
//
//===----------------------------------------------------------------------===//
//

#include "../UCPS.h"
#include "UCPSFixupKinds.h"
#include "UCPSMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCDirectives.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
using namespace llvm;

namespace {
	class UCPSAsmBackend: public MCAsmBackend {
	  private:
			uint8_t OSABI;

		public:
			UCPSAsmBackend(const Target &T, uint8_t osabi)
					: MCAsmBackend(), OSABI(osabi)
			{
			}

			MCObjectWriter *createObjectWriter(raw_pwrite_stream &OS) const override
			{
				return createUCPSELFObjectWriter(OS, OSABI);
			}

      bool immNeedsRelaxation(const MCInst & Inst,  int64_t Imm) const
      {
        //if (!Inst.getOperand(Inst.getNumOperands() - 1).isInst()) return false;

        switch (Inst.getOpcode()) {

//           case UCPSInst::LoadSI8JI:
//           case UCPSInst::LoadUI8JI:
//           case UCPSInst::LoadSI16JI:
//           case UCPSInst::LoadUI16JI:
//           case UCPSInst::LoadI32JI:
//           case UCPSInst::LoadF32JI:
//           case UCPSInst::LoadF64JI:
//           case UCPSInst::LoadPtrJI:
//           case UCPSInst::LoadPtrOffsetJI:
// 
//           case UCPSInst::StoreI8JI:
//           case UCPSInst::StoreI16JI:
//           case UCPSInst::StoreI32JI:
//           case UCPSInst::StoreF32JI:
//           case UCPSInst::StoreF64JI:
//           case UCPSInst::StorePtrJI:
//
//           case UCPSInst::LdImm:
//           case UCPSInst::StImm:
// 
//           case UCPSInst::AssignRRegImm11:
//           case UCPSInst::AssignJRegImm11:
// 
//           case UCPSInst::AssignI32:
//           case UCPSInst::AssignPtr:
//           case UCPSInst::AssignF32:
//             if(! isInt<11> (Imm) ) return true;
//             else return false;

          case UCPSInst::JumpImm:
//          case UCPSInst::JumpBasicBlock:
          case UCPSInst::JumpImmCond:
//          case UCPSInst::JumpBasicBlockCond:

          case UCPSInst::CallImm:
          case UCPSInst::CallImmCond:

          case UCPSInst::LoopL0:
          case UCPSInst::LoopL1:
            if(!isInt<18>(Imm)) return true;
            else return false;

          default:
            return false;
        }
      }

			/// ApplyFixup - Apply the \p Value for given \p Fixup into the provided
			/// data fragment, at the offset specified by the fixup and following the
			/// fixup kind as appropriate.
			/// Data: the start byte of encoding section
			/// DataSize : encoding section size in bytes
			void applyFixup(const MCFixup &Fixup, char *Data, unsigned DataSize,
					            uint64_t Value, bool IsPCRel) const override
			{
				int64_t sval = (int64_t)(Value);

        // Where do we start in the instruction line
				int64_t offset = Fixup.getOffset();

				MCFixupKind Kind = Fixup.getKind();

				switch((UCPS::FixupKind)Kind) {
					case UCPS::fixup_UCPS_PC28:
					case UCPS::fixup_UCPS_28:
					  sval>>=4; break;

					case UCPS::fixup_UCPS_12:
					  sval /= MSPUEncodingBytes; break;

					default: break;
				}

				// Grab current value from 'Data'
				uint64_t insnCodes = 0;
				for (unsigned i = 0; i < 4; ++i, ++offset) {
					insnCodes |=  ((uint64_t)(Data[offset]) & 0xFFu) << (i*8);
				}

				uint64_t Mask = ((uint64_t)(-1)	>> (64 - getFixupKindInfo(Kind).TargetSize));
				sval &= Mask; // filter all irrelated higher-bits.
				sval <<= getFixupKindInfo(Kind).TargetOffset; // bit offset for instruction encoding

				insnCodes |= sval;

				// Where do we start in the object instruction line
				// (i.e., byte offset for instruction encoding from section)
				offset = Fixup.getOffset();
				// Write out the fixed up bytes back to the code/data bits.
				for (unsigned i = 0; i < 4; ++i, ++offset) {
					Data[offset] = (char)( (insnCodes >> (i * 8)) & 0xFFu );
				}
			}

			unsigned getNumFixupKinds() const override
			{
				return UCPS::NumTargetFixupKinds;
			}

			const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override
			{
				const static MCFixupKindInfo Infos[UCPS::NumTargetFixupKinds] = {
						// This table *must* be in the same order with those fixup_XXX_YYY kinds
            // defined in UCPSFixupKinds.h

						// name    	       		bit-offset bits flags
						{ "fixup_UCPS_PC18", 	/*21-*/5,  18, MCFixupKindInfo::FKF_IsPCRel},
						{ "fixup_UCPS_PC28", 	/*27-*/0,  28, MCFixupKindInfo::FKF_IsPCRel},
						{ "fixup_UCPS_11_B5",	/*15-*/5,  11, 0 },
						{ "fixup_UCPS_11_B10",/*20-*/10, 11, 0 },
						{ "fixup_UCPS_12",    /*21-*/	5, 12, 0 },
						{ "fixup_UCPS_28",    /*27-*/	0, 28, 0 }

				};

				if (Kind < FirstTargetFixupKind)
					return MCAsmBackend::getFixupKindInfo(Kind);

				assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() && "Invalid kind!");
				return Infos[Kind - FirstTargetFixupKind];
			}

			int getImmIndx(const MCInst &Inst) const
			{
        switch(Inst.getOpcode()) {

//          case UCPSInst::LdImm:
//            return 3;
//
//           case UCPSInst::LoadSI8JI:
//           case UCPSInst::LoadUI8JI:
//           case UCPSInst::LoadSI16JI:
//           case UCPSInst::LoadUI16JI:
//           case UCPSInst::LoadI32JI:
//           case UCPSInst::LoadF32JI:
//           case UCPSInst::LoadF64JI:
//           case UCPSInst::LoadPtrJI:
//           case UCPSInst::LoadPtrOffsetJI:
//           case UCPSInst::StImm:
//             return 2;
// 
//           case UCPSInst::StoreI8JI:
//           case UCPSInst::StoreI16JI:
//           case UCPSInst::StoreI32JI:
//           case UCPSInst::StoreF32JI:
//           case UCPSInst::StoreF64JI:
//           case UCPSInst::StorePtrJI:
// 
//           case UCPSInst::AssignI32:
//           case UCPSInst::AssignPtr:
//           case UCPSInst::AssignF32:
//           case UCPSInst::AssignRRegImm11:
//           case UCPSInst::AssignJRegImm11:

          case UCPSInst::JumpImmCond:
//          case UCPSInst::JumpBasicBlockCond:
          case UCPSInst::CallImmCond:
            return 1;

          case UCPSInst::JumpImm:
//          case UCPSInst::JumpBasicBlock:
          case UCPSInst::CallImm:

          case UCPSInst::LoopL0:
          case UCPSInst::LoopL1:
            return 0;

          default:
            // none of above
            return -1;
        }
			}

			/// @name Target Relaxation Interfaces
			/// @{

			/// MayNeedRelaxation - Check whether the given instruction may need
			/// relaxation.
			///
			/// \param Inst - The instruction to test.
			bool mayNeedRelaxation(const MCInst &Inst) const override
			{
        const MCInst *MIP = &Inst;
        while (MIP->getNumOperands() &&
               MIP->getOperand(MIP->getNumOperands() - 1).isInst()) {
          MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst();
//          if (MIP->getOpcode() == UCPSInst::ImmExt) return false;
        }

        MIP = &Inst;
        do {
          if (getImmIndx(*MIP) != -1) break;
        } while (MIP->getNumOperands() &&
                 MIP->getOperand(MIP->getNumOperands() - 1).isInst() &&
                 (MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst()));

        int index = getImmIndx(*MIP);

        if (index < 0) return false;
        else {
          const MCOperand & op = MIP->getOperand(index);

          // note: we do not generate fixups for imm or constant expression,
          // so they will not be relaxed.
          if(op.isExpr()) {
            const MCExpr * expr = op.getExpr();
            if(expr->getKind() != MCExpr::Constant)
              return true;
          }
        }

        return false;
			}

			/// fixupNeedsRelaxation - Target specific predicate for whether a given
			/// fixup requires the associated instruction to be relaxed.
			bool fixupNeedsRelaxation(const MCFixup &Fixup,
		                            uint64_t Value,
		                            const MCRelaxableFragment *DF,
		                            const MCAsmLayout &Layout) const override
			{

				const MCInst *inst = &DF->getInst();
				int64_t val = (int64_t) (Value);

				/*MCFixupKind Kind = Fixup.getKind();

        switch(Kind) {
          case UCPS::fixup_UCPS_PC17:
            std::cout << "fixup_UCPS_PC17" << std::endl;
            if(!isInt<17> (val) ) return true;
            else return false;
          case UCPS::fixup_UCPS_11_B5:
          case UCPS::fixup_UCPS_11_B10:
            std::cout << "fixup_UCPS_11_B10" << std::endl;
            if(!isInt<11> (val) ) return true;
            else return false;

          default: return false;
        }*/

        bool res = immNeedsRelaxation(*inst, val);
        while (!res && inst->getNumOperands() &&
            inst->getOperand(inst->getNumOperands() - 1).isInst()) {
          inst = inst->getOperand(inst->getNumOperands() - 1).getInst();
          res = immNeedsRelaxation(*inst, val);
        }
				return res;
			}

			/// RelaxInstruction - Relax the instruction in the given fragment
			/// to the next wider instruction.
			///
			/// \param Inst - The instruction to relax, which may be the same
			/// as the output.
			/// \param [out] Res On return, the relaxed instruction.

			// Note: there is at most one instruction that can be relaxed.
			// So if an inst list need relaxation, the first instruction that
			// allows relaxation found by order MUST be the instruction that
			// needs be relaxed.
			// If there is no fixup in an instruction, relaxInstruction will not
			// be performed.
			void relaxInstruction(const MCInst &Inst, MCInst &Res) const override
			{
        const MCInst *MIP = &Inst;
        do {
          if (getImmIndx(*MIP) != -1) break;
        } while (MIP->getNumOperands() &&
                 MIP->getOperand(MIP->getNumOperands() - 1).isInst() &&
                 (MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst()));

        if (getImmIndx(*MIP) == -1) return;

        int index = getImmIndx(*MIP);

        MCInst *ImmExtInst = new MCInst();
//        ImmExtInst->setOpcode(UCPSInst::ImmExt);
//        ImmExtInst->addOperand(MIP->getOperand(index));

        /*if (MIP->getNumOperands() &&
            MIP->getOperand(MIP->getNumOperands() - 1).isInst()) {
          ImmExtInst->addOperand(MIP->getOperand(MIP->getNumOperands() - 1));
        }*/
        MCInst *RMIP = &Res;
        MIP = &Inst;
        do {
          RMIP->setOpcode(MIP->getOpcode());
          for (unsigned i = 0; i < MIP->getNumOperands() - 1; i++)
            RMIP->addOperand(MIP->getOperand(i));
          if (MIP->getOperand(MIP->getNumOperands() - 1).isInst()) {
            MIP = MIP->getOperand(MIP->getNumOperands() - 1).getInst();
            if (RMIP == &Res) RMIP->addOperand(ImmExtInst->getOperand(0));
            MCInst *newInst = new MCInst();
            RMIP->addOperand(MCOperand::createInst(newInst));
            RMIP = newInst;
          } else {
            RMIP->addOperand(MIP->getOperand(MIP->getNumOperands() - 1));
            if (RMIP == &Res) RMIP->addOperand(ImmExtInst->getOperand(0));
            RMIP->addOperand(MCOperand::createInst(ImmExtInst));
            break;
          }
        } while (1);

        return;
			}
			/// @}

		  /// getMinimumNopSize - Returns the minimum size of a nop in bytes on this
		  /// target. The assembler will use this to emit excess padding in situations
		  /// where the padding required for simple alignment would be less than the
		  /// minimum nop size.
		  ///
		  virtual unsigned getMinimumNopSize() const { return 4; }

			/// WriteNopData - Write an (optimal) nop sequence of Count bytes
			/// to the given output. If the target cannot generate such a sequence,
			/// it should return an error.
			///
			/// \return - True on success.
			/// used for .align
			bool writeNopData(uint64_t Count, MCObjectWriter *OW) const override
			{
			  // Count bytes and 4-bytes per instruction.
				// Check for a less than instruction size number of bytes
				if (Count % 4) return false;

				uint64_t NumNops = Count / 4; // 4 bytes per instruction.

        if(NumNops > 0) { // at least one instruction
          unsigned lineNum = NumNops / 4; // 4 instructions per line.
          unsigned instNum = NumNops % 4;

          for(unsigned i = 0; i != lineNum; ++i) {
            OW->writeLE32(0);
            OW->writeLE32(0);
            OW->writeLE32(0);
            OW->writeLE32(0x80000000u);
          }

          for(uint64_t i = 0; i != instNum-1; ++i)
            OW->writeLE32(0);

          OW->writeLE32(0x80000000u); // end a instruction line
        }

				return true;
			}
			/// @}

	}; // class UCPSAsmBackend

} // namespace

// MCAsmBackend
MCAsmBackend *llvm::createUCPSAsmBackend(const Target &T,
                                         const MCRegisterInfo &MRI,
                                         const Triple &TT, StringRef CPU)
{
	uint8_t OSABI = MCELFObjectTargetWriter::getOSABI(TT.getOS());
	return new UCPSAsmBackend(T, OSABI);
}
