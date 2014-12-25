#ifndef __ARCH_MAPU_LOCKED_MEM_HH__
#define __ARCH_MAPU_LOCKED_MEM_HH__

/**
 * @file
 *
 * ISA-specific helper functions for locked memory accesses.
 */

#include "mem/request.hh"

namespace MapuISA
{
template <class XC>
inline void
handleLockedRead(XC *xc, Request *req)
{
}


template <class XC>
inline bool
handleLockedWrite(XC *xc, Request *req)
{
    req->setExtraData(2);
    return true;
}


} // namespace MapuISA

#endif
