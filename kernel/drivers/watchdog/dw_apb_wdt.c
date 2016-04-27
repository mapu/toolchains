/*
 *      Watchdog driver for the mapu watchdog timer
 *      Authored by Yang Xin
 *      Mapu_watchdog v1.2
 */
#include <linux/device.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/clk.h>
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
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/io.h>

#include <linux/sched.h>//only for test kicking
//#include <sysdev/fsl_soc.h>

/* WDOG_REG_OFFSET       R-read W-write   */
#define MAPU_WDT_CR             0x00    //(R/W) Control Register
#define MAPU_WDT_TORR           0x04    //(R/W) Timeout Range Register
#define MAPU_WDT_CCVR           0x08    //(R)   Current Counter Value Register
#define MAPU_WDT_CRR            0x0c    //(W)   Counter Restart Register
#define MAPU_WDT_STAT           0x10    //(R)   Interrupt Status Register
#define MAPU_WDT_EOI            0x14    //(R)   Interrupt Clear Register
#define MAPU_WDT_COMP_PARAMS_5  0xe4    //(R)   Component Parameters Register 5
#define MAPU_WDT_COMP_PARAMS_4  0xe8    //(R)   Component Parameters Register 4
#define MAPU_WDT_COMP_PARAMS_3  0xec    //(R)   Component Parameters Register 3
#define MAPU_WDT_COMP_PARAMS_2  0xf0    //(R)   Component Parameters Register 2
#define MAPU_WDT_COMP_PARAMS_1  0xf4    //(R)   Component Parameters Register 1
#define MAPU_WDT_COMP_VERSION   0xf8    //(R)   Component Version Register
#define MAPU_WDT_COMP_TYPE      0xfc    //(R)   Component Type Register



#define MAPU_WDT_MAX_TOP        15
/*********** �Ĵ������� ***********/
/*MAPU_WDT_CR*/
#define MAPU_WDT_EN             (1<<0)      /* MAPU_WDT_CR[0]set 1 enable watchdog */
#define MAPU_WDT_RMOD           (1<<1)      /* MAPU_WDT_CR[1] set 1 generate an interrupt mode*/
#define MAPU_WDT_RPL            (0x5<<2)    /* MAPU_WDT_CR[4:2] set 101  Reset pulse length--64 pclk cycles */
/*MAPU_WDT_TORR*/
#define MAPU_TOP                            /* MAPU_WDT_TORR[3:0] */
#define MAPU_TOP_INIT                       /* MAPU_WDT_TORR[7:4] */
/*MAPU_WDT_CRR*/
#define MAPU_CRR                (0x76)      /* MAPU_WDT_CRR[7:0] set 0x76 */


static struct platform_device *mapu_wdt_pdev;   /* platform device attached to */

static struct {
    struct clk *clk;                    //wathcdog  clk resource
    struct resource *wdt_mem;           //wathcdog  IO resource
    struct resource *wdt_irq;           //wathcdog  irq resource
    void __iomem *wdt_base;             //wathcdog  virtual base address
    unsigned int top_s;                 //watchdog  value of top in seconds   = 2^(top + 16) / clk_get_rate(mapu_wdt.clk)
} mapu_wdt;


