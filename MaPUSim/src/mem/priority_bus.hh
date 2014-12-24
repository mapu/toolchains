/**
 * @file
 * Declaration of a prioritized multi-master bus.
 */

#ifndef __MEM_PRIORITY_BUS_HH__
#define __MEM_PRIORITY_BUS_HH__

#include "mem/bus.hh"
#include "mem/qport.hh"
#include "params/PriorityBus.hh"

/**
 * A non-coherent bus connects a number of non-snooping masters and
 * slaves, and routes the request and response packets based on the
 * address. The request packets issued by the master connected to a
 * non-coherent bus could still snoop in caches attached to a coherent
 * bus, as is the case with the I/O bus and memory bus in most system
 * configurations. No snoops will, however, reach any master on the
 * non-coherent bus itself.
 *
 * The non-coherent bus can be used as a template for modelling PCI,
 * PCIe, and non-coherent AMBA and OCP buses, and is typically used
 * for the I/O buses.
 */
class PriorityBus: public BaseBus {

protected:

  /**
   * Declaration of the non-coherent bus slave port type, one will
   * be instantiated for each of the master ports connecting to the
   * bus.
   */
  class PriorityBusSlavePort: public SlavePort {
  private:

    /** A reference to the bus to which this port belongs. */
    PriorityBus &bus;

  public:

    PriorityBusSlavePort(const std::string &_name, PriorityBus &_bus,
                         PortID _id)
    : SlavePort(_name, &_bus, _id), bus(_bus) {}

  protected:

    /**
     * When receiving a timing request, pass it to the bus.
     */
    virtual bool recvTimingReq(PacketPtr pkt) {
      return bus.recvTimingReq(pkt, id);
    }

    /**
     * When receiving an atomic request, pass it to the bus.
     */
    virtual Tick recvAtomic(PacketPtr pkt) {
      return bus.recvAtomic(pkt, id);
    }

    /**
     * When receiving a functional request, pass it to the bus.
     */
    virtual void recvFunctional(PacketPtr pkt) {
      bus.recvFunctional(pkt, id);
    }

    /**
     * When receiving a retry, pass it to the bus.
     */
    virtual void recvRetry() {
      panic("Bus slave ports always succeed and should never retry.\n");
    }

    /**
     * Return the union of all adress ranges seen by this bus.
     */
    virtual AddrRangeList getAddrRanges() {
      return bus.getAddrRanges();
    }

    /**
     * Get the maximum block size as seen by the bus.
     */
    virtual unsigned deviceBlockSize() const {
      return bus.findBlockSize();
    }

  };

  /**
   * Declaration of the bus master port type, one will be
   * instantiated for each of the slave ports connecting to the
   * bus.
   */
  class PriorityBusMasterPort: public QueuedMasterPort {
  private:

    /** A reference to the bus to which this port belongs. */
    PriorityBus &bus;

    MasterPacketQueue queue;

  public:

    PriorityBusMasterPort(const std::string &_name, PriorityBus &_bus,
                          PortID _id) :
      QueuedMasterPort(_name, &_bus, _id, queue), bus(_bus),
                       queue(bus, *this) {}

    bool sendPriorityReq(PacketPtr pkt, Tick Priority) {
      queue.schedSendTiming(pkt, curTick() + Priority);
      return true;
    }

  protected:

    /**
     * When receiving a timing response, pass it to the bus.
     */
    virtual bool recvTimingResp(PacketPtr pkt) {
      return bus.recvTimingResp(pkt, id);
    }

    /** When reciving a range change from the peer port (at id),
     pass it to the bus. */
    virtual void recvRangeChange() {
      bus.recvRangeChange(id);
    }

    /** When reciving a retry from the peer port (at id),
     pass it to the bus. */
    virtual void recvRetry() {
      bus.recvRetry();
    }

    /**
     * Get the maximum block size as seen by the bus.
     */
    virtual unsigned deviceBlockSize() const {
      return bus.findBlockSize();
    }

  };

  /** Function called by the port when the bus is recieving a Timing
   request packet.*/
  bool recvTimingReq(PacketPtr pkt, PortID slave_port_id);

  /** Function called by the port when the bus is recieving a Timing
   response packet.*/
  bool recvTimingResp(PacketPtr pkt, PortID master_port_id);

  /** Function called by the port when the bus is recieving a Atomic
   transaction.*/
  Tick recvAtomic(PacketPtr pkt, PortID slave_port_id);

  /** Function called by the port when the bus is recieving a Functional
   transaction.*/
  void recvFunctional(PacketPtr pkt, PortID slave_port_id);

public:

  PriorityBus(const PriorityBusParams *p);

};

#endif //__MEM_NONCOHERENT_BUS_HH__
