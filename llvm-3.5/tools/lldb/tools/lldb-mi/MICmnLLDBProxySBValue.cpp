//===-- MICmnLLDBProxySBValue.cpp -------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmnLLDBProxySBValue.cpp
//
// Overview:    CMICmnLLDBProxySBValue implementation.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#include <stdlib.h>

// Third Party Headers:
#include <lldb/API/SBError.h>

// In-house headers:
#include "MICmnLLDBProxySBValue.h"
#include "MIUtilString.h"
#include "MICmnLLDBDebugSessionInfo.h"

//++ ------------------------------------------------------------------------------------
// Details: Retrieve the numerical value from the SBValue object. If the function fails
//          it could indicate the SBValue object does not represent an internal type.
// Type:    Static method.
// Args:    vrValue - (R) The SBValue object to get a value from.
//          vwValue - (W) The numerical value.
// Return:  MIstatus::success - Functionality succeeded.
//          MIstatus::failure - Functionality failed.
// Throws:  None.
//--
bool
CMICmnLLDBProxySBValue::GetValueAsUnsigned(const lldb::SBValue &vrValue, MIuint64 &vwValue)
{
    lldb::SBValue &rValue = const_cast<lldb::SBValue &>(vrValue);
    bool bCompositeType = true;
    MIuint64 nFailValue = 0;
    MIuint64 nValue = rValue.GetValueAsUnsigned(nFailValue);
    if (nValue == nFailValue)
    {
        nFailValue = 5; // Some arbitary number
        nValue = rValue.GetValueAsUnsigned(nFailValue);
        if (nValue != nFailValue)
        {
            bCompositeType = false;
            vwValue = nValue;
        }
    }
    else
    {
        bCompositeType = false;
        vwValue = nValue;
    }

    return (bCompositeType ? MIstatus::failure : MIstatus::success);
}

//++ ------------------------------------------------------------------------------------
// Details: Retrieve the numerical value from the SBValue object. If the function fails
//          it could indicate the SBValue object does not represent an internal type.
// Type:    Static method.
// Args:    vrValue - (R) The SBValue object to get a value from.
//          vwValue - (W) The numerical value.
// Return:  MIstatus::success - Functionality succeeded.
//          MIstatus::failure - Functionality failed.
// Throws:  None.
//--
bool
CMICmnLLDBProxySBValue::GetValueAsSigned(const lldb::SBValue &vrValue, MIint64 &vwValue)
{
    lldb::SBValue &rValue = const_cast<lldb::SBValue &>(vrValue);
    bool bCompositeType = true;
    MIuint64 nFailValue = 0;
    MIuint64 nValue = rValue.GetValueAsSigned(nFailValue);
    if (nValue == nFailValue)
    {
        nFailValue = 5; // Some arbitary number
        nValue = rValue.GetValueAsSigned(nFailValue);
        if (nValue != nFailValue)
        {
            bCompositeType = false;
            vwValue = nValue;
        }
    }
    else
    {
        bCompositeType = false;
        vwValue = nValue;
    }

    return (bCompositeType ? MIstatus::failure : MIstatus::success);
}

//++ ------------------------------------------------------------------------------------
// Details: Retrieve the NUL terminated string from the SBValue object if it of the type
//          unsigned char *.
// Type:    Static method.
// Args:    vrValue     - (R) The SBValue object to get a value from.
//          vwCString   - (W) The text data '\0' terminated.
// Return:  MIstatus::success - Functionality succeeded.
//          MIstatus::failure - Functionality failed, not suitable type.
// Throws:  None.
//--
bool
CMICmnLLDBProxySBValue::GetCString(const lldb::SBValue &vrValue, CMIUtilString &vwCString)
{
    lldb::SBValue &rValue = const_cast<lldb::SBValue &>(vrValue);
    const MIchar *pCType = rValue.GetTypeName();
    if (pCType == nullptr)
        return MIstatus::failure;

    const MIchar *pType = "unsigned char *";
    if (!CMIUtilString::Compare(pCType, pType))
        return MIstatus::failure;

    const CMIUtilString strAddr(rValue.GetValue());
    MIint64 nNum = 0;
    if (!strAddr.ExtractNumber(nNum))
        return MIstatus::failure;

    CMICmnLLDBDebugSessionInfo &rSessionInfo(CMICmnLLDBDebugSessionInfo::Instance());
    lldb::SBProcess &rProcess = rSessionInfo.m_lldbProcess;
    MIuint nBufferSize = 64;
    bool bNeedResize = false;
    MIchar *pBuffer = static_cast<MIchar *>(::malloc(nBufferSize));
    do
    {
        lldb::SBError error;
        const size_t nReadSize = rProcess.ReadCStringFromMemory((lldb::addr_t)nNum, pBuffer, nBufferSize, error);
        if (nReadSize == (nBufferSize - 1))
        {
            bNeedResize = true;
            nBufferSize = nBufferSize << 1;
            pBuffer = static_cast<MIchar *>(::realloc(pBuffer, nBufferSize));
        }
        else
            bNeedResize = false;
    } while (bNeedResize);

    vwCString = pBuffer;
    free((void *)pBuffer);

    return MIstatus::success;
}