static struct{
    unsigned char WDT_CNT_WIDTH;         //WDT_COMP_PARAMS_1[28:24] WDT_CNT_WIDTH                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    unsigned char WDT_DFLT_TOP_INIT;     //WDT_COMP_PARAMS_1[23:20] WDT_DFLT_TOP_INIT                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    unsigned char WDT_DFLT_TOP;          //WDT_COMP_PARAMS_1[19:16] WDT_DFLT_TOP                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    unsigned char WDT_DFLT_RPL;          //WDT_COMP_PARAMS_1[12:10] WDT_DFLT_RPL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    unsigned char WDT_APB_DATA_WIDTH;    //WDT_COMP_PARAMS_1[9:8]   WDT_APB_DATA_WIDTH                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
    unsigned char WDT_PAUSE;             //WDT_COMP_PARAMS_1[7]     WDT_PAUSE                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    unsigned char WDT_USE_FIX_TOP;       //WDT_COMP_PARAMS_1[6]     WDT_USE_FIX_TOP                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
    unsigned char WDT_HC_TOP;            //WDT_COMP_PARAMS_1[5]     WDT_HC_TOP                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
    unsigned char WDT_HC_RPL;            //WDT_COMP_PARAMS_1[4]     WDT_HC_RPL                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
    unsigned char WDT_HC_RMOD;           //WDT_COMP_PARAMS_1[3]     WDT_HC_RMOD                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
    unsigned char WDT_DUAL_TOP;          //WDT_COMP_PARAMS_1[2]     WDT_DUAL_TOP                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
    unsigned char WDT_DFLT_RMOD;         //WDT_COMP_PARAMS_1[1]     WDT_DFLT_RMOD                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    unsigned char WDT_ALWAYS_EN;         //WDT_COMP_PARAMS_1[0]     WDT_ALWAYS_EN                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                       
    unsigned long WDT_CNT_RST;           //WDT_COMP_PARAMS_2[31:0]  WDT_CNT_RST                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
    unsigned long WDT_TOP_RST;           //WDT_COMP_PARAMS_3[31:0]  WDT_TOP_RST                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
    unsigned long WDT_USER_TOP_INIT_MAX; //WDT_COMP_PARAMS_4[31:0]  WDT_USER_TOP_INIT_MAX                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
    unsigned long WDT_USER_TOP_MAX;      //WDT_COMP_PARAMS_5[31:0]  WDT_USER_TOP_MAX                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
} mapu_wdt_params;
                                                
/*   do not use now                                                    
static void mapu_wdt_read_params(void)                   
{                                                       
    unsigned long mapu_wdt_params_1 = 0;                                             
    unsigned long mapu_wdt_params_2 = 0;                                             
    unsigned long mapu_wdt_params_3 = 0;                                             
    unsigned long mapu_wdt_params_4 = 0;                                             
    unsigned long mapu_wdt_params_5 = 0;                                             
                                                                                 
    mapu_wdt_params_1 = readl(mapu_wdt.wdt_base + MAPU_WDT_COMP_PARAMS_1);           
    mapu_wdt_params_2 = readl(mapu_wdt.wdt_base + MAPU_WDT_COMP_PARAMS_2);           
    mapu_wdt_params_3 = readl(mapu_wdt.wdt_base + MAPU_WDT_COMP_PARAMS_3);           
    mapu_wdt_params_4 = readl(mapu_wdt.wdt_base + MAPU_WDT_COMP_PARAMS_4);           
    mapu_wdt_params_5 = readl(mapu_wdt.wdt_base + MAPU_WDT_COMP_PARAMS_5);           
                                                                                 
    printk("WDT_COMP_PARAMS_1 = 0x%x\n",mapu_wdt_params_1);                          
    printk("WDT_COMP_PARAMS_2 = 0x%x\n",mapu_wdt_params_2);                          
    printk("WDT_COMP_PARAMS_3 = 0x%x\n",mapu_wdt_params_3);                          
    printk("WDT_COMP_PARAMS_4 = 0x%x\n",mapu_wdt_params_4);                          
    printk("WDT_COMP_PARAMS_5 = 0x%x\n",mapu_wdt_params_5);                          
                                                                                 
    mapu_wtd_params.WDT_ALWAYS_EN = mapu_wdt_params_1 & 0x01;                        
    mapu_wtd_params.WDT_DFLT_RMOD = (mapu_wdt_params_1 >> 1) & 0x01;                 
    mapu_wtd_params.WDT_DUAL_TOP = (mapu_wdt_params_1 >> 2) & 0x01;                  
    mapu_wtd_params.WDT_HC_RMOD = (mapu_wdt_params_1 >> 3) & 0x01;                   
    mapu_wtd_params.WDT_HC_RPL = (mapu_wdt_params_1 >> 4) & 0x01;                    
    mapu_wtd_params.WDT_HC_TOP = (mapu_wdt_params_1 >> 5) & 0x01;                    
    mapu_wtd_params.WDT_USE_FIX_TOP = (mapu_wdt_params_1 >> 6) & 0x01;               
    mapu_wtd_params.WDT_PAUSE = (mapu_wdt_params_1 >> 7) & 0x01;                     
    mapu_wtd_params.WDT_APB_DATA_WIDTH = (mapu_wdt_params_1 >> 8) & 0x03;            
    mapu_wtd_params.WDT_DFLT_RPL = (mapu_wdt_params_1 >> 10) & 0x07;                 
    mapu_wtd_params.WDT_DFLT_TOP = (mapu_wdt_params_1 >> 16) & 0x0f;                 
    mapu_wtd_params.WDT_DFLT_TOP_INIT = (mapu_wdt_params_1 >> 20) & 0x0f;            
    mapu_wtd_params.WDT_CNT_WIDTH = (mapu_wdt_params_1 >> 24) & 0x1f;                
                                                                                 
    mapu_wtd_params.WDT_CNT_RST = mapu_wdt_params_2;                                                                                                                  
    mapu_wtd_params.WDT_TOP_RST = mapu_wdt_params_3;                                                                                                              
    mapu_wtd_params.WDT_USER_TOP_INIT_MAX = mapu_wdt_params_4;                                                                                                        
    mapu_wtd_params.WDT_USER_TOP_MAX = mapu_wdt_params_5;                                                                                        
}                       
*/         
static int mapu_wdt_time_left(void)  
{ 
    return readl(mapu_wdt.wdt_base + MAPU_WDT_CCVR) / clk_get_rate(mapu_wdt.clk);   //left seconds value
}

