#ifndef MSPU_INST_LINE_H
#define MSPU_INST_LINE_H

#include "llvm/MC/MCInst.h"

namespace llvm
{

	class MSPUInst;

	class MSPUInstLine
	{
		private:
			bool IsFilledSlot[4];
			bool IsImmExtended;
			unsigned NumInsts;

			MSPUInst * Inst;

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

			MSPUInst *
			getInst()
			{
				return Inst;
			}

			void
			setInst(MSPUInst * inst)
			{
				Inst = inst;
			}

	};

	class MSPUInst: public MCInst
	{

		private:
			MSPUInstLine* Line;

		public:
			is

	};
}

#endif //MSPU_INST_LINE_H
