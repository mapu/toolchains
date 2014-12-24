#ifndef MMPU_MC_INST_H
#define MMPU_MC_INST_H

#include <stdint.h>
#include <string>

#include "llvm/MC/MCInst.h"

namespace llvm {
  namespace MMPU {

    enum InstOptionID
    {
      e_KPP = 0,
      e_APP,
      e_BR,
      e_M,

      e_T,
      e_U,
      e_B,
      e_H,
      e_S,
      e_D,
      e_IND,

      e_C,
      e_NC,
      e_L,

      e_CR,

      e_I,
      e_TC,

      e_WF1,
      e_WF2,
      e_WF3,
      e_WX1,
      e_WX2,
      e_WX3,
      e_WX4
    };

    enum InstOptionCode
    {
      c_KPP = 1 << e_KPP,
      c_APP = 1 << e_APP,
      c_BR = 1 << e_BR,
      c_M = 1 << e_M,

      c_T = 1 << e_T,
      c_U = 1 << e_U,
      c_B = 1 << e_B,
      c_H = 1 << e_H,
      c_S = 1 << e_S,
      c_D = 1 << e_D,
      c_IND = 1 << e_IND,

      c_C = 1 << e_C,
      c_NC = 1 << e_NC,
      c_L = 1 << e_L,

      c_CR = 1 << e_CR,

      c_I = 1 << e_I,
      c_TC = 1 << e_TC,

      c_WF1 = 1 << e_WF1,
      c_WF2 = 1 << e_WF2,
      c_WF3 = 1 << e_WF3,
      c_WX1 = 1 << e_WX1,
      c_WX2 = 1 << e_WX2,
      c_WX3 = 1 << e_WX3,
      c_WX4 = 1 << e_WX4
    };

 //////////////////////////////////////////////////////
    enum HardwareResource {
      BIU0Port,
      BIU1Port,
      BIU2Port,

      FALUPort,
      FMACPort,
      IALUPort,
      IMACPort,
      IMACMR  ,

      MR0Port ,
      MR1Port ,
      MR2Port ,
      MR3Port ,

      SHU0Port,
      SHU1Port,
    };

    // constraint description for instruction options
    struct InstDesc
    {
        uint32_t opcode;        // the opcode of this instruction

        /// option constraints of each instruction.
        uint32_t opnum;         // the id of the option operand
        int32_t mask;          // all possible options
        int32_t mandatory;     // must-have
        int32_t exclusive[4];  // at most one, or null.
        int32_t inclusive[4];  // at least one, maybe more.

        int32_t wb_cycle;      // when to write back.

        // hardware resource constraints of each instruction.
        // format: { res_name, time, cycles, res_name, time, cycles, ... , -1 }
        int32_t res_constraints[7]; //3n+1
    };

    void PrintInstOptions(int64_t options, std::string& reports);
    const InstDesc* GetISADesc();

    class MMPUMCInst : public MCInst {

      private:
        // where this instruction locates in the source file.
        const char* Loc;
        // where the options of this instruction starts in the source file.
        const char* OptionsLoc;

      public:
        inline const char* getLoc()        { return Loc; }
        inline const char* getOptionsLoc() { return OptionsLoc; }

        inline void setLoc(const char * loc)        { Loc        = loc; }
        inline void setOptionsLoc(const char * loc) { OptionsLoc = loc; }
    };
  }
}

#endif  // MMPU_MC_INST_H
