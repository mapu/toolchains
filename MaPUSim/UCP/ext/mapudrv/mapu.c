/*
 * mapu.c
 *
 *  Created on: Nov 16, 2013
 *      Author: wangl
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/mm.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <asm/uaccess.h>
#include <asm/pgtable.h>
#include <asm/current.h>
#include "mapu.h"

int mapu_major = MAPU_MAJOR;
const int mapu_minor = 0;
const int ape_cores = APE_CORES;

MODULE_AUTHOR("Lei Wang");
MODULE_LICENSE("Dual BSD/GPL");

/* The clone-specific data structure includes a key field */
enum pair_state {
  PAIR_NONE = 0,
  PAIR_SOUND,
  PAIR_LOSE_USER,
  PAIR_LOSE_SIM,
};

struct vmapu {
  struct mapu_dev device;
  pid_t u_key;
  pid_t m_key;
  uid_t owner;
  int urefcnt;
  int mrefcnt;
  enum pair_state pstat;
  struct list_head list;
};

/* The list of devices, and a lock to protect it */

static LIST_HEAD(mapu_list);

static spinlock_t mapu_lock = SPIN_LOCK_UNLOCKED;

static spinlock_t ddr_pool_lock = SPIN_LOCK_UNLOCKED;

static struct page *share_mem_pool[MAX_NUM_PAIRS];
static struct page *ddr_pool[MAX_NUM_PAIRS][16];
static unsigned int pair_map;

//static LIST_HEAD(ddr_pool);

int mapu_trim(struct mapu_dev *dev);
void mapu_cleanup(void);

static struct mapu_dev *mapu_lookfor_device(pid_t key) {
  struct vmapu *lptr;
  list_for_each_entry(lptr, &mapu_list, list) {
    if (lptr->owner != current_uid()) continue;
    if (lptr->u_key == key || lptr->m_key == key) {
      if (lptr->u_key == key) {
        lptr->urefcnt++;
      } else {
        lptr->mrefcnt++;
      }
      return &(lptr->device);
    }
  }

  list_for_each_entry(lptr, &mapu_list, list) {
    if (lptr->owner != current_uid()) continue;
    // Occupy an unused MaPU device.
    // Note: this is potentially mistake that a simulator occupies a simulator
    // and will be handled by later ioctl
    if (lptr->u_key == -1 && lptr->pstat == PAIR_NONE) {
      lptr->u_key = key;
      return &(lptr->device);
    }
  }

  lptr = kmalloc(sizeof(struct vmapu), GFP_KERNEL);
  if (!lptr) return NULL;

  memset(lptr, 0, sizeof(struct vmapu));
  lptr->u_key = key;
  lptr->owner = current_uid();
  lptr->m_key = -1;
  mapu_trim(&(lptr->device));
  sema_init(&lptr->device.sem, 1);

  list_add(&lptr->list, &mapu_list);
  return &(lptr->device);
}

int mapu_open(struct inode *inode, struct file *filp) {
  struct mapu_dev *dev;

  //dev = container_of(inode->i_cdev, struct mapu_dev, cdev);

  spin_lock(&mapu_lock);

  dev = mapu_lookfor_device(current->pid);

  spin_unlock(&mapu_lock);

  if (!dev) {
    printk(KERN_ERR "Mapu device initialization failed.\n");
    return -EFAULT;
  }

  filp->private_data = dev;

  return 0;
}

