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
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/spinlock.h>
#include <linux/kfifo.h>

#include <asm/current.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <asm/div64.h>
#include <asm/string.h>
#include <asm/barrier.h>

#include "apc.h"

#define AREAS_CAPACITY 64  // 最多记录64个分配的内存区域

unsigned areas_capacity = AREAS_CAPACITY;

struct apc_info {
  u32 num_of_apes;
  rwlock_t lock;
  unsigned long free_ape_mask;   // 没有被分配的ape
  unsigned long busy_ape_mask;   // 正在执行kernel的ape

  void __iomem *sm_virt_base;   // SM的虚拟地址
  resource_size_t sm_bus_base; /* resource base */           // apc的CPU总线地址
  resource_size_t sm_region_size;

  struct {
    void __iomem *virt_base; /* ioremap cookie or NULL */    // apc的虚拟地址
    resource_size_t bus_base; /* resource base */           // apc的CPU总线地址
    resource_size_t region_size;
    unsigned int dma_irq; /* DMA interrupt number */
    unsigned int mb_irq; /* Mailbox interrupt number */

    rwlock_t csu_lock;  // 线程级原子读写控制寄存器所需

    rwlock_t dma_free_lock;
    struct kfifo dma_free_num;   // 还没有被分配的DMA组号

    struct file *host_filp;   // 持有该ape的文件指针
    int ret;        // 上次launch后的返回值  // 大数据块的传输应该使用dma, 应该将已分配好的内存的起始地址作为参数传入, 以便arm端可以读取
  } *ape;
} apc_data;

struct allocated_area {
  unsigned long start, end;
};

struct allocated_areas {
  int cnt;
  struct allocated_area areas[0];     // 动态分配大小
};

struct file_ctx {  // 每一个打开的apc文件描述符, 都有一个上下文结构体
  union csu_mmap *dma_caches;   // 暂存dma的配置, 有一些浪费
  struct allocated_areas allocated_areas;
};

static DECLARE_WAIT_QUEUE_HEAD (dmaq);
static DECLARE_WAIT_QUEUE_HEAD (launchq);

static unsigned long long junk = 0x55aa55aa55aa55aaULL;

static inline void dma_clear(unsigned char i, int ape_id) {
  unsigned long savedirq;

  write_lock_irqsave(&apc_data.ape[ape_id].dma_free_lock, savedirq);
  kfifo_in(&apc_data.ape[ape_id].dma_free_num, &i, 1);
  write_unlock_irqrestore(&apc_data.ape[ape_id].dma_free_lock, savedirq);
}

static inline bool dma_have_spare(int ape_id) {
  return !kfifo_is_empty(&apc_data.ape[ape_id].dma_free_num);
}

static unsigned char dma_alloc(int ape_id) {
  unsigned char i = DMA_QUENE_CAPACITY;
  unsigned long savedirq;

  while (1) {
    write_lock_irqsave(&apc_data.ape[ape_id].dma_free_lock, savedirq);

    if (dma_have_spare(ape_id)) {
      kfifo_out(&apc_data.ape[ape_id].dma_free_num, &i, 1);
      break;
    } else {
      write_unlock_irqrestore(&apc_data.ape[ape_id].dma_free_lock, savedirq);

      wait_event_interruptible(dmaq, dma_have_spare(ape_id));  // 直到有空闲再尝试
      continue;   // 没分配成功就继续分配
    }
  }

  write_unlock_irqrestore(&apc_data.ape[ape_id].dma_free_lock, savedirq);
  return i;
}

// 同一个中断不会在任意一个smp上重入, 见 linux设计与实现 97页
static irqreturn_t apc_dma_int(int irq, void *dev_id) {
  union csu_mmap *ape;
  unsigned int ape_id;
  unsigned status;

  ape_id = (irq - 42 - 32) / 2;
  ape = apc_data.ape[ape_id].virt_base;

  // 外部操作控制寄存器时全部禁止中断并加锁, 此处不必再加锁
  status = ioread32(&(ape->csu_if.DMAQueryStatus));
  iowrite32(~status, &(ape->csu_if.DMAQueryMask));

  wake_up_interruptible(&dmaq);

  return IRQ_HANDLED;
}

static irqreturn_t apc_mb_int(int irq, void *dev_id) {
  struct csu_if *csu_if;
  unsigned int ape_id = (irq - 42 - 32) / 2;

  printk(KERN_ERR "apc_mb_int\n");

  csu_if = apc_data.ape[ape_id].virt_base;
  apc_data.ape[ape_id].ret = ioread32(&(csu_if->MailBoxOutInt));
  apc_data.busy_ape_mask &= ~(1 << ape_id);
  wake_up_interruptible(&launchq);

  printk(KERN_ERR "leaving apc_mb_int\n");

  return IRQ_HANDLED;
}

