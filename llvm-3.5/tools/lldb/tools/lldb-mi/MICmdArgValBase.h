//===-- Platform.h ----------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MICmdArgValBase.h
//
// Overview:    CMICmdArgValBase interface.
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
#include "MICmdArgSet.h"

//++ ============================================================================
// Details: MI common code class. Command argument base class. Arguments objects
//          needing specialization derived from *this class. An argument knows
//          what type of argument it is and how it is to interpret the options
//          (context) string to find and validate a matching argument and so
//          extract a value from it.
//          Argument objects are added to the CMICmdArgSet container object.
//          Once added the container they belong to that contain and will be
//          deleted when the container goes out of scope. Allocate argument
//          objects on the heap and pass in to the Add().
//          Note the code is written such that a command will produce an error
//          should it be presented with arguments or options it does not understand.
//          A command can recognise an option or argument then ignore if it
//          wishes (a warning is sent to the MI's Log file). This is so it is
//          hardwired to fail and catch arguments or options that presented by
//          different driver clients.
//          Based on the Interpreter pattern.
// Gotchas: None.
// Authors: Illya Rudkin 14/04/2014.
// Changes: None.
//--
class CMICmdArgValBase : public CMICmdArgSet::IArg
{
    // Methods:
  public:
    /* ctor */ CMICmdArgValBase(void);
    /* ctor */ CMICmdArgValBase(const CMIUtilString &vrArgName, const bool vbMandatory, const bool vbHandleByCmd);

    // Overrideable:
  public:
    /* dtor */ virtual ~CMICmdArgValBase(void);

    // Overridden:
  public:
    // From CMICmdArgSet::IArg
    virtual bool GetFound(void) const;
    virtual bool GetIsHandledByCmd(void) const;
    virtual bool GetIsMandatory(void) const;
    virtual bool GetIsMissingOptions(void) const;
    virtual const CMIUtilString &GetName(void) const;
    virtual bool GetValid(void) const;
    virtual bool Validate(CMICmdArgContext &vwArgContext);

    // Attributes:
  protected:
    bool m_bFound;     // True = yes found in arguments options text, false = not found
    bool m_bValid;     // True = yes argument parsed and valid, false = not valid
    bool m_bMandatory; // True = yes arg must be present, false = optional argument
    CMIUtilString m_strArgName;
    bool m_bHandled;          // True = Command processes *this option, false = not handled
    bool m_bIsMissingOptions; // True = Command needs more information, false = ok
};

//++ ============================================================================
// Details: MI common code class. Templated command argument base class.
// Gotchas: None.
// Authors: Illya Rudkin 14/04/2014.
// Changes: None.
//--
template <class T> class CMICmdArgValBaseTemplate : public CMICmdArgValBase
{
    // Methods:
  public:
    /* ctor */ CMICmdArgValBaseTemplate(void);
    /* ctor */ CMICmdArgValBaseTemplate(const CMIUtilString &vrArgName, const bool vbMandatory, const bool vbHandleByCmd);
    //
    const T &GetValue(void) const;

    // Overrideable:
  public:
    /* dtor */ virtual ~CMICmdArgValBaseTemplate(void);

    // Attributes:
  protected:
    T m_argValue;
};

//++ ------------------------------------------------------------------------------------
// Details: CMICmdArgValBaseTemplate constructor.
// Type:    Method.
// Args:    None.
// Return:  None.
// Throws:  None.
//--
template <class T> CMICmdArgValBaseTemplate<T>::CMICmdArgValBaseTemplate(void)
{
}

//++ ------------------------------------------------------------------------------------
// Details: CMICmdArgValBaseTemplate constructor.
// Type:    Method.
// Args:    vrArgName       - (R) Argument's name to search by.
//          vbMandatory     - (R) True = Yes must be present, false = optional argument.
//          vbHandleByCmd   - (R) True = Command processes *this option, false = not handled.
// Return:  None.
// Throws:  None.
//--
template <class T>
CMICmdArgValBaseTemplate<T>::CMICmdArgValBaseTemplate(const CMIUtilString &vrArgName, const bool vbMandatory, const bool vbHandleByCmd)
    : CMICmdArgValBase(vrArgName, vbMandatory, vbHandleByCmd)
{
}

//++ ------------------------------------------------------------------------------------
// Details: CMICmdArgValBaseTemplate destructor.
// Type:    Overrideable.
// Args:    None.
// Return:  None.
// Throws:  None.
//--
template <class T> CMICmdArgValBaseTemplate<T>::~CMICmdArgValBaseTemplate(void)
{
}

//++ ------------------------------------------------------------------------------------
// Details: Retrieve the value the argument parsed from the command's argument / options
//          text string.
// Type:    Method.
// Args:    None.
// Return:  Template type & - The arg value of *this object.
// Throws:  None.
//--
template <class T>
const T &
CMICmdArgValBaseTemplate<T>::GetValue(void) const
{
    return m_argValue;
}
