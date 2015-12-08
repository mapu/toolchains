/*
 * io.c
 *
 *  Created on: Nov 26, 2015
 *      Author: wangl
 */

#include <linux/device.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <asm/div64.h>

#include "apc.h"

struct apc_info {
  u32 num_of_apes;
  struct {
    unsigned long iobase;   /* io base address */
    void __iomem  *membase; /* ioremap cookie or NULL */
    resource_size_t mapbase;  /* resource base */
    resource_size_t region_size;
    unsigned int  dma_irq;    /* DMA interrupt number */
    unsigned int  mb_irq;    /* Mailbox interrupt number */
    unsigned int  spuclk;  /* SPU clock rate */
    unsigned int  mpuclk;  /* MPU clock rate */
  } *ape_info;
} apc_data;

static DECLARE_WAIT_QUEUE_HEAD(dmaq);
static volatile int dma_complete;

ssize_t apc_read(struct file *filp, char __user *buf, size_t count,
                  loff_t *f_pos) {
  //struct mapu_dev *dev = filp->private_data;
  u32 ape_id;
  loff_t offset;
  //struct ape_dev *ape;
  loff_t next_byte;
  u32 *reg_copy;
  u32 i;
  //unsigned int status;

  // Sanity check
  //if (!dev) return -EFAULT;
  if (*f_pos < 0) {
    // Wrong APE control register address
    printk(KERN_ERR "User program accessed unknown APE SoC address space.\n");
    return -EFAULT;
  }

  ape_id = *f_pos;
  offset = do_div(ape_id, apc_data.ape_info[ape_id].region_size);

  if (ape_id >= apc_data.num_of_apes) {
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }

  if (offset >= apc_data.ape_info[ape_id].region_size) {
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

  /*if (down_interruptible(&dev->sem))
    return -EBUSY;*/

  /*if (!dev->ape_if) {
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
  up(&dev->sem);*/
  reg_copy = vmalloc(count);
  if (!reg_copy) {
    printk(KERN_ERR "Insufficent memory for copying value.\n");
    return -ENOMEM; // Operating more than one csu once is not allowed
  }
  for (i = 0; i < (count << 2); ++i)
    reg_copy[i] = readl(apc_data.ape_info[ape_id].membase + (offset << 2));
  if (copy_to_user(buf, reg_copy, count)) {
    vfree(reg_copy);
    printk(KERN_ERR "Error when copying value to user space.\n");
    return -ENOMEM;
  }
  *f_pos += count;
  vfree(reg_copy);
  return count;
}

ssize_t apc_write(struct file *filp, const char __user *buf, size_t count,
                   loff_t *f_pos) {
  //struct mapu_dev *dev = filp->private_data;
  u32 ape_id;
  loff_t offset;
  //struct ape_dev *ape;
  //union csu_mmap *ubuf_p;
  //loff_t cur;
  loff_t next_byte;
  u32 *reg_copy;
  u32 i;
  //unsigned char *in;

  // Sanity check
  //if (!dev) return -EFAULT;
  if (*f_pos < 0) {
    // Wrong APE control register address
    printk(KERN_ERR "User program accessed unknown APE SoC address space.\n");
    return -EFAULT;
  }

  ape_id = *f_pos;
  offset = do_div(ape_id, apc_data.ape_info[ape_id].region_size);

  if (ape_id >= apc_data.num_of_apes) {
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }

  if (offset >= apc_data.ape_info[ape_id].region_size) {
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

  /*if (down_interruptible(&dev->sem))
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
  vfree(ubuf_p);*/
  reg_copy = vmalloc(count);
  if (!reg_copy) {
    printk(KERN_ERR "Insufficent memory for copying value.\n");
    return -ENOMEM; // Operating more than one csu once is not allowed
  }
  if (copy_from_user(reg_copy, buf, count)) {
    vfree(reg_copy);
    printk(KERN_ERR "Error when copying value from user space.\n");
    return -ENOMEM;
  }
  for (i = 0; i < (count << 2); ++i)
    writel(reg_copy[i], apc_data.ape_info[ape_id].membase + (offset << 2));
  *f_pos += count;
  vfree(reg_copy);
  return count;
}

static irqreturn_t apc_dma_int(int irq, void *dev_id)
{
  union csu_mmap *ape;
  unsigned int ape_id = (irq - 42) / 2;

  ape = apc_data.ape_info[ape_id].membase;
  dma_complete = 0;
  writel(1, &(ape->csu_if.DMAQueryType));
  wake_up_interruptible(&dmaq);
  return IRQ_HANDLED;
}

long apc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  int ret = 0;
  unsigned int ape_id = filp->f_pos >> 24;
  union csu_mmap *ape;
  if (ape_id >= apc_data.num_of_apes) {
    printk(KERN_ERR "User program accessed unsupported number of APEs.\n");
    return -ENOMEM;
  }

  if (_IOC_TYPE(cmd) != APC_IOC_MAGIC) {
    return -ENOTTY;
  }

  ape = apc_data.ape_info[ape_id].membase;

  switch (cmd) {
  case APC_IOCWAITDMA:
    // arg: DMA group No.
    dma_complete = 1;
    writel(~(1UL << arg), &(ape->csu_if.DMAQueryMask));
    writel(5, &(ape->csu_if.DMAQueryType));
    wait_event_interruptible(dmaq, dma_complete == 0);
    writel(1UL << arg, &(ape->csu_if.DMAGrpIntClr));
    writel(1, &(ape->csu_if.DMAQueryType));
    break;

  default:
    ret = -EFAULT;
    break;
  }

  return -EFAULT;
}

loff_t apc_llseek(struct file *filp, loff_t off, int whence) {
  long newpos;

  switch (whence) {
  case 0:
    newpos = off;
    break;

  case 1:
    newpos = filp->f_pos + off;
    break;

  case 2:
    newpos = sizeof(union csu_mmap) + off;
    break;

  default:
    return -EINVAL;
  }
  if (newpos < 0) return -EINVAL;
  filp->f_pos = newpos;
  return newpos;
}

int apc_open(struct inode *inode, struct file *filp) {
  int ret = 0;
  u32 i;
  if (!apc_data.num_of_apes || !apc_data.ape_info) {
    printk(KERN_ERR "APC device initialization failed.\n");
    return -EFAULT;
  }

  for (i = 0; i < apc_data.num_of_apes; i++) {
    ret = request_irq(apc_data.ape_info[i].dma_irq, apc_dma_int, 0, "apc", NULL);

    if (ret) {
      printk(KERN_ERR "apc: failed to claim IRQ%d\n", apc_data.ape_info[i].dma_irq);
      return ret;
    }
  }

  return 0;
}

int apc_release(struct inode *inode, struct file *filp)
{
  return 0;
}

const char region_name[8][5] = {"ape0", "ape1", "ape2", "ape3",
                                "ape4", "ape5", "ape6", "ape7"};

static int __devinit apc_probe(struct platform_device *pdev)
{
  struct resource *regs, *irq_dma, *irq_mb;
  struct device_node *np = pdev->dev.of_node;
  u32 val, i;

  if (of_property_read_u32(np, "num-apes", &val)) {
    dev_err(&pdev->dev, "no number of apes property set\n");
    return -EINVAL;
  }

  if (!val) {
    dev_err(&pdev->dev, "no ape is defined\n");
    return -EINVAL;
  }

  apc_data.num_of_apes = val;

  apc_data.ape_info = kmalloc(sizeof(*apc_data.ape_info) * val, GFP_KERNEL);

  for (i = 0; i < val; i++) {
    regs = platform_get_resource(pdev, IORESOURCE_MEM, i);
    irq_dma = platform_get_resource(pdev, IORESOURCE_IRQ, i << 1);
    irq_mb = platform_get_resource(pdev, IORESOURCE_IRQ, (i << 1) + 1);

    if (!regs || !irq_dma || !irq_mb) {
      dev_err(&pdev->dev, "no registers/irq defined\n");
      kfree(apc_data.ape_info);
      apc_data.ape_info = NULL;
      return -EINVAL;
    }
    apc_data.ape_info[i].mapbase = regs->start;
    apc_data.ape_info[i].dma_irq = irq_dma->start;
    apc_data.ape_info[i].mb_irq = irq_mb->start;
    apc_data.ape_info[i].region_size = resource_size(regs);

    if (!request_mem_region(apc_data.ape_info[i].mapbase,
                            apc_data.ape_info[i].region_size, region_name[i])) {
      dev_err(&pdev->dev, "failed to request memory region for apc\n");
      kfree(apc_data.ape_info);
      apc_data.ape_info = NULL;
      return -EBUSY;
    }

    apc_data.ape_info[i].membase = ioremap_nocache(apc_data.ape_info[i].mapbase,
                                                   apc_data.ape_info[i].region_size);
    if (!apc_data.ape_info[i].membase) {
      dev_err(&pdev->dev, "failed to ioremap apc address\n");
      release_mem_region(apc_data.ape_info[i].mapbase,
                         apc_data.ape_info[i].region_size);
      kfree(apc_data.ape_info);
      apc_data.ape_info = NULL;
      return -ENOMEM;
    }
  }

  printk(KERN_INFO "Found %d APEs.\n", apc_data.num_of_apes);
  for (i = 0; i < val; i++)
    printk(KERN_INFO "Mapped APE%d at 0x%08x.\n", i, apc_data.ape_info[i].membase);

  //platform_set_drvdata(pdev, data);

  return 0;
}

static int __devexit apc_remove(struct platform_device *pdev)
{
  u32 i;
  if (!apc_data.ape_info) {
    for (i = 0; i < apc_data.num_of_apes; i++) {
      iounmap(apc_data.ape_info[i].membase);
      apc_data.ape_info[i].membase = NULL;
      release_mem_region(apc_data.ape_info[i].mapbase,
                         apc_data.ape_info[i].region_size);
    }
    kfree(apc_data.ape_info);
    apc_data.ape_info = NULL;
  }
  return 0;
}

static const struct of_device_id apc_match[] = {
  { .compatible = "mapu,apc" },
  { /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, apc_match);

static struct platform_driver apc_platform_driver = {
  .driver = {
    .name   = "mapu-apc",
    .owner    = THIS_MODULE,
    .of_match_table = apc_match,
  },
  .probe      = apc_probe,
  .remove     = __devexit_p(apc_remove),
};

static const struct file_operations apc_fops = {
  .llseek   = apc_llseek,
  .read   = apc_read,
  .write    = apc_write,
  .unlocked_ioctl = apc_ioctl,
  .open   = apc_open,
  .release  = apc_release,
};

struct class *apc_class;

static char *apc_devnode(struct device *dev, umode_t *mode)
{
  if (!mode)
    return NULL;
  *mode = 0666;
  return NULL;
}

static struct platform_device *apc_devs;
static struct cdev apc_cdev;
static int major;    /* major device number */
static int minor_start;  /* start of minor device number */

static int __init apc_init(void)
{
  int ret;
  struct device *d;

  dev_t dev;

  ret = alloc_chrdev_region(&dev, 0, 1, "apc");

  if (!ret) {
    major = MAJOR(dev);
    minor_start = MINOR(dev);
  }
  if (ret < 0) {
    return ret;
  }
  cdev_init(&apc_cdev, &apc_fops);
  ret = cdev_add(&apc_cdev, dev, 1);
  if (ret) {
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  apc_class = class_create(THIS_MODULE, "apc");
  if (IS_ERR(apc_class))
    return PTR_ERR(apc_class);
  apc_class->devnode = apc_devnode;

  d = device_create(apc_class, NULL, dev, NULL, "apc");
  if (IS_ERR(d)) {
    unregister_chrdev_region(dev, 1);
    return PTR_ERR(d);
  }

  apc_devs = platform_device_alloc("apc", 0);
  if (!apc_devs) {
    ret = -ENOMEM;
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  ret = platform_device_add(apc_devs);
  if (ret) {
    platform_device_put(apc_devs);
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  ret = platform_driver_register(&apc_platform_driver);
  if (ret == 0)
    return ret;

  platform_device_del(apc_devs);
  platform_device_put(apc_devs);
  unregister_chrdev_region(dev, 1);
  return ret;
}

static void __exit apc_exit(void)
{
  struct platform_device *isa_dev = apc_devs;

  /*
   * This tells serial8250_unregister_port() not to re-register
   * the ports (thereby making serial8250_isa_driver permanently
   * in use.)
   */
  apc_devs = NULL;

  platform_driver_unregister(&apc_platform_driver);
  platform_device_unregister(isa_dev);
  unregister_chrdev_region(MKDEV(major, minor_start), 1);
}

module_init(apc_init);
module_exit(apc_exit);

MODULE_AUTHOR("Lei Wang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IACAS Algebra Processing Cluster");
