/* This file defines data structures for encoding/decoding MMPU instruction lines. */
#ifndef MMPU_INSN_LINE_H
#define MMPU_INSN_LINE_H

#include "llvm/Support/raw_ostream.h"
#include <stdint.h>

namespace llvm
{
	namespace MMPU
	{
		struct EncodingLayout
		{
				unsigned startBit;  // start bit in current byte
				unsigned numBits;   // how many bits

				unsigned startByte; // offset in code buffer
				unsigned numBytes;  // how many bytes
		};

		#define  NUMSLOTS     14
		extern const unsigned numBitsForSlot[];
		extern const unsigned SumEncodingBytes;

		class InsnLine
		{
			private:
				EncodingLayout layout[NUMSLOTS];
				uint8_t* codeBuf;
				unsigned sumBits;
				unsigned sumBytes;

				void
				initLayout();

			public:

				InsnLine() {
					initLayout();
					codeBuf = new uint8_t[sumBytes];
				}

				virtual ~InsnLine() { delete codeBuf; }

				void
				ConcatCode(uint64_t code, unsigned slot);

				void
				ExtractCode(uint64_t &code, unsigned slot);

				void
				EmitCode(raw_ostream &OS);

				uint8_t*
				getCodeBuf() {return codeBuf;}

				unsigned
				getSumBytes() {return sumBytes;}
		};
	}
}

#endif // MMPU_INSN_LINE_H
