//===- PDBNameMap.h - PDB Name Map ------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUGINFO_PDB_RAW_PDBNAMEMAP_H
#define LLVM_DEBUGINFO_PDB_RAW_PDBNAMEMAP_H

#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

#include <stdint.h>
#include <utility>

namespace llvm {
class PDBStream;
class PDBNameMap {
public:
  PDBNameMap();

  std::error_code load(PDBStream &Stream);

  bool tryGetValue(StringRef Name, uint32_t &Value) const;

private:
  StringMap<uint32_t> Mapping;
};
}

#endif