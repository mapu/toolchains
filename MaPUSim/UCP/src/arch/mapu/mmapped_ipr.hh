#ifndef __ARCH_MAPU_MMAPPED_IPR_HH__
#define __ARCH_MAPU_MMAPPED_IPR_HH__

/**
 * @file
 *
 * ISA-specific helper functions for memory mapped IPR accesses.
 */

#include "base/misc.hh"
#include "mem/packet.hh"

class ThreadContext;

namespace MapuISA
{

inline Tick
handleIprRead(ThreadContext *xc, Packet *pkt)
{
    panic("No implementation for handleIprRead in Mapu\n");
}

inline Tick
handleIprWrite(ThreadContext *xc, Packet *pkt)
{
    panic("No implementation for handleIprWrite in Mapu\n");
}

} // namespace MapuISA

#endif
