#ifndef MMPU_RES_CONFLICT_CHECKER_H
#define MMPU_RES_CONFLICT_CHECKER_H

#include "../MCTargetDesc/MMPUMCInst.h"

namespace llvm {
  namespace MMPU {
    enum _mpu_resources_
    {
      SHU, 
      BIU,

      NUM_RESOURCE
    };

    struct Resource {
        int Cycles; // how many cycles this resource should be occupied in future.
        MMPUMCInst *Inst;
    };

    Resource * GetResourceTable();
  }
}

#endif  // MMPU_RES_CONFLICT_CHECKER_H