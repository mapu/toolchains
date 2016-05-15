#ifndef __ARCH_UCP_LINUX_THREAD_INFO_H__
#define __ARCH_UCP_LINUX_THREAD_INFO_H__

#include "arch/ucp/linux/hwrpb.hh"

namespace Linux {
    struct thread_info {
        struct pcb_struct       pcb;
        Addr_a                  task;
    };
}

#endif // __ARCH_UCP_LINUX_THREAD_INFO_H__
