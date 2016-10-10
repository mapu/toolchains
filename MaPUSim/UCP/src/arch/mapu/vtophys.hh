#ifndef __ARCH_MAPU_VTOPHYS_HH__
#define __ARCH_MAPU_VTOPHYS_HH__

#include "arch/mapu/isa_traits.hh"
#include "arch/mapu/utility.hh"

class ThreadContext;

namespace MapuISA {

    Addr vtophys(Addr vaddr);
    Addr vtophys(ThreadContext *tc, Addr vaddr);
    Addr vtophys(int cpuid, Addr addr);
} // namespace MapuISA

#endif // __ARCH_MAPU_VTOPHYS_HH__

