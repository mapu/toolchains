#ifndef __ARCH_UCP_VTOPHYS_HH__
#define __ARCH_UCP_VTOPHYS_HH__

#include "arch/ucp/isa_traits.hh"
#include "arch/ucp/utility.hh"

class ThreadContext;

namespace UcpISA {

    Addr vtophys(Addr vaddr);
    Addr vtophys(ThreadContext *tc, Addr vaddr);
    Addr vtophys(int cpuid, Addr addr);
} // namespace UcpISA

#endif // __ARCH_UCP_VTOPHYS_HH__