int apc_open(struct inode *inode, struct file *filp) {
  if (!apc_data.num_of_apes || !apc_data.ape) {
    printk(KERN_ERR "APC device initialization failed.\n");
    return -EFAULT;
  }

  filp->private_data = kmalloc(sizeof(struct file_ctx) +
                               sizeof(struct allocated_area) * areas_capacity +
                               sizeof(union csu_mmap) * apc_data.num_of_apes, GFP_KERNEL);
  if (filp->private_data == 0) {
    printk(KERN_ERR "apc: failed to allocate memory");
    return -EFAULT;
  }

  memset(filp->private_data, 0, sizeof(struct file_ctx) +
         sizeof(struct allocated_area) * areas_capacity + sizeof(union csu_mmap) * apc_data.num_of_apes);
  ((struct file_ctx *) filp->private_data)->dma_caches = filp->private_data +
                                                         sizeof(struct file_ctx) + sizeof(struct allocated_area) * areas_capacity;
  return 0;
}

int apc_release(struct inode *inode, struct file *filp) {
  unsigned long savedirq, i;
  printk(KERN_NOTICE "apc_release\n");
  kfree(filp->private_data);

  write_lock_irqsave(&apc_data.lock, savedirq);
  for (i = 0; i < apc_data.num_of_apes; ++i) {
    if (apc_data.ape[i].host_filp == filp) {
      apc_data.ape[i].host_filp = 0;
      apc_data.free_ape_mask |= 1UL << i;
    }
  }
  write_unlock_irqrestore(&apc_data.lock, savedirq);

  return 0;
}

static const int offsetOfDMAGlobalAddr = offsetof(struct csu_if, dma) + offsetof(struct dma_if, DMAGlobalAddr);
static const int offsetOfDMACmd = offsetof(struct csu_if, dma) + offsetof(struct dma_if, DMACmd);
static const int offsetOfVPUControl = offsetof(struct csu_if, VPUControl);
static const int offsetOfDMAGroupNum = offsetof(struct csu_if, dma) + offsetof(struct dma_if, DMAGroupNum);

static unsigned long get_phy(struct allocated_areas *areas, unsigned long virt, unsigned long size) {
  int i;

  if (virt) {
    struct vm_area_struct *vma = find_vma(current->mm, virt);
    if (vma) {
      unsigned long phy = vma->vm_pgoff << PAGE_SHIFT;

      for (i = 0; i < areas->cnt; i++)
        if (areas->areas[i].end > phy && areas->areas[i].start <= phy) {
          if (areas->areas[i].end < phy + size) { printk(KERN_ERR "Attmpted to go beyond the range of an allocated area. virt: %lu, size: %lu", virt, size);return 0;}
          else return phy + ((unsigned long) virt - (unsigned long) vma->vm_start);
        }
    }
  }

  return 0;
}

static inline int get_ape_id(loff_t *f_pos) {
  int ape_id;

  if (*f_pos < 0) return -EFAULT;

  ape_id = *f_pos - apc_data.ape[0].bus_base;
  do_div(ape_id, apc_data.ape[0].region_size);

  return ape_id;
}

#define check_rw(f_pos, next_byte, offset, ape_id, count)                                                                      \
    /* Sanity check */                                                                                                         \
    ape_id = get_ape_id(f_pos);                                                                                                \
    if (ape_id < 0 || ape_id >= apc_data.num_of_apes) {                                                                        \
      printk(KERN_ERR "User program accessed unknown APE. (ape_id: %d).\n", ape_id);                                           \
      return -EFAULT;                                                                                                          \
    }                                                                                                                          \
                                                                                                                               \
    offset = *f_pos - apc_data.ape[ape_id].bus_base;                                                                           \
    if (offset >= sizeof(union csu_mmap) || (next_byte = offset + count) > sizeof(union csu_mmap)) {                           \
      printk(KERN_ERR "User program accessed wrong APE control registers. (offset: %d, count: %d)\n", offset, count);          \
      return -EFAULT;                                                                                                          \
    }                                                                                                                          \
                                                                                                                               \
    if (count % 4 != 0 || count < 4 || offset % 8 != 0) {                                                                      \
      printk(KERN_ERR "User program's access is unaligned to 8 bytes. (offset: %d, count: %d)\n", offset, count);              \
      return -EFAULT; /* Any unaligned access is not allowed */                                                                \
    }                                                                                                                          \

#define R   1
#define W   2
#define RW  3
#define N   0
static const u32 reg_access[] = { W, R, N, N, W, W, W, W, W, W, W, W, W, W, N/*组号由软件分配, 写无效*/, W, R, R, RW, RW, R, W, R, R };  // 8字节为单位规定访问权限

// 第index个8字节是否可读
static inline bool canRead(int index) {
  return reg_access[index] & R;
}
// 第index个8字节是否可写
static inline bool canWrite(int index) {
  return reg_access[index] & W;
}

#undef R
#undef W
#undef RW
#undef N

