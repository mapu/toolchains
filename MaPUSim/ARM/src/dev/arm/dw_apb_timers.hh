/*
 * dw_apb_timers.hh
 *
 *  Created on: Mar 20, 2014
 *      Author: wangl
 */

#ifndef DW_APB_TIMERS_HH_
#define DW_APB_TIMERS_HH_

#include "dev/arm/amba_device.hh"
#include "params/DWTimers.hh"

/** @file
 * This implements the dual Sp804 timer block
 */

class BaseGic;

class DWTimers: public AmbaPioDevice {
protected:
  class Timer {
  public:
    enum {
      LoadReg = 0x00,
      CurrentReg = 0x04,
      ControlReg = 0x08,
      IntClear = 0x0C,
      MaskedISR = 0x10,
      Size = 0x14
    };

    BitUnion32(CTRL)
      Bitfield<0> timerEnable;
      Bitfield<1> timerMode;
      Bitfield<2> intEnable;
    EndBitUnion(CTRL)

  protected:
    std::string _name;

    /** Pointer to parent class */
    DWTimers *parent;

    /** Number of interrupt to cause/clear */
    const uint32_t intNum;

    /** Number of ticks in a clock input */
    const Tick clock;

    /** Control register as specified above */
    CTRL control;

    /** If timer has caused an interrupt. This is irrespective of
     * interrupt enable */
    bool rawInt;

    /** If an interrupt is currently pending. Logical and of CTRL.intEnable
     * and rawInt */
    bool pendingInt;

    /** Value to load into counter when periodic mode reaches 0 */
    uint32_t loadValue;

    /** Called when the counter reaches 0 */
    void counterAtZero();
    EventWrapper<Timer, &Timer::counterAtZero> zeroEvent;

  public:
    /** Restart the counter ticking at val
     * @param val the value to start at (pre-16 bit masking if en) */
    void restartCounter(uint32_t val);

    Timer(std::string __name, DWTimers *parent, int int_num, Tick clock);

    std::string name() const {
      return _name;
    }

    /** Handle read for a single timer */
    void read(PacketPtr pkt, Addr daddr);

    /** Handle write for a single timer */
    void write(PacketPtr pkt, Addr daddr);

    bool getRawInt() { return rawInt; }

    bool getMaskedInt() { return pendingInt; }

    void clearInt() { rawInt = pendingInt = 0; }

    void serialize(std::ostream &os);
    void unserialize(Checkpoint *cp, const std::string &section);

  };

  /** Pointer to the GIC for causing an interrupt */
  BaseGic *gic;

  /** Timers that do the actual work */
  Timer* timer[8];

public:
  typedef DWTimersParams Params;
  const Params *
  params() const {
    return dynamic_cast<const Params *>(_params);
  }
  /**
   * The constructor for RealView just registers itself with the MMU.
   * @param p params structure
   */
  DWTimers(Params *p);
  ~DWTimers();

  /**
   * Handle a read to the device
   * @param pkt The memory request.
   * @param data Where to put the data.
   */
  virtual Tick read(PacketPtr pkt);

  /**
   * All writes are simply ignored.
   * @param pkt The memory request.
   * @param data the data
   */
  virtual Tick write(PacketPtr pkt);

  //void updateInt();

  virtual void serialize(std::ostream &os);

  virtual void unserialize(Checkpoint *cp, const std::string &section);
};

#endif /* DW_APB_TIMERS_HH_ */
