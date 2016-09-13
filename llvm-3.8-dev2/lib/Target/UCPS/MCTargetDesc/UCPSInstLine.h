#ifndef UCPS_INST_LINE_H
#define UCPS_INST_LINE_H

#include "llvm/MC/MCInst.h"

namespace llvm
{

	class UCPSInst;

	class UCPSInstLine
	{
		private:
			bool IsFilledSlot[4];
			bool IsImmExtended;
			unsigned NumInsts;

			UCPSInst * Inst;

		public:
			void
			setSlot(int s, bool f)
			{
				IsFilledSlot[s] = f;
			}

			bool
			isSlotFilled(int s)
			{
				return IsFilledSlot[s];
			}

			void
			setImmExtended(bool f)
			{
				IsImmExtended = f;
			}

			bool
			isImmExtended()
			{
				return IsImmExtended;
			}

			UCPSInst *
			getInst()
			{
				return Inst;
			}

			void
			setInst(UCPSInst * inst)
			{
				Inst = inst;
			}

	};

	class UCPSInst: public MCInst
	{

		private:
			UCPSInstLine* Line;

		public:
			is

	};
}

#endif //UCPS_INST_LINE_H