// 返回应该直接写入控制寄存器的索引. dma_if缓冲并一次写入, 其余的是直接写入的
static inline bool shouldWriteThrough(int index) {
  return canWrite(index) && (index < offsetof(struct csu_if, dma) / 8 || index >= offsetof(struct csu_if, dma) / 8 + sizeof(struct dma_if) / 8);
}

/**
 * read from control registers (csu_interface) of some ape
 */
ssize_t apc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
  u32 ape_id;
  loff_t offset;
  loff_t next_byte;
  u32 *reg_copy;
  u32 i;
  unsigned long savedirq;

  printk(KERN_NOTICE "apc_read");

  check_rw(f_pos, next_byte, offset, ape_id, count);

  reg_copy = vmalloc(count);
  if (!reg_copy) {
    printk(KERN_ERR "Insufficient memory for copying value.\n");
    return -ENOMEM; /* Operating more than one csu once is not allowed */
  }

  read_lock_irqsave(apc_data.ape[ape_id].csu_lock, savedirq);
  for (i = 0; i < count / 4; ++i)
    reg_copy[i] = canRead(offset / 8 + i / 2) ? ioread32(apc_data.ape[ape_id].virt_base + offset + i * 4) : junk;
  read_unlock_irqrestore(apc_data.ape[ape_id].csu_lock, savedirq);

  if (copy_to_user(buf, reg_copy, count)) {
    printk(KERN_ERR "Error when copying value to user space.\n");
    vfree(reg_copy);
    return -ENOMEM;
  }

  *f_pos += count;
  vfree(reg_copy);
  return count;
}

/**
 * write to control registers (csu_interface) of some ape
 */
ssize_t apc_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
  u32 ape_id;
  loff_t offset;
  loff_t next_byte;
  u32 i;
  bool launch = false;
  union csu_mmap *csu_mmap;
  union csu_mmap *csu_cache;
  unsigned long savedirq;

  printk(KERN_NOTICE "apc_write\n");

  check_rw(f_pos, next_byte, offset, ape_id, count);

  if (apc_data.ape[ape_id].host_filp != filp) {
    printk(KERN_ERR "Attempted to access ape %d, which you don't possess.\n", ape_id);
    return -EFAULT;
  }

  csu_cache = (union csu_mmap *) & ((struct file_ctx *) filp->private_data)->dma_caches[ape_id];

  // 将本进程缓存的值刷新成用户的值, 在写入DMACmd之前, DMAGlobalAddr和DMAGroupNum要重新转义
  if (copy_from_user(((void *) csu_cache) + offset, buf, count)) {
    printk(KERN_ERR "Error occurred when copying value from user space.\n");
    return -ENOMEM;
  }

  csu_mmap = apc_data.ape[ape_id].virt_base;

  if (offsetOfVPUControl >= offset && offsetOfVPUControl < next_byte) {
    launch = csu_cache->csu_if.VPUControl == CMD_START;
    if (!launch) {
      printk (KERN_ERR "Requesting undefined SPU operations: %d\n", csu_cache->csu_if.VPUControl);
      return -EFAULT;
    } else {
      write_lock_irqsave(apc_data.lock, savedirq);
      if ((apc_data.busy_ape_mask & (1 << ape_id)) || ioread32(&(csu_mmap->csu_if.VPUStatus))) {
        write_unlock_irqrestore(apc_data.lock, savedirq);
        printk (KERN_ERR "Attempted to launch on a busy APE. (ape_id: %d).\n", ape_id);
        return -EFAULT;
      } else {
        apc_data.busy_ape_mask |= (1 << ape_id);
        write_unlock_irqrestore(apc_data.lock, savedirq);
      }
    }
  }

  write_lock_irqsave(apc_data.ape[ape_id].csu_lock, savedirq);
  for (i = 0; i < count / 4; ++i)
    if (shouldWriteThrough(offset / 8 + i / 2))
      iowrite32(csu_cache->mem[offset / 4 + i], csu_mmap->mem + offset / 4 + i);
  write_unlock_irqrestore(apc_data.ape[ape_id].csu_lock, savedirq);

  // launch, 需要等待结束
  if (launch) {
    i = wait_event_interruptible(launchq, !(apc_data.busy_ape_mask & (1UL<<ape_id)) || !ioread32(&(csu_mmap->csu_if.VPUStatus)));

    write_lock_irqsave(apc_data.lock, savedirq);
    apc_data.busy_ape_mask &= ~(1 << ape_id);  // 清掉busy标志, 并不一定代表已经执行完成. 只是在驱动端释放此APE的执行权. 如果跑飞了, 只能重启机器
    write_unlock_irqrestore(apc_data.lock, savedirq);

    if (i) {
      printk (KERN_ERR "Interruption occurred when APE %d is running.\n", ape_id);
      return -EFAULT;
    }
  }

  // 写入DMACmd, 用此进程已配置的寄存器来配dma
  if (offsetOfDMACmd >= offset && offsetOfDMACmd < next_byte) {
    struct dma_if *dma = (struct dma_if *)(&csu_cache->csu_if.dma);

    // 将分配过的虚拟地址翻译成物理地址
    unsigned long complete_rows = dma->DMAGlobalYAllNum;
    unsigned long mod = do_div(complete_rows, dma->DMAGlobalYNum);   // 完整的行数及不完整的行的元素个数
    unsigned long size = dma->DMAGlobalZStep * complete_rows + (mod - 1) * dma->DMAGlobalYStep + min(dma->DMAGlobalXNum, dma->DMAGlobalYStep);
    unsigned long phy = get_phy(&((struct file_ctx *) filp->private_data)->allocated_areas, dma->DMAGlobalAddr, size);

    if (phy == 0) {
      printk(KERN_ERR "Attempted to access non-DMA memory. \n");
      return -EFAULT;
    }

    dma->DMAGlobalAddr = phy;

    int num = dma_alloc(ape_id);  // 先分配真正的组号, 如果没有空闲则会引起阻塞

    dma->DMAGroupNum = num;

    write_lock_irqsave(apc_data.ape[ape_id].csu_lock, savedirq);
    for (i = 0; i < sizeof(struct dma_if) / 4; ++i) {  // 写入所有缓冲值
      if (i == sizeof(struct dma_if) / 4 - 2) smp_wmb();  // 确保DMACmd最后写入
      iowrite32(csu_cache->mem[offsetof(struct csu_if, dma) / 4 + i],
                csu_mmap->mem + offsetof(struct csu_if, dma) / 4 + i);
    }
    iowrite32(ioread32(&(csu_mmap->csu_if.DMAQueryMask)) | (1UL << num), &(csu_mmap->csu_if.DMAQueryMask));  // 关注此组号
    write_unlock_irqrestore(apc_data.ape[ape_id].csu_lock, savedirq);

    // 此处不必加读锁. 因为在释放编号之前, 这个编号的状态不会被改变
    i = wait_event_interruptible(dmaq, !(ioread32(&(csu_mmap->csu_if.DMAQueryMask)) & (1UL << num)));

    if (i) { // 等待dma时被中断, 主动取消关注此组号
      write_lock_irqsave(apc_data.ape[ape_id].csu_lock, savedirq);
      iowrite32(ioread32(&(csu_mmap->csu_if.DMAQueryMask)) & ~(1UL << num), &(csu_mmap->csu_if.DMAQueryMask));
      write_unlock_irqrestore(apc_data.ape[ape_id].csu_lock, savedirq);
    }

    dma_clear(num, ape_id);

    if (i)   // 等待dma时被中断, 应该返回错误, 用户选择是否重试
      return -EFAULT;
  }

  *f_pos += count;
  return count;
}

