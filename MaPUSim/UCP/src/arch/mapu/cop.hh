/*
 * cop.hh
 *
 *  Created on: Apr 17, 2014
 *      Author: wangl
 */

#ifndef COP_HH_
#define COP_HH_

#include "base/loader/csu_object.hh"
#include "mem/mem_object.hh"
#include "mem/tport.hh"
#include "params/CoProcessor.hh"

class CoProcessor;
class MapuCPU;

class CoPPort: public SimpleTimingPort {
protected:
  /** The device that this port serves. */
  CoProcessor *cop;

  virtual Tick recvAtomic(PacketPtr pkt);

  virtual AddrRangeList getAddrRanges();

public:

  CoPPort(CoProcessor *dev);
};

/**
 * This device is the base class which all devices senstive to an address range
 * inherit from. There are three pure virtual functions which all devices must
 * implement getAddrRanges(), read(), and write(). The magic do choose which
 * mode we are in, etc is handled by the PioPort so the device doesn't have to
 * bother.
 */
class CoProcessor: public MemObject {
protected:
  MapuCPU *cpu;

  /** The pioPort that handles the requests for us and provides us requests
   * that it sees. */
  CoPPort copPort;

  /** Address that the device listens to. */
  Addr copAddr;

  /** Size that the device's address range. */
  Addr copSize;

  /** Delay that the device experinces on an access. */
  Tick copDelay;

  /**
   * Every PIO device is obliged to provide an implementation that
   * returns the address ranges the device responds to.
   *
   * @return a list of non-overlapping address ranges
   */
  virtual AddrRangeList getAddrRanges();

  /** Pure virtual function that the device must implement. Called
   * when a read command is recieved by the port.
   * @param pkt Packet describing this request
   * @return number of ticks it took to complete
   */
  virtual Tick read(PacketPtr pkt) = 0;

  /** Pure virtual function that the device must implement. Called when a
   * write command is recieved by the port.
   * @param pkt Packet describing this request
   * @return number of ticks it took to complete
   */
  virtual Tick write(PacketPtr pkt) = 0;

public:
  typedef CoProcessorParams Params;
  CoProcessor(const Params *p);
  virtual ~CoProcessor();

  const Params *params() const {
    return dynamic_cast<const Params *>(_params);
  }

  virtual void init();

  virtual unsigned int drain(Event *de);

  virtual SlavePort &getSlavePort(const std::string &if_name, int idx = -1);

  friend class CoPPort;

  void sendEvent(enum CsuObject::CSU_EVENT e);

  void clearEvent(enum CsuObject::CSU_EVENT e);
};

#endif /* COP_HH_ */