static void release_mapu_dev(struct mapu_dev *dev) {
  int i = 0;
  int j = 0;
  if (!dev) return;
  down(&dev->sem);
  if (dev->ape_if) {
    for (i = 0; i < ape_cores; i++) {
      if (dev->ape_if[i].ape2ddr.data) {
        vfree(dev->ape_if[i].ape2ddr.data);
        dev->ape_if[i].ape2ddr.data = NULL;
      }
      if (dev->ape_if[i].ddr2ape.data) {
        vfree(dev->ape_if[i].ddr2ape.data);
        dev->ape_if[i].ddr2ape.data = NULL;
      }
    }
    kfree(dev->ape_if);
    dev->ape_if = NULL;
  }
  if (dev->pair_id != -1) {
    pair_map |= 1 << dev->pair_id;

    share_mem_pool[dev->pair_id] = alloc_pages(GFP_KERNEL | __GFP_NOFAIL, 10);
    if (!share_mem_pool[dev->pair_id]) {
      printk(KERN_ERR "Out of memory when allocating SHM.\n");
    }
    get_page(share_mem_pool[dev->pair_id]);
    for (j = 0; j < MAX_NUM_PAIRS; j++) {
      ddr_pool[dev->pair_id][j] = alloc_pages(GFP_KERNEL | __GFP_NOFAIL, 10);
      if (!ddr_pool[dev->pair_id][j]) {
        printk(KERN_ERR "Out of memory when allocating DDR.\n");
      }
      get_page(ddr_pool[dev->pair_id][j]);
    }
    dev->pair_id = -1;
  }
}

static void remove_mapu_from_list(struct file *filp) {
  struct vmapu *lptr;
  list_for_each_entry(lptr, &mapu_list, list) {
    if (lptr->owner != current_uid()) continue;
    if (lptr->u_key == current->pid) {
      if (filp->private_data != &lptr->device) {
        printk(KERN_ERR "Mapu device is not consistent.\n");
        lptr->u_key = -1;
        lptr->pstat = PAIR_LOSE_USER;
        return;
      }
      if (lptr->urefcnt != 0) {
        lptr->urefcnt--;
        return;
      }
      lptr->pstat = PAIR_LOSE_USER;
      if (lptr->m_key == -1) {
        list_del(&lptr->list);
        release_mapu_dev(&lptr->device);
        kfree(lptr);
        filp->private_data = NULL;
        return;
      } else {
        down(&lptr->device.sem);
        lptr->device.ape_if[0].csu_if.csu_if.VPUControl = CMD_SHUTDOWN;
        up(&lptr->device.sem);
        lptr->u_key = -1;
        return;
      }
    }
    if (lptr->m_key == current->pid) {
      if (filp->private_data != &lptr->device) {
        printk(KERN_ERR "Mapu device is not consistent.\n");
        lptr->m_key = -1;
        lptr->pstat = PAIR_LOSE_SIM;
        return;
      }
      if (lptr->mrefcnt != 0) {
        lptr->mrefcnt--;
        return;
      }
      lptr->pstat = PAIR_LOSE_SIM;
      if (lptr->u_key == -1) {

        list_del(&lptr->list);
        release_mapu_dev(&lptr->device);
        kfree(lptr);
        filp->private_data = NULL;
        return;
      } else {
        lptr->m_key = -1;
        return;
      }
    }
  }
  printk(KERN_ERR "Mapu is not on the virtual device list.\n");
}

int mapu_release(struct inode *inode, struct file *filp) {

  spin_lock(&mapu_lock);

  remove_mapu_from_list(filp);

  spin_unlock(&mapu_lock);

  return 0;
}

