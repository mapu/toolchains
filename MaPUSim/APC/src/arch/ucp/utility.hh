#ifndef __ARCH_UCP_UTILITY_HH__
#define __ARCH_UCP_UTILITY_HH__

#include "arch/ucp/isa_traits.hh"
#include "arch/ucp/types.hh"
#include "base/misc.hh"
#include "base/types.hh"
#include "cpu/static_inst.hh"
#include "cpu/thread_context.hh"

class ThreadContext;

namespace UcpISA {

static inline bool
inUserMode(ThreadContext *tc) {
    return true;
}

inline PCState
buildRetPC(const PCState &curPC, const PCState &callPC)
{
    PCState ret = callPC;
    ret.advance();
    ret.pc(curPC.npc());
    return ret;
}

uint64_t getArgument(ThreadContext *tc, int &number, uint16_t size, bool fp);

////////////////////////////////////////////////////////////////////////
//
// Floating Point Utility Functions
//
uint64_t fpConvert(ConvertType cvt_type, double fp_val);
double roundFP(double val, int digits);
double truncFP(double val);

bool isNan(void *val_ptr, int size);
bool isQnan(void *val_ptr, int size);
bool isSnan(void *val_ptr, int size);

uint32_t CrtRsqrtSeed32(uint32_t crtRsqrtSeed32In);
uint64_t CrtRsqrtSeed64(uint64_t crtRsqrtSeed64In);
uint32_t recips(uint32_t recipsIn);
uint64_t recipdr(uint64_t recipdrIn);
uint32_t rsqrts(uint32_t rsqrtsIn);
uint64_t rsqrtdr(uint64_t rsqrtdrIn);
////////////////////////////////////////////////////////////////////////
//
//  Translation stuff
//
inline Addr
TruncPage(Addr addr)
{ return addr & ~(PageBytes - 1); }

inline Addr
RoundPage(Addr addr)
{ return (addr + PageBytes - 1) & ~(PageBytes - 1); }

////////////////////////////////////////////////////////////////////////
//
// CPU Utility
//
void startupCPU(ThreadContext *tc, int cpuId);
void initCPU(ThreadContext *tc, int cpuId);

void copyRegs(ThreadContext *src, ThreadContext *dest);
void copyMiscRegs(ThreadContext *src, ThreadContext *dest);

void skipFunction(ThreadContext *tc);

inline void
advancePC(PCState &pc, const StaticInstPtr inst)
{
	if (inst->isMCode()) pc.madvance();
	else pc.advance();
}

inline uint64_t
getExecutingAsid(ThreadContext *tc)
{
    return 0;
}

void crc_sim(unsigned int &t_crc, unsigned char data_in, bool crc_type);

};


#endif
