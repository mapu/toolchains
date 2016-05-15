#include <string>

#include "arch/ucp/vtophys.hh"
#include "base/chunk_generator.hh"
#include "base/trace.hh"
#include "cpu/thread_context.hh"
#include "cpu/base.hh"
#include "debug/VtoPhys.hh"
using namespace std;

Addr
UcpISA::vtophys(Addr vaddr)
{
    fatal("vtophys: Unimplemented on UCP\n");
}

Addr
UcpISA::vtophys(ThreadContext *tc, Addr addr) {
  return addr & 0xFF000000 ? addr : addr | (tc->getCpuPtr()->cpuId() << 24);
}

Addr
UcpISA::vtophys(int cpuid, Addr addr) {
  return addr & 0xFF000000 ? addr : addr | (cpuid << 24);
}

