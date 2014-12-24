/* This file defines data structures for encoding/decoding MMPU instruction lines. */

#include "MMPUInsnLine.h"
using namespace llvm::MMPU;

const unsigned llvm::MMPU::numBitsForSlot[] =
						{ /* M0~4   */ 23, 20, 21, 15,
						  /* SHU0~1 */ 28, 28,
						  /* IALU */ 31, /* IMAC */ 29, /* FALU */ 26, /* FMAC */ 24,
						  /* BIU0~2 */ 20, 20, 20,
						  /* Seq */ 22 };

/*unsigned SumEncodingBytes() {
  int sum=0;

  for(int i = 0; i<NUMSLOTS; i++) {
    sum += numBitsForSlot[i];
  }

  return (sum+7)/NUMSLOTS;
}*/

const unsigned llvm::MMPU::SumEncodingBytes = (
      numBitsForSlot[0]+numBitsForSlot[1]+numBitsForSlot[2]+numBitsForSlot[3]+
      numBitsForSlot[4]+numBitsForSlot[5]+numBitsForSlot[6]+numBitsForSlot[7]+
      numBitsForSlot[8]+numBitsForSlot[9]+numBitsForSlot[10]+numBitsForSlot[11]+
      numBitsForSlot[12]+numBitsForSlot[13] + 7) / 8;

void
InsnLine::initLayout()
{
	unsigned n, p1;
	sumBits = 0;
	for(n = 0; n < NUMSLOTS; n++) {
		unsigned num = numBitsForSlot[n];
		p1 = sumBits / 8;
		layout[n].startBit = sumBits - p1 * 8;
		layout[n].numBits = num;

		layout[n].startByte = p1;
		layout[n].numBytes = (num + layout[n].startBit + 7) / 8; // (n - (8-s) + 7) / 8 + 1
		sumBits += num;
	}
	sumBytes = (sumBits + 7) / 8;
}

void
InsnLine::ConcatCode(uint64_t code, unsigned slot)
{
	uint64_t mask = ~( (uint64_t)(-1) << (layout[slot].startBit) );
	uint64_t oldval = (uint64_t)(codeBuf[layout[slot].startByte]) & mask;

	code <<= layout[slot].startBit;
	code |= oldval;
	for(unsigned i = 0; i < layout[slot].numBytes; i++) {
		codeBuf[layout[slot].startByte + i] = (uint8_t)(code & 0xFFu);
		code >>= 8;
	}
}

// For a NOP, we won't add slot number encoding at bits 47 ~ 44.
void
InsnLine::ExtractCode(uint64_t &code, unsigned slot)
{
	code = 0;
	for(unsigned i = 0; i < layout[slot].numBytes; i++) {
		code |= ((uint64_t) (codeBuf[ layout[slot].startByte + i ]) & 0xFFu) << (i*8);
	}

	code >>= layout[slot].startBit;

	uint64_t mask = ~( (uint64_t)(-1) << (layout[slot].numBits) );
	code &= mask;

	// slot is encoded at 4 bits 47 ~ 44.
	// If code is 0, it must be a NOP instruction. And thus we cannot encode slot
	// number into its bits 47 ~ 44.
	if(code != 0)	code |= ((uint64_t)(slot) << 44);
}

void
InsnLine::EmitCode(raw_ostream &OS) {
	for(unsigned n=0; n < sumBytes; n++) {
		OS << codeBuf[n];
	}
}
