/*
 * mapu_vif.hh
 *
 *  Created on: Mar 26, 2014
 *      Author: wangl
 */

#ifndef MAPU_VIF_HH_
#define MAPU_VIF_HH_

#include <fstream>

#include "dev/arm/amba_device.hh"
#include "params/VideoInterface.hh"
#include "sim/serialize.hh"

class VncInput;
class Bitmap;

class VideoInterface: public AmbaDmaDevice {
protected:
  static const uint64_t AMBA_ID = ULL(0xb105f00d00141111);
  enum VIF_MMAP {
    V_SYNC = 0x000,
    V_BACKPORCH = 0x002,
    V_ACTIVE = 0x004,
    V_FRONTPORCH = 0x006,
    H_SYNC = 0x008,
    H_BACKPORCH = 0x00A,
    H_ACTIVE = 0x00C,
    H_FRONTPORCH = 0x00E,
    VIF_MODE = 0x010,
    VIF_CFG = 0x014,
    VIF_CTRL = 0x018,
    SCREEN_WIDTH = 0x01C,
    SCREEN_HEIGHT = 0x020,
    LAYER_CFG_BASE = 0x024,
    BASIC_BASE = 0x0C0,
    INT_C_F = 0x0C4,
    VIF_MONITOR = 0x0C8,
    VIF_AXI = 0x0CC,
  };

  enum LcdMode {
    bpp1 = 0, bpp2, bpp4, bpp8, bpp16, bpp24, bpp16m565, bpp12
  };

  uint16_t v_sync;
  uint16_t v_backporch;
  uint16_t v_active;
  uint16_t v_frontporch;
  uint16_t h_sync;
  uint16_t h_backporch;
  uint16_t h_active;
  uint16_t h_frontporch;

  BitUnion32(Mode)
    Bitfield<23, 0> rgb_test;
    Bitfield<25, 24> test_mode;
    Bitfield<26> dual_mode;
    Bitfield<27> dual_swap;
    Bitfield<28> de_polar;
    Bitfield<29> hsync_polar;
    Bitfield<30> vsync_polar;
    Bitfield<31> basic_mode;
  EndBitUnion(Mode)

  Mode mode;

  BitUnion32(Cfg)
    Bitfield<2, 0> seq_layer0;
    Bitfield<6, 4> seq_layer1;
    Bitfield<10, 8> seq_layer2;
    Bitfield<14, 12> seq_layer3;
    Bitfield<18, 16> seq_layer4;
    Bitfield<26, 24> active_layer;
    Bitfield<29> mouse_layer_en;
    Bitfield<30> write_active;
    Bitfield<31> write_seq;
  EndBitUnion(Cfg)

  Cfg cfg;

  BitUnion32(Ctrl)
    Bitfield<0> layer0_en;
    Bitfield<4> layer1_en;
    Bitfield<8> layer2_en;
    Bitfield<12> layer3_en;
    Bitfield<16> layer4_en;
    Bitfield<24> int_en;
    Bitfield<28> display_en;
  EndBitUnion(Ctrl)

  Ctrl ctrl;

  uint16_t screen_width;
  uint16_t screen_height;

  struct layer_conf {
    uint32_t base;
    uint16_t layer_x;
    uint16_t layer_y;
    uint16_t layer_w;
    uint16_t layer_h;
    uint16_t win_x;
    uint16_t win_y;
    uint16_t win_w;
    uint16_t win_h;
    uint32_t opacity;
    uint32_t pad0;
    uint32_t pad1;
  } layers[5];

  uint32_t basic_base;

  BitUnion8(Int_C_F)
    Bitfield<0> clear;
    Bitfield<4> flag;
  EndBitUnion(Int_C_F)

  Int_C_F int_c_f;

  BitUnion32(Monitor)
    Bitfield<15, 0> frame_cnt;
    Bitfield<16> pclk;
    Bitfield<17> de;
    Bitfield<18> hsync;
    Bitfield<19> vsync;
    Bitfield<20> cam_arvalid;
    Bitfield<21> cam_arready;
    Bitfield<22> cam_rvalid;
    Bitfield<23> cam_rready;
    Bitfield<24> layer0_en;
    Bitfield<25> layer1_en;
    Bitfield<26> layer2_en;
    Bitfield<27> layer3_en;
    Bitfield<28> layer4_en;
    Bitfield<29> int_flag;
    Bitfield<30> int_clear;
  EndBitUnion(Monitor)

  Monitor monitor;

  BitUnion32(Axi)
    Bitfield<2, 0> arprot;
    Bitfield<5, 4> arlock;
    Bitfield<11, 8> arcache;
  EndBitUnion(Axi)

  Axi axi;

  /**
   * Event wrapper for dmaDone()
   *
   * This event calls pushes its this pointer onto the freeDoneEvent
   * vector and calls dmaDone() when triggered.
   */
  class DmaDoneEvent: public Event {
  private:
    VideoInterface &obj;

  public:
    DmaDoneEvent(VideoInterface *_obj) :
        Event(), obj(*_obj) {
    }

    void process() {
      obj.dmaDone();
    }

    const std::string name() const {
      return obj.name() + ".DmaDoneEvent";
    }
  };

  /** VNC server */
  VncInput *vnc;

  /** Helper to write out bitmaps */
  Bitmap *bmp;

  /** CLCDC supports up to 1024x768 */
  uint8_t *dmaBuffer;
  uint32_t startAddr;
  Tick startTime;
  uint32_t maxAddr;

  /** Send updated parameters to the vnc server */
  void updateVideoParams();

  /** DMA framebuffer read */
  void readFramebuffer();

  /** Generate dma framebuffer read event */
  void generateReadEvent();

  /** Function to generate interrupt */
  void generateInterrupt();

  /** fillFIFO event */
  void fillFifo();

  /** start the dmas off after power is enabled */
  void startDma();

  /** DMA done event */
  void dmaDone();

  /** DMA framebuffer read event */
  EventWrapper<VideoInterface, &VideoInterface::readFramebuffer> readEvent;

  /** Fill fifo */
  EventWrapper<VideoInterface, &VideoInterface::fillFifo> fillFifoEvent;

  /**@{*/
  /**
   * All pre-allocated DMA done events
   *
   * The PL111 model preallocates maxOutstandingDma number of
   * DmaDoneEvents to avoid having to heap allocate every single
   * event when it is needed. In order to keep track of which events
   * are in flight and which are ready to be used, we use two
   * different vectors. dmaDoneEventAll contains <i>all</i>
   * DmaDoneEvents that the object may use, while dmaDoneEventFree
   * contains a list of currently <i>unused</i> events. When an
   * event needs to be scheduled, the last element of the
   * dmaDoneEventFree is used and removed from the list. When an
   * event fires, it is added to the end of the
   * dmaEventFreeList. dmaDoneEventAll is never used except for in
   * initialization and serialization.
   */
  DmaDoneEvent dmaDoneEvent;

  /** Wrapper to create an event out of the interrupt */
  EventWrapper<VideoInterface, &VideoInterface::generateInterrupt> intEvent;

  bool enableCapture;

public:
  typedef VideoInterfaceParams Params;

  const Params *
  params() const {
    return dynamic_cast<const Params *>(_params);
  }
  VideoInterface(const Params *p);
  ~VideoInterface();

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


#endif /* MAPU_VIF_HH_ */
