#ifndef __ARCH_UCP_PAGETABLE_H__
#define __ARCH_UCP_PAGETABLE_H__

#include "base/misc.hh"
#include "base/types.hh"
#include "sim/serialize.hh"

namespace UcpISA {

struct VAddr
{
};

// ITB/DTB page table entry
struct PTE
{
    Addr Mask;
    Addr VPN;
    uint8_t asid;

    bool G;

    /* Contents of Entry Lo0 */
    Addr PFN0;  // Physical Frame Number - Even
    bool D0;    // Even entry Dirty Bit
    bool V0;    // Even entry Valid Bit
    uint8_t C0; // Cache Coherency Bits - Even

    /* Contents of Entry Lo1 */
    Addr PFN1;  // Physical Frame Number - Odd
    bool D1;    // Odd entry Dirty Bit
    bool V1;    // Odd entry Valid Bit
    uint8_t C1; // Cache Coherency Bits (3 bits)

    /* 
     * The next few variables are put in as optimizations to reduce
     * TLB lookup overheads. For a given Mask, what is the address shift
     * amount, and what is the OffsetMask
     */
    int AddrShiftAmount;
    int OffsetMask;

    bool Valid() { return (V0 | V1); };
    void serialize(std::ostream &os);
    void unserialize(Checkpoint *cp, const std::string &section);
};

// WARN: This particular TLB entry is not necessarily conformed to Ucp ISA
struct TlbEntry
{
    Addr _pageStart;
    TlbEntry() {}
    TlbEntry(Addr asn, Addr vaddr, Addr paddr) : _pageStart(paddr) {}

    Addr pageStart()
    {
        return _pageStart;
    }

    void
    updateVaddr(Addr new_vaddr) {}

    void serialize(std::ostream &os)
    {
        SERIALIZE_SCALAR(_pageStart);
    }

    void unserialize(Checkpoint *cp, const std::string &section)
    {
        UNSERIALIZE_SCALAR(_pageStart);
    }

};

};
#endif // __ARCH_UCP_PAGETABLE_H__

