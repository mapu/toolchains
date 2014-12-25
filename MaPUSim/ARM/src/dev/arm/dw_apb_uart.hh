/*
 * dw_apb_uart.hh
 *
 *  Created on: Mar 7, 2014
 *      Author: wangl
 */

#ifndef DW_APB_UART_HH_
#define DW_APB_UART_HH_

#include "base/bitfield.hh"
#include "base/bitunion.hh"
#include "dev/arm/amba_device.hh"
#include "dev/io_device.hh"
#include "dev/uart.hh"
#include "params/DW_Uart.hh"

class BaseGic;

class DW_Uart : public Uart, public AmbaDevice {
protected:
  static const uint64_t AMBA_ID = ULL(0xb105f00d44570110);
  enum UART_R_MMAP {
    RBR = 0x0,
    IER_R = 0x4,
    IIR = 0x8,
    LCR_R = 0xC,
    MCR_R = 0x10,
    LSR = 0x14,
    MSR = 0x18,
    SCR_R = 0x1C,
    LPDLL = 0x20,
    LPDLH = 0x24,
    SRBR_START = 0x30,
    FAR = 0x70,
    TFR = 0x74,
    USR = 0x7C,
    TFL = 0x80,
    RFL = 0x84,
    SRTS = 0x8C,
    SBCR = 0x90,
    SDMAM = 0x94,
    SFE = 0x98,
    SRT = 0x9C,
    STET = 0xA0,
    HTX = 0xA4,
    CPR = 0xF4,
    UCV = 0xF8,
    CTR = 0xFC,
  };
  enum UART_W_MMAP {
    THR = 0x0,
    IER_W = 0x4,
    FCR = 0x8,
    LCR_W = 0xC,
    MCR_W = 0x10,
    SCR_W = 0x1C,
    LPDLL_W = 0x20,
    LPDLH_W = 0x24,
    STHR_W = 0x30,
    FAR_W = 0x70,
    SRTS_W = 0x8C,
    SBCR_W = 0x90,
    SDMAM_W = 0x94,
    SFE_W = 0x98,
    SRT_W = 0x9C,
    STET_W = 0xA0,
    DMASA = 0xA8,
  };

  uint16_t control;

  /** fractional baud rate divisor. Not used for anything but reporting
   * written value */
  uint16_t fbrd;

  /** integer baud rate divisor. Not used for anything but reporting
   * written value */
  uint16_t ibrd;

  /** Line control register. Not used for anything but reporting
   * written value */
  uint16_t lcr;

  uint16_t lsr;

  /** interrupt fifo level register. Not used for anything but reporting
   * written value */
  uint16_t ifls;

  uint16_t iir;

  uint16_t msr;

  uint8_t scr;

  BitUnion16(INTREG)
    Bitfield<0> rxim;
    Bitfield<1> txim;
    Bitfield<2> dcdmim;
    Bitfield<3> dsrmim;
    Bitfield<4> rimim;
    Bitfield<5> ctsmim;
    Bitfield<6> rtim;
    Bitfield<7> feim;
    Bitfield<8> peim;
    Bitfield<9> beim;
    Bitfield<10> oeim;
    Bitfield<15,11> rsvd;
  EndBitUnion(INTREG)

  INTREG imsc;

  INTREG rawInt;

  /** Interrupt number to generate */
  int intNum;

  /** Gic to use for interrupting */
  BaseGic *gic;

  /** Should the simulation end on an EOT */
  bool endOnEOT;

  /** Delay before interrupting */
  Tick intDelay;

  /** Function to generate interrupt */
  void generateInterrupt();

  /** Wrapper to create an event out of the thing */
  EventWrapper<DW_Uart, &DW_Uart::generateInterrupt> intEvent;

public:
  typedef DW_UartParams Params;
  const Params *params() const {
      return dynamic_cast<const Params *>(_params);
  }
  DW_Uart(const Params *p);

  virtual Tick read(PacketPtr pkt);
  virtual Tick write(PacketPtr pkt);

  /**
   * Inform the uart that there is data available.
   */
  virtual void dataAvailable();

  virtual bool intStatus() { return false; }

  virtual void serialize(std::ostream &os);
  virtual void unserialize(Checkpoint *cp, const std::string &section);
};
#endif /* DW_APB_UART_HH_ */
