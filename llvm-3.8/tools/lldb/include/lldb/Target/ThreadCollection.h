//===-- ThreadCollection.h --------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ThreadCollection_h_
#define liblldb_ThreadCollection_h_

#include <vector>

#include "lldb/lldb-private.h"
#include "lldb/Host/Mutex.h"
#include "lldb/Utility/Iterable.h"

namespace lldb_private {
    
class ThreadCollection
{
public:
    typedef std::vector<lldb::ThreadSP> collection;
    typedef LockingAdaptedIterable<collection, lldb::ThreadSP, vector_adapter> ThreadIterable;
    
    ThreadCollection();
    
    ThreadCollection(collection threads);
    
    virtual
    ~ThreadCollection()
    {
    }
    
    uint32_t
    GetSize();
    
    void
    AddThread (const lldb::ThreadSP &thread_sp);
    
    void
    InsertThread (const lldb::ThreadSP &thread_sp, uint32_t idx);
    
    // Note that "idx" is not the same as the "thread_index". It is a zero
    // based index to accessing the current threads, whereas "thread_index"
    // is a unique index assigned
    lldb::ThreadSP
    GetThreadAtIndex (uint32_t idx);

    virtual ThreadIterable
    Threads ()
    {
        return ThreadIterable(m_threads, GetMutex());
    }
    
    virtual Mutex &
    GetMutex()
    {
        return m_mutex;
    }
    
protected:
    collection m_threads;
    Mutex m_mutex;
};
    
} // namespace lldb_private

#endif  // liblldb_ThreadCollection_h_
