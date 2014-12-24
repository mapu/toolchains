#include "MMPUResConflictChecker.h"
#include "../MCTargetDesc/MMPUMCTargetDesc.h"
#include "../MCTargetDesc/MMPUMCInst.h"

namespace llvm {
  namespace MMPU {
    static Resource ResourceTable[NUM_RESOURCE] = { };

    Resource * GetResourceTable()
    {
      return ResourceTable;
    }

  } // end namespace MMPU
} // end namespace llvm
