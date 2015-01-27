//===-- MICmnStreamStderr.h -------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmnStreamStderr.h
//
// Overview:    CMICmnStreamStderr interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// In-house headers:
#include "MIUtilString.h"
#include "MICmnBase.h"
#include "MIUtilSingletonBase.h"
#include "MIUtilThreadBaseStd.h"

//++ ============================================================================
// Details: MI common code class. The MI driver requires this object.
//          CMICmnStreamStderr sets up and tears downs stderr for the driver.
//
//          Singleton class.
// Gotchas: None.
// Authors: Illya Rudkin 19/03/2014.
// Changes: None.
//--
class CMICmnStreamStderr : public CMICmnBase, public MI::ISingleton<CMICmnStreamStderr>
{
    friend class MI::ISingleton<CMICmnStreamStderr>;

    // Statics:
  public:
    static bool TextToStderr(const CMIUtilString &vrTxt);
    static bool LLDBMsgToConsole(const CMIUtilString &vrTxt);

    // Methods:
  public:
    bool Initialize(void);
    bool Shutdown(void);
    //
    bool Lock(void);
    bool Unlock(void);
    bool Write(const CMIUtilString &vText, const bool vbSendToLog = true);
    bool WriteLLDBMsg(const CMIUtilString &vText, const bool vbSendToLog = true);

    // Methods:
  private:
    /* ctor */ CMICmnStreamStderr(void);
    /* ctor */ CMICmnStreamStderr(const CMICmnStreamStderr &);
    void operator=(const CMICmnStreamStderr &);
    //
    bool WritePriv(const CMIUtilString &vText, const CMIUtilString &vTxtForLogFile, const bool vbSendToLog = true);

    // Overridden:
  private:
    // From CMICmnBase
    /* dtor */ virtual ~CMICmnStreamStderr(void);

    // Attributes:
  private:
    CMIUtilThreadMutex m_mutex; // Mutex object for sync during Write()
};
