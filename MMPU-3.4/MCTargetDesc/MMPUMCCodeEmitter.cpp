//===-- MMPUMCCodeEmitter.cpp - Convert MMPU code to machine code -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MMPUMCCodeEmitter class
// and originates from "PPCMCCodeEmitter.cpp"
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "mccodeemitter"
#include "MMPUFixupKinds.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/ADT/APFloat.h"
#include "MMPUInsnLine.h"
#include "MMPUMCTargetDesc.h"
#include "MMPUInsnLine.h"
#include "MMPUMCInst.h"

using namespace llvm;

// I don not know how this static var is utilized. A potential way is to sum up
// the size of .text segment. --yangyy
//STATISTIC(MCNumEmitted, "Number of MC instructions emitted");

static uint64_t mmpu_encode_c_option(int64_t options)
{
  int64_t  code = 0;

  if(options & MMPU::c_C)
    code = 2; // 0b10
  else if(options & MMPU::c_NC)
    code = 3; // 0b11

  return code;
}

/// getMMPURegisterEncoding - Given the enum value for some register, return
/// the encoding it corresponds to.
static uint64_t
getMMPURegisterEncoding(unsigned RegEnum)
{
  switch(RegEnum) {
    case MMPUReg::M0    :    return 0  ;
    case MMPUReg::M1    :    return 1  ;
    case MMPUReg::M2    :    return 2  ;
    case MMPUReg::M3    :    return 3  ;
    case MMPUReg::M4    :    return 4  ;
    case MMPUReg::M5    :    return 5  ;
    case MMPUReg::M6    :    return 6  ;
    case MMPUReg::M7    :    return 7  ;
    case MMPUReg::M8    :    return 8  ;
    case MMPUReg::M9    :    return 9  ;
    case MMPUReg::M10   :    return 10 ;
    case MMPUReg::M11   :    return 11 ;
    case MMPUReg::M12   :    return 12 ;
    case MMPUReg::M13   :    return 13 ;
    case MMPUReg::M14   :    return 14 ;
    case MMPUReg::M15   :    return 15 ;
    case MMPUReg::M16   :    return 16 ;
    case MMPUReg::M17   :    return 17 ;
    case MMPUReg::M18   :    return 18 ;
    case MMPUReg::M19   :    return 19 ;
    case MMPUReg::M20   :    return 20 ;
    case MMPUReg::M21   :    return 21 ;
    case MMPUReg::M22   :    return 22 ;
    case MMPUReg::M23   :    return 23 ;
    case MMPUReg::M24   :    return 24 ;
    case MMPUReg::M25   :    return 25 ;
    case MMPUReg::M26   :    return 26 ;
    case MMPUReg::M27   :    return 27 ;
    case MMPUReg::M28   :    return 28 ;
    case MMPUReg::M29   :    return 29 ;
    case MMPUReg::M30   :    return 30 ;
    case MMPUReg::M31   :    return 31 ;
    case MMPUReg::M32   :    return 32 ;
    case MMPUReg::M33   :    return 33 ;
    case MMPUReg::M34   :    return 34 ;
    case MMPUReg::M35   :    return 35 ;
    case MMPUReg::M36   :    return 36 ;
    case MMPUReg::M37   :    return 37 ;
    case MMPUReg::M38   :    return 38 ;
    case MMPUReg::M39   :    return 39 ;
    case MMPUReg::M40   :    return 40 ;
    case MMPUReg::M41   :    return 41 ;
    case MMPUReg::M42   :    return 42 ;
    case MMPUReg::M43   :    return 43 ;
    case MMPUReg::M44   :    return 44 ;
    case MMPUReg::M45   :    return 45 ;
    case MMPUReg::M46   :    return 46 ;
    case MMPUReg::M47   :    return 47 ;
    case MMPUReg::M48   :    return 48 ;
    case MMPUReg::M49   :    return 49 ;
    case MMPUReg::M50   :    return 50 ;
    case MMPUReg::M51   :    return 51 ;
    case MMPUReg::M52   :    return 52 ;
    case MMPUReg::M53   :    return 53 ;
    case MMPUReg::M54   :    return 54 ;
    case MMPUReg::M55   :    return 55 ;
    case MMPUReg::M56   :    return 56 ;
    case MMPUReg::M57   :    return 57 ;
    case MMPUReg::M58   :    return 58 ;
    case MMPUReg::M59   :    return 59 ;
    case MMPUReg::M60   :    return 60 ;
    case MMPUReg::M61   :    return 61 ;
    case MMPUReg::M62   :    return 62 ;
    case MMPUReg::M63   :    return 63 ;
    case MMPUReg::M64   :    return 64 ;
    case MMPUReg::M65   :    return 65 ;
    case MMPUReg::M66   :    return 66 ;
    case MMPUReg::M67   :    return 67 ;
    case MMPUReg::M68   :    return 68 ;
    case MMPUReg::M69   :    return 69 ;
    case MMPUReg::M70   :    return 70 ;
    case MMPUReg::M71   :    return 71 ;
    case MMPUReg::M72   :    return 72 ;
    case MMPUReg::M73   :    return 73 ;
    case MMPUReg::M74   :    return 74 ;
    case MMPUReg::M75   :    return 75 ;
    case MMPUReg::M76   :    return 76 ;
    case MMPUReg::M77   :    return 77 ;
    case MMPUReg::M78   :    return 78 ;
    case MMPUReg::M79   :    return 79 ;
    case MMPUReg::M80   :    return 80 ;
    case MMPUReg::M81   :    return 81 ;
    case MMPUReg::M82   :    return 82 ;
    case MMPUReg::M83   :    return 83 ;
    case MMPUReg::M84   :    return 84 ;
    case MMPUReg::M85   :    return 85 ;
    case MMPUReg::M86   :    return 86 ;
    case MMPUReg::M87   :    return 87 ;
    case MMPUReg::M88   :    return 88 ;
    case MMPUReg::M89   :    return 89 ;
    case MMPUReg::M90   :    return 90 ;
    case MMPUReg::M91   :    return 91 ;
    case MMPUReg::M92   :    return 92 ;
    case MMPUReg::M93   :    return 93 ;
    case MMPUReg::M94   :    return 94 ;
    case MMPUReg::M95   :    return 95 ;
    case MMPUReg::M96   :    return 96 ;
    case MMPUReg::M97   :    return 97 ;
    case MMPUReg::M98   :    return 98 ;
    case MMPUReg::M99   :    return 99 ;
    case MMPUReg::M100  :    return 100;
    case MMPUReg::M101  :    return 101;
    case MMPUReg::M102  :    return 102;
    case MMPUReg::M103  :    return 103;
    case MMPUReg::M104  :    return 104;
    case MMPUReg::M105  :    return 105;
    case MMPUReg::M106  :    return 106;
    case MMPUReg::M107  :    return 107;
    case MMPUReg::M108  :    return 108;
    case MMPUReg::M109  :    return 109;
    case MMPUReg::M110  :    return 110;
    case MMPUReg::M111  :    return 111;
    case MMPUReg::M112  :    return 112;
    case MMPUReg::M113  :    return 113;
    case MMPUReg::M114  :    return 114;
    case MMPUReg::M115  :    return 115;
    case MMPUReg::M116  :    return 116;
    case MMPUReg::M117  :    return 117;
    case MMPUReg::M118  :    return 118;
    case MMPUReg::M119  :    return 119;
    case MMPUReg::M120  :    return 120;
    case MMPUReg::M121  :    return 121;
    case MMPUReg::M122  :    return 122;
    case MMPUReg::M123  :    return 123;
    case MMPUReg::M124  :    return 124;
    case MMPUReg::M125  :    return 125;
    case MMPUReg::M126  :    return 126;
    case MMPUReg::M127  :    return 127;
    case MMPUReg::MIPP  :    return 128;
    case MMPUReg::MSPP  :    return 1<<8;

    case MMPUReg::BIU0  :    return 0;
    case MMPUReg::BIU1  :    return 1;
    case MMPUReg::BIU2  :    return 2;

    case MMPUReg::Shu0  :    return 0;
    case MMPUReg::Shu1  :    return 1;

    case MMPUReg::IALU  :    return 0;
    case MMPUReg::IMAC  :    return 1;
    case MMPUReg::FALU  :    return 2;
    case MMPUReg::FMAC  :    return 3;

    case MMPUReg::r0    :    return 0;
    case MMPUReg::r1    :    return 1;
    case MMPUReg::r2    :    return 2;
    case MMPUReg::r3    :    return 3;

    case MMPUReg::w0    :    return 0;
    case MMPUReg::w1    :    return 1;
    case MMPUReg::w2    :    return 2;
    case MMPUReg::w3    :    return 3;

    case MMPUReg::T0    :    return 0;
    case MMPUReg::T1    :    return 1;
    case MMPUReg::T2    :    return 2;
    case MMPUReg::T3    :    return 3;

    case MMPUReg::TBB    :    return 4;
    case MMPUReg::TBH    :    return 5;
    case MMPUReg::TBW    :    return 6;
    case MMPUReg::TBD    :    return 7;
    case MMPUReg::TSQ    :    return 8;

    case MMPUReg::I0      : return 0+1;
    case MMPUReg::I1      : return 1+1;
    case MMPUReg::I2      : return 2+1;

    case MMPUReg::KI0    :    return 0  ;
    case MMPUReg::KI1    :    return 1  ;
    case MMPUReg::KI2    :    return 2  ;
    case MMPUReg::KI3    :    return 3  ;
    case MMPUReg::KI4    :    return 4  ;
    case MMPUReg::KI5    :    return 5  ;
    case MMPUReg::KI6    :    return 6  ;
    case MMPUReg::KI7    :    return 7  ;
    case MMPUReg::KI8    :    return 8  ;
    case MMPUReg::KI9    :    return 9  ;
    case MMPUReg::KI10   :    return 10 ;
    case MMPUReg::KI11   :    return 11 ;
    case MMPUReg::KI12   :    return 12 ;
    case MMPUReg::KI13   :    return 13 ;
    case MMPUReg::KI14   :    return 14 ;
    case MMPUReg::KI15   :    return 15 ;

    default:
      llvm_unreachable("Unknown register number!");
  }
}

