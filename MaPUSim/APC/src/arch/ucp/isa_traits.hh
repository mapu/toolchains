#ifndef __ARCH_UCP_ISA_TRAITS_HH__
#define __ARCH_UCP_ISA_TRAITS_HH__

#include "arch/ucp/types.hh"
#include "base/types.hh"
#include "cpu/static_inst_fwd.hh"

namespace LittleEndianGuest {}

namespace UcpISA
{

using namespace LittleEndianGuest;

StaticInstPtr decodeInst(ExtMachInst);

// Ucp DOES have a delay slot
#define ISA_HAS_DELAY_SLOT 0

const Addr PageShift = 13;
const Addr PageBytes = ULL(1) << PageShift;
const Addr Page_Mask = ~(PageBytes - 1);
const Addr PageOffset = PageBytes - 1;


////////////////////////////////////////////////////////////////////////
//
//  Translation stuff
//

//// All 'Mapped' segments go through the TLB
//// All other segments are translated by dropping the MSB, to give
//// the corresponding physical address
const Addr IMSegBase = ULL(0x0);
const Addr IMSegEnd = ULL(0x1FFFF);

const Addr MIMSegBase = ULL(0x200000);
const Addr MIMSegEnd = ULL(0x21FFFF);

const Addr DM0SDA0SegBase = ULL(0x400000);
const Addr DM0SDA0SegEnd = ULL(0x43FFFF);
const Addr DM1SDA0SegBase = ULL(0x600000);
const Addr DM1SDA0SegEnd = ULL(0x63FFFF);

const Addr DM2SDA1SegBase = ULL(0x800000);
const Addr DM2SDA1SegEnd = ULL(0x83FFFF);
const Addr DM3SDA1SegBase = ULL(0xA00000);
const Addr DM3SDA1SegEnd = ULL(0xA3FFFF);

const Addr DM4SDA2SegBase = ULL(0xC00000);
const Addr DM4SDA2SegEnd = ULL(0xC3FFFF);
const Addr DM5SDA2SegBase = ULL(0xE00000);
const Addr DM5SDA2SegEnd = ULL(0xE3FFFF);

const int LogVMPageSize = 13;       // 8K bytes
const int VMPageSize = (1 << LogVMPageSize);

const bool HasUnalignedMemAcc = false;
}
#endif // __ARCH_UCP_ISA_TRAITS_HH__