ssize_t mapu_read(struct file *filp, char __user *buf, size_t count,
                  loff_t *f_pos) {
  struct mapu_dev *dev = filp->private_data;
  unsigned int ape_id;
  loff_t offset;
  struct ape_dev *ape;
  loff_t next_byte;
  unsigned int status;

  // Sanity check
  if (!dev) return -EFAULT;
  if (*f_pos < APE_CR_BASE) {
    // Wrong APE control register address
    printk(KERN_ERR "User program accessed unknown APE SoC address space.\n");
    return -EFAULT;
  }
  ape_id = (*f_pos - APE_CR_BASE) / APE_CR_OFFSET;
  offset = (*f_pos - APE_CR_BASE) % APE_CR_OFFSET;

  if (ape_id >= ape_cores) {
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }

  if (offset >= APE_CR_LEN) {
    printk(KERN_ERR "User program accessed unknown APE control register.\n");
    return -ENOMEM;
  }

  next_byte = offset + count;
  if (next_byte > sizeof(union csu_mmap)) {
    printk(KERN_ERR "User program's access exceeds vaild range of APE control register space %llx.\n", next_byte);
    return -ENOMEM; // Operating more than one csu once is not allowed
  }

  if (count % 4 != 0 || count < 4 || offset % 4 != 0) {
    printk(KERN_ERR "User program's access is unaligned to 4 bytes.\n");
    return -EFAULT; // Any unaligned access is not allowed
  }

  if (down_interruptible(&dev->sem))
    return -EBUSY;

  if (!dev->ape_if) {
    up(&dev->sem);
    printk(KERN_ERR "User program accessed APE before a simulator is paired.\n");
    return -EFAULT;
  }

  ape = &(dev->ape_if[ape_id]);

  if (ape->ape2ddr.data != NULL) {
    if (ape->ape2ddr.len != 0) {
      if (copy_to_user((void __user *)(ape->ape2ddr.u_addr), ape->ape2ddr.data,
                       ape->ape2ddr.len)) {
        printk(KERN_ERR "Fail to respond ARM user space(0x%lx) data fetching request, length of %d.\n",
               ape->ape2ddr.u_addr, ape->ape2ddr.len);
        vfree(ape->ape2ddr.data);
        ape->ape2ddr.data = NULL;
        ape->ape2ddr.len = 0;
        ape->ape2ddr.u_addr = 0;
        up(&dev->sem);
        return -ENOMEM;
      }
    } else {
      printk(KERN_NOTICE "Nothing to respond ARM user space(0x%lx) data fetching request, length is 0.\n",
             ape->ape2ddr.u_addr);
    }
    vfree(ape->ape2ddr.data);
    ape->ape2ddr.data = NULL;
    ape->ape2ddr.len = 0;
    ape->ape2ddr.u_addr = 0;
  }

  status = ape->csu_if.csu_if.DMAQueryType;

  // special register handling
  if (offset <= 0x98 && next_byte > 0x98) {
    unsigned int orResult = (ape->csu_if.csu_if.DMAQueryStatus &
                             ape->csu_if.csu_if.DMAQueryMask) != 0;
    unsigned int andResult = (ape->csu_if.csu_if.DMAQueryStatus &
                              ape->csu_if.csu_if.DMAQueryMask) !=
                                ape->csu_if.csu_if.DMAQueryMask;
    ape->csu_if.csu_if.DMAQueryType = status == 1 ? orResult : andResult;
  }

  if (copy_to_user(buf, &(ape->csu_if.mem[offset>>2]), count)) {
    ape->csu_if.csu_if.DMAQueryType = status;
    up(&dev->sem);
    return -ENOMEM;
  }

  ape->csu_if.csu_if.DMAQueryType = status;

  // post handling
  if (offset <= 0xA8 && next_byte > 0xA8) {
    unsigned char *out = ((unsigned char *)&(ape->csu_if.csu_if.MailNum)) + 2;
    if (*out > 0) (*out) = 0;
  }
  if (offset <= 0xB8 && next_byte > 0xB8) {
    unsigned char *out = (unsigned char *)&(ape->csu_if.csu_if.MailNum);
    if (*out > 0) (*out) = 0;
  }
  *f_pos += count;
  up(&dev->sem);
  return count;
}

