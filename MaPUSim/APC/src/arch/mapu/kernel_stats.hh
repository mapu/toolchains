#ifndef __ARCH_MAPU_KERNEL_STATS_HH__
#define __ARCH_MAPU_KERNEL_STATS_HH__

#include "kern/kernel_stats.hh"

namespace MapuISA {
namespace Kernel {

enum cpu_mode { hypervisor, kernel, user, idle, cpu_mode_num };
extern const char *modestr[];

class Statistics : public ::Kernel::Statistics
{
  public:
    Statistics(System *system) : ::Kernel::Statistics(system)
    {}
};


} // namespace MapuISA::Kernel
} // namespace MapuISA

#endif // __ARCH_MAPU_KERNEL_STATS_HH__
