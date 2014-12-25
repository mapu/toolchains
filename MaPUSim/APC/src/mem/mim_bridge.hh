/**
 * @file
 * Declaration of a memory-mapped bus bridge that connects a master
 * and a slave through a request and response queue.
 */

#ifndef __MEM_IM_BRIDGE_HH__
#define __MEM_IM_BRIDGE_HH__

#include <list>
#include <queue>
#include <string>

#include "base/types.hh"
#include "mem/mem_object.hh"
#include "mem/packet.hh"
#include "mem/port.hh"
#include "mem/dm_bridge.hh"
#include "params/MIMBridge.hh"
#include "sim/eventq.hh"

/**
 * A bridge is used to interface two different busses (or in general a
 * memory-mapped master and slave), with buffering for requests and
 * responses. The bridge has a fixed delay for packets passing through
 * it and responds to a fixed set of address ranges.
 *
 * The bridge comprises a slave port and a master port, that buffer
 * outgoing responses and requests respectively. Buffer space is
 * reserved when a request arrives, also reserving response space
 * before forwarding the request. An incoming request is always
 * accepted (recvTiming returns true), but is potentially NACKed if
 * there is no request space or response space.
 */
class MIMBridge: public DMBridge {
  class MIMBridgeMasterPort;
  /**
   * The port on the side that receives requests and sends
   * responses. The slave port has a set of address ranges that it
   * is responsible for. The slave port also has a buffer for the
   * responses not yet sent.
   */
  class MIMBridgeSlavePort: public DMBridge::BridgeSlavePort {
  private:
    /**
     * Master port on the other side of the bridge
     * (connected to the other bus).
     */
    MIMBridgeMasterPort& masterPort;

  public:

    /**
     * Constructor for the BridgeSlavePort.
     *
     * @param _name the port name including the owner
     * @param _bridge the structural owner
     * @param _masterPort the master port on the other side of the bridge
     * @param _delay the delay from seeing a response to sending it
     * @param _ranges a number of address ranges to forward
     */
    MIMBridgeSlavePort(const std::string &_name, MIMBridge *_bridge,
                       MIMBridgeMasterPort& _masterPort, int _delay,
                       std::vector<Range<Addr> > _ranges);

  protected:

    /** When receiving a timing request from the peer port,
     pass it to the bridge. */
    virtual bool recvTimingReq(PacketPtr pkt);
  };

  /**
   * Port on the side that forwards requests and receives
   * responses. The master port has a buffer for the requests not
   * yet sent.
   */
  class MIMBridgeMasterPort: public DMBridge::BridgeMasterPort {

  private:

    /** A pointer to the bridge to which this port belongs. */
    MIMBridge* bridge;
    /**
     * Pointer to the slave port on the other side of the bridge
     * (connected to the other bus).
     */
    MIMBridgeSlavePort& slavePort;

    /**
     * Request packet queue. Request packets are held in this
     * queue for a specified delay to model the processing delay
     * of the bridge.
     */
    std::list<DeferredRequest> requestQueue;

    /** Max queue size for request packets */
    unsigned int reqQueueLimit;

    /**
     * Handle send event, scheduled when the packet at the head of
     * the outbound queue is ready to transmit (for timing
     * accesses only).
     */
    void trySend();

  public:

    /**
     * Constructor for the BridgeMasterPort.
     *
     * @param _name the port name including the owner
     * @param _bridge the structural owner
     * @param _slavePort the slave port on the other side of the bridge
     * @param _delay the delay from seeing a request to sending it
     * @param _req_limit the size of the request queue
     */
    MIMBridgeMasterPort(const std::string &_name, MIMBridge *_bridge,
                        MIMBridgeSlavePort& _slavePort, int _delay,
                        int _queue_delay, int _req_limit);

    /**
     * Is this side blocked from accepting new request packets.
     *
     * @return true if the occupied space has reached the set limit
     */
    bool reqQueueFull();

    /**
     * Queue a request packet to be sent out later and also schedule
     * a send if necessary.
     *
     * @param pkt a request to send out after a delay
     */
    void queueForSendTiming(PacketPtr pkt);

    /**
     * Check a functional request against the packets in our
     * request queue.
     *
     * @param pkt packet to check against
     *
     * @return true if we find a match
     */
    bool checkFunctional(PacketPtr pkt);

  protected:

    /** When receiving a timing request from the peer port,
     pass it to the bridge. */
    virtual bool recvTimingResp(PacketPtr pkt);

    /** When receiving a retry request from the peer port,
     pass it to the bridge. */
    virtual void recvRetry();
  };

private:

  /** Slave port of the bridge. */
  MIMBridgeSlavePort slavePort;

  /** Master port of the bridge. */
  MIMBridgeMasterPort masterPort;

public:
  typedef MIMBridgeParams Params;

protected:
  Params *_params;

public:
  const Params *params() const { return _params; }

  virtual MasterPort& getMasterPort(const std::string& if_name, int idx = -1);
  virtual SlavePort& getSlavePort(const std::string& if_name, int idx = -1);

  virtual void init();

  MIMBridge(Params *p);
};

#endif //__MEM_BUS_HH__