static int mapu_wdt_top_in_seconds( unsigned int top)
{
        /*
         * There are 16 possible timeout values in 0..15 where the number of
         * cycles is 2 ^ (16 + i) and the watchdog counts down.
         */
    return (1 << (16 + top)) / clk_get_rate(mapu_wdt.clk);   //counter value to second value
}

static int mapu_wdt_get_get_status(void)
{
    return readl(mapu_wdt.wdt_base + MAPU_WDT_STAT);  //1 = Interrupt is active regardless of polarity. 0 = Interrupt is inactive.  
}

static int mapu_wdt_get_top(void)
{
    unsigned int top = readl(mapu_wdt.wdt_base + MAPU_WDT_TORR) & 0xF;
    return mapu_wdt_top_in_seconds(top);    
}

static void mapu_wdt_clear_interrupt(void) //clear interrupt
{
        readl(mapu_wdt.wdt_base + MAPU_WDT_EOI);  //read WDT_EOI to clear interrupt 
}  

static int mapu_wdt_set_top_s(unsigned int top_s)
{
    unsigned int i, top_val = MAPU_WDT_MAX_TOP;
    /*
     * Iterate over the timeout values until we find the closest match. We
     * always look for >=.
     */
    for (i = 0; i <= MAPU_WDT_MAX_TOP; ++i)
        if (mapu_wdt_top_in_seconds(i) >= top_s) 
        {
            top_val = i;
            break;
        }  
        
    /* Set the new value in the watchdog. */   
    writel(top_val , mapu_wdt.wdt_base + MAPU_WDT_TORR);//write value of top into MAPU_WDT_TORR
    top_val = readl(mapu_wdt.wdt_base + MAPU_WDT_TORR);
    printk("MAPU_WDT_TORR = 0x%x\n",top_val);
    return mapu_wdt_top_in_seconds(top_val);
}

static void mapu_wdt_keepalive(void)
{
    /* MAPU_WDT_CRR can not read */
    writel(MAPU_CRR , mapu_wdt.wdt_base + MAPU_WDT_CRR);//write 0x76 into MAPU_WDT_CRR[7:0] Counter Restart  
}

static void mapu_wdt_disable(void)   //close watchdog
{
    unsigned int mapu_wdt_cr = 0;
    mapu_wdt_cr = readl(mapu_wdt.wdt_base + MAPU_WDT_CR);   
    mapu_wdt_cr &= ~(MAPU_WDT_EN);
    writel(mapu_wdt_cr , mapu_wdt.wdt_base + MAPU_WDT_CR);
    if(mapu_wdt_cr&MAPU_WDT_EN)      //for  debug    printf the value of MAPU_WDT_CR.
    {
            printk(KERN_ALERT"MAPU_WDT_CR = 0x%x,close watchdog failed!\n",mapu_wdt_cr);
    }
    else
    {
            printk(KERN_ALERT"MAPU_WDT_CR = 0x%x,close watchdog succeed!\n",mapu_wdt_cr);
    }               
}

