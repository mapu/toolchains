//===-- VMInfo.h - Virtual Machine Info ----------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===--------------------------------------------------------------------===//
//
//===--------------------------------------------------------------------===//

#ifndef VMINFO_H
#define VMINFO_H

/* nom de l'architecture */
#define VM_ARCH_NAME "i386"


/* taille des types (sert aussi pour l'alignement) */
#define VM_CHAR_SIZE    1
#define VM_SHORT_SIZE   2
#define VM_INT_SIZE     4
#define VM_POINTER_SIZE 4

/* taille minimum d'un ÈlÈment mis sur la pile */
#define VM_STACK_ALIGN  4

/* alignement des segments */
#define VM_SEG_ALIGN  16

/* EndianitÈ */
#define VM_LITTLE_ENDIAN 1

/* modËle de pile de la machine virtuelle */
#define VM_LOCAL_START        4
#define VM_LOCAL_PARAM_END    -8

#endif
