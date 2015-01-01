/* This file defines data structures for encoding/decoding MMPULite instruction lines. */
#ifndef MMPULITE_INSN_LINE_H
#define MMPULITE_INSN_LINE_H

#include "llvm/Support/raw_ostream.h"
#include <stdint.h>

namespace llvm
{
	namespace MMPULite
	{
		struct EncodingLayout
		{
				unsigned startBit; // start bit in current byte
				unsigned numBits;  // how many bits

				unsigned startByte;  // offset in code buffer
				unsigned numBytes; // how many bytes
		};

		#define  NUMSLOTS  14
		extern const unsigned numBitsForSlot[];

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
					memset(codeBuf, 0, sumBytes);
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

#endif // MMPULite_INSN_LINE_H