namespace
{
	class MMPUMCCodeEmitter: public MCCodeEmitter
	{
			MMPUMCCodeEmitter(const MMPUMCCodeEmitter &) LLVM_DELETED_FUNCTION;

			void
			operator=(const MMPUMCCodeEmitter &) LLVM_DELETED_FUNCTION;

			int numEncodedInsts;

		public:
			MMPUMCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
								MCContext &ctx) : numEncodedInsts(0)
			{
			}

			~MMPUMCCodeEmitter()
			{
			}

			void
			EncodeInstList(MMPU::InsnLine & line, const MCInst &MI,
			               unsigned & slot, SmallVectorImpl<MCFixup> &Fixups) const {

			  uint64_t code = getBinaryCodeForInstr(MI, Fixups);

				unsigned num = MI.getNumOperands();
				// the last MCInst might not have any operand, say NOP
				if(num>0 && MI.getOperand(num - 1).isInst()) {
					line.ConcatCode(code, slot);
					slot += 1;
					EncodeInstList(line, *(MI.getOperand(num - 1).getInst()), slot, Fixups);
				}
				else line.ConcatCode(code, slot);
			}

			/// getMachineOpValue - Return binary encoding of operand. If the machine
			/// operand requires fixup, record the fixup and return zero.
			/// This func is called within tblgen'erated getBinaryCodeForInstr().
			uint64_t
			getMachineOpValue(const MCInst &MI, const MCOperand &MO,
								SmallVectorImpl<MCFixup> &Fixups) const;

