/*
 * apc_if.c
 *
 *  Created on: Oct 15, 2015
 *      Author: wangl
 */
#include "qemu/sockets.h"
#include "hw/sysbus.h"
#include "qemu-common.h"
#include "qemu/main-loop.h"
#include <errno.h>
#include "qemu/thread.h"
#include "apc_if.h"
#include "sys/ioctl.h"

//#define DEBUG_APC_IF

#ifdef DEBUG_APC_IF
#define DPRINTF(fmt, ...) \
do { fprintf(stderr, "ape_if: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) \
do {} while (0)
#endif

#define TYPE_APC_IF "apc_if"

#define APC_IF(obj) OBJECT_CHECK(APCIfState, (obj), TYPE_APC_IF)

#define REGS_SIZE (32*1024*1024)

//static const uint64_t NUM_APES = 4;
union csu_mmap ape[4];

typedef struct APCIfState {
  SysBusDevice parent_obj;

  MemoryRegion iomem;
  qemu_irq  irq[8];

  //unsigned char *regs;



  int listen_fd;
  int fd;

  struct sockaddr_in addr;
  IOHandler *send_fn;

} APCIfState;

static void apc_if_socket_accept(void *opaque);

static ssize_t apc_if_atomic_write(int fd, const void *s, size_t n) {
  const char *p = s;
  size_t pos = 0;

  // Keep writing until we've written all of the data
  while (n > pos) {
    ssize_t result = write(fd, p + pos, n - pos);

    // We didn't manage to write anything this time, so we should
    // probably punt, otherwise we'd just keep spinning
    if (result == 0)
    break;

    // If there was an error, try again on EINTR/EAGAIN, pass the
    // error up otherwise.
    if (result == -1) {
      if (errno == EINTR || errno == EAGAIN)
      continue;
      return result;
    }

    pos += result;
  }

  return pos;
}

static void detach_socket(void *opaque)
{
	APCIfState *s = opaque;
	qemu_set_fd_handler(s->fd, NULL, NULL, s);
	if (s->listen_fd != -1)
	{
		qemu_set_fd_handler(s->listen_fd, apc_if_socket_accept, NULL, s);
	}
	closesocket(s->fd);
}

static void clearIntr(void * opaque, unsigned core_id, unsigned type) {
  APCIfState *s = opaque;
  // type: 0: DMA, 1: Mailbox
  qemu_irq_lower(s->irq[(core_id << 1) + type]);

}

static void updateIntr(void * opaque, unsigned core_id) {
  APCIfState *s = opaque;
  if (ape[core_id].csu_if.DMAQueryType == 0x4) {
    if ((ape[core_id].csu_if.DMAQueryStatus &
         ape[core_id].csu_if.DMAQueryMask) ==
        ape[core_id].csu_if.DMAQueryMask)
      if ((ape[core_id].csu_if.DMAQueryMask &
           ape[core_id].csu_if.DMAGrpIntClr) == 0)
        // sendIntr(core_id, dma)
        qemu_irq_raise(s->irq[(core_id << 1) + 0]);
      else clearIntr(s, core_id, 0);
    else clearIntr(s, core_id, 0);
  } else if (ape[core_id].csu_if.DMAQueryType == 0x5) {
    if ((ape[core_id].csu_if.DMAQueryStatus & 
         ape[core_id].csu_if.DMAQueryMask &
         ~ape[core_id].csu_if.DMAGrpIntClr) != 0)
      // sendIntr(core_id, dma)
      qemu_irq_raise(s->irq[(core_id << 1) + 0]);
    else clearIntr(s, core_id, 0);
  } else clearIntr(s, core_id, 0);
}

static uint64_t apc_if_read(void *opaque, hwaddr offset, unsigned size) {
  APCIfState *s = opaque;
  if (s->fd == -1) apc_if_socket_accept(s);

  uint64_t data;

  uint32_t core_id = offset / (4096 * 1024);
  uint32_t core_off = offset % (4096 * 1024);
  DPRINTF("Reading from APE %d at core_off: %#x\n", core_id, core_off);
  assert(core_id < NUM_APES);
  assert(core_off < sizeof(union csu_mmap));

  data = ape[core_id].mem[core_off / 4];

  switch(size) {
    case 1:
      data &= 0xFFUL;
      break;
    case 2:
      data &= 0xFFFFUL;
      break;
    case 4:
      data &= 0xFFFFFFFFUL;
      break;
    default:
      fprintf(stderr, "APC read size too big?\n");
      break;
  }
  if (core_off == 0xB0) {
    ape[core_id].csu_if.MailNum &= 0xFFFFFF00;
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (s->fd < 0)
      fprintf(stderr, "APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = apc_if_atomic_write(s->fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF("Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      fprintf(stderr, "APC is incompatible, accept size is %d.\n", (int)ret);
      detach_socket(s);
    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  } else if (core_off == 0xB8) {
    ape[core_id].csu_if.MailNum &= 0xFF00FFFF;
    clearIntr(s, core_id, 1);
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (s->fd < 0)
      fprintf(stderr, "APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = apc_if_atomic_write(s->fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF("Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      fprintf(stderr, "APC is incompatible, accept size is %d.\n", (int)ret);
      detach_socket(s);
    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  }

  return data;
}

static void apc_if_write(void * opaque, hwaddr offset,
                         uint64_t value, unsigned size) {
  APCIfState *s = opaque;
  if (s->fd == -1) apc_if_socket_accept(s);

  uint32_t core_id = offset / (4096 * 1024);
  uint32_t core_off = offset % (4096 * 1024);
  DPRINTF("Writing 0x%llx to APE %d at core_off: %#x\n", value, core_id, core_off);
  assert(core_id < NUM_APES);
  assert(core_off < sizeof(union csu_mmap));

  uint32_t data = 0;

  switch(size) {
    case 1:
      data = value & 0xFF;
      break;
    case 2:
      data = value & 0xFFFF;
      break;
    case 4:
      data = value;
      break;
    default:
      fprintf(stderr, "APC write size too big?\n");
      break;
  }

  ape[core_id].mem[core_off / 4] = data;

  //if (data_fd < 0) panic("APC not properly attached.\n");
  if (core_off == 0x0) {
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (ape[core_id].csu_if.VPUControl == 0) {
      ape[core_id].csu_if.VPUControl = CMD_START;
      ape[core_id].csu_if.VPUStatus = 1;
    } else if (ape[core_id].csu_if.VPUControl == 1) {
      ape[core_id].csu_if.VPUControl = CMD_STOP;
    } else if (ape[core_id].csu_if.VPUControl == CMD_SHUTDOWN) {
      ape[core_id].csu_if.VPUControl = CMD_SHUTDOWN;
    } else {
      fprintf(stderr, "Unknown APC command.");
      ape[core_id].csu_if.VPUControl = CMD_NONE;
    }
    if (s->fd < 0)
      fprintf(stderr, "APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = apc_if_atomic_write(s->fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF("Send csu package to core %d.\n", core_id);
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
    if (ret < sizeof(struct csu_pkt)) {
      fprintf(stderr, "APC is incompatible, accept size is %d.\n", (int)ret);
      detach_socket(s);
    } else {
      ape[core_id].csu_if.VPUControl = CMD_NONE;
    }
  } else if (core_off == 0x78) {
    ape[core_id].csu_if.dma.DMACommandStatus = 1;
    if (s->fd < 0)
      fprintf(stderr, "APC not properly attached.\n");
    struct csu_pkt cpkt = {core_id, ape[core_id]};
    ssize_t ret = apc_if_atomic_write(s->fd, &cpkt, sizeof(struct csu_pkt));
    DPRINTF("Send csu package to core %d.\n", core_id);
    if (ret < sizeof(struct csu_pkt)) {
      fprintf(stderr, "APC is incompatible, accept size is %d.\n", (int)ret);
      detach_socket(s);
    }
  } else if (core_off == 0xA8) {
    uint32_t saved_cmd_status = ape[core_id].csu_if.dma.DMACommandStatus;
    ape[core_id].csu_if.dma.DMACommandStatus = 0;
    if (ape[core_id].csu_if.MailNum & 0xFF00) {
      ape[core_id].csu_if.MailNum = ape[core_id].csu_if.MailNum - 0x100;
      if (s->fd < 0)
        fprintf(stderr, "APC not properly attached.\n");
      struct csu_pkt cpkt = {core_id, ape[core_id]};
      ssize_t ret = apc_if_atomic_write(s->fd, &cpkt, sizeof(struct csu_pkt));
      DPRINTF("Send csu package to core %d.\n", core_id);
      if (ret < sizeof(struct csu_pkt)) {
    	fprintf(stderr, "APC is incompatible, accept size is %d.\n", (int)ret);
    	detach_socket(s);
      }
    } else if (core_off == 0x98) {
      updateIntr(s, core_id);
    } else if (core_off == 0xC8) {
      updateIntr(s, core_id);
    }
    ape[core_id].csu_if.dma.DMACommandStatus = saved_cmd_status;
  }
}

static const MemoryRegionOps apc_if_ops = {
  .read = apc_if_read,
  .write = apc_if_write,
  .endianness = DEVICE_NATIVE_ENDIAN,
};


static void apc_if_socket_send(void *opaque){
  APCIfState *s = opaque;

  size_t ret;
  uint32_t pkt[3];
  int size = 0;
  int err;

  if (s->fd < 0)
	fprintf(stderr, "APC not properly attached.\n");
  ioctl(s->fd, FIONREAD, &size);

  if (size < 0) {
    err = socket_error();
    if (err != EWOULDBLOCK)
    goto eoc;
  } else if (size == 0) {
    /* end of connection */
  eoc:
    qemu_set_fd_handler(s->fd, NULL, NULL, s);
    if (s->listen_fd != -1) {
      qemu_set_fd_handler(s->listen_fd, apc_if_socket_accept, NULL, s);
    }
    closesocket(s->fd);

    s->fd = -1;
    return;
  }

  do {
    ret = read(s->fd, pkt, 4 * 3);
    DPRINTF("Received packet: 0x%x\n", pkt[2]);
    if (ret == 12) {
      DPRINTF("Updating from APE %d at core_off: %#x\n", pkt[0], pkt[1]);
      assert(pkt[0] < NUM_APES);
      assert(pkt[1] < sizeof(union csu_mmap));
      ape[pkt[0]].mem[pkt[1] / 4] = pkt[2];
      if ((pkt[1]) == 0xB0) {
        ape[pkt[0]].csu_if.MailNum |= 0x1;
      } else if ((pkt[1]) == 0xB8) {
        ape[pkt[0]].csu_if.MailNum |= 0x10000;
        // sendIntr(pkt[0], mail)
        qemu_irq_raise(s->irq[(pkt[0] << 1) + 1]);
      } else if ((pkt[1]) == 0xA0) {
        updateIntr(s, pkt[0]);
      }
      size -= 4 * 3;
    } else break;
  } while (((ret == -1) && (errno == EINTR)) || size > 0);
}

static void apc_if_socket_connect(void *opaque) {
    APCIfState *s = opaque;
    s->send_fn = apc_if_socket_send;
    qemu_set_fd_handler(s->fd, s->send_fn, NULL, s);
}

static void apc_if_socket_accept(void *opaque) {
  APCIfState *s = opaque;
  struct sockaddr_in saddr;
  socklen_t len;
  int fd;

  for (;;) {
    len = sizeof(saddr);
    fd = qemu_accept(s->listen_fd, (struct sockaddr *)&saddr, &len);
    if (fd < 0 && errno != EINTR) {
      return;
    } else if (fd >= 0) {
      qemu_set_fd_handler(s->listen_fd, NULL, NULL, NULL);
      break;
    }
  }

  s->fd = fd;
  apc_if_socket_connect(s);
  DPRINTF("socket: connection from %s:%d",
          inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port));
}

static int apc_if_listen_init(void* opaque, int port)
{
  APCIfState *s = (APCIfState*)opaque;
  struct sockaddr_in saddr;
  int fd, ret;
  FILE * infoout = NULL;

  infoout = fopen("info.out", "a+");
  assert(infoout != NULL);

  fd = qemu_socket(PF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    perror("socket");
    return -1;
  }
  qemu_set_block(fd);

  socket_set_fast_reuse(fd);

  saddr.sin_family = PF_INET;
  saddr.sin_addr.s_addr = INADDR_ANY;

  saddr.sin_port = htons(port);

  while (bind(fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
    DPRINTF("can't bind address terminal port %d inuse PID %d\n",
            port, getpid());
    port++;
    saddr.sin_port = htons(port);
  }

  fprintf(infoout, "\nListening for system.realview.apc connection on port %d\n", port);
  fclose(infoout);

  ret = listen(fd, 0);
  if (ret < 0) {
    perror("listen");
    closesocket(fd);
    return -1;
  }

  s->fd = -1;
  s->listen_fd = fd;

  qemu_set_fd_handler(s->listen_fd, apc_if_socket_accept, NULL, s);
  return 0;
}

static int apc_if_init(SysBusDevice *dev) {
  APCIfState *s = APC_IF(dev);
  int port = 4000;
  int i;

  //s->regs = (unsigned char*)malloc(REGS_SIZE);
  //memset(s->regs, 0, REGS_SIZE);
  for(i=0; i<8; i++)
    sysbus_init_irq(dev, &s->irq[i]);


  memory_region_init_io(&s->iomem, OBJECT(s), &apc_if_ops, s, "apc_if",
                        REGS_SIZE);
  sysbus_init_mmio(dev, &s->iomem);

  apc_if_listen_init(s, port);
  return 0;
}

static void apc_if_class_init(ObjectClass *klass, void *data) {
  //DeviceClass *dc = DEVICE_CLASS(klass);
  SysBusDeviceClass *k = SYS_BUS_DEVICE_CLASS(klass);

  k->init = apc_if_init;
  //dc->vmsd = &vmstate_dw_apb_timer;
}

static const TypeInfo apc_if_info = {
    .name          = TYPE_APC_IF,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(APCIfState),
    .class_init    = apc_if_class_init,
};

static void apc_if_register_types(void)
{
    type_register_static(&apc_if_info);
}

type_init(apc_if_register_types);
