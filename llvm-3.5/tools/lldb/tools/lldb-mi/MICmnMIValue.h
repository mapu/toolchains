//===-- MICmnMIValue.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmnMIValue.h
//
// Overview:    CMICmnMIValue interface.
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

//++ ============================================================================
// Details: MI common code MI Result class. Part of the CMICmnMIValueRecord
//          set of objects.
//          The syntax is as follows:
//          result-record ==>  [ token ] "^" result-class ( "," result )* nl
//          token = any sequence of digits
//          * = 0 to many
//          nl = CR | CR_LF
//          result-class ==> "done" | "running" | "connected" | "error" | "exit"
//          result ==> variable "=" value
//          value ==> const | tuple | list
//          const ==> c-string (7 bit iso c string content)
//          tuple ==>  "{}" | "{" result ( "," result )* "}"
//          list ==>  "[]" | "[" value ( "," value )* "]" | "[" result ( "," result )* "]"
//          More information see:
//          http://ftp.gnu.org/old-gnu/Manuals/gdb-5.1.1/html_chapter/gdb_22.html
// Gotchas: None.
// Authors: Illya Rudkin 24/02/2014.
// Changes: None.
//--
class CMICmnMIValue : public CMICmnBase
{
    // Methods:
  public:
    /* ctor */ CMICmnMIValue(void);
    //
    const CMIUtilString &GetString(void) const;

    // Overridden:
  public:
    // From CMICmnBase
    /* dtor */ virtual ~CMICmnMIValue(void);

    // Attributes:
  protected:
    CMIUtilString m_strValue;
    bool m_bJustConstructed; // True = *this just constructed with no value, false = *this has had value added to it
};