static void mapu_wdt_enable(void)   //enable watchdog
{            
    unsigned int mapu_wdt_cr = 0;
    mapu_wdt_cr |= MAPU_WDT_EN|MAPU_WDT_RMOD|MAPU_WDT_RPL;
    writel(mapu_wdt_cr , mapu_wdt.wdt_base + MAPU_WDT_CR);
    mapu_wdt_cr = readl(mapu_wdt.wdt_base + MAPU_WDT_CR);
    if(mapu_wdt_cr&MAPU_WDT_EN)  //for  debug    printf the value of MAPU_WDT_CR.
    {
            printk(KERN_ALERT"MAPU_WDT_CR = 0x%x,Enable WTD succeed!\n",mapu_wdt_cr);
    }
    else
    {
            printk(KERN_ALERT"MAPU_WDT_CR = 0x%x,Enable WTD failed!\n",mapu_wdt_cr);
    }       
} 

ssize_t mapu_wdt_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos) 
{
    /* Null Do not know what should be read? */
    return 0;       
}
                
ssize_t mapu_wdt_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos) 
{
    /* Null Do not know what should be write? */
    return 0;                          
}       


long mapu_wdt_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) 
{
    unsigned int val;
    
    switch (cmd) 
    {       
        case WDIOC_GETSTATUS:           //get status
            return put_user(mapu_wdt_get_get_status(), (int __user *)arg);
        
        /*
         *  WDIOS_DISABLECARD   0x0001  Turn off the watchdog timer
         *  WDIOS_ENABLECARD    0x0002  Turn on the watchdog timer
         */ 
        case WDIOC_SETOPTIONS:          //set opention
            if (get_user(val, (int __user *)arg))
                return -EFAULT;
            if (val == WDIOS_DISABLECARD)
                mapu_wdt_disable();     //close watchdog
            if (val == WDIOS_ENABLECARD)
                mapu_wdt_enable();      //enable watchdog
            return 0; 
        /* counter reset */
        case WDIOC_KEEPALIVE:
            mapu_wdt_keepalive();
            return 0; 
             
        /* set value of top_s */
        case WDIOC_SETTIMEOUT:
            if (get_user(val, (int __user *)arg))
                return -EFAULT;
            
            mapu_wdt.top_s = val;     //second
            mapu_wdt_set_top_s(mapu_wdt.top_s);
            return put_user(mapu_wdt_get_top() , (int __user *)arg);
                
        /* gain value of top */
        case WDIOC_GETTIMEOUT:
            return put_user(mapu_wdt_get_top(), (int __user *)arg);
        
        case WDIOC_GETTIMELEFT:
            /* Get the time left until expiry */
            if (get_user(val, (int __user *)arg))
                return -EFAULT;
            return put_user(mapu_wdt_time_left(), (int __user *)arg);
        
        default:
            return -ENOTTY;
    }               
}
static struct timer_list test;

int mapu_wdt_open(struct inode *inode, struct file *filp) 
{                
    mapu_wdt.top_s = 3; //for test open /dev/watchdog  set timeout = 3s
    mapu_wdt_set_top_s(mapu_wdt.top_s);
    mapu_wdt_enable();  //enable watchdog



    return 0;               
}

int mapu_wdt_release(struct inode *inode, struct file *filp)
{            
    mapu_wdt_disable();   //disable watchdog
    return 0;
}

static const struct file_operations mapu_wdt_fops = {
    .read   = mapu_wdt_read,
    .write    = mapu_wdt_write,
    .unlocked_ioctl = mapu_wdt_ioctl,
    .open   = mapu_wdt_open,
    .release  = mapu_wdt_release,
};

static struct miscdevice mapu_wdt_miscdev = {
    .minor  = WATCHDOG_MINOR,    //watchdog minor device number in miscdevice.h
    .name   = "watchdog",
    .fops   = &mapu_wdt_fops,
};

/* interrupt handler code */
static irqreturn_t mapu_wdt_irq(int irq, void *dev_id)
{
    printk("watchdog goes into interrupt\n");   
    mapu_wdt_set_top_s(mapu_wdt.top_s);   //set value of top
    mapu_wdt_clear_interrupt();         //clear interrupt
    mapu_wdt_keepalive();               //restart counter
    return IRQ_HANDLED;
}