long apc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {
  int ret = 0, i;
  unsigned long savedirq, tmpl;

  printk(KERN_NOTICE "apc_ioctl\n");

  if (_IOC_TYPE(cmd) != APC_IOC_MAGIC) return -ENOTTY;

  switch (cmd) {
    case APC_IOC_QUERY_APE_NUM:
      ret = apc_data.num_of_apes;
      break;

    case APC_IOC_QUERY_FREE_APES:  // 只是查询一下, 并不保证原子性
      ret = apc_data.free_ape_mask;
      break;

    case APC_IOC_ACQUIRE_APE:
      write_lock_irqsave(&apc_data.lock, savedirq);
      ret = ~apc_data.free_ape_mask & arg;  // 返回冲突的apeid

      if (!ret) {
        apc_data.free_ape_mask &= ~arg;

        for (i = 0; arg; arg >>= 1, ++i)
          if (arg & 1)
            apc_data.ape[i].host_filp = filp;
      }

      write_unlock_irqrestore(&apc_data.lock, savedirq);
      break;

    case APC_IOC_RELEASE_APE:
      write_lock_irqsave(&apc_data.lock, savedirq);
      tmpl = arg;

      for (i = 0; arg; arg >>= 1, ++i) {
        if (arg & 1) {
          if (apc_data.ape[i].host_filp != filp) {  // 不属于自己的不能释放
            tmpl &= ~(1UL << i);   // 去掉这一位
            ret++;
          } else
            apc_data.ape[i].host_filp = 0;  // 即将被释放的要清空对应的host
        }
      }
      apc_data.free_ape_mask |= tmpl;
      write_unlock_irqrestore(&apc_data.lock, savedirq);
      break;

    case APC_IOC_RESET_APE:
      apc_data.free_ape_mask = ((unsigned long) - 1) >> (8 * sizeof(unsigned long) - apc_data.num_of_apes);

      for (i = 0; i < apc_data.num_of_apes; i++) {
        unsigned char j;

        kfifo_reset(&apc_data.ape[i].dma_free_num);
        for (j = 0; j < DMA_QUENE_CAPACITY; ++j)
          kfifo_in(&apc_data.ape[i].dma_free_num, &j, 1);

        apc_data.ape[i].host_filp = 0;
      }
      break;

    case APC_IOC_WRITE_DB_ENTRY: {
      unsigned long tmp[3];
      if (copy_from_user(tmp, arg, 3 * 4)) {
        printk(KERN_ERR "Error occurred when copying value from user space.\n");
        return -ENOMEM;
      }

      unsigned long *to = tmp[0], *from = tmp[1], size = tmp[2];

      if (size % 8 != 0 || size > 16 * 8 || size < 0) {
        printk(KERN_ERR "Invalid entry size.\n");
        return -EFAULT;
      }

      int ape_id = ((unsigned long)to - apc_data.sm_bus_base) / (16 * 8);

      if (ape_id < 0 || ape_id >= apc_data.num_of_apes || ((unsigned long)to + size - 1 - apc_data.sm_bus_base) / (16 * 8) != ape_id) {
        printk(KERN_ERR "User program accessed unknown APE.\n");
        return -EFAULT;
      }

      to = (unsigned long)to - (unsigned long)apc_data.sm_bus_base + (unsigned long)apc_data.sm_virt_base;

      if (apc_data.ape[ape_id].host_filp != filp) {
        printk(KERN_ERR "Attempted to access db entry of ape that you don't possess.\n");
        return -EFAULT;
      }

      unsigned long *data = vmalloc(size);
      if (!data) {
        printk(KERN_ERR "Insufficient memory for copying value.\n");
        return -ENOMEM; /* Operating more than one csu once is not allowed */
      }

      if (copy_from_user((void *) data, from, size)) {
        printk(KERN_ERR "Error occurred when copying value from user space.\n");
        vfree(data);
        return -ENOMEM;
      }

      for (i = 0; i < size / 8; ++i) {
        data[i * 2] = get_phy(&((struct file_ctx *) filp->private_data)->allocated_areas, data[i * 2], data[i * 2 + 1]);
        if (!data[i * 2]) {
          printk(KERN_ERR "Attempted to write db entry of unheld memory.\n");
          vfree(data);
          return -EFAULT;
        }

        to[i * 2] = data[i * 2];
        to[i * 2 + 1] =  data[i * 2 + 1];
      }

      vfree(data);
    }
    break;

    default:
      ret = -EFAULT;
      break;
  }

  return ret;
}

