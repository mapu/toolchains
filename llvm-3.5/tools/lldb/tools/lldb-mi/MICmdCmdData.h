//===-- MICmdCmdData.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmdCmdData.h
//
// Overview:    CMICmdCmdDataEvaluateExpression     interface.
//              CMICmdCmdDataDisassemble            interface.
//              CMICmdCmdDataReadMemoryBytes        interface.
//              CMICmdCmdDataReadMemory             interface.
//              CMICmdCmdDataListRegisterNames      interface.
//              CMICmdCmdDataListRegisterValues     interface.
//              CMICmdCmdDataListRegisterChanged    interface.
//              CMICmdCmdDataWriteMemoryBytes       interface.
//              CMICmdCmdDataWriteMemory            interface.
//
//              To implement new MI commands derive a new command class from the command base
//              class. To enable the new command for interpretation add the new command class
//              to the command factory. The files of relevance are:
//                  MICmdCommands.cpp
//                  MICmdBase.h / .cpp
//                  MICmdCmd.h / .cpp
//              For an introduction to adding a new command see CMICmdCmdSupportInfoMiCmdQuery
//              command class as an example.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// In-house headers:
#include "MICmdBase.h"
#include "MICmnMIValueTuple.h"
#include "MICmnMIValueList.h"

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-evaluate-expression".
// Gotchas: None.
// Authors: Illya Rudkin 26/03/2014.
// Changes: None.
//--
class CMICmdCmdDataEvaluateExpression : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataEvaluateExpression(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataEvaluateExpression(void);

    // Methods:
  private:
    bool HaveInvalidCharacterInExpression(const CMIUtilString &vrExpr, MIchar &vrwInvalidChar);

    // Attributes:
  private:
    bool m_bExpressionValid;     // True = yes is valid, false = not valid
    bool m_bEvaluatedExpression; // True = yes is expression evaluated, false = failed
    CMIUtilString m_strValue;
    CMICmnMIValueTuple m_miValueTuple;
    bool m_bCompositeVarType; // True = yes composite type, false = internal type
    bool m_bFoundInvalidChar; // True = yes found unexpected character in the expression, false = all ok
    MIchar m_cExpressionInvalidChar;
    const CMIUtilString m_constStrArgThread; // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgFrame;  // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgExpr;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-disassemble".
// Gotchas: None.
// Authors: Illya Rudkin 19/05/2014.
// Changes: None.
//--
class CMICmdCmdDataDisassemble : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataDisassemble(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataDisassemble(void);

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThread;    // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgAddrStart; // MI spec non mandatory, *this command mandatory
    const CMIUtilString m_constStrArgAddrEnd;   // MI spec non mandatory, *this command mandatory
    const CMIUtilString m_constStrArgConsume;
    const CMIUtilString m_constStrArgMode;
    CMICmnMIValueList m_miValueList;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-read-memory-bytes".
// Gotchas: None.
// Authors: Illya Rudkin 20/05/2014.
// Changes: None.
//--
class CMICmdCmdDataReadMemoryBytes : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataReadMemoryBytes(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataReadMemoryBytes(void);

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThread; // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgByteOffset;
    const CMIUtilString m_constStrArgAddrStart;
    const CMIUtilString m_constStrArgNumBytes;
    MIuchar *m_pBufferMemory;
    MIuint64 m_nAddrStart;
    MIuint64 m_nAddrNumBytesToRead;
    MIuint64 m_nAddrOffset;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-read-memory".
// Gotchas: None.
// Authors: Illya Rudkin 21/05/2014.
// Changes: None.
//--
class CMICmdCmdDataReadMemory : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataReadMemory(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataReadMemory(void);
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-list-register-names".
// Gotchas: None.
// Authors: Illya Rudkin 21/05/2014.
// Changes: None.
//--
class CMICmdCmdDataListRegisterNames : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataListRegisterNames(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataListRegisterNames(void);

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThreadGroup; // Not specified in MI spec but Eclipse gives this option
    const CMIUtilString m_constStrArgRegNo;       // Not handled by *this command
    CMICmnMIValueList m_miValueList;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-list-register-values".
// Gotchas: None.
// Authors: Illya Rudkin 21/05/2014.
// Changes: None.
//--
class CMICmdCmdDataListRegisterValues : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataListRegisterValues(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataListRegisterValues(void);

    // Methods:
  private:
    lldb::SBValue GetRegister(const MIuint vRegisterIndex) const;

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThread; // Not specified in MI spec but Eclipse gives this option
    const CMIUtilString m_constStrArgSkip;   // Not handled by *this command
    const CMIUtilString m_constStrArgFormat;
    const CMIUtilString m_constStrArgRegNo;
    CMICmnMIValueList m_miValueList;
    lldb::SBProcess *m_pProcess;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-list-changed-registers".
// Gotchas: None.
// Authors: Illya Rudkin 22/05/2014.
// Changes: None.
//--
class CMICmdCmdDataListRegisterChanged : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataListRegisterChanged(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataListRegisterChanged(void);
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-read-memory-bytes".
// Gotchas: None.
// Authors: Illya Rudkin 30/05/2014.
// Changes: None.
//--
class CMICmdCmdDataWriteMemoryBytes : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataWriteMemoryBytes(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataWriteMemoryBytes(void);

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThread; // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgAddr;
    const CMIUtilString m_constStrArgContents;
    const CMIUtilString m_constStrArgCount;
    CMIUtilString m_strContents;
};

//++ ============================================================================
// Details: MI command class. MI commands derived from the command base class.
//          *this class implements MI command "data-read-memory".
//          Not specified in MI spec but Eclipse gives *this command.
// Gotchas: None.
// Authors: Illya Rudkin 02/05/2014.
// Changes: None.
//--
class CMICmdCmdDataWriteMemory : public CMICmdBase
{
    // Statics:
  public:
    // Required by the CMICmdFactory when registering *this command
    static CMICmdBase *CreateSelf(void);

    // Methods:
  public:
    /* ctor */ CMICmdCmdDataWriteMemory(void);

    // Overridden:
  public:
    // From CMICmdInvoker::ICmd
    virtual bool Execute(void);
    virtual bool Acknowledge(void);
    virtual bool ParseArgs(void);
    // From CMICmnBase
    /* dtor */ virtual ~CMICmdCmdDataWriteMemory(void);

    // Attributes:
  private:
    const CMIUtilString m_constStrArgThread;   // Not specified in MI spec but Eclipse gives this option. Not handled by command.
    const CMIUtilString m_constStrArgOffset;   // Not specified in MI spec but Eclipse gives this option.
    const CMIUtilString m_constStrArgAddr;     // Not specified in MI spec but Eclipse gives this option.
    const CMIUtilString m_constStrArgD;        // Not specified in MI spec but Eclipse gives this option.
    const CMIUtilString m_constStrArgNumber;   // Not specified in MI spec but Eclipse gives this option.
    const CMIUtilString m_constStrArgContents; // Not specified in MI spec but Eclipse gives this option.
    MIuint64 m_nAddr;
    CMIUtilString m_strContents;
    MIuint64 m_nCount;
    MIuchar *m_pBufferMemory;
};
