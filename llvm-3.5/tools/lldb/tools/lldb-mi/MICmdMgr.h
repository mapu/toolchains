//===-- MICmdMgr.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmdMgr.h
//
// Overview:    CMICmdMgr interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// Third party headers
#include <set>

// In-house headers:
#include "MICmnBase.h"
#include "MICmdBase.h"
#include "MICmdMgrSetCmdDeleteCallback.h"
#include "MIUtilSingletonBase.h"

// Declarations:
class CMICmdInterpreter;
class CMICmdFactory;
class CMICmdInvoker;
class CMICmdBase;

//++ ============================================================================
// Details: MI command manager. Oversees command operations, controls command
//          production and the running of commands.
//          Command Invoker, Command Factory and Command Monitor while independant
//          units are overseen/managed by *this manager.
//          A singleton class.
// Gotchas: None.
// Authors: Illya Rudkin 19/02/2014.
// Changes: None.
//--
class CMICmdMgr : public CMICmnBase, public MI::ISingleton<CMICmdMgr>
{
    friend class MI::ISingleton<CMICmdMgr>;

    // Methods:
  public:
    bool Initialize(void);
    bool Shutdown(void);

    bool CmdInterpret(const CMIUtilString &vTextLine, bool &vwbYesValid, bool &vwbCmdNotInCmdFactor, SMICmdData &rwCmdData);
    bool CmdExecute(const SMICmdData &vCmdData);
    bool CmdDelete(SMICmdData vCmdData);
    bool CmdRegisterForDeleteNotification(CMICmdMgrSetCmdDeleteCallback::ICallback &vObject);
    bool CmdUnregisterForDeleteNotification(CMICmdMgrSetCmdDeleteCallback::ICallback &vObject);

    // Methods:
  private:
    /* ctor */ CMICmdMgr(void);
    /* ctor */ CMICmdMgr(const CMICmdMgr &);
    void operator=(const CMICmdMgr &);

    // Overridden:
  public:
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdMgr(void);

    // Attributes:
  private:
    CMICmdInterpreter &m_interpretor;
    CMICmdFactory &m_factory;
    CMICmdInvoker &m_invoker;
    CMICmdMgrSetCmdDeleteCallback::CSetClients m_setCmdDeleteCallback;
};
