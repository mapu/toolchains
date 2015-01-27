//===-- MICmnLogMediumFile.h ------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmnLogMediumFile.h
//
// Overview:    CMICmnLogMediumFile interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// In-house headers:
#include "MIUtilFileStd.h"
#include "MIUtilString.h"
#include "MICmnBase.h"
#include "MICmnLog.h"
#include "MIUtilDateTimeStd.h"

//++ ============================================================================
// Details: MI common code implementation class. Logs application fn trace/message/
//          error messages to a file. Used as part of the CMICmnLog Logger
//          system. When instantiated *this object is register with the Logger
//          which the Logger when given data to write to registered medium comes
//          *this medium.
//          Singleton class.
// Gotchas: None.
// Authors: Illya Rudkin 30/01/2014.
// Changes: None.
//--
class CMICmnLogMediumFile : public CMICmnBase, public CMICmnLog::IMedium
{
    // Statics:
  public:
    static CMICmnLogMediumFile &Instance(void);

    // Methods:
  public:
    bool SetHeaderTxt(const CMIUtilString &vText);
    bool SetVerbosity(const MIuint veType);
    MIuint GetVerbosity(void) const;
    const CMIUtilString &GetFileName(void) const;
    const CMIUtilString &GetFileNamePath(void) const;
    bool IsOk(void) const;
    bool IsFileExist(void) const;
    const CMIUtilString &GetLineReturn(void) const;

    // Overridden:
  public:
    // From CMICmnBase
    /* dtor */ virtual ~CMICmnLogMediumFile(void);
    // From CMICmnLog::IMedium
    virtual bool Initialize(void);
    virtual const CMIUtilString &GetName(void) const;
    virtual bool Write(const CMIUtilString &vData, const CMICmnLog::ELogVerbosity veType);
    virtual const CMIUtilString &GetError(void) const;
    virtual bool Shutdown(void);

    // Methods:
  private:
    /* ctor */ CMICmnLogMediumFile(void);
    /* ctor */ CMICmnLogMediumFile(const CMICmnLogMediumFile &);
    void operator=(const CMICmnLogMediumFile &);

    bool FileWriteEnglish(const CMIUtilString &vData);
    bool FileFormFileNamePath(void);
    CMIUtilString MassagedData(const CMIUtilString &vData, const CMICmnLog::ELogVerbosity veType);
    bool FileWriteHeader(void);
    MIchar ConvertLogVerbosityTypeToId(const CMICmnLog::ELogVerbosity veType) const;
    CMIUtilString ConvertCr(const CMIUtilString &vData) const;

    // Attributes:
  private:
    const CMIUtilString m_constThisMediumName;
    const CMIUtilString m_constMediumFileName;
    //
    CMIUtilString m_fileNamePath;
    MIuint m_eVerbosityType;
    CMIUtilString m_strDate;
    CMIUtilString m_fileHeaderTxt;
    CMIUtilFileStd m_file;
    CMIUtilDateTimeStd m_dateTime;
};
