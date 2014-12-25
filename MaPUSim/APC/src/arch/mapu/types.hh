#ifndef __ARCH_MAPU_TYPES_HH__
#define __ARCH_MAPU_TYPES_HH__

#include "arch/generic/types.hh"
#include "base/types.hh"

namespace MapuISA {

typedef uint32_t MachInst;
typedef uint64_t ExtMachInst;

const int NumSlots = 14;

template<class MachInst>
class MaPUPCState: public GenericISA::VLIWPCState<MachInst> {
protected:
  typedef GenericISA::VLIWPCState<MachInst> Base;

  Addr _npcb;
  Addr _pcb;

  unsigned slotNum;

public:

  Addr pc() const {
    return Base::_pc;
  }
  void pc(Addr val) {
    Base::_pc = val;
    _pcb = val * 8;
  }
  Addr npc() const {
    return Base::_npc;
  }
  void npc(Addr val) {
    Base::_npc = val;
    _npcb = val * 8;
  }
  Addr npcb() const {
    return _npcb;
  }
  Addr pcb() const {
    return _pcb;
  }

  unsigned sn() const {
    return slotNum;
  }
  ;

  void set(Addr val) {
    Base::set(val);
    _pcb = val * 8;
    _npcb = val * 8 + 32;
  }

  void mset(Addr val) {
    Base::_pc = val;
    Base::_npc = (val * 8 + 23) / 8;
    Base::_nlpc = val + 41;
    _pcb = val * 8;
    _npcb = val * 8 + 23;
    slotNum = 0;
  }

  // Advance the PC.
  void advance() {
    Base::_pc = Base::_npc;
    Base::_npc += sizeof(MachInst);
    _pcb = _npcb;
    _npcb += sizeof(MachInst) * 8;
  }

  MaPUPCState() {
    slotNum = 0;
  }
  MaPUPCState(Addr val) {
    slotNum = 0;
    set(val);
  }

  // Advance the PC.
  void madvance() {
    const unsigned slotSize[MapuISA::NumSlots] = { 23, 20, 21, 15, 28, 28, 31,
                                                   29, 26, 24, 20, 20, 20, 23 };
    Base::_pc = Base::_npc;
    _pcb = _npcb;
    slotNum = (slotNum + 1) % MapuISA::NumSlots;
    Base::_nlpc += (slotNum == 0) ? 41 : 0;
    _npcb += slotSize[slotNum];
    Base::_npc = _npcb / 8;
  }

  bool operator ==(const MaPUPCState<MachInst> &opc) const {
    return _pcb == opc._pcb && _npcb == opc._npcb;
  }

  bool operator !=(const MaPUPCState<MachInst> &opc) const {
    return !(*this == opc);
  }

  void serialize(std::ostream &os) {
    Base::serialize(os);
    SERIALIZE_SCALAR(_pcb);
    SERIALIZE_SCALAR(_npcb);
  }

  void unserialize(Checkpoint *cp, const std::string &section) {
    Base::unserialize(cp, section);
    UNSERIALIZE_SCALAR(_pcb);
    UNSERIALIZE_SCALAR(_npcb);
  }
};

template<class MachInst>
std::ostream &
operator<<(std::ostream & os, const MaPUPCState<MachInst> &pc) {
  ccprintf(os, "(%#x=>%#x:%#x)", pc.pc(), pc.npc(), pc.nlpc());
  return os;
}

typedef MaPUPCState<MachInst> PCState;

typedef uint64_t LargestRead;

//used in FP convert & round function
enum ConvertType {
  SINGLE_TO_DOUBLE, SINGLE_TO_WORD, SINGLE_TO_UWORD, SINGLE_TO_LONG,

  DOUBLE_TO_SINGLE,
  DOUBLE_TO_WORD, DOUBLE_TO_UWORD, DOUBLE_TO_LONG,

  LONG_TO_SINGLE,
  LONG_TO_DOUBLE, LONG_TO_WORD, LONG_TO_PS,

  WORD_TO_SINGLE,
  WORD_TO_DOUBLE,

  WORD_TO_LONG,
  WORD_TO_PS,

  PL_TO_SINGLE,
  PU_TO_SINGLE
};

//used in FP convert & round function
enum RoundMode {
  RND_ZERO, RND_DOWN, RND_UP, RND_NEAREST
};

} // namespace MapuISA
#endif
