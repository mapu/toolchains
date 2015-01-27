//===-- MIUtilSingletonBase.h -----------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//++
// File:        MIUtilSingletonBase.h
//
// Overview:    MI::ISingleton interface.
//
// Environment: Compilers:  Visual C++ 12.
//                          gcc (Ubuntu/Linaro 4.8.1-10ubuntu9) 4.8.1
//              Libraries:  See MIReadmetxt.
//
// Copyright:   None.
//--

#pragma once

namespace MI
{

//   MI::ISingleton base class usage:
//
//   class CMIDerivedClass
//       : public MI::ISingleton< CMIDerivedClass >
//   {
//       friend MI::ISingleton< CMIDerivedClass >;
//
//   // Overridden:
//   public:
//       // From MI::ISingleton
//       virtual bool Initialize( void );
//       virtual bool Shutdown( void );
//   };

//++ ============================================================================
// Details: Base class for the singleton pattern.
// Gotchas: Derived class must specify MI::ISingleton<> as a friend class.
// Authors: Aidan Dodds 17/03/2014.
// Changes: None.
//--
template <typename T> class ISingleton
{
    // Statics:
  public:
    // Return an instance of the derived class
    static T &
    Instance(void)
    {
        // This will fail if the derived class has not
        // declared itself to be a friend of MI::ISingleton
        static T instance;

        return instance;
    }

    // Overrideable:
  public:
    virtual bool Initialize(void) = 0;
    virtual bool Shutdown(void) = 0;
    //
    /* dtor */ virtual ~ISingleton(void){};
};

} // namespace MI
