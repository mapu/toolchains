/*
 * mapu_vif.cc
 *
 *  Created on: Mar 26, 2014
 *      Author: wangl
 */

#include "base/vnc/vncinput.hh"
#include "base/bitmap.hh"
#include "base/output.hh"
#include "base/trace.hh"
#include "debug/VIF.hh"
#include "dev/arm/amba_device.hh"
#include "dev/arm/base_gic.hh"
#include "dev/arm/mapu_vif.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"
#include "sim/system.hh"

using std::vector;

// initialize VIF registers
VideoInterface::VideoInterface(const Params *p) :
  AmbaDmaDevice(p), v_sync(0), v_backporch(0), v_active(0), v_frontporch(0),
  h_sync(0), h_backporch(0), h_active(0), h_frontporch(0), mode(0), cfg(0),
  ctrl(0), screen_width(480), screen_height(320), basic_base(0), int_c_f(0),
  monitor(0), axi(0), vnc(p->vnc), bmp(NULL), dmaBuffer(NULL), startAddr(0),
  startTime(0), maxAddr(0),
  readEvent(this), fillFifoEvent(this), dmaDoneEvent(this), intEvent(this),
  enableCapture(p->enable_capture) {
  pioSize = 0xFFFF;

  dmaBuffer = new uint8_t[1920 * 1080 * 4];
  memset(dmaBuffer, 0, 1920 * 1080 * 4);

  memset(layers, 0, sizeof(layers));

  if (vnc) vnc->setFramebufferAddr(dmaBuffer);
}

VideoInterface::~VideoInterface() {
  delete[] dmaBuffer;
}

