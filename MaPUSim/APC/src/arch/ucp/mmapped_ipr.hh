#ifndef __ARCH_UCP_MMAPPED_IPR_HH__
#define __ARCH_UCP_MMAPPED_IPR_HH__

/**
 * @file
 *
 * ISA-specific helper functions for memory mapped IPR accesses.
 */

#include "base/misc.hh"
#include "mem/packet.hh"

class ThreadContext;

namespace UcpISA
{

inline Tick
handleIprRead(ThreadContext *xc, Packet *pkt)
{
    panic("No implementation for handleIprRead in Ucp\n");
}

inline Tick
handleIprWrite(ThreadContext *xc, Packet *pkt)
{
    panic("No implementation for handleIprWrite in Ucp\n");
}

} // namespace UcpISA

#endif