			// getBinaryCodeForInstr - TableGen'erated function for getting the
			// binary encoding for an instruction.
			uint64_t
			getBinaryCodeForInstr(const MCInst &MI,
			                      SmallVectorImpl<MCFixup> &Fixups) const;

			uint64_t
      getUnitEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        unsigned reg = MO.getReg();
        uint64_t options = getMMPURegisterEncoding(reg);

        // one bit encoding for shu
        switch(reg) {
          case MMPUReg::Shu0:
          case MMPUReg::Shu1:
            options <<= 2;
            options |= 1;
            break;

          case MMPUReg::IALU:
          case MMPUReg::FALU:
          case MMPUReg::IMAC:
          case MMPUReg::FMAC:
            options <<= 2;
            options |= 3;
            break;

          default:
            llvm_unreachable("invalid register for Unit group");
        }

        return options;
      }

			uint64_t
			getUImm10Encoding(const MCInst &MI, unsigned OpNo,
					SmallVectorImpl<MCFixup> &Fixups) const
			{
				const MCOperand &MO = MI.getOperand(OpNo);
				// If the destination is an immediate, we have nothing to do.
				if (MO.isImm())
					return MO.getImm()/MMPU::SumEncodingBytes;

				assert(MO.isExpr() && "getUImm10Encoding expects only expressions or an immediate");
				const MCExpr *Expr = MO.getExpr();

				switch (Expr->getKind()) {
				  case MCExpr::Constant: {
				      int64_t code =  static_cast<const MCConstantExpr*>(Expr)->getValue();
				      return code/MMPU::SumEncodingBytes;
				  }
				  default:
				    break;
				}

				Fixups.push_back(MCFixup::Create(37 /* Byte Offset */, Expr, MCFixupKind(MMPU::fixup_MMPU_11)));
				return 0;
			}

			uint64_t
      getIALUOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();

        int64_t  codes = 0;
        // options 23-18: X X T U B H
        if(options & MMPU::c_H) codes |= 1<<0;
        if(options & MMPU::c_B) codes |= 1<<1;
        if(options & MMPU::c_U) codes |= 1<<2;
        if(options & MMPU::c_T) codes |= 1<<3;

        codes |= mmpu_encode_c_option(options)<<4;

        return codes;
      }

			uint64_t
			getFALUOptionsEncoding(const MCInst &MI, unsigned OpNo,
			                    SmallVectorImpl<MCFixup> &Fixups) const {
				const MCOperand &MO = MI.getOperand(OpNo);
				int64_t options = MO.getImm();
			  int64_t  codes = 0;
			  // T U S D
			  if(options & MMPU::c_D) codes |= 1<<0;
        if(options & MMPU::c_S) codes |= 1<<1;
        if(options & MMPU::c_U) codes |= 1<<2;
        if(options & MMPU::c_T) codes |= 1<<3;

        codes |= mmpu_encode_c_option(options)<<4;
			  return codes;
			}

      uint64_t
      getFALUDblOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();
        int64_t  codes = 0;
        // T U
        if(options & MMPU::c_T) codes |= 1<<1;

        codes |= mmpu_encode_c_option(options)<<2;
        return codes;
      }

			uint64_t
      getIMACOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();

        int64_t  codes = 0;

        // bits 22-17 : C NC CR  L  I  U  B H
        // Note: we do not need to encode (L)
        if(options & MMPU::c_H) codes |= 1<<0;
        if(options & MMPU::c_B) codes |= 1<<1;
        if(options & MMPU::c_U) codes |= 1<<2;
        if(options & MMPU::c_I) codes |= 1<<3;

        if(options & MMPU::c_CR) codes |= 1<<5;

        codes |= mmpu_encode_c_option(options)<<6;

        return codes;
      }

			uint64_t
      getCOptionEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const
      {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();
        return mmpu_encode_c_option(options);
      }

			// bits 16 - 11
			// C !C M BR A++ K++
			uint64_t
      getBIUOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();

        int64_t  codes = 0;
        if(options & MMPU::c_KPP) codes |= (1<<0);
        if(options & MMPU::c_APP) codes |= (1<<1);
        if(options & MMPU::c_BR)  codes |= (1<<2);
        if(options & MMPU::c_M)   codes |= (1<<3);

        codes |= (mmpu_encode_c_option(options) << 4);

        return codes;
      }

      // bits 16 - 11      ... 0
      // C !C M BR A++ K++ ... IND
      uint64_t
      getBIUStOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();

        int64_t  codes = 0;
        if(options & MMPU::c_IND) codes |= (1<<0);
        if(options & MMPU::c_KPP) codes |= (1<<11);
        if(options & MMPU::c_APP) codes |= (1<<12);
        if(options & MMPU::c_BR)  codes |= (1<<13);
        if(options & MMPU::c_M)   codes |= (1<<14);

        codes |= (mmpu_encode_c_option(options) << 15);

        return codes;
      }

      uint64_t
      getCombineOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();
        int64_t  codes = 0;
        codes |= (mmpu_encode_c_option(options) << 1);

        if(options & MMPU::c_TC) codes |= 1;

        return codes;
      }

      // bits 19-18: C
      // bits 17-15: WF or WX
      uint64_t
      getMr2OptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();
        int64_t  codes = 0;

        if(options & MMPU::c_WF1) codes = 1; else
        if(options & MMPU::c_WF2) codes = 2; else
        if(options & MMPU::c_WF3) codes = 3; else
        if(options & MMPU::c_WX1) codes = 4; else
        if(options & MMPU::c_WX2) codes = 5; else
        if(options & MMPU::c_WX3) codes = 6; else
        if(options & MMPU::c_WX4) codes = 7;

        codes |= (mmpu_encode_c_option(options) << 3);

        return codes;
      }

      uint64_t
      getFMACOptionsEncoding(const MCInst &MI, unsigned OpNo,
                          SmallVectorImpl<MCFixup> &Fixups) const {
        const MCOperand &MO = MI.getOperand(OpNo);
        int64_t options = MO.getImm();
        int64_t  codes = 0;

        // options 18-17 T/CR D/S:1/0
        if(options & MMPU::c_D) codes |= 1;

        switch(MI.getOpcode()) {
          case MMPUInst::FMACMulToMw3:
          case MMPUInst::FMACMulToBIU:
          case MMPUInst::FMACMulToUnit:
            if(options & MMPU::c_T) codes |= 1<<1; // (T)
            break;

          case MMPUInst::FMACMulAccOnMRXferToMw3:
          case MMPUInst::FMACMulAccOnMRXferToBIU:
          case MMPUInst::FMACMulAccOnMRXferToUnit:

          case MMPUInst::FMACComplexMulToMw3:
          case MMPUInst::FMACComplexMulToBIU:
          case MMPUInst::FMACComplexMulToUnit:

          case MMPUInst::FMACMulAddToMR:
          case MMPUInst::FMACMulAccOnMR:
            if(options & MMPU::c_CR) codes |= 1<<1; // (CR)
            break;

          default:
            llvm_unreachable("unknown fmac instruction opcode");
        }

        codes |= (mmpu_encode_c_option(options)<<2);
        return codes;
      }

			void
			EncodeInstruction(const MCInst &MI, raw_ostream &OS,
			                  SmallVectorImpl<MCFixup> &Fixups) const
			{
				MMPU::InsnLine line;
				unsigned slot=0;
				EncodeInstList(line, MI, slot, Fixups);
				line.EmitCode(OS);
			}
	};
}  // end anonymous namespace

