/*
 * Copyright (c) 2011-2012 ARM Limited
 * All rights reserved
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2006 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Ali Saidi
 *          Steve Reinhardt
 *          Andreas Hansson
 */

/**
 * @file
 * Implementation of a memory-mapped bus bridge that connects a master
 * and a slave through a request and response queue.
 */

#include "base/trace.hh"
#include "debug/BusBridge.hh"
#include "mem/dm_bridge.hh"
#include "params/DMBridge.hh"

DMBridge::BridgeSlavePort::
BridgeSlavePort(const std::string &_name, DMBridge* _bridge,
                BridgeMasterPort& _masterPort, int _delay,
                std::vector<Range<Addr> > _ranges) :
  SlavePort(_name, _bridge), bridge(_bridge), masterPort(_masterPort),
  delay(_delay), ranges(_ranges.begin(), _ranges.end()), sendEvent(*this) {}

DMBridge::BridgeMasterPort::
BridgeMasterPort(const std::string &_name, DMBridge* _bridge,
                 BridgeSlavePort& _slavePort, int _delay, int _queue_delay,
                 int _req_limit) :
    MasterPort(_name, _bridge), bridge(_bridge), slavePort(_slavePort),
    delay(_delay), queueDelay(_queue_delay), reqQueueLimit(_req_limit),
    sendEvent(*this) {}

DMBridge::DMBridge(Params *p) :
    MemObject(p),
    slavePort(p->name + "-slave", this, masterPort, p->resp_delay, p->ranges),
    masterPort(p->name + "-master", this, slavePort, p->req_delay,
               p->queue_delay, p->req_size) {}

MasterPort&
DMBridge::getMasterPort(const std::string &if_name, int idx) {
  if (if_name == "master") return masterPort;
  // pass it along to our super class
  else return MemObject::getMasterPort(if_name, idx);
}

SlavePort&
DMBridge::getSlavePort(const std::string &if_name, int idx) {
  if (if_name == "slave") return slavePort;
  // pass it along to our super class
  else return MemObject::getSlavePort(if_name, idx);
}

void DMBridge::init() {
  // make sure both sides are connected and have the same block size
  if (!slavePort.isConnected() || !masterPort.isConnected())
    fatal("Both ports of bus bridge are not connected to a bus.\n");

  if (slavePort.peerBlockSize() != masterPort.peerBlockSize())
    fatal("Slave port size %d, master port size %d \n "
          "Busses don't have the same block size... Not supported.\n",
          slavePort.peerBlockSize(), masterPort.peerBlockSize());

  // notify the master side  of our address ranges
  slavePort.sendRangeChange();
}

bool DMBridge::BridgeMasterPort::reqQueueFull() {
  return requestQueue.size() >= reqQueueLimit;
}

bool DMBridge::BridgeMasterPort::recvTimingResp(PacketPtr pkt) {
  // all checks are done when the request is accepted on the slave
  // side, so we are guaranteed to have space for the response
  DPRINTF(BusBridge, "recvTiming: response %s addr 0x%x\n", pkt->cmdString(),
          pkt->getAddr());

  DPRINTF(BusBridge, "Request queue size: %d\n", requestQueue.size());

  slavePort.queueForSendTiming(pkt);

  return true;
}

bool DMBridge::BridgeSlavePort::recvTimingReq(PacketPtr pkt) {
  DPRINTF(BusBridge, "recvTiming: request %s addr 0x%x\n", pkt->cmdString(),
          pkt->getAddr());

  DPRINTF(BusBridge, "Response queue size: %d\n", responseQueue.size());

  if (masterPort.reqQueueFull()) {
    DPRINTF(BusBridge, "Request queue full, queuing\n");
    masterPort.queueForSendTiming(pkt, true);
    return true;
  }

  masterPort.queueForSendTiming(pkt);

  return true;
}

void DMBridge::BridgeMasterPort::
queueForSendTiming(PacketPtr pkt, bool isQueued) {
  Tick readyTime = isQueued ? requestQueue.back().ready + queueDelay
                            : curTick() + delay;

  // If we expect to see a response, we need to restore the source
  // and destination field that is potentially changed by a second
  // bus
  if (!pkt->memInhibitAsserted() && pkt->needsResponse()) {
    // Update the sender state so we can deal with the response
    // appropriately
    RequestState *req_state = new RequestState(pkt);
    pkt->senderState = req_state;
  }

  // If we're about to put this packet at the head of the queue, we
  // need to schedule an event to do the transmit.  Otherwise there
  // should already be an event scheduled for sending the head
  // packet.
  if (requestQueue.empty()) {
    bridge->schedule(sendEvent, readyTime);
  }

  requestQueue.push_back(DeferredRequest(pkt, readyTime));
}