ssize_t mapu_write(struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos) {
  struct mapu_dev *dev = filp->private_data;
  unsigned int ape_id;
  loff_t offset;
  struct ape_dev *ape;
  union csu_mmap *ubuf_p;
  loff_t cur;
  loff_t next_byte;
  unsigned char *in;

  // Sanity check
  if (!dev) return -EFAULT;
  if (*f_pos < APE_CR_BASE) {
    // Wrong APE control register address
    printk(KERN_ERR "User program accessed unknown APE SoC address space.\n");
    return -EFAULT;
  }
  ape_id = (*f_pos - APE_CR_BASE) / APE_CR_OFFSET;
  offset = (*f_pos - APE_CR_BASE) % APE_CR_OFFSET;

  if (ape_id >= ape_cores) {
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }

  if (offset >= APE_CR_LEN) {
    printk(KERN_ERR "User program accessed unknown APE control register.\n");
    return -ENOMEM;
  }

  next_byte = offset + count;
  if (next_byte > sizeof(union csu_mmap)) {
    printk(KERN_ERR "User program's access exceeds vaild range of APE control register space %llx.\n", next_byte);
    return -ENOMEM; // Operating more than one csu once is not allowed
  }

  if (count % 4 != 0 || count < 4 || offset % 4 != 0) {
    printk(KERN_ERR "User program's access is unaligned to 4 bytes.\n");
    return -EFAULT; // Any unaligned access is not allowed
  }

  if (down_interruptible(&dev->sem))
    return -EBUSY;

  if (!dev->ape_if) {
    up(&dev->sem);
    printk(KERN_ERR "User program accessed APE before a simulator is paired.\n");
    return -EFAULT;
  }

  ape = &(dev->ape_if[ape_id]);
  ubuf_p = vmalloc(sizeof(union csu_mmap));

  if (!ubuf_p) return -ENOMEM;
  memset(ubuf_p, 0, sizeof(union csu_mmap));

  if (copy_from_user(&ubuf_p->mem[offset>>2],
                     (const char __user *)buf,
                     count)) {
    up(&dev->sem);
    printk(KERN_ERR "Failed to fetch CSU context update from user program.\n");
    vfree(ubuf_p);
    return -EFAULT;
  }

  cur = offset;

  while (cur < next_byte) {
    switch (cur) {
    case 0x0:
      if (ubuf_p->csu_if.VPUControl == 0) {
        ape->csu_if.csu_if.VPUControl = CMD_START;
        ape->csu_if.csu_if.VPUStatus = 1;
      } else if (ubuf_p->csu_if.VPUControl == 1)
        ape->csu_if.csu_if.VPUControl = CMD_STOP;
      else if (ubuf_p->csu_if.VPUControl == CMD_SHUTDOWN)
        ape->csu_if.csu_if.VPUControl = CMD_SHUTDOWN;
      else {
        up(&dev->sem);
        printk(KERN_ERR "Unknown APE Command %x.\n", ubuf_p->csu_if.VPUControl);
        vfree(ubuf_p);
        return -ENXIO;
      }
    case 0x8:
      cur = 0x10;
      break;

    case 0x78: {
      if (((ubuf_p->csu_if.dma.DMACmd & 0x3) == 0) &&
          (ape->csu_if.csu_if.dma.DMAGroupNum == 0)) {
        ape->ddr2ape.len = ape->csu_if.csu_if.dma.DMAGlobalYAllNum *
                           ape->csu_if.csu_if.dma.DMAGlobalXNum;
        if (ape->ddr2ape.len == 0) {
          printk(KERN_NOTICE "Nothing to copy from user, length is 0.\n");
          break;
        }
        if (ape->ddr2ape.data) {
          printk(KERN_NOTICE "Data overwritten. DMA status %x. DMA cmd status %x\n",
                 ape->csu_if.csu_if.DMAQueryStatus,
                 ape->csu_if.csu_if.dma.DMACommandStatus);
          vfree(ape->ddr2ape.data); // drop overlapped data
        }
        ape->ddr2ape.data = vmalloc(ape->ddr2ape.len);
        if (!ape->ddr2ape.data) {
          up(&dev->sem);
          vfree(ubuf_p);
          return -ENOMEM;
        }
        ape->ddr2ape.u_addr = ape->csu_if.csu_if.dma.DMAGlobalAddr;
        if (copy_from_user(ape->ddr2ape.data,
                           (const char __user *)(ape->ddr2ape.u_addr),
                           ape->ddr2ape.len)) {
          vfree(ape->ddr2ape.data);
          ape->ddr2ape.data = NULL;
          up(&dev->sem);
          printk(KERN_ERR "Fail to fetch ARM user space(0x%lx) data for sending request, length of %d.\n",
                 ape->ddr2ape.u_addr, ape->ddr2ape.len);
          vfree(ubuf_p);
          return -ENOMEM;
        }
        ape->csu_if.csu_if.dma.DMAGlobalAddr = 0x60000000; // indicate that data is from user space
      } else if (((ubuf_p->csu_if.dma.DMACmd & 0x3) == 1) &&
                 (ape->csu_if.csu_if.dma.DMAGroupNum == 1)) {
        ape->ape2ddr.u_addr = ape->csu_if.csu_if.dma.DMAGlobalAddr;
        ape->ape2ddr.len = ape->csu_if.csu_if.dma.DMAGlobalYAllNum *
                           ape->csu_if.csu_if.dma.DMAGlobalXNum;
        ape->csu_if.csu_if.dma.DMAGlobalAddr = 0x60000000;
        if (ape->ape2ddr.data) {
          printk(KERN_NOTICE "Data overwritten. DMA status %x. DMA cmd status %x\n",
                 ape->csu_if.csu_if.DMAQueryStatus,
                 ape->csu_if.csu_if.dma.DMACommandStatus);
          vfree(ape->ape2ddr.data); // drop overlapped data
          ape->ape2ddr.data = NULL;
        }
      }
      ape->csu_if.csu_if.dma.DMACmd = ubuf_p->csu_if.dma.DMACmd;
      ape->csu_if.csu_if.dma.DMACommandStatus = 1;
    }
    case 0x80:
    case 0x88: {
      cur = 0x90;
      break;
    }
    case 0xA8: {
     in = ((unsigned char *)&(ape->csu_if.csu_if.MailNum)) + 1;
     if (*in != 0) {
       (*in)--;
       ape->csu_if.csu_if.MailBoxIn = ubuf_p->csu_if.MailBoxIn;
     }
    }
    case 0xB0:
    case 0xB8:
    case 0xC0: {
      cur = 0xC8;
      break;
    }

    default:{
      ape->csu_if.mem[cur>>2] = ubuf_p->mem[cur>>2];
      cur += 4;
      break;
    }
    }
  }
  *f_pos += count;
  up(&dev->sem);
  vfree(ubuf_p);
  return count;
}

