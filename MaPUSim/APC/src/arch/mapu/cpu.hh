/*
 * cpu.hh
 *
 *  Created on: Jun 27, 2013
 *      Author: wangl
 */

#ifndef __ARCH_MAPU_INORDER_HH__
#define __ARCH_MAPU_INORDER_HH__

#include <string>
#include <vector>

#include "cpu/inorder/cpu.hh"
#include "sim/sim_object.hh"

#include "process.hh"

struct TheCPUParams;
class TheCPU: public InOrderCPU {
protected:
  LocalMemPort sfetchPort;

  LocalMemPort mfetchPort;

  LocalMemPort aguPort;

  LocalMemPort biu0Port;

  LocalMemPort biu1Port;

  LocalMemPort biu2Port;

  CpuPort csuExtPort;

  CpuPort csuIntPort;

  CpuPort turboDecPort;

  class CsuTickEvent: public Event {
    /** Pointer to the CPU. */
    TheCPU *cpu;

  public:
    CsuTickEvent(TheCPU *_cpu);
    ~CsuTickEvent() {
    }

    /** Processes a tick event, calling tick() on the CPU. */
    void process();

    /** Returns the description of the tick event. */
    const char *description() const;

  };

  /** The tick event used for scheduling CSU ticks. */
  CsuTickEvent ctickEvent;

  /** Ticks CSU, calling ctick() on each stage, the CSU should never be
   *  descheduled.
   */
  void ctick();
public:
  typedef TheCPUParams Params;
  TheCPU(Params *p);
  ~TheCPU();

  /** Return a reference to the data port. */
  virtual CpuPort &getDataPort() { return aguPort; }

  /** Return a reference to the instruction port. */
  virtual CpuPort &getInstPort() { return sfetchPort; }

  /** Return a reference to the MPU instruction port. */
  virtual CpuPort &getMInstPort() { return mfetchPort; }

  /** Return a reference to the mpu data port. */
  virtual CpuPort &getBiuPort(uint8_t id) {
    if (id == 0) return biu0Port;
    if (id == 1) return biu1Port;
    if (id == 2) return biu2Port;
    panic("BIU ID is out of range.\n");
  }

  /** Return a reference to the dma external port. */
  virtual CpuPort &getCsuExtPort() { return csuExtPort; }

  /** Return a reference to the dma internal port. */
  virtual CpuPort &getCsuIntPort() { return csuIntPort; }

  /** Return a reference to the turbo decoder port. */
  virtual CpuPort &getTurboDecPort() { return turboDecPort; }

  MasterPort &getMasterPort(const std::string &if_name, int idx);

  /** Schedule ctick event, regardless of its current state. */
  void scheduleCTickEvent(int delay)
  {
      assert(!ctickEvent.scheduled() || ctickEvent.squashed());
      reschedule(&ctickEvent, nextCycle(curTick() + ticks(delay)), true);
  }

  /** Forwards an instruction read to the appropriate data
   *  resource (indexes into Resource Pool thru "dataPortIdx")
   */
  Fault read(DynInstPtr inst, Addr addr,
             uint8_t *data, unsigned size, unsigned flags);

  /** Forwards an instruction write. to the appropriate data
   *  resource (indexes into Resource Pool thru "dataPortIdx")
   */
  Fault write(DynInstPtr inst, uint8_t *data, unsigned size,
              Addr addr, unsigned flags, uint64_t *write_res = NULL);
};

#endif

