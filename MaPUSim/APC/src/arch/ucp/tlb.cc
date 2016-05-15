#include <string>
#include <vector>

#include "arch/ucp/faults.hh"
#include "arch/ucp/pagetable.hh"
#include "arch/ucp/tlb.hh"
#include "arch/ucp/utility.hh"
#include "base/inifile.hh"
#include "base/str.hh"
#include "base/trace.hh"
#include "base/loader/mapu.hh"
#include "cpu/thread_context.hh"
#include "cpu/base.hh"
#include "debug/UcpPRA.hh"
#include "debug/TLB.hh"
#include "mem/page_table.hh"
#include "params/UcpTLB.hh"
#include "sim/process.hh"

using namespace std;
using namespace UcpISA;

///////////////////////////////////////////////////////////////////////
//
//  Ucp TLB
//

TLB::TLB(const Params *p)
    : BaseTLB(p), size(p->size), nlu(0), cpu_id(p->cpuid) {
  table = new PTE[size];
  memset(table, 0, sizeof(PTE[size]));
  smallPages = 0;
}

TLB::~TLB() {
  if (table) delete[] table;
}

// look up an entry in the TLB
UcpISA::PTE *
TLB::lookup(Addr vpn, uint8_t asn) const {
  // assume not found...
  PTE *retval = &table[0];
  if (!((vpn >= IMSegBase && vpn + asn - 1 <= IMSegEnd)
      || (vpn >= DM0SDA0SegBase && vpn + asn - 1 <= DM0SDA0SegEnd)
      || (vpn >= DM1SDA0SegBase && vpn + asn - 1 <= DM1SDA0SegEnd)
      || (vpn >= DM2SDA1SegBase && vpn + asn - 1 <= DM2SDA1SegEnd)
      || (vpn >= DM3SDA1SegBase && vpn + asn - 1 <= DM3SDA1SegEnd)
      || (vpn >= DM4SDA2SegBase && vpn + asn - 1 <= DM4SDA2SegEnd)
      || (vpn >= DM5SDA2SegBase && vpn + asn - 1 <= DM5SDA2SegEnd)
      || (vpn >= SHMSTART && vpn + asn - 1 <= SHMEND)))
    return NULL;

  return retval;
}

UcpISA::PTE*
TLB::getEntry(unsigned Index) const {
  // Make sure that Index is valid
  assert(Index < size);
  return &table[Index];
}

int TLB::probeEntry(Addr vpn, uint8_t asn) const {
  UcpISA::PTE* p = lookup(vpn, asn);
  return (p != NULL);
}

inline Fault TLB::checkCacheability(RequestPtr &req) {
  return NoFault;
}

void TLB::insertAt(PTE &pte, unsigned Index, int _smallPages) {
}

// insert a new TLB entry
void TLB::insert(Addr addr, PTE &pte) {
  fatal("TLB Insert not yet implemented\n");
}

void TLB::flushAll() {
  DPRINTF(TLB, "flushAll\n");
  memset(table, 0, sizeof(PTE[size]));
  lookupTable.clear();
  nlu = 0;
}

void TLB::serialize(ostream &os) {
  SERIALIZE_SCALAR(size);
  SERIALIZE_SCALAR(nlu);

  for (int i = 0; i < size; i++) {
    nameOut(os, csprintf("%s.PTE%d", name(), i));
    table[i].serialize(os);
  }
}

void TLB::unserialize(Checkpoint *cp, const string &section) {
  UNSERIALIZE_SCALAR(size);
  UNSERIALIZE_SCALAR(nlu);

  for (int i = 0; i < size; i++) {
    table[i].unserialize(cp, csprintf("%s.PTE%d", section, i));
    if (table[i].V0 || table[i].V1) {
      lookupTable.insert(make_pair(table[i].VPN, i));
    }
  }
}

void TLB::regStats() {
  read_hits.name(name() + ".read_hits").desc("DTB read hits");

  read_misses.name(name() + ".read_misses").desc("DTB read misses");

  read_accesses.name(name() + ".read_accesses").desc("DTB read accesses");

  write_hits.name(name() + ".write_hits").desc("DTB write hits");

  write_misses.name(name() + ".write_misses").desc("DTB write misses");

  write_accesses.name(name() + ".write_accesses").desc("DTB write accesses");

  hits.name(name() + ".hits").desc("DTB hits");

  misses.name(name() + ".misses").desc("DTB misses");

  accesses.name(name() + ".accesses").desc("DTB accesses");

  hits = read_hits + write_hits;
  misses = read_misses + write_misses;
  accesses = read_accesses + write_accesses;
}