static int mapu_sim_reg(struct file *filp) {
  struct vmapu *uptr, *mptr;
  list_for_each_entry(uptr, &mapu_list, list) {
    if (uptr->owner != current_uid()) continue;
    if (uptr->u_key == current->pid) {
      uptr->u_key = -1;
      if (uptr->m_key == -1) {
        // New simulator
        list_for_each_entry(mptr, &mapu_list, list) {
          if (mptr->owner != current_uid()) continue;
          if (mptr->m_key == -1 && mptr->u_key != -1 &&
              mptr->pstat == PAIR_NONE) {
            mptr->pstat = PAIR_SOUND;
            mptr->m_key = current->pid;
            filp->private_data = &mptr->device;
            return 0;
          }
        }
        uptr->m_key = current->pid;
        return 0;
      } else {
        // Simulator paired with simulator
        list_for_each_entry(mptr, &mapu_list, list) {
          if (mptr->owner != current_uid()) continue;
          if (mptr->m_key == -1) {
            mptr->m_key = current->pid;
            filp->private_data = &mptr->device;
            return 0;
          }
        }

        mptr = kmalloc(sizeof(struct vmapu), GFP_KERNEL);
        if (!mptr) return -EFAULT;

        memset(mptr, 0, sizeof(struct vmapu));
        mptr->m_key = current->pid;
        mptr->u_key = -1;
        mptr->owner = current_uid();
        mapu_trim(&(mptr->device));

        list_add(&mptr->list, &mapu_list);
        filp->private_data = &mptr->device;
        sema_init(&mptr->device.sem, 1);
        return 0;
      }
    } else if (uptr->m_key == current->pid) {
      return 1;
    }
  }
  return -EFAULT;
}