void DMBridge::BridgeSlavePort::queueForSendTiming(PacketPtr pkt) {
  // This is a response for a request we forwarded earlier.  The
  // corresponding request state should be stored in the packet's
  // senderState field.
  RequestState *req_state = dynamic_cast<RequestState*>(pkt->senderState);
  assert(req_state != NULL);
  // set up new packet dest & senderState based on values saved
  // from original request
  req_state->fixResponse(pkt);

  // the bridge assumes that at least one bus has set the
  // destination field of the packet
  assert(pkt->isDestValid());
  DPRINTF(BusBridge, "response, new dest %d\n", pkt->getDest());
  delete req_state;

  Tick readyTime = curTick() + delay;

  // If we're about to put this packet at the head of the queue, we
  // need to schedule an event to do the transmit.  Otherwise there
  // should already be an event scheduled for sending the head
  // packet.
  if (responseQueue.empty()) {
    bridge->schedule(sendEvent, readyTime);
  }
  responseQueue.push_back(DeferredResponse(pkt, readyTime));
}

void DMBridge::BridgeMasterPort::trySend() {
  assert(!requestQueue.empty());

  DeferredRequest req = requestQueue.front();

  assert(req.ready <= curTick());

  PacketPtr pkt = req.pkt;

  DPRINTF(BusBridge, "trySend request: addr 0x%x\n", pkt->getAddr());

  if (sendTimingReq(pkt)) {
    // send successful
    requestQueue.pop_front();

    // If there are more packets to send, schedule event to try again.
    if (!requestQueue.empty()) {
      req = requestQueue.front();
      DPRINTF(BusBridge, "Scheduling next send\n");
      bridge->schedule(sendEvent, std::max(req.ready, curTick() + 1));
    }
  }

  DPRINTF(BusBridge, "trySend: request queue size: %d\n", requestQueue.size());
}

void DMBridge::BridgeSlavePort::trySend() {
  assert(!responseQueue.empty());

  DeferredResponse resp = responseQueue.front();

  assert(resp.ready <= curTick());

  PacketPtr pkt = resp.pkt;

  DPRINTF(BusBridge, "trySend response: dest %d addr 0x%x\n", pkt->getDest(),
          pkt->getAddr());

  if (sendTimingResp(pkt)) {
    DPRINTF(BusBridge, "  successful\n");
    // send successful
    responseQueue.pop_front();

    // If there are more packets to send, schedule event to try again.
    if (!responseQueue.empty()) {
      resp = responseQueue.front();
      DPRINTF(BusBridge, "Scheduling next send\n");
      bridge->schedule(sendEvent, std::max(resp.ready, curTick() + 1));
    }
  } else {
    DPRINTF(BusBridge, "  unsuccessful\n");
  }

  DPRINTF(BusBridge, "trySend: queue size: %d\n",
          responseQueue.size());
}

void DMBridge::BridgeMasterPort::recvRetry() {
  Tick nextReady = requestQueue.front().ready;
  if (nextReady <= curTick()) trySend();
  else bridge->schedule(sendEvent, nextReady);
}

void DMBridge::BridgeSlavePort::recvRetry() {
  Tick nextReady = responseQueue.front().ready;
  if (nextReady <= curTick()) trySend();
  else bridge->schedule(sendEvent, nextReady);
}

Tick DMBridge::BridgeSlavePort::recvAtomic(PacketPtr pkt) {
  return delay + masterPort.sendAtomic(pkt);
}

void DMBridge::BridgeSlavePort::recvFunctional(PacketPtr pkt) {
  std::list<DeferredResponse>::iterator i;

  pkt->pushLabel(name());

  // check the response queue
  for (i = responseQueue.begin(); i != responseQueue.end(); ++i) {
    if (pkt->checkFunctional((*i).pkt)) {
      pkt->makeResponse();
      return;
    }
  }

  // also check the master port's request queue
  if (masterPort.checkFunctional(pkt)) {
    return;
  }

  pkt->popLabel();

  // fall through if pkt still not satisfied
  masterPort.sendFunctional(pkt);
}

bool DMBridge::BridgeMasterPort::checkFunctional(PacketPtr pkt) {
  bool found = false;
  std::list<DeferredRequest>::iterator i = requestQueue.begin();

  while (i != requestQueue.end() && !found) {
    if (pkt->checkFunctional((*i).pkt)) {
      pkt->makeResponse();
      found = true;
    }
    ++i;
  }

  return found;
}

AddrRangeList DMBridge::BridgeSlavePort::getAddrRanges() {
  return ranges;
}

DMBridge *
DMBridgeParams::create() {
  return new DMBridge(this);
}
