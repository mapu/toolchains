#ifndef __ARCH_MAPU_LINUX_THREAD_INFO_H__
#define __ARCH_MAPU_LINUX_THREAD_INFO_H__

#include "arch/mapu/linux/hwrpb.hh"

namespace Linux {
    struct thread_info {
        struct pcb_struct       pcb;
        Addr_a                  task;
    };
}

#endif // __ARCH_MAPU_LINUX_THREAD_INFO_H__
