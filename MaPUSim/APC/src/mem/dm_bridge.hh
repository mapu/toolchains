/**
 * @file
 * Declaration of a memory-mapped bus bridge that connects a master
 * and a slave through a request and response queue.
 */

#ifndef __MEM_DM_BRIDGE_HH__
#define __MEM_DM_BRIDGE_HH__

#include <list>
#include <queue>
#include <string>

#include "base/types.hh"
#include "mem/mem_object.hh"
#include "mem/packet.hh"
#include "mem/port.hh"
#include "params/DMBridge.hh"
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
class DMBridge: public MemObject {
protected:
  /**
   * A DM bridge request state stores packets along with their sender
   * state and original source. It has enough information to also
   * restore the response once it comes back to the bridge.
   */
  class RequestState: public Packet::SenderState {
  public:
    Packet::SenderState *origSenderState;
    PortID origSrc;

    RequestState(PacketPtr _pkt) :
      origSenderState(_pkt->senderState), origSrc(_pkt->getSrc()) {}

    void fixResponse(PacketPtr pkt) {
      assert(pkt->senderState == this);
      pkt->setDest(origSrc);
      pkt->senderState = origSenderState;
    }
  };

  /**
   * A deferred request stores a packet along with its scheduled
   * transmission time, and whether we can expect to see a response
   * or not.
   */
  class DeferredRequest {
  public:
    Tick ready;
    PacketPtr pkt;
    bool expectResponse;

    DeferredRequest(PacketPtr _pkt, Tick t) :
      ready(t), pkt(_pkt), expectResponse(_pkt->needsResponse()) {}
  };

  /**
   * A deferred response stores a packet along with its scheduled
   * transmission time. It also contains information of whether the
   * bridge NACKed the packet to be able to correctly maintain
   * counters of outstanding responses.
   */
  class DeferredResponse {
  public:
    Tick ready;
    PacketPtr pkt;
    bool nackedHere;
    DeferredResponse(PacketPtr _pkt, Tick t, bool nack = false) :
        ready(t), pkt(_pkt), nackedHere(nack) {}
  };

  // Forward declaration to allow the slave port to have a pointer
  class BridgeMasterPort;

  /**
   * The port on the side that receives requests and sends
   * responses. The slave port has a set of address ranges that it
   * is responsible for. The slave port also has a buffer for the
   * responses not yet sent.
   */
  class BridgeSlavePort: public SlavePort {
  private:
    /** A pointer to the bridge to which this port belongs. */
    DMBridge *bridge;

    /**
     * Master port on the other side of the bridge
     * (connected to the other bus).
     */
    BridgeMasterPort& masterPort;

  protected:

    /** Minimum request delay though this bridge. */
    Tick delay;

    /** Address ranges to pass through the bridge */
    AddrRangeList ranges;

    /**
     * Response packet queue. Response packets are held in this
     * queue for a specified delay to model the processing delay
     * of the bridge.
     */
    std::list<DeferredResponse> responseQueue;

    /**
     * Handle send event, scheduled when the packet at the head of
     * the response queue is ready to transmit (for timing
     * accesses only).
     */
    virtual void trySend();

    /**
     * Private class for scheduling sending of responses from the
     * response queue.
     */
    class SendEvent: public Event {
      BridgeSlavePort& port;

    public:
      SendEvent(BridgeSlavePort& p) : port(p) {}
      virtual void process() { port.trySend(); }
      virtual const char *description() const { return "DM bridge send"; }
    };

    /** Send event for the response queue. */
    SendEvent sendEvent;

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
    BridgeSlavePort(const std::string &_name, DMBridge *_bridge,
                    BridgeMasterPort& _masterPort, int _delay,
                    std::vector<Range<Addr> > _ranges);

    /**
     * Queue a response packet to be sent out later and also schedule
     * a send if necessary.
     *
     * @param pkt a response to send out after a delay
     */
    virtual void queueForSendTiming(PacketPtr pkt);

  protected:

    /** When receiving a timing request from the peer port,
     pass it to the bridge. */
    virtual bool recvTimingReq(PacketPtr pkt);

    /** When receiving a retry request from the peer port,
     pass it to the bridge. */
    virtual void recvRetry();

    /** When receiving a Atomic requestfrom the peer port,
     pass it to the bridge. */
    virtual Tick recvAtomic(PacketPtr pkt);

    /** When receiving a Functional request from the peer port,
     pass it to the bridge. */
    virtual void recvFunctional(PacketPtr pkt);

    /** When receiving a address range request the peer port,
     pass it to the bridge. */
    virtual AddrRangeList getAddrRanges();
  };

  /**
   * Port on the side that forwards requests and receives
   * responses. The master port has a buffer for the requests not
   * yet sent.
   */
  class BridgeMasterPort: public MasterPort {

  private:

    /** A pointer to the bridge to which this port belongs. */
    DMBridge* bridge;

    /**
     * Pointer to the slave port on the other side of the bridge
     * (connected to the other bus).
     */
    BridgeSlavePort& slavePort;

  protected:

    /** Minimum delay though this bridge. */
    Tick delay;

    /** Min delay with queuing. */
    Tick queueDelay;

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
    virtual void trySend();

    /**
     * Private class for scheduling sending of requests from the
     * request queue.
     */
    class SendEvent: public Event {
      BridgeMasterPort& port;

    public:
      SendEvent(BridgeMasterPort& p) : port(p) {}
      virtual void process() { port.trySend(); }
      virtual const char *description() const { return "bridge send"; }
    };

    /** Send event for the request queue. */
    SendEvent sendEvent;

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
    BridgeMasterPort(const std::string &_name, DMBridge *_bridge,
                     BridgeSlavePort& _slavePort, int _delay, int _queue_delay,
                     int _req_limit);

    /**
     * Is this side blocked from accepting new request packets.
     *
     * @return true if the occupied space has reached the set limit
     */
    virtual bool reqQueueFull();

    /**
     * Queue a request packet to be sent out later and also schedule
     * a send if necessary.
     *
     * @param pkt a request to send out after a delay
     */
    virtual void queueForSendTiming(PacketPtr pkt, bool isNacked = false);

    /**
     * Check a functional request against the packets in our
     * request queue.
     *
     * @param pkt packet to check against
     *
     * @return true if we find a match
     */
    virtual bool checkFunctional(PacketPtr pkt);

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
  BridgeSlavePort slavePort;

  /** Master port of the bridge. */
  BridgeMasterPort masterPort;

public:
  typedef DMBridgeParams Params;

protected:
  Params *_params;

public:
  const Params *params() const { return _params; }

  virtual MasterPort& getMasterPort(const std::string& if_name, int idx = -1);
  virtual SlavePort& getSlavePort(const std::string& if_name, int idx = -1);

  virtual void init();

  DMBridge(Params *p);
};

#endif //__MEM_BUS_HH__
