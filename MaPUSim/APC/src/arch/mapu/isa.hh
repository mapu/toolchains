#ifndef __ARCH_MAPU_ISA_HH__
#define __ARCH_MAPU_ISA_HH__

#include <queue>
#include <string>
#include <vector>

#include "arch/mapu/registers.hh"
#include "arch/mapu/types.hh"
#include "sim/eventq.hh"
#include "sim/fault_fwd.hh"

class BaseCPU;
class Checkpoint;
class EventManager;
class ThreadContext;

namespace MapuISA {

class ISA {
protected:

  std::vector<std::vector<MiscReg> > miscRegFile;
  MPUReg MPURegFile[NumMPURegs];

  struct MCContext {
    uint8_t MStart;
    uint8_t MEnd;
    uint8_t IStart;
    uint8_t IEnd;
    uint8_t StepSize;
    uint8_t CurIdx;
  };

  struct MCContext MRegCtrlCtx[5];

public:
  ISA();

  void clear();

  //////////////////////////////////////////////////////////
  //
  // READ/WRITE Misc Registers
  //
  //
  //////////////////////////////////////////////////////////
  //@TODO: Mapu MT's register view automatically connects
  //       Status to TCStatus depending on current thread
  //void updateCP0ReadView(int misc_reg, ThreadID tid) { }
  MiscReg readMiscRegNoEffect(int misc_reg, ThreadID tid = 0);

  //template <class TC>
  MiscReg readMiscReg(int misc_reg,
                      ThreadContext *tc, ThreadID tid = 0);
  MPUReg &readMPUReg(int mpu_reg, ThreadID tid = 0);

  //MiscReg filterCP0Write(int misc_reg, int reg_sel, const MiscReg &val);
  //void setRegMask(int misc_reg, const MiscReg &val, ThreadID tid = 0);
  void setMiscRegNoEffect(int misc_reg, const MiscReg &val,
                          ThreadID tid = 0);

  //template <class TC>
  void setMiscReg(int misc_reg, const MiscReg &val,
                  ThreadContext *tc, ThreadID tid = 0);

  void setMPUReg(int mpu_reg, const MPUReg &val, ThreadID tid = 0);

  void setPartialMPUReg(RegIndex mpu_reg, int size, int idx,
                        IntReg val, ThreadID tid);

  static std::string miscRegNames[NumMiscRegs];

public:

  int flattenIntIndex(int reg) { return reg; }

  int flattenFloatIndex(int reg) { return reg; }

  int flattenMPURegIndex(int reg);

  int calcMIndex(int reg);

  void serialize(EventManager *em, std::ostream &os) {}

  void unserialize(EventManager *em, Checkpoint *cp,
                   const std::string &section) {}
};
}

#endif // __ARCH_MAPU_ISA_HH__
