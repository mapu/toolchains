#ifndef __ARCH_UCP_KERNEL_STATS_HH__
#define __ARCH_UCP_KERNEL_STATS_HH__

#include "kern/kernel_stats.hh"

namespace UcpISA {
namespace Kernel {

enum cpu_mode { hypervisor, kernel, user, idle, cpu_mode_num };
extern const char *modestr[];

class Statistics : public ::Kernel::Statistics
{
  public:
    Statistics(System *system) : ::Kernel::Statistics(system)
    {}
};


} // namespace UcpISA::Kernel
} // namespace UcpISA

#endif // __ARCH_UCP_KERNEL_STATS_HH__