int mapu_ioctl(struct inode *inode, struct file *filp,
               unsigned int cmd, unsigned long arg) {
  int ret = 0;
  int i = 0;
  const char verify[4] = "APE";
  struct mapu_dev *dev = filp->private_data;
  unsigned int ape_id = filp->f_pos >> 24;
  loff_t offset = filp->f_pos & 0xFFFFFF;
  struct ape_dev *ape;
  struct vmapu *uptr;

  // Sanity check

  if (!dev) return -EFAULT;

  if (down_interruptible(&dev->sem)) return -EBUSY;

  if (!dev->ape_if) {
    up(&dev->sem);
    printk(KERN_ERR "Device is no longer existing.\n");
    return -EFAULT;
  }

  if (ape_id >= ape_cores) {
    up(&dev->sem);
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }
  ape = &(dev->ape_if[ape_id]);

  if (_IOC_TYPE(cmd) != MAPU_IOC_MAGIC) {
    up(&dev->sem);
    return -ENOTTY;
  }

  if (cmd == MAPU_IOCVERIFY) {
    up(&dev->sem);
    if (copy_to_user((void __user *)arg, verify, 4))
      return -EFAULT;
    return 0;
  }

  switch (cmd) {
  case MAPU_IOCAPE2DDR:
    if (ape->ape2ddr.data) {
      printk(KERN_NOTICE "Previous data from APE to user program is not fetched.\n");
      ret = -EFAULT;
      break;
    }
    if (ape->ape2ddr.len == 0) {
      printk(KERN_NOTICE "Nothing to send for responding fetching request by user program.\n");
      ret = -EFAULT;
      break;
    }
    ape->ape2ddr.data = vmalloc(ape->ape2ddr.len);
    if (copy_from_user(ape->ape2ddr.data, (const void __user *)arg,
                       ape->ape2ddr.len)) {
      printk(KERN_ERR "Fail to fetch APE(0x%lx) data for fetching request by user program, length of %d.\n",
             arg, ape->ape2ddr.len);
      vfree(ape->ape2ddr.data);
      ape->ape2ddr.data = NULL;
      ret = -EFAULT;
    }
    break;

  case MAPU_IOCDDR2APE:
    if (ape->ddr2ape.data == NULL) {
      printk(KERN_ERR "Data from user program is missing.\n");
      ret = -EFAULT;
      break;
    }
    if (ape->ddr2ape.len == 0) {
      printk(KERN_NOTICE "Nothing to get from user program for sending respond.\n");
      ret = -EFAULT;
      break;
    }
    if (copy_to_user((void __user *)arg, ape->ddr2ape.data,
                     ape->ddr2ape.len)) {
      printk(KERN_ERR "Fail to send data to APE(0x%lx) for sending respond, length of %d.\n",
             arg, ape->ddr2ape.len);
      ret = -EFAULT;
      break;
    }
    vfree(ape->ddr2ape.data);
    ape->ddr2ape.data = NULL;
    ape->ddr2ape.len = 0;
    ape->ddr2ape.u_addr = 0;
    break;

  case MAPU_IOCSTATUS:
    if (copy_to_user((void __user *)arg, ape->csu_if.mem,
                     sizeof(struct csu_interface))) {
      printk(KERN_ERR "Fail to update CSU context to simulator(0x%lx).\n", arg);
      ret = -EFAULT;
      break;
    }
    ape->csu_if.csu_if.VPUControl = 0;
    break;

  case MAPU_IOCSTATUS_CHANGE:
    if (offset >= 0xC0 ) {
      printk(KERN_ERR "CSU context update by simulator is out of range.\n");
      up(&dev->sem);
      ret = -ENOMEM;
      break;
    }
    if (offset % 4 != 0) {
      printk(KERN_ERR "CSU context update by simulator is unaligned.\n");
      up(&dev->sem);
      ret = -EFAULT;
      break;
    }
    ape->csu_if.mem[offset>>2] = arg;
    break;

  case MAPU_IOCSIMREG: {
    spin_lock(&mapu_lock);
    ret = mapu_sim_reg(filp);
    spin_unlock(&mapu_lock);
    if (ret < 0) break;
    up(&dev->sem);
    dev = filp->private_data;
    if (down_interruptible(&dev->sem)) return -EBUSY;
    if (dev->pair_id != -1) break;
    else if (pair_map & 0xFFFF) {
      for (i = 0; i < 16; i++) {
        if (pair_map & (1 << i)) {
          dev->pair_id = i;
          pair_map &= ~(1 << i);
          break;
        }
      }
      if (i == 16) {
        printk(KERN_ERR "Pair map is inconsistent.\n");
        ret = -ENOMEM;
        break;
      }
      dev->shm = share_mem_pool[dev->pair_id];
      for (i = 0; i < 16; i++)
        dev->ddr[i] = ddr_pool[dev->pair_id][i];
    } else {
      printk(KERN_ERR "Out of memory when allocating SHM and DDR.\n");
      ret = -ENOMEM;
      break;
    }
    break;
  }

  case MAPU_IOCCHECKPAIR: {
    spin_lock(&mapu_lock);
    list_for_each_entry(uptr, &mapu_list, list) {
      if (uptr->u_key == current->pid) {
        if (uptr->m_key != -1 && uptr->pstat != PAIR_LOSE_SIM) {
          uptr->pstat = PAIR_SOUND;
          spin_unlock(&mapu_lock);
          up(&dev->sem);
          return 0;
        } else {
          spin_unlock(&mapu_lock);
          up(&dev->sem);
          return -EFAULT;
        }
      }
    }
    spin_unlock(&mapu_lock);
    ret = -EFAULT;
    break;
  }

  default:
    ret = -EFAULT;
    break;
  }
  up(&dev->sem);
  return ret;
}