// read registers and frame buffer
Tick VideoInterface::read(PacketPtr pkt) {
  // use a temporary data since the LCD registers are read/written with
  // different size operations

  uint32_t data = 0;
  uint32_t layer_id;
  uint32_t offset;

  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;
  pkt->allocate();

  DPRINTF(VIF, " read register %#x size=%d\n", daddr, pkt->getSize());

  switch (daddr) {
  case V_SYNC:
    data = v_backporch;
    data <<= 16;
    data |= v_sync;
    break;
  case V_BACKPORCH:
    data = v_backporch;
    break;
  case V_ACTIVE:
    data = v_frontporch;
    data <<= 16;
    data |= v_active;
    break;
  case V_FRONTPORCH:
    data = v_frontporch;
    break;
  case H_SYNC:
    data = h_sync;
    break;
  case H_BACKPORCH:
    data = h_backporch;
    break;
  case H_ACTIVE:
    data = h_active;
    break;
  case H_FRONTPORCH:
    data = h_frontporch;
    break;
  case VIF_MODE:
    data = mode;
    break;
  case VIF_CFG:
    data = cfg;
    break;
  case VIF_CTRL:
    data = ctrl;
    break;
  case SCREEN_WIDTH:
    data = screen_height;
    data <<= 16;
    data |= screen_width;
    break;
  case SCREEN_HEIGHT:
    data = screen_height;
    break;
  case BASIC_BASE:
    data = basic_base;
    break;
  case INT_C_F:
    data = int_c_f;
    break;
  case VIF_MONITOR:
    data = monitor;
    break;
  case VIF_AXI:
    data = axi;
    break;
  default:
    if (daddr >= LAYER_CFG_BASE && daddr < BASIC_BASE) {
      layer_id = (daddr - LAYER_CFG_BASE) / sizeof (struct layer_conf);
      offset = (daddr - LAYER_CFG_BASE) % sizeof (struct layer_conf);
      data = *(uint32_t *)(((uint8_t *)&layers[layer_id]) + offset);
    } else {
      panic("Tried to read CLCD register at offset %#x that doesn't exist\n",
            daddr);
    }
    break;
  }

  switch (pkt->getSize()) {
  case 4:
    pkt->set<uint32_t>(data);
    break;
  default:
    panic("VIF controller read size is incompatible?\n");
    break;
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

// write registers and frame buffer
Tick VideoInterface::write(PacketPtr pkt) {
  // use a temporary data since the LCD registers are read/written with
  // different size operations
  //
  uint32_t data = 0;
  uint32_t layer_id;
  uint32_t offset;

  switch (pkt->getSize()) {
  case 4:
    data = pkt->get<uint32_t>();
    break;
  default:
    panic("VIF controller write size too big?\n");
    break;
  }

  assert(pkt->getAddr() >= pioAddr && pkt->getAddr() < pioAddr + pioSize);

  Addr daddr = pkt->getAddr() - pioAddr;

  DPRINTF(VIF, " write register %#x value %#x size=%d\n", daddr,
          pkt->get<uint8_t>(), pkt->getSize());

  switch (daddr) {
  case V_SYNC:
    v_sync = data & 0xFFFF;
    v_backporch = data >> 16;
    break;
  case V_ACTIVE:
    v_sync = data & 0xFFFF;
    v_frontporch = data >> 16;
    break;
  case H_SYNC:
    h_sync = data & 0xFFFF;
    h_backporch = data >> 16;
    break;
  case H_ACTIVE:
    h_sync = data & 0xFFFF;
    h_frontporch = data >> 16;
    break;
  case VIF_MODE:
    mode = data;
    break;
  case VIF_CFG:
    cfg = data;
    break;
  case VIF_CTRL:
    ctrl = data;
    if (!ctrl.display_en || !ctrl.int_en)
      gic->clearInt(intNum);
    if (ctrl.display_en) {
      DPRINTF(VIF, " lcd size: height %d width %d\n", screen_height, screen_width);
      startDma();
    }
    break;
  case SCREEN_WIDTH:
    screen_width = data & 0xFFFF;
    screen_height = data >> 16;
    updateVideoParams();
    break;
  case BASIC_BASE:
    basic_base = data;
    break;
  case INT_C_F:
    int_c_f = data;
    if ((data & 0x1) && int_c_f.flag) {
      gic->clearInt(intNum);
      int_c_f.flag = 0;
    }
    break;
  case VIF_AXI:
    axi = data;
    break;
  default:
    if (daddr >= LAYER_CFG_BASE && daddr < BASIC_BASE) {
      layer_id = (daddr - LAYER_CFG_BASE) / sizeof (struct layer_conf);
      offset = (daddr - LAYER_CFG_BASE) % sizeof (struct layer_conf);
      *(uint32_t *)(((uint8_t *)&layers[layer_id]) + offset) = data;
      break;
    } else {
      panic("Tried to write PL111 register at offset %#x that doesn't exist\n",
          daddr);
      break;
    }
  }

  pkt->makeAtomicResponse();
  return pioDelay;
}

void VideoInterface::updateVideoParams() {
  if (vnc)
    vnc->setFrameBufferParams(VideoConvert::rgb8888, screen_width,
                              screen_height);

  if (bmp) delete bmp;

  bmp = new Bitmap(VideoConvert::rgb8888, screen_width, screen_height,
                   dmaBuffer);
}

void VideoInterface::startDma() {
  if (readEvent.scheduled()) return;
  readFramebuffer();
}

void VideoInterface::readFramebuffer() {
  // initialization for dma read from frame buffer to dma buffer
  uint32_t length = screen_height * screen_width;

  if (mode.basic_mode) {
    startAddr = basic_base;
  } else {
    // layer mode
  }

  if (!intEvent.scheduled()) schedule(intEvent, clockEdge());
  startTime = curTick();

  maxAddr = length * 4;

  DPRINTF(VIF, " lcd frame buffer size of %d bytes \n", maxAddr);

  fillFifo();
}

void VideoInterface::fillFifo() {
  assert(!dmaDoneEvent.scheduled());

  // We use a uncachable request here because the requests from the CPU
  // will be uncacheable as well. If we have uncacheable and cacheable
  // requests in the memory system for the same address it won't be
  // pleased
  dmaPort.dmaAction(MemCmd::ReadReq, startAddr, maxAddr,
                    &dmaDoneEvent, dmaBuffer, 0, Request::UNCACHEABLE);
}

void VideoInterface::dmaDone() {
  DPRINTF(VIF, "DMA Done\n");

  Tick maxFrameTime = 1000000;

  if ((curTick() - startTime) > maxFrameTime)
     warn("VIF controller buffer underrun, took %d ticks when should"
          " have taken %d\n", curTick() - startTime, maxFrameTime);


  assert(!readEvent.scheduled());


  if (vnc) vnc->setDirty();

  if (enableCapture) {
    DPRINTF(VIF, "-- write out frame buffer into bmp\n");

    std::ostream *pic =
      simout.create(csprintf("%s.framebuffer.bmp", sys->name()), true);

    assert(bmp);
    assert(pic);
    pic->seekp(0);
    bmp->write(pic);
  }

  // schedule the next read based on when the last frame started
  // and the desired fps (i.e. maxFrameTime), we turn the
  // argument into a relative number of cycles in the future
  if (ctrl.display_en)
    schedule(readEvent,
             clockEdge(ticksToCycles(startTime - curTick() + maxFrameTime)));
}

void VideoInterface::serialize(std::ostream &os) {
    DPRINTF(VIF, "Serializing MaPU VIF\n");
}

void VideoInterface::unserialize(Checkpoint *cp, const std::string &section) {
    DPRINTF(VIF, "Unserializing MaPU VIF\n");
}

void VideoInterface::generateInterrupt() {
  if (ctrl.int_en) {
    gic->sendInt(intNum);
    DPRINTF(VIF, "Int. Generated\n");
  }
}

AddrRangeList VideoInterface::getAddrRanges() const {
  AddrRangeList ranges;
  ranges.push_back(RangeSize(pioAddr, pioSize));
  return ranges;
}

VideoInterface *VideoInterfaceParams::create() {
    return new VideoInterface(this);
}
