#ifndef __ARCH_UCP_LINUX_HWRPB_HH__
#define __ARCH_UCP_LINUX_HWRPB_HH__

#include "arch/ucp/linux/aligned.hh"

namespace Linux {
    struct pcb_struct {
        uint64_ta rpb_ksp;
        uint64_ta rpb_usp;
        uint64_ta rpb_ptbr;
        uint32_t rpb_cc;
        uint32_t rpb_psn;
        uint64_ta rpb_unique;
        uint64_ta rpb_fen;
        uint64_ta res1, res2;
    };
}
#endif // __ARCH_UCP_LINUX_HWRPB_HH__
