//===-- MICmdArgValThreadGrp.h ----------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmdArgValThreadGrp.h
//
// Overview:    CMICmdArgValThreadGrp interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

// In-house headers:
#include "MICmdArgValBase.h"

// Declarations:
class CMICmdArgContext;

//++ ============================================================================
// Details: MI common code class. Command argument class. Arguments object
//          needing specialization derived from the CMICmdArgValBase class.
//          An argument knows what type of argument it is and how it is to
//          interpret the options (context) string to find and validate a matching
//          argument and so extract a value from it. Thread group looks like
//          "i1" in the options text.
//          Based on the Interpreter pattern.
// Gotchas: None.
// Authors: Illya Rudkin 15/04/2014.
// Changes: None.
//--
class CMICmdArgValThreadGrp : public CMICmdArgValBaseTemplate<MIuint>
{
    // Methods:
  public:
    /* ctor */ CMICmdArgValThreadGrp(void);
    /* ctor */ CMICmdArgValThreadGrp(const CMIUtilString &vrArgName, const bool vbMandatory, const bool vbHandleByCmd);
    //
    bool IsArgThreadGrp(const CMIUtilString &vrTxt) const;

    // Overridden:
  public:
    // From CMICmdArgValBase
    /* dtor */ virtual ~CMICmdArgValThreadGrp(void);
    // From CMICmdArgSet::IArg
    virtual bool Validate(CMICmdArgContext &vArgContext);

    // Methods:
  private:
    bool ExtractNumber(const CMIUtilString &vrTxt);
    MIuint GetNumber(void) const;

    // Attributes:
  private:
    MIuint m_nThreadGrp;
};