loff_t mapu_llseek(struct file *filp, loff_t off, int whence) {
  long newpos;

  switch (whence) {
  case 0:
    newpos = off;
    break;

  case 1:
    newpos = filp->f_pos + off;
    break;

  case 2:
    newpos = sizeof(struct mapu_dev) + off;
    break;

  default:
    return -EINVAL;
  }
  if (newpos < 0) return -EINVAL;
  filp->f_pos = newpos;
  return newpos;
}

int mapu_trim(struct mapu_dev *dev) {
  unsigned int i;

  dev->ape_if = kmalloc(sizeof(struct ape_dev) * ape_cores, GFP_KERNEL);

  if (!dev->ape_if) {
    printk(KERN_ERR "Out of memory.\n");
    return -ENOMEM;
  }

  memset(dev->ape_if, 0, sizeof(struct ape_dev) * ape_cores);
  for (i = 0; i < ape_cores; i++) {
    dev->ape_if[i].csu_if.csu_if.DMAQueryStatus = 0xFFFFFFFF;
    dev->ape_if[i].csu_if.csu_if.dma.DMACommandStatus = 0;
    dev->ape_if[i].csu_if.csu_if.MailNum = 0x00000400;
  }
  dev->pair_id = -1;
  return 0;
}

void mapu_vma_open(struct vm_area_struct *vma) {

}

void mapu_vma_close(struct vm_area_struct *vma) {

}

int mapu_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf) {
  pgoff_t pgoff = vmf->pgoff;
  struct mapu_dev *dev = vma->vm_private_data;

  if (!dev) return VM_FAULT_SIGBUS;

  if (vma->vm_start >= 0x70000000 && vma->vm_end <= 0x80000000 && pgoff < 16 * 1024) {
    if (!dev->ddr) {
      printk(KERN_ERR "DDR is not initialized.\n");
      return -VM_FAULT_SIGBUS;
    }
    if (!dev->ddr[pgoff/1024]) {
      printk(KERN_ERR "DDR is insufficient 0x%lx.\n", pgoff);
      return -VM_FAULT_SIGBUS;
    }
    vmf->page = &dev->ddr[pgoff/1024][pgoff%1024];
    get_page(vmf->page);
  } else if (vma->vm_start >= 0x40400000 && vma->vm_end <= 0x40800000 && pgoff < 1024) {
    if (!dev->shm) {
      printk(KERN_ERR "SHM is not initialized.\n");
      return -VM_FAULT_SIGBUS;
    }
    vmf->page = &dev->shm[pgoff];
    get_page(vmf->page);
  } else {
    printk(KERN_ERR "DDR/SHM is insufficient start from 0x%lx, offset: 0x%lx.\n", vma->vm_start, pgoff);
    return -VM_FAULT_SIGBUS;
  }
  return 0;
}

