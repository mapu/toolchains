/*
 * dw_ahb_dmac.hh
 *
 *  Created on: Mar 5, 2014
 *      Author: wangl
 */

#ifndef DW_AHB_DMAC_HH_
#define DW_AHB_DMAC_HH_

#include "dev/arm/amba_device.hh"
#include "params/DW_Dma_Controller.hh"

class DW_Dma_Controller : public AmbaDmaDevice {
protected:
  static const uint64_t AMBA_ID = ULL(0xb105f00d44571110);

  static const int maxOutstandingDma  = 8;

  enum DMA_CTRL_MMAP {
    CH_CFG_START = 0x0,
    RAW_START = 0x2C0,
    STATUS_START = 0x2E8,
    MASK_START = 0x310,
    CLEAR_START = 0x338,
    STATUS_INT = 0x360,
    REQ_SRC_REG = 0x368,
    REQ_DEST_REG = 0x370,
    SINGLE_REQ_SRC_REG = 0x378,
    SINGLE_REQ_DEST_REG = 0x380,
    LAST_REQ_SRC_REG = 0x388,
    LAST_REQ_DEST_REG = 0x390,
    DMA_CFG_REG = 0x398,
    CH_EN_REG = 0x3A0,
    DMA_ID_REG = 0x3A8,
    DMA_TEST_REG = 0x3B0,
    DMA_COMP_PARAMS_6 = 0x3C8,
    DMA_COMP_PARAMS_5 = 0x3D0,
    DMA_COMP_PARAMS_4 = 0x3D8,
    DMA_COMP_PARAMS_3 = 0x3E0,
    DMA_COMP_PARAMS_2 = 0x3E8,
    DMA_COMP_PARAMS_1 = 0x3F0,
    DMA_COMPONENT_ID = 0x3F8,
  };

  struct dma_conf_strt {
    uint64_t src_addr_reg;
    uint64_t dest_addr_reg;
    uint64_t link_list_pointer_reg;
    uint64_t ctrl_reg;
    uint64_t src_stat_reg;
    uint64_t dest_stat_reg;
    uint64_t src_stat_addr_reg;
    uint64_t dest_stat_addr_reg;
    uint64_t conf_reg;
    uint64_t src_gather_reg;
    uint64_t dest_scatter_reg;
  };

  struct int_type_strt {
    uint64_t transfer;
    uint64_t block;
    uint64_t src_tran;
    uint64_t dest_tran;
    uint64_t error;
  };

  dma_conf_strt ch_conf[8];
  int_type_strt raw_int;
  int_type_strt status_int;
  int_type_strt mask_int;
  int_type_strt clear_int;
  uint64_t status;
  uint64_t dma_cfg;
  uint64_t ch_en;

  void updateStatus();

  /** Function to generate interrupt */
  void generateInterrupt();

  /** start the dmas off after power is enabled */
  void startDma(uint8_t ch_id);

  /** DMA done event */
  void dmaDone(uint8_t ch_id);

  class DmaDoneEvent: public Event {
  private:
    DW_Dma_Controller &obj;
    uint8_t ch_id;
    uint8_t *dmaBuffer;

  public:
    DmaDoneEvent(DW_Dma_Controller *_obj) :
      Event(), obj(*_obj), ch_id(0), dmaBuffer(0) {
    }

    void process() {
      if (obj.ch_en & (1 << ch_id)) {
        if (obj.ch_conf[ch_id].ctrl_reg & 1) {
          obj.raw_int.src_tran |= 1 << ch_id;
          obj.updateStatus();
        }
        Addr destAddr = obj.ch_conf[ch_id].dest_addr_reg;
        uint64_t dmaSize = (obj.ch_conf[ch_id].ctrl_reg >> 32) <<
                           ((obj.ch_conf[ch_id].ctrl_reg >> 4) & 0x7);
        obj.dmaPort.dmaAction(MemCmd::WriteReq, destAddr, dmaSize,
                              this, dmaBuffer, 0, Request::UNCACHEABLE);
        obj.ch_en = obj.ch_en & (~(1 << ch_id));
      } else {
        if (obj.ch_conf[ch_id].ctrl_reg & 1)
          obj.raw_int.dest_tran |= 1 << ch_id;
        delete[] dmaBuffer;
        dmaBuffer = NULL;
        obj.dmaDoneEventFree.push_back(this);
        obj.dmaDone(ch_id);
      }
    }

    void setBuffer(uint8_t *bp) { dmaBuffer = bp; }

    void setCh(uint8_t id) { ch_id = id; }

    const std::string name() const {
      return obj.name() + ".DmaDoneEvent";
    }
  };
  std::vector<DmaDoneEvent> dmaDoneEventAll;
  std::vector<DmaDoneEvent *> dmaDoneEventFree;

  /** Wrapper to create an event out of the interrupt */
  EventWrapper<DW_Dma_Controller,
               &DW_Dma_Controller::generateInterrupt> intEvent;
  BaseGic *gic;

public:
  typedef DW_Dma_ControllerParams Params;

  const Params *params() const {
    return dynamic_cast<const Params *>(_params);
  }

  DW_Dma_Controller(const Params *p);
  ~DW_Dma_Controller() {}

  virtual Tick read(PacketPtr pkt);
  virtual Tick write(PacketPtr pkt);

  virtual void serialize(std::ostream &os);
  virtual void unserialize(Checkpoint *cp, const std::string &section);

  /**
   * Determine the address ranges that this device responds to.
   *
   * @return a list of non-overlapping address ranges
   */
  AddrRangeList getAddrRanges() const;
};
#endif /* DW_AHB_DMAC_HH_ */
