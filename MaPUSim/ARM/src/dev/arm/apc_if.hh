/*
 * apc_if.hh
 *
 *  Created on: Mar 13, 2014
 *      Author: wangl
 */

#ifndef APC_IF_HH_
#define APC_IF_HH_

#include <iostream>

#include "base/pollevent.hh"
#include "base/socket.hh"
#include "dev/arm/amba_device.hh"
#include "cpu/intr_control.hh"
#include "params/APCInterface.hh"
#include "sim/sim_object.hh"

class APCInterface: public AmbaIntDevice {
protected:
  static const uint64_t AMBA_ID = ULL(0xb105f00d44571111);
  static const uint64_t NUM_APES = 4;

  enum CSU_CMD {
    CMD_NONE = 0,
    CMD_START = 1,
    CMD_STOP = 2,
    CMD_SHUTDOWN = 3,
  };

  enum QUERY_TYPE {
    QT_NOINT_AND = 0,
    QT_NOINT_OR = 1,
    QT_INT_AND = 4,
    QT_INT_OR = 5,
  };

  #pragma pack(push, 4)
  struct dma_if {
    unsigned int DMALocalAddr;
    unsigned int pad0;
    unsigned int DMALocalXNum;
    unsigned int pad1;
    int DMALocalYStep;
    unsigned int pad2;
    unsigned int DMALocalYNum;
    unsigned int pad3;
    int DMALocalZStep;
    unsigned int pad4;
    unsigned int DMALocalYAllNum;
    unsigned int pad5;
    unsigned int DMAGlobalAddr;
    unsigned int pad6;
    unsigned int DMAGlobalXNum;
    unsigned int pad7;
    int DMAGlobalYStep;
    unsigned int pad8;
    unsigned int DMAGlobalYNum;
    unsigned int pad9;
    int DMAGlobalZStep;
    unsigned int pad10;
    unsigned int DMAGlobalYAllNum;
    unsigned int pad11;
    unsigned int DMAGroupNum;
    unsigned int pad12;
    unsigned int DMACmd;        //0x78
    union {
      struct {
        unsigned int pad13;
        unsigned int DMACommandStatus;       //0x80
      };
      struct dma_if *Next;
    };
  };

  struct csu_interface {
    unsigned int VPUControl;             //0x00
    unsigned int pad0;
    unsigned int VPUStatus;              //0x08
    unsigned int pad1;
    struct dma_if dma;                   //0x10
    unsigned int pad3;
    unsigned int DMAQueueNum;            //0x88
    unsigned int pad4;
    unsigned int DMAQueryMask;           //0x90
    unsigned int pad5;
    unsigned int DMAQueryType;           //0x98
    unsigned int pad6;
    unsigned int DMAQueryStatus;         //0xA0
    unsigned int pad7;
    unsigned int MailBoxIn;              //0xA8
    unsigned int pad8;
    unsigned int MailBoxOut;             //0xB0
    unsigned int pad9;
    unsigned int MailBoxOutInt;          //0xB8
    unsigned int pad10;
    unsigned int MailNum;                //0xC0
    unsigned int pad11;
    unsigned int DMAGrpIntClr;           //0xC8
  };

  union csu_mmap {
    struct csu_interface csu_if;
    unsigned int mem[52];
  };
  #pragma pack(pop)

  union csu_mmap ape[NUM_APES];

  struct csu_pkt {
    uint32_t core_id;
    union csu_mmap ape;
  };

protected:
  class ListenEvent: public PollEvent {
  protected:
    APCInterface *apc;

  public:
    ListenEvent(APCInterface *a, int fd, int e);
    void process(int revent);
  };

  friend class ListenEvent;
  ListenEvent *listenEvent;

  class DataEvent: public PollEvent {
  protected:
    APCInterface *apc;

  public:
    DataEvent(APCInterface *a, int fd, int e);
    void process(int revent);
  };

  friend class DataEvent;
  DataEvent *dataEvent;

protected:
  int data_fd;

  class SyncEvent: public Event {
  protected:
    APCInterface *apc;
    int *fd_p;

  public:
    SyncEvent(APCInterface *a, int *fd) : Event(Sim_Exit_Pri + 1), apc(a), fd_p(fd) {}
    void process();
  };

  SyncEvent syncEvent;

public:
  typedef APCInterfaceParams Params;
  APCInterface(const Params *p);
  ~APCInterface();

protected:
  ListenSocket listener;

  void listen(int port);
  void accept();

public:
  ///////////////////////
  // Terminal Interface

  void data();
  void detach();

  /**
   * Handle a read to the device
   * @param pkt The memory request.
   * @param data Where to put the data.
   */
  virtual Tick read(PacketPtr pkt);

  /**
   * Handle writes to the device
   * @param pkt The memory request.
   * @param data the data
   */
  virtual Tick write(PacketPtr pkt);

  virtual void serialize(std::ostream &os) {}
  virtual void unserialize(Checkpoint *cp, const std::string &section) {}
};

#endif /* APC_IF_HH_ */