static struct vm_operations_struct mapu_nopage_vm_ops = {
  .open = mapu_vma_open,
  .close = mapu_vma_close,
  .fault = mapu_vma_fault,
};

static int mapu_nopage_mmap(struct file *filp, struct vm_area_struct *vma) {
  vma->vm_ops = &mapu_nopage_vm_ops;
  vma->vm_flags |= VM_LOCKED | VM_RESERVED;
  vma->vm_private_data = filp->private_data;
  mapu_vma_open(vma);
  return 0;
}

struct file_operations mapu_fops = {
  .owner =      THIS_MODULE,
  .llseek =     mapu_llseek,
  .read =       mapu_read,
  .write =      mapu_write,
  .ioctl =      mapu_ioctl,
  .open =       mapu_open,
  .release =    mapu_release,
  .mmap =       mapu_nopage_mmap,
};

static void mapu_setup_cdev(struct mapu_dev *dev) {
  int err, devno = MKDEV(mapu_major, 0);

  cdev_init(&dev->cdev, &mapu_fops);
  dev->cdev.owner = THIS_MODULE;
  dev->cdev.ops = &mapu_fops;
  err = cdev_add(&dev->cdev, devno, 1);
  if (err)
    printk(KERN_ALERT "Error %d adding virtual mapu.\n", err);
}

static struct mapu_dev *mapu_device;

int mapu_init(void) {
  int result;
  int i = 0, j = 0;
  dev_t dev = 0;

  if (mapu_major) {
    dev = MKDEV(mapu_major, mapu_minor);
    result = register_chrdev_region(dev, 1, "mapu");
  } else {
    result = alloc_chrdev_region(&dev, mapu_minor, 1, "mapu");
    mapu_major = MAJOR(dev);
  }
  if (result < 0) return result;

  mapu_device = kmalloc(sizeof(struct mapu_dev), GFP_KERNEL);
  if (!mapu_device) {
    result = -ENOMEM;
    goto fail_malloc;
  }
  memset(mapu_device, 0, sizeof(struct mapu_dev));
  sema_init(&mapu_device->sem, 1);

  for (i = 0; i < MAX_NUM_PAIRS; i++) {
    share_mem_pool[i] = alloc_pages(GFP_KERNEL | __GFP_NOFAIL, 10);
    if (!share_mem_pool[i]) {
      printk(KERN_ERR "Out of memory when allocating SHM.\n");
      result = -ENOMEM;
      goto fail_malloc;
    }
    get_page(share_mem_pool[i]);
    for (j = 0; j < MAX_NUM_PAIRS; j++) {
      ddr_pool[i][j] = alloc_pages(GFP_KERNEL | __GFP_NOFAIL, 10);
      if (!ddr_pool[i][j]) {
        printk(KERN_ERR "Out of memory when allocating DDR.\n");
        result = -ENOMEM;
        goto fail_malloc;
      }
      get_page(ddr_pool[i][j]);
    }
  }
  pair_map = 0xFFFF;

  mapu_setup_cdev(mapu_device);

  return 0;

fail_malloc:
  unregister_chrdev_region(dev, 1);
  return result;
}

void mapu_cleanup(void) {
  int i = 0, j = 0;
  cdev_del(&mapu_device->cdev);
  kfree(mapu_device);

  for (i = 0; i < MAX_NUM_PAIRS; i++) {
    __free_pages(share_mem_pool[i], 10);
    for (j = 0; j < MAX_NUM_PAIRS; j++)
      __free_pages(ddr_pool[i][j], 10);
  }

  unregister_chrdev_region(MKDEV(mapu_major, mapu_minor), 1);
}

module_init(mapu_init);
module_exit(mapu_cleanup);
