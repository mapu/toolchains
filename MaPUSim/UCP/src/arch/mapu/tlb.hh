#ifndef __ARCH_MAPU_TLB_HH__
#define __ARCH_MAPU_TLB_HH__

#include <map>

#include "arch/mapu/isa_traits.hh"
#include "arch/mapu/pagetable.hh"
#include "arch/mapu/utility.hh"
#include "arch/mapu/vtophys.hh"
#include "base/statistics.hh"
#include "mem/request.hh"
#include "params/MapuTLB.hh"
#include "sim/fault_fwd.hh"
#include "sim/sim_object.hh"
#include "sim/tlb.hh"

class ThreadContext;

/* MAPU does not distinguish between a DTLB and an ITLB -> unified TLB
   However, to maintain compatibility with other architectures, we'll
   simply create an ITLB and DTLB that will point to the real TLB */
namespace MapuISA {

class TLB : public BaseTLB
{
  protected:
    typedef std::multimap<Addr, int> PageTable;
    PageTable lookupTable;      // Quick lookup into page table

    MapuISA::PTE *table;        // the Page Table
    int size;                   // TLB Size
    int nlu;                    // not last used entry (for replacement)
    int cpu_id;

    void nextnlu() { if (++nlu >= size) nlu = 0; }
    MapuISA::PTE *lookup(Addr vpn, uint8_t asn) const;

    mutable Stats::Scalar read_hits;
    mutable Stats::Scalar read_misses;
    mutable Stats::Scalar read_acv;
    mutable Stats::Scalar read_accesses;
    mutable Stats::Scalar write_hits;
    mutable Stats::Scalar write_misses;
    mutable Stats::Scalar write_acv;
    mutable Stats::Scalar write_accesses;
    Stats::Formula hits;
    Stats::Formula misses;
    Stats::Formula accesses;

  public:
    typedef MapuTLBParams Params;
    TLB(const Params *p);

    int probeEntry(Addr vpn,uint8_t) const;
    MapuISA::PTE *getEntry(unsigned) const;
    virtual ~TLB();
    int smallPages;
    int getsize() const { return size; }

    MapuISA::PTE &index(bool advance = true);
    void insert(Addr vaddr, MapuISA::PTE &pte);
    void insertAt(MapuISA::PTE &pte, unsigned Index, int _smallPages);
    void flushAll();
    void demapPage(Addr vaddr, uint64_t asn)
    {
        panic("demapPage unimplemented.\n");
    }

    // static helper functions... really
    static bool validVirtualAddress(Addr vaddr);

    static Fault checkCacheability(RequestPtr &req);

    // Checkpointing
    void serialize(std::ostream &os);
    void unserialize(Checkpoint *cp, const std::string &section);

    void regStats();

    Fault translateAtomic(RequestPtr req, ThreadContext *tc, Mode mode);
    void translateTiming(RequestPtr req, ThreadContext *tc,
            Translation *translation, Mode mode);

    /** Function stub for CheckerCPU compilation issues. Mapu does not
     *  support the Checker model at the moment.
     */
    Fault translateFunctional(RequestPtr req, ThreadContext *tc, Mode mode);

  private:
    Fault translateInst(RequestPtr req, ThreadContext *tc);
    Fault translateMInst(RequestPtr req, ThreadContext *tc);
    Fault translateData(RequestPtr req, ThreadContext *tc, bool write);
    Fault translateMData(RequestPtr req, ThreadContext *tc);
};

}



#endif