static int __devinit mapu_wdt_probe(struct platform_device *pdev)
{
    struct device *dev;
    int ret;
    int size;
    
    dev = &pdev->dev;
    mapu_wdt_pdev = pdev;  //platform device attached to
    
    mapu_wdt.wdt_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);   //gain platform IO resource
    if (mapu_wdt.wdt_mem == NULL) 
    {
            dev_err(dev, "no memory resource specified\n");
            return -ENOENT;
    }
    
    mapu_wdt.wdt_irq = platform_get_resource(pdev, IORESOURCE_IRQ, 0);   //gain platform interrupt resource
    if (mapu_wdt.wdt_irq == NULL) 
    {
            dev_err(dev, "no irq resource specified\n");
            return -ENOENT;
    }
    
    ret = request_irq(mapu_wdt.wdt_irq->start, mapu_wdt_irq, 0, pdev->name, pdev);  //register interrupt function
    if (ret != 0) 
    {
            dev_err(dev, "failed to install irq (%d)\n", ret);
            return -ENOENT;;
    }
    
    /* get the memory region for the watchdog timer */
    
    size = resource_size(mapu_wdt.wdt_mem);
    if (!request_mem_region(mapu_wdt.wdt_mem->start, size, pdev->name)) 
    {
            dev_err(dev, "failed to get memory region\n");
            return -EBUSY;
    }
    
    mapu_wdt.wdt_base = ioremap(mapu_wdt.wdt_mem->start, size);  //physics address to virtual address
    if (mapu_wdt.wdt_base == NULL) 
    {
            dev_err(dev, "failed to ioremap() region\n");
            return -EINVAL;
    }
    
    printk(KERN_ALERT"probe: mapped wdt_base=%p\n", mapu_wdt.wdt_base);
    
    mapu_wdt.clk = clk_get(&pdev->dev, "watchdog");  //gain platform clk resource  match by "watchdog"
    if (IS_ERR(mapu_wdt.clk))
    {
            dev_err(dev, "failed to find watchdog clock source\n");
            return PTR_ERR(mapu_wdt.clk);
    }
    
    clk_enable(mapu_wdt.clk);
    
    /* register miscdev*/
    mapu_wdt_miscdev.parent = &pdev->dev;
    ret = misc_register(&mapu_wdt_miscdev); //ע������豸
    if (ret) 
    {
            dev_err(dev, "cannot register watchdog (%d)\n", ret);
            return ret;
    } 
    
    //mapu_wdt_disable();
    return 0;

}

static int __devexit mapu_wdt_remove(struct platform_device *dev)
{
    misc_deregister(&mapu_wdt_miscdev);
    
    /* release resource  */
    free_irq(mapu_wdt.wdt_irq->start, dev); //release interrupt resource
    
    clk_disable(mapu_wdt.clk);//release clk resource
    clk_put(mapu_wdt.clk);
    mapu_wdt.clk = NULL;
    
    iounmap(mapu_wdt.wdt_base); //release IO resource
    release_mem_region(mapu_wdt.wdt_mem->start, resource_size(mapu_wdt.wdt_mem));
    mapu_wdt.wdt_irq = NULL;
    mapu_wdt.wdt_mem = NULL;
    return 0;
}


static void mapu_wdt_shutdown(struct platform_device *dev)
{
    mapu_wdt_disable();   //close watchdog
}


static const struct of_device_id mapu_wdt_match[] = {  //match .dts .compatible = "mapu-wdt"
    { .compatible = "wdt" },
    {},
};

MODULE_DEVICE_TABLE(of, mapu_wdt_match);

static struct platform_driver mapu_wdt_driver = {
    .probe      = mapu_wdt_probe,
    .remove     = __devexit_p(mapu_wdt_remove),
    .shutdown   = mapu_wdt_shutdown,
    .driver     = {
                    .owner  = THIS_MODULE,
                    .name   = "50440000.wdt",
                    .of_match_table = mapu_wdt_match,
        },
};


static int __init watchdog_init(void)
{

    return platform_driver_register(&mapu_wdt_driver);   //register platform driver 
}

static void __exit watchdog_exit(void)
{
    platform_driver_unregister(&mapu_wdt_driver);       //unregister platform driver
}

module_init(watchdog_init);
module_exit(watchdog_exit);

MODULE_AUTHOR("Yang Xin");
MODULE_DESCRIPTION("MAPU Watchdog Device Driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS_MISCDEV(WATCHDOG_MINOR);
