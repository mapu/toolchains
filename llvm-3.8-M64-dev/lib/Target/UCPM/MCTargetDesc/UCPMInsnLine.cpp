/* This file defines data structures for encoding/decoding UCPM instruction lines. */

#include "UCPMInsnLine.h"
using namespace llvm::UCPM;

const unsigned llvm::UCPM::numBitsForSlot[] =
						{/*MFetch*/23,/*Mr0*/16,/*Mr1~5*/14,15,14,15,14,/*SHU0~2*/25,25,25,/*IALU*/24,/*IMAC*/29,/*IFALU*/25,/*IFMAC*/29,/*BIU0~2*/23,23,23
						};


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
	uint64_t mask = ~( (uint64_t)(-1) << (layout[slot].numBits) );
	code &= mask;
	mask = ~( (uint64_t)(-1) << (layout[slot].startBit) );
	uint64_t oldval = (uint64_t)(codeBuf[layout[slot].startByte]) & mask;

	code <<= layout[slot].startBit;
	code |= oldval;

	oldval = codeBuf[layout[slot].startByte + layout[slot].numBytes - 1];
	/*for(unsigned i = 0; i < layout[slot].numBytes; i++) {
		codeBuf[layout[slot].startByte + i] = (uint8_t)(code & 0xFFu);
		code >>= 8;
	}*/
	memcpy(&(codeBuf[layout[slot].startByte]), &code, layout[slot].numBytes);
	codeBuf[layout[slot].startByte + layout[slot].numBytes - 1] |= oldval;
}

// For a NOP, we won't add slot number encoding at bits 47 ~ 44.
void
InsnLine::ExtractCode(uint64_t &code, unsigned slot)
{
	code = 0;
	/*for(unsigned i = 0; i < layout[slot].numBytes; i++) {
		code |= (codeBuf[layout[slot].startByte + i] & 0xFFu) << (i*8);
	}*/
	memcpy(&code, &(codeBuf[layout[slot].startByte]), layout[slot].numBytes);
	code >>= layout[slot].startBit;

	uint64_t mask = ~( (uint64_t)(-1) << (layout[slot].numBits) );
	code &= mask;

	// slot is encoded at 5 bits 47 ~ 43.
	// If code is 0, it must be a NOP instruction. And thus we cannot encode slot
	// number into its bits 47 ~ 43.
	if(code != 0)	code |= ((uint64_t)(slot) << 43);
}

void
InsnLine::EmitCode(raw_ostream &OS) {
	for(unsigned n=0; n < sumBytes; n++) {
		OS << codeBuf[n];
	}
}
