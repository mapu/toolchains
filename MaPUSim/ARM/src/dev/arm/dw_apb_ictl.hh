/*
 * dw_aph_ictl.hh
 *
 *  Created on: Mar 6, 2014
 *      Author: wangl
 */

#ifndef DW_APH_ICTL_HH_
#define DW_APH_ICTL_HH_

#include "base/bitunion.hh"
#include "cpu/intr_control.hh"
#include "dev/arm/base_gic.hh"
#include "dev/io_device.hh"
#include "dev/platform.hh"
#include "params/DW_Ictl.hh"

class DW_Ictl : public BaseGic {
  enum INTR_CTRL_MMAP {
    IRQ_INTEN_L       = 0x00,
    IRQ_INTEN_H       = 0x04,
    IRQ_INTMASK_L     = 0x08,
    IRQ_INTMASK_H     = 0x0C,
    IRQ_FORCE_L       = 0x10,
    IRQ_FORCE_H       = 0x14,
    IRQ_RAWSTATUS_L   = 0x18,
    IRQ_RAWSTATUS_H   = 0x1C,
    IRQ_STATUS_L      = 0x20,
    IRQ_STATUS_H      = 0x24,
    IRQ_MASKSTATUS_L  = 0x28,
    IRQ_MASKSTATUS_H  = 0x2C,
    IRQ_FINALSTATUS_L = 0x30,
    IRQ_FINALSTATUS_H = 0x34,
    IRQ_VECTOR        = 0x38,
    IRQ_VECTOR_0      = 0x40,
    IRQ_VECTOR_1      = 0x48,
    IRQ_VECTOR_2      = 0x50,
    IRQ_VECTOR_3      = 0x58,
    IRQ_VECTOR_4      = 0x60,
    IRQ_VECTOR_5      = 0x68,
    IRQ_VECTOR_6      = 0x70,
    IRQ_VECTOR_7      = 0x78,
    IRQ_VECTOR_8      = 0x80,
    IRQ_VECTOR_9      = 0x88,
    IRQ_VECTOR_10     = 0x90,
    IRQ_VECTOR_11     = 0x98,
    IRQ_VECTOR_12     = 0xA0,
    IRQ_VECTOR_13     = 0xA8,
    IRQ_VECTOR_14     = 0xB0,
    IRQ_VECTOR_15     = 0xB8,
    FIQ_INTEN         = 0xC0,
    FIQ_INTMASK       = 0xC4,
    FIQ_FORCE         = 0xC8,
    FIQ_RAWSTATUS     = 0xCC,
    FIQ_STATUS        = 0xD0,
    FIQ_FINALSTATUS   = 0xD4,
    IRQ_PLEVEL        = 0xD8,
    APB_ICTL_COMP_VERSION = 0xE0,
    IRQ_PR_START      = 0xE8,
    IRQ_VECTOR_DEFAULT = 0x1E8,
  };
protected:
  struct {
    uint64_t irq_en;
    uint64_t irq_mask;
    uint64_t irq_force;
    uint64_t irq_raw;
    uint64_t irq_status;
    uint64_t irq_mask_status;
    uint64_t irq_final_status;
    uint64_t irq_vector;
    uint64_t irq_vector_default;
    uint64_t irq_vectors[16];
    uint64_t irq_pri_level;
    uint64_t irq_pri[64];
  } reg_mmap;
  Addr pioAddr;
  Addr pioSize;
  Tick pioDelay;
  Tick intLatency;

  /** Post an interrupt to a CPU
   */
  void postInt(Tick when);

  void updateIntState();

  /** Event definition to post interrupt to CPU after a delay
  */
  class PostIntEvent : public Event {
  private:
    Platform *platform;
  public:
    PostIntEvent(Platform* p) : platform(p) { }
    void process() { platform->intrctrl->post( ArmISA::INT_IRQ, 0);}
    const char *description() const { return "Post Interrupt to CPU"; }
  };
  PostIntEvent postIntEvent;

public:
  typedef DW_IctlParams Params;
  const Params *params() const {
    return dynamic_cast<const Params *>(_params);
  }
  DW_Ictl(const Params *p);

  /** @{ */
  /** Return the address ranges used by the Gic
   * This is the distributor address + all cpu addresses
   */
  virtual AddrRangeList getAddrRanges() const;

  virtual Tick read(PacketPtr pkt);

  virtual Tick write(PacketPtr pkt);

  void sendInt(uint32_t num);
  void sendPPInt(uint32_t num, uint32_t cpu);

  void clearInt(uint32_t num);
  void clearPPInt(uint32_t num, uint32_t cpu);

  virtual void serialize(std::ostream &os);
  virtual void unserialize(Checkpoint *cp, const std::string &section);

};

#endif /* DW_APH_ICTL_HH_ */