MCCodeEmitter *
llvm::createMMPUMCCodeEmitter(const MCInstrInfo &MCII,
								const MCRegisterInfo &MRI,
								const MCSubtargetInfo &STI,
								MCContext &Ctx)
{
	return new MMPUMCCodeEmitter(MCII, STI, Ctx);
}

uint64_t
MMPUMCCodeEmitter::
getMachineOpValue(const MCInst &MI, const MCOperand &MO,
					SmallVectorImpl<MCFixup> &Fixups) const
{
	if (MO.isReg()) {
		unsigned Reg = MO.getReg();
		return getMMPURegisterEncoding(Reg);
	} else if (MO.isImm()) {
		return static_cast<unsigned>(MO.getImm());
	} else if (MO.isFPImm()) {
		return static_cast<unsigned>(APFloat(MO.getFPImm()).bitcastToAPInt()
		                             .getHiBits(32).getLimitedValue());
	}

	// MO must be an Expr.
	assert(MO.isExpr());

	const MCExpr *Expr = MO.getExpr();
	MCExpr::ExprKind Kind = Expr->getKind();

  switch(Kind) {
    case MCExpr::Binary:
    case MCExpr::SymbolRef:{
      MMPU::FixupKind fixupKind = MMPU::FixupKind(0);
      Fixups.push_back(MCFixup::Create(0/*Offset*/, MO.getExpr(), MCFixupKind(fixupKind)));
      // All of the information is in the fixup.
      return 0;
    }
    case MCExpr::Constant: {
      int64_t code = static_cast<const MCConstantExpr*>(Expr)->getValue();
      return code;
    }
    default:
      break;
  }

  return 0;
}

#include "MMPUGenMCCodeEmitter.inc"