loff_t apc_llseek(struct file *filp, loff_t off, int whence) {
  long newpos;

  printk(KERN_NOTICE "apc_llseek\n");

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

void apc_vma_open(struct vm_area_struct *vma) {
  vma->vm_private_data++;
  printk(KERN_NOTICE "APC VMA open, virt %lx, phys %lx, refcnt %ld\n", vma->vm_start, vma->vm_pgoff << PAGE_SHIFT, (long) vma->vm_private_data);
}

void apc_vma_close(struct vm_area_struct *vma) {
  vma->vm_private_data--;
  printk(KERN_NOTICE "APC VMA close, virt %lx, phys %lx, refcnt %ld\n", vma->vm_start, vma->vm_pgoff << PAGE_SHIFT, (long) vma->vm_private_data);
  if (vma->vm_private_data == 0) {
    int i;
    struct allocated_areas *areas = &((struct file_ctx *) vma->vm_file->private_data)->allocated_areas;

    for (i = 0; i < areas->cnt; i++)
      if (areas->areas[i].start == (vma->vm_pgoff << PAGE_SHIFT)) {
        if (i != areas->cnt - 1) areas->areas[i] = areas->areas[areas->cnt - 1];

        areas->cnt--;
        return;
      }
  }
}

struct vm_operations_struct apc_vm_ops = { .open = apc_vma_open, .close = apc_vma_close };

#ifndef get_order
unsigned long get_order(unsigned long size) {
  unsigned long order;
  size = (size - 1) / 4096; // 变成页数-1

  for (order = 0; size; size >>= 1, order++);

  return order;
}
#endif

static int apc_mmap(struct file *filp, struct vm_area_struct *vma) {
  unsigned size = vma->vm_end - vma->vm_start;
  unsigned long page_addr;
  unsigned log_size;
  struct allocated_areas *areas = &((struct file_ctx *) filp->private_data)->allocated_areas;

  if (areas->cnt >= areas_capacity) {
    printk(KERN_ERR "too much areas allocated!\n");
    return -EFAULT;
  }

  if (size == 0) {
    printk(KERN_ERR "vma size equals zero!\n");
    return -EFAULT;
  }

  log_size = get_order(size);
  // 得到所分配的页的内核虚拟地址, 和物理地址之间只有一个偏移
  page_addr = __get_free_pages(GFP_KERNEL | __GFP_ZERO | GFP_DMA, log_size);

  if (!page_addr) {
    printk(KERN_ERR "no sufficient memory!\n");
    return -ENOMEM;
  }

  if (remap_pfn_range(vma, vma->vm_start, virt_to_phys(page_addr) >> PAGE_SHIFT, size, vma->vm_page_prot)) {   // 设置vma->vm_pgoff
    printk(KERN_ERR "remap_pfn_range failed!\n");
    free_pages(page_addr, log_size);
    return -EAGAIN;
  }

  areas->areas[areas->cnt].start = vma->vm_pgoff << PAGE_SHIFT;
  areas->areas[areas->cnt].end = (vma->vm_pgoff + (1 << log_size)) << PAGE_SHIFT;
  areas->cnt++;

  printk(KERN_ERR "apc_mmap, cnt %d, start %lu, end %lu\n", areas->cnt, areas->areas[areas->cnt].start, areas->areas[areas->cnt].end);

  vma->vm_ops = &apc_vm_ops;
  vma->vm_flags |= VM_RESERVED | VM_DONTEXPAND | VM_IO | VM_PFNMAP;
  vma->vm_private_data = 0; // 自定义的引用计数
  apc_vma_open(vma);
  return 0;
}

const char region_name[8][5] = { "ape0", "ape1", "ape2", "ape3", "ape4", "ape5", "ape6", "ape7" };

static int __devinit apc_probe(struct platform_device *pdev) {
  struct resource *regs, *irq_dma, *irq_mb;
  struct device_node *np = pdev->dev.of_node;
  u32 val, i;

  if (of_property_read_u32(np, "num-apes", &val)) {      // 从dts设置这个值
    dev_err(&pdev->dev, "no number of apes property set\n");
    return -EINVAL;
  }

  if (val <= 0) {
    dev_err(&pdev->dev, "no ape is defined\n");
    return -EINVAL;
  }

  apc_data.num_of_apes = val;   // 存起来
  apc_data.ape = kmalloc(sizeof(*apc_data.ape) * val, GFP_KERNEL); // 是个数组

  if (!apc_data.ape) {
    dev_err(&pdev->dev, "insufficient memory\n");
    return -EINVAL;
  }

  // 解析每个ape的io地址和中断号, 将io区域映射到内核空间.
  for (i = 0; i < val; i++) {
    regs = platform_get_resource(pdev, IORESOURCE_MEM, i); // 从dts设置这些资源, 每个resource都包含开始结束和flag等属性
    irq_dma = platform_get_resource(pdev, IORESOURCE_IRQ, i << 1); // platform_get_resource函数得到第index个type型的资源
    irq_mb = platform_get_resource(pdev, IORESOURCE_IRQ, (i << 1) + 1); // 一共有四个内存区域和八个中断

    if (!regs || !irq_dma || !irq_mb) {
      dev_err(&pdev->dev, "no registers/irq defined\n");
      kfree(apc_data.ape);
      apc_data.ape = NULL;
      return -EINVAL;
    }

    apc_data.ape[i].bus_base = regs->start;    // 把查询来的属性填入每个ape的信息
    apc_data.ape[i].dma_irq = irq_dma->start;
    apc_data.ape[i].mb_irq = irq_mb->start;
    apc_data.ape[i].region_size = resource_size(regs);

    // 分配内存区域, 结果在/proc/iomem中可见
    if (!request_mem_region(apc_data.ape[i].bus_base, apc_data.ape[i].region_size, region_name[i])) {
      dev_err(&pdev->dev, "failed to request memory region for apc\n");
      kfree(apc_data.ape);
      apc_data.ape = NULL;
      return -EBUSY;
    }

    // 分配区域后内核仍不可见. 需要建立映射. 返回的地址不能直接引用, 应该使用内核函数
    apc_data.ape[i].virt_base = ioremap_nocache(apc_data.ape[i].bus_base, apc_data.ape[i].region_size);
    if (!apc_data.ape[i].virt_base) {
      dev_err(&pdev->dev, "failed to ioremap apc address\n");
      release_mem_region(apc_data.ape[i].bus_base, apc_data.ape[i].region_size);
      kfree(apc_data.ape);
      apc_data.ape = NULL;
      return -ENOMEM;
    }
  }

  regs = platform_get_resource(pdev, IORESOURCE_MEM, val);
  apc_data.sm_bus_base = regs->start;
  apc_data.sm_region_size = resource_size(regs);

  if (!request_mem_region(apc_data.sm_bus_base, apc_data.sm_region_size, "APC-SM")) {
    dev_err(&pdev->dev, "failed to request memory region for apc\n");
    for (i = 0; i < apc_data.num_of_apes; i++) {
      iounmap(apc_data.ape[i].virt_base);
      apc_data.ape[i].virt_base = NULL;
      release_mem_region(apc_data.ape[i].bus_base, apc_data.ape[i].region_size);
    }
    kfree(apc_data.ape);
    apc_data.ape = NULL;
    return -EBUSY;
  }

  // 分配区域后内核仍不可见. 需要建立映射. 返回的地址不能直接引用, 应该使用内核函数
  apc_data.sm_virt_base = ioremap_nocache(apc_data.sm_bus_base, apc_data.sm_region_size);
  if (!apc_data.ape[i].virt_base) {
    dev_err(&pdev->dev, "failed to ioremap apc address\n");
    for (i = 0; i < apc_data.num_of_apes; i++) {
      iounmap(apc_data.ape[i].virt_base);
      apc_data.ape[i].virt_base = NULL;
      release_mem_region(apc_data.ape[i].bus_base, apc_data.ape[i].region_size);
    }
    release_mem_region(apc_data.sm_bus_base, apc_data.sm_region_size);
    kfree(apc_data.ape);
    apc_data.ape = NULL;
    return -ENOMEM;
  }

  printk(KERN_INFO "Found %d APEs.\n", apc_data.num_of_apes);
  for (i = 0; i < val; i++)
    printk(KERN_INFO "Mapped APE%d at 0x%08x.\n", i, (u32) apc_data.ape[i].virt_base);

  return 0;
}

static int __devexit apc_remove(struct platform_device *pdev) {
  u32 i;

  iounmap(apc_data.sm_virt_base);
  apc_data.sm_virt_base = NULL;
  release_mem_region(apc_data.sm_bus_base, apc_data.sm_region_size);

  if (!apc_data.ape) {
    for (i = 0; i < apc_data.num_of_apes; i++) {
      iounmap(apc_data.ape[i].virt_base);
      apc_data.ape[i].virt_base = NULL;
      release_mem_region(apc_data.ape[i].bus_base, apc_data.ape[i].region_size);
    }
    kfree(apc_data.ape);
    apc_data.ape = NULL;
  }
  return 0;
}

static const struct of_device_id apc_match[] = { { .compatible = "mapu,apc" }, { /* Sentinel */} };
MODULE_DEVICE_TABLE(of, apc_match);

static struct platform_driver apc_platform_driver = {
  .driver = {
    .name = "mapu-apc",
    .owner = THIS_MODULE,
    .of_match_table = apc_match,
  },
  .probe = apc_probe,
  .remove = __devexit_p(apc_remove),
};

static const struct file_operations apc_fops = {
  .llseek = apc_llseek,
  .read = apc_read,
  .write = apc_write,
  .unlocked_ioctl = apc_ioctl,
  .open = apc_open,
  .release = apc_release,
  .mmap = apc_mmap,
};

struct class *apc_class;

static char *apc_devnode(struct device *dev, umode_t *mode) {
  if (!mode) return NULL;
  *mode = 0666;
  return NULL;
}

static struct platform_device *apc_devs;
static struct cdev apc_cdev;
static int major; /* major device number */
static int minor_start; /* start of minor device number */

static int __init apc_init(void) {
  int ret, i, j;
  struct device *d;
  dev_t dev;
  void *tmp;

  ret = alloc_chrdev_region(&dev, 0, 1, "apc");   // 动态申请主设备号, 暂时只有一个设备

  if (!ret) {
    major = MAJOR(dev);
    minor_start = MINOR(dev);
  } else return ret;

  cdev_init(&apc_cdev, &apc_fops);     // 用操作初始化一个空的字符设备
  ret = cdev_add(&apc_cdev, dev, 1);   // 把初始化后的设备绑定到刚申请的设备号上
  if (ret) {
    unregister_chrdev_region(dev, 1); // 绑定失败则释放设备号
    return ret;
  }

  apc_class = class_create(THIS_MODULE, "apc");  // 创建一个设备类, 在/sys/class创建apc子目录
  if (IS_ERR(apc_class)) return PTR_ERR(apc_class);

  apc_class->devnode = apc_devnode;

  d = device_create(apc_class, NULL, dev, NULL, "apc"); // 在apc类下创建一个apc设备, 既出现在/sys/class/apc下, 也出现在/sys/下
  if (IS_ERR(d)) {
    class_destroy(apc_class);  // 我感觉应该调用这个函数把class释放掉
    unregister_chrdev_region(dev, 1);
    return PTR_ERR(d);
  }

  apc_devs = platform_device_alloc("apc", 0);   // 初始化一个platform设备
  if (!apc_devs) {
    ret = -ENOMEM;
    class_destroy(apc_class);
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  ret = platform_device_add(apc_devs);   // 在platform总线体系上创建设备, 触发probe
  if (ret) {
    platform_device_put(apc_devs);   // 添加失败则只释放设备关联的内存(调用release函数)
    class_destroy(apc_class);
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  ret = platform_driver_register(&apc_platform_driver); // 注册处理上一个platform设备的platform驱动, 期间会调用probe(?)
  if (ret) {
    platform_device_unregister(apc_devs);  // 驱动注册失败要先del再put
    class_destroy(apc_class);
    unregister_chrdev_region(dev, 1);
    return ret;
  }

  for (i = 0; i < apc_data.num_of_apes; i++) {
    union csu_mmap *ape;
    ret = request_irq(apc_data.ape[i].dma_irq, apc_dma_int, 0, "apc", NULL);

    if (ret) {
      printk(KERN_ERR "apc: failed to claim IRQ%d\n", apc_data.ape[i].dma_irq);

      for (j = 0; j < i; ++j)
        free_irq(apc_data.ape[j].dma_irq, NULL);

      platform_driver_unregister(&apc_platform_driver);
      platform_device_unregister(apc_devs);  // 驱动注册失败要先del再put
      class_destroy(apc_class);
      unregister_chrdev_region(dev, 1);
      return ret;
    }

    ape = apc_data.ape[i].virt_base;
    iowrite32(0, &(ape->csu_if.DMAQueryMask));             // 关心谁就开启谁的mask
    iowrite32(5, &(ape->csu_if.DMAQueryType));             // 开启中断
  }

  for (i = 0; i < apc_data.num_of_apes; i++) {
    ret = request_irq(apc_data.ape[i].mb_irq, apc_mb_int, 0, "apc", NULL);

    if (ret) {
      printk(KERN_ERR "apc: failed to claim IRQ%d\n", apc_data.ape[i].mb_irq);

      for (j = 0; j < i; ++j)
        free_irq(apc_data.ape[j].mb_irq, NULL);

      for (j = 0; j < apc_data.num_of_apes; j++) {
        union csu_mmap *ape = apc_data.ape[j].virt_base;
        iowrite32(1, &(ape->csu_if.DMAQueryType));             // 关闭中断

        free_irq(apc_data.ape[j].dma_irq, NULL);
      }

      platform_driver_unregister(&apc_platform_driver);
      platform_device_unregister(apc_devs);  // 驱动注册失败要先del再put
      class_destroy(apc_class);
      unregister_chrdev_region(dev, 1);
      return ret;
    }
  }

  tmp = kmalloc(apc_data.num_of_apes * 2 * DMA_QUENE_CAPACITY * sizeof(unsigned char), GFP_KERNEL);
  if (tmp == 0) {
    printk(KERN_ERR "apc: failed to allocate memory");

    for (i = 0; i < apc_data.num_of_apes; i++)
      free_irq(apc_data.ape[i].mb_irq, NULL);

    for (j = 0; j < apc_data.num_of_apes; j++) {
      union csu_mmap *ape = apc_data.ape[j].virt_base;
      iowrite32(1, &(ape->csu_if.DMAQueryType));             // 关闭中断

      free_irq(apc_data.ape[j].dma_irq, NULL);
    }

    platform_driver_unregister(&apc_platform_driver);
    platform_device_unregister(apc_devs);  // 驱动注册失败要先del再put
    class_destroy(apc_class);
    unregister_chrdev_region(dev, 1);
  }

  rwlock_init(&apc_data.lock);
  apc_data.free_ape_mask = ((unsigned long) - 1) >> (8 * sizeof(unsigned long) - apc_data.num_of_apes);
  apc_data.busy_ape_mask = 0;

  for (i = 0; i < apc_data.num_of_apes; i++) {
    unsigned char j;

    kfifo_init(&apc_data.ape[i].dma_free_num, tmp + i * 2 * DMA_QUENE_CAPACITY * sizeof(unsigned char), DMA_QUENE_CAPACITY);

    for (j = 0; j < DMA_QUENE_CAPACITY; ++j)
      kfifo_in(&apc_data.ape[i].dma_free_num, &j, 1);

    rwlock_init(&apc_data.ape[i].dma_free_lock);
    rwlock_init(&apc_data.ape[i].csu_lock);

    apc_data.ape[i].host_filp = 0;
  }

  return ret;
}

static void __exit apc_exit(void) {
  struct platform_device *isa_dev = apc_devs;
  int i;

  for (i = 0; i < apc_data.num_of_apes; i++)
    kfree(apc_data.ape[i].dma_free_num.kfifo.data);
  for (i = 0; i < apc_data.num_of_apes; i++)
    free_irq(apc_data.ape[i].mb_irq, NULL);
  for (i = 0; i < apc_data.num_of_apes; i++)
    free_irq(apc_data.ape[i].dma_irq, NULL);

  /*
   * This tells serial8250_unregister_port() not to re-register
   * the ports (thereby making serial8250_isa_driver permanently
   * in use.)
   */
  apc_devs = NULL;

  platform_driver_unregister(&apc_platform_driver);
  platform_device_unregister(isa_dev);
  class_destroy(apc_class);
  unregister_chrdev_region(MKDEV(major, minor_start), 1);
}

module_init(apc_init);
module_exit(apc_exit);

MODULE_AUTHOR("Lei Wang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("IACAS Algebra Processing Cluster");
