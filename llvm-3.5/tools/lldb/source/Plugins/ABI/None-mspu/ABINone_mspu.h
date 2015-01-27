//===-- ABINone_mspu.h ----------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef liblldb_ABINone_mspu_h_

#define liblldb_ABINone_mspu_h_

// C Includes
// C++ Includes
// Other libraries and framework includes
// Project includes
#include "lldb/lldb-private.h"
#include "lldb/Target/ABI.h"
#include "lldb/Core/Value.h"
    
class ABINone_mspu :
    public lldb_private::ABI
{
public:

    ~ABINone_mspu() { }
    
    virtual size_t 
    GetRedZoneSize () const;
    
    virtual bool
    PrepareTrivialCall (lldb_private::Thread &thread, 
                        lldb::addr_t sp,
                        lldb::addr_t func_addr,
                        lldb::addr_t return_addr, 
                        llvm::ArrayRef<lldb::addr_t> args) const;
    
    virtual bool
    PrepareNormalCall (lldb_private::Thread &thread,
                       lldb::addr_t sp,
                       lldb::addr_t func_addr,
                       lldb::addr_t return_addr,
                       lldb_private::ValueList &args) const;
    
    virtual bool
    GetArgumentValues (lldb_private::Thread &thread,
                       lldb_private::ValueList &values) const;
    
    virtual lldb_private::Error
    SetReturnValueObject(lldb::StackFrameSP &frame_sp, lldb::ValueObjectSP &new_value);

protected:
    virtual lldb::ValueObjectSP
    GetReturnValueObjectImpl (lldb_private::Thread &thread,
                    lldb_private::ClangASTType &ast_type) const;

public:

    virtual bool
    CreateFunctionEntryUnwindPlan (lldb_private::UnwindPlan &unwind_plan);
    
    virtual bool
    CreateDefaultUnwindPlan (lldb_private::UnwindPlan &unwind_plan);
    
    virtual bool
    RegisterIsVolatile (const lldb_private::RegisterInfo *reg_info);
    
    virtual bool
    StackUsesFrames ()
    {
        return true;
    }
    
    virtual bool
    CallFrameAddressIsValid (lldb::addr_t cfa)
    {
        // Make sure the stack call frame addresses are are 4 byte aligned
        if (cfa & (4ull - 1ull))
            return false;   // Not 4 byte aligned

        if (cfa == 0)
            return false;   // Zero is not a valid stack address

        return true;
    }

    virtual bool
    CodeAddressIsValid (lldb::addr_t pc)
    {
        // Just make sure the address is a valid 32 bit address. 
        return pc <= UINT32_MAX;
    }

    virtual bool
    FunctionCallsChangeCFA ()
    {
        return true;
    }

    virtual const lldb_private::RegisterInfo *
    GetRegisterInfoArray (uint32_t &count);

    //------------------------------------------------------------------
    // Static Functions
    //------------------------------------------------------------------
    static void
    Initialize();
    
    static void
    Terminate();
    
    static lldb::ABISP
    CreateInstance (const lldb_private::ArchSpec &arch);
    
    //------------------------------------------------------------------
    // PluginInterface protocol
    //------------------------------------------------------------------
    static lldb_private::ConstString
    GetPluginNameStatic ();

    virtual lldb_private::ConstString
    GetPluginName();
    
    virtual uint32_t
    GetPluginVersion();
    
protected:
    bool
    RegisterIsCalleeSaved (const lldb_private::RegisterInfo *reg_info);

private:
    ABINone_mspu() : lldb_private::ABI()
    { } // Call CreateInstance instead.

};

#endif  // liblldb_ABINone_mspu_h_