Fault TLB::translateInst(RequestPtr req, ThreadContext *tc) {
  if (req->getVaddr() < IMSegBase || req->getVaddr() > IMSegEnd)
    panic("SPU fetching address is out of IM range.\n");
  req->setPaddr(TheISA::vtophys(cpu_id, req->getVaddr()));

  return NoFault;
}

Fault TLB::translateMInst(RequestPtr req, ThreadContext *tc) {
  if (req->getVaddr() < 0 || req->getVaddr() > MIMSegEnd - MIMSegBase)
    panic("MPU fetching address is out of MIM range.\n");
  req->setPaddr(TheISA::vtophys(cpu_id, req->getVaddr() + MIMSegBase));
  return NoFault;
}

Fault TLB::translateMData(RequestPtr req, ThreadContext *tc) {
  Addr *EA = (Addr *) (req->getVaddr());
  DPRINTF(TLB, "BIU is accessing address %#08x.\n", EA[0]);
  if (EA[0] != 0 && !((EA[0] >= DM0SDA0SegBase && EA[0] <= DM0SDA0SegEnd)
      || (EA[0] >= DM1SDA0SegBase && EA[0] <= DM1SDA0SegEnd)
      || (EA[0] >= DM2SDA1SegBase && EA[0] <= DM2SDA1SegEnd)
      || (EA[0] >= DM3SDA1SegBase && EA[0] <= DM3SDA1SegEnd)
      || (EA[0] >= DM4SDA2SegBase && EA[0] <= DM4SDA2SegEnd)
      || (EA[0] >= DM5SDA2SegBase && EA[0] <= DM5SDA2SegEnd)))
    panic("BIU accessing address %#08x is out of DM range.\n", EA[0]);
  for (unsigned i = 0; i <= 64; i++)
    if (EA[i] != 0) EA[i] = TheISA::vtophys(cpu_id, EA[i]);
  req->setPaddr(Addr(EA));
  return NoFault;
}

Fault TLB::translateData(RequestPtr req, ThreadContext *tc, bool write) {
  if (!((req->getVaddr() >= IMSegBase && req->getVaddr() <= IMSegEnd)
      || (req->getVaddr() >= DM0SDA0SegBase && req->getVaddr() <= DM0SDA0SegEnd)
      || (req->getVaddr() >= DM1SDA0SegBase && req->getVaddr() <= DM1SDA0SegEnd)
      || (req->getVaddr() >= DM2SDA1SegBase && req->getVaddr() <= DM2SDA1SegEnd)
      || (req->getVaddr() >= DM3SDA1SegBase && req->getVaddr() <= DM3SDA1SegEnd)
      || (req->getVaddr() >= DM4SDA2SegBase && req->getVaddr() <= DM4SDA2SegEnd)
      || (req->getVaddr() >= DM5SDA2SegBase && req->getVaddr() <= DM5SDA2SegEnd)
      || (req->getVaddr() >= SHMSTART && req->getVaddr() <= SHMEND)
      || (req->getVaddr() >= 0x80000000 && req->getVaddr() <= 0x80000FFF)))
    panic("Load/Store address is out of IM and DM range.\n");
  req->setPaddr(TheISA::vtophys(cpu_id, req->getVaddr()));

  return NoFault;
}

Fault TLB::translateAtomic(RequestPtr req, ThreadContext *tc, Mode mode) {
  Fault fault;
  if (mode == Execute) fault = translateInst(req, tc);
  else if (mode == MExecute) fault = translateMInst(req, tc);
  else if (mode == MAccess) fault = translateMData(req, tc);
  else fault = translateData(req, tc, mode == Write);
  return fault;
}

void TLB::translateTiming(RequestPtr req, ThreadContext *tc,
                          Translation *translation, Mode mode) {
  assert(translation);
  translation->finish(translateAtomic(req, tc, mode), req, tc, mode);
}

Fault TLB::translateFunctional(RequestPtr req, ThreadContext *tc, Mode mode) {
  panic("Not implemented\n");
  return NoFault;
}

UcpISA::PTE &
TLB::index(bool advance) {
  PTE *pte = &table[nlu];

  if (advance) nextnlu();

  return *pte;
}

UcpISA::TLB *
UcpTLBParams::create() {
  return new TLB(this);
}
