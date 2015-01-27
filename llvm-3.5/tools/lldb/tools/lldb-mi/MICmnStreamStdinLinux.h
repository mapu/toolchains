//===-- MICmnStreamStdinWindows.h --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MIUtilStreamStdin.h
//
// Overview:    CMICmnStreamStdinLinux interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// In-house headers:
#include "MICmnBase.h"
#include "MICmnStreamStdin.h"
#include "MIUtilSingletonBase.h"

//++ ============================================================================
// Details: MI common code class. Specific OS stdin handling implementation.
//          CMICmnStreamStdin instance is set with stdin handler before using the
//          the stream. An instance of this class must be set up and ready to give
//          to the CMICmnStreamStdin before it initialises other CMICmnStreamStdin
//          will give an error.
// Gotchas: None.
// Authors: Illya Rudkin 16/06/2014.
// Changes: None.
//--
class CMICmnStreamStdinLinux : public CMICmnBase, public CMICmnStreamStdin::IOSStdinHandler, public MI::ISingleton<CMICmnStreamStdinLinux>
{
    // Give singleton access to private constructors
    friend MI::ISingleton<CMICmnStreamStdinLinux>;

    // Methods:
  public:
    bool Initialize(void);
    bool Shutdown(void);

    // Overridden:
  public:
    // From CMICmnStreamStdin::IOSpecificReadStreamStdin
    virtual bool InputAvailable(bool &vwbAvail);
    virtual const MIchar *ReadLine(CMIUtilString &vwErrMsg);
    virtual void InterruptReadLine(void);

    // Methods:
  private:
    /* ctor */ CMICmnStreamStdinLinux(void);
    /* ctor */ CMICmnStreamStdinLinux(const CMICmnStreamStdin &);
    void operator=(const CMICmnStreamStdin &);

    // Overridden:
  private:
    // From CMICmnBase
    /* dtor */ virtual ~CMICmnStreamStdinLinux(void);

    // Attributes:
  private:
    const MIuint m_constBufferSize;
    FILE *m_pStdin;
    MIchar *m_pCmdBuffer;
};
