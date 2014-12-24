// mpu.h -- ELF definitions specific to EM_MPU  -*- C++ -*-

// Copyright 2012 CASIA.
// Written by Shawn.Xie <shaolin.xie@ia.ac.cn>.

// This file is part of elfcpp.
   
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public License
// as published by the Free Software Foundation; either version 2, or
// (at your option) any later version.

// In addition to the permissions in the GNU Library General Public
// License, the Free Software Foundation gives you unlimited
// permission to link the compiled version of this file into
// combinations with other programs, and to distribute those
// combinations without any restriction coming from the use of this
// file.  (The Library Public License restrictions do apply in other
// respects; for example, they cover modification of the file, and
/// distribution when not linked into a combined executable.)

// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.

// You should have received a copy of the GNU Library General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef ELFCPP_MSPU_H
#define ELFCPP_MSPU_H
/******************************************************************
We define following symbol for relocations:
A  -- r_addend
PC -- The PC address of the output instruction or data
S  -- The Symbol value, after symbol resolution.
R  -- The result of relocation.
[PC] -- The 32bit data the PC points to 
The Relocation of Mpu are defined as follows :
----------------------------------------------------------------------------------
r_type      | relocation action
----------------------------------------------------------------------------------
R_MSPU_PC17 | R = S + A -PC ， [PC] = [PC] | ( R << 5)
----------------------------------------------------------------------------------
R_MSPU_28   | R = S + A， [PC] = [PC] | ( R << 0)
----------------------------------------------------------------------------------
R_MSPU_11   | R = S + A ，[PC] = [PC] | ( R << 10)
----------------------------------------------------------------------------------
R_MSPU_8    | R = S      ，[S] = S
******************************************************************/
namespace elfcpp
{

enum
{
  R_MSPU_None      = 0,

  R_MSPU_PC17, // PC-relative address, bits 21-5, for jump/loop instruction 
  R_MSPU_PC28, // PC-relative address, bits 27-0, imm extension for jump/loop instruction 
  R_MSPU_11_B5, // 11 bits relo, bits 15-5   ,for imm assignment 
  R_MSPU_11_B10, // 11 bits relo, bits 20-10 ,for load/store
  R_MSPU_17, // PC-irrelative address, bits 21-5, for CallM instructions.
  R_MSPU_28,  // 28-bit extension, at bit 27-0,    imm extension for imm assign & load/store.
  //Write the R_MSPU_PC_8 to  PC - R_MSPU_PC_8
  R_MSPU_PC_8,     // 1 byte, 8 bits
  R_MSPU_PC_16,    // 2 bytes, 16 bits
  R_MSPU_PC_32,    // 32 bits
  R_MSPU_PC_64,     // 64 bits

  // pc irrelative,//Write the R_MSPU_8 to  *R_MSPU_8
  R_MSPU_8,     // 1 byte, 8 bits
  R_MSPU_16,    // 2 bytes, 16 bits
  R_MSPU_32,    // 32 bits
  R_MSPU_64     // 64 bits
};


} // End namespace elfcpp.

#endif // !defined(ELFCPP_POWERPC_H)
