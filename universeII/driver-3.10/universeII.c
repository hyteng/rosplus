/*
    Linux driver for Tundra universeII PCI to VME bridge, kernel 2.6.x
    Copyright (C) 2006 Andreas Ehmanns <universeII@gmx.de>
 
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <linux/version.h>

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/spinlock.h>

#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h> // for kernel 3.10 new proc fops function
#include <linux/delay.h>

#include "universeII.h"
#include "vmeioctl.h"
#include "linux/sched.h"

MODULE_DESCRIPTION("VME driver for the Tundra Universe II PCI to VME bridge");
MODULE_AUTHOR("Andreas Ehmanns <universeII@gmx.de>");
MODULE_LICENSE("GPL");

static const char Version[] = "0.94 (January 2008)";

//#define VMIC
#ifdef VMIC
    #include "vmic.h"
    static void __iomem *VmicBaseAddr;
    static unsigned int VmicBase;
#endif


//----------------------------------------------------------------------------
// Module parameters
//----------------------------------------------------------------------------

static int sys_ctrl = 1;
module_param(sys_ctrl, int, 0);
MODULE_PARM_DESC(sys_ctrl, " Set to 1 to enable VME system controller (default)");

static int br_level = 3;
module_param(br_level, int, 0);
MODULE_PARM_DESC(br_level, " VMEBus request level (default is BR3)");

static int req_mode = 0;
module_param(req_mode, int, 0);
MODULE_PARM_DESC(req_mode, " Request mode. Default: demand");

static int rel_mode = 0;
module_param(rel_mode, int, 0);
MODULE_PARM_DESC(rel_mode, " Release mode. Default: Release when done (RWD)");

static int vrai_bs = 0;
module_param(vrai_bs, int, 0);
MODULE_PARM_DESC(vrai_bs, "  Enable VMEBus access to universeII registers. Default: Disabled");

static int vbto = 3;
module_param(vbto, int, 0);
MODULE_PARM_DESC(vbto, "     VMEBus Time-out");

static int varb = 0;
module_param(varb, int, 0);
MODULE_PARM_DESC(varb, "     VMEBus Arbitration Mode");

static int varbto = 1;
module_param(varbto, int, 0);
MODULE_PARM_DESC(varbto, "   VMEBus Arbitration Time-out");

static int img_ovl = 1;
module_param(img_ovl, int, 0);
MODULE_PARM_DESC(img_ovl, "  Set to 0 to forbid overlapping images. Default: Allowed");


//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------

static int universeII_open(struct inode *, struct file *);
static int universeII_release(struct inode *, struct file *);
static ssize_t universeII_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t universeII_write(struct file *, const char __user *, size_t,
                                loff_t *);
static int universeII_ioctl(struct file *, unsigned int,
                            unsigned long);
static int universeII_mmap(struct file *, struct vm_area_struct *);

static int universeII_procinfo(char *buf, char **start, off_t fpos, int lenght, int *eof, void *data);
static int universeII_proc_show(struct seq_file *m, void *v);
static int universeII_proc_open(struct inode *inode, struct  file *file);

/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
_/                                            _/
_/  Types and Constants                       _/
_/                                            _/
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/


static struct file_operations universeII_fops = {
	.owner   = THIS_MODULE,
    .open    = universeII_open,
    .release = universeII_release,
    .read    = universeII_read,
    .write   = universeII_write,
    .unlocked_ioctl   = universeII_ioctl,
    .mmap    = universeII_mmap
};

// new for kernel 3.10, specify the proc_fops
static struct proc_dir_entry* universeII_file;
static const struct file_operations universeII_proc_fops = {
  .owner = THIS_MODULE,
  .open = universeII_proc_open,
  .read = seq_read,//universeII_procinfo,
  .llseek = seq_lseek,
  .release = single_release,
};

static const int aCTL[18] = { LSI0_CTL, LSI1_CTL, LSI2_CTL, LSI3_CTL,
                              LSI4_CTL, LSI5_CTL, LSI6_CTL, LSI7_CTL, 0, 0,
                              VSI0_CTL, VSI1_CTL, VSI2_CTL, VSI3_CTL,
                              VSI4_CTL, VSI5_CTL, VSI6_CTL, VSI7_CTL };

static const int aBS[18] = { LSI0_BS, LSI1_BS, LSI2_BS, LSI3_BS,
                             LSI4_BS, LSI5_BS, LSI6_BS, LSI7_BS, 0, 0,
                             VSI0_BS, VSI1_BS, VSI2_BS, VSI3_BS,
                             VSI4_BS, VSI5_BS, VSI6_BS, VSI7_BS };

static const int aBD[18] = { LSI0_BD, LSI1_BD, LSI2_BD, LSI3_BD,
                             LSI4_BD, LSI5_BD, LSI6_BD, LSI7_BD, 0, 0,
                             VSI0_BD, VSI1_BD, VSI2_BD, VSI3_BD,
                             VSI4_BD, VSI5_BD, VSI6_BD, VSI7_BD };

static const int aTO[18] = { LSI0_TO, LSI1_TO, LSI2_TO, LSI3_TO,
                             LSI4_TO, LSI5_TO, LSI6_TO, LSI7_TO, 0, 0,
                             VSI0_TO, VSI1_TO, VSI2_TO, VSI3_TO,
                             VSI4_TO, VSI5_TO, VSI6_TO, VSI7_TO };

static const int aVIrq[7] = { V1_STATID, V2_STATID, V3_STATID, V4_STATID,
                              V5_STATID, V6_STATID, V7_STATID};

static const int mbx[4] = { MAILBOX0, MAILBOX1, MAILBOX2, MAILBOX3 };


/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
_/                                            _/
_/  Vars and Defines                          _/
_/                                            _/
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/

#define UNI_MAJOR   221
#define MAX_IMAGE     8
#define MAX_MINOR    17
#define CONTROL_MINOR 8
#define DMA_MINOR     9

#define PCI_BUF_SIZE  0x20000            // Size of one slave image buffer
#define DMA_ACTIVE_TIMEOUT HZ            // 1s is the maximum time the
                                         // DMA is allowed to be active

static struct pci_dev *universeII_dev = NULL;


// Tundra chip and image internal handling addresses

static void __iomem *baseaddr = 0;       // Base address of Tundra chip

static void __iomem *dmaBuf = 0;         // DMA buf address in kernel space
static dma_addr_t dmaHandle = 0;

static unsigned int dmaBufSize = 0;      // Size of one DMA buffer
static unsigned int dma_dctl;            // DCTL register for DMA
static int dma_in_use = 0;


// All image related information like start address, end address, ...

static image_desc_t image[18];


// Pointers to 256 available linked lists

static struct cpl cpLists[256];

static irq_device_t irq_device[7][256];


// Structure holds information about driver statistics (reads, writes, ...)

static driver_stats_t statistics;


// VMEBus interrupt wait queue

DECLARE_WAIT_QUEUE_HEAD(vmeWait);


// DMA timer and DMA wait queue

static struct timer_list DMA_timer;      // This is a timer for returning status

DECLARE_WAIT_QUEUE_HEAD(dmaWait);


// Mailbox wait queues

static struct timer_list MBX_timer[4];   // This is a timer for returning status
static wait_queue_head_t mbxWait[4];


static vme_Berr_t vmeBerrList[32];       // A circular buffer for storing
                                         // the last 32 VME BERR.


// Spinlocks
/*
static spinlock_t get_image_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t set_image_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t vme_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t dma_lock = SPIN_LOCK_UNLOCKED;
static spinlock_t mbx_lock = SPIN_LOCK_UNLOCKED;
//static spinlock_t ioctl_lock = SPIN_LOCK_UNLOCKED;
*/
// SPIN_LOCK_UNLOCKED has been deprecated since 2.6.19 and will get removed in 2.6.39. use DEFINE_SPINLOCK(variable) instand
static DEFINE_SPINLOCK(get_image_lock);
static DEFINE_SPINLOCK(set_image_lock);
static DEFINE_SPINLOCK(vme_lock);
static DEFINE_SPINLOCK(dma_lock);
static DEFINE_SPINLOCK(mbx_lock);
//static DEFINE_SPINLOCK(ioctl_lock);





/*
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
_/                                            _/
_/  Functions                                 _/
_/                                            _/ 
_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/_/
*/


//----------------------------------------------------------------------------
//
//  DMA_timeout
//
//----------------------------------------------------------------------------
static void DMA_timeout(unsigned long ptr)
{
    wake_up_interruptible(&dmaWait);
    statistics.timeouts++;
}


//----------------------------------------------------------------------------
//
//  MBX_timeout
//
//----------------------------------------------------------------------------
static void MBX_timeout(unsigned long ptr)
{
    MBX_timer[ptr].data = 0xFFFF;
    wake_up_interruptible(&mbxWait[ptr]);
    statistics.timeouts++;
}


//----------------------------------------------------------------------------
//
//  VIRQ_timeout
//
//----------------------------------------------------------------------------
static void VIRQ_timeout(unsigned long ptr)
{
    irq_device[ptr >> 8][ptr & 0xFF].virqTimer.data = 0xFFFF;
    wake_up_interruptible(&irq_device[ptr >> 8][ptr & 0xFF].irqWait);
    statistics.timeouts++;
}


//----------------------------------------------------------------------------
//
//  irq_handler()
//
//----------------------------------------------------------------------------
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    int i;
    u32 status, enable, statVme;

    printk("UniverseII: VME IRQ handler! %d \n", irq);

    enable = ioread32(baseaddr + LINT_EN);
    status = ioread32(baseaddr + LINT_STAT);

    status &= enable;        // check only irq sources that are enabled

    if (!status)             // we use shared ints, so we first check
        return IRQ_NONE;     // if this irq origins from universeII chip

    statistics.irqs++;

    printk("UniverseII: VME IRQ handler! %d\n", irq);

    // VMEbus interrupt

    if (status & 0x00FE)
    {
        for (i = 7; i > 0; i--)          // find which VME irq line is set
            if (status & (1 << i))
                break;

        if (i) {
            i--;
            statVme = ioread32(baseaddr + aVIrq[i]);   // read Status/ID byte
            if (statVme & 0x100)
                printk ("UniverseII: VMEbus error during IACK cycle level %d !\n",
                        i + 1);
            else {
                if (irq_device[i][statVme].ok) {
                    if (irq_device[i][statVme].vmeAddrCl != 0)
                        iowrite32(irq_device[i][statVme].vmeValCl,
                               irq_device[i][statVme].vmeAddrCl);
                    printk("UniverseII: VME IRQ handler: wakeup %d, %d!\n", i, statVme);
                    wake_up_interruptible(&irq_device[i][statVme].irqWait);
                }
            }
            udelay(2);
        }
    }

    // DMA interrupt
    if (status & 0x0100)
        wake_up_interruptible(&dmaWait);

    // mailbox interrupt
    if (status & 0xF0000)
        for (i = 0; i < 4; i++)
            if (status & (0x10000 << i))
                wake_up_interruptible(&mbxWait[i]);

    // IACK interrupt
    if (status & 0x1000)
        wake_up_interruptible(&vmeWait);

    // VMEBus error
    if (status & 0x0400) {
        statVme = ioread32(baseaddr + V_AMERR);
        if (statVme & 0x00800000)   // Check if error log is valid
        {
            if (statVme & 0x01000000)   // Check if multiple errors occured
            {
                printk ("UniverseII: Multiple VMEBus errors detected! "
                        "Lost interrupt?\n");
                vmeBerrList[statistics.berrs & 0x1F].merr = 1;
            }
            vmeBerrList[statistics.berrs & 0x1F].valid = 1;
            vmeBerrList[statistics.berrs & 0x1F].AM = (statVme >> 26) & 0x3f;
            vmeBerrList[statistics.berrs & 0x1F].address = 
                                                     ioread32(baseaddr + VAERR);
            statistics.berrs++;

            iowrite32(0x00800000, baseaddr + V_AMERR);
        }
        else printk("UniverseII: VMEBus error log invalid!\n");
    }
    
// other interrupt sources are (at the moment) not supported

    iowrite32(status, baseaddr + LINT_STAT);   // Clear all pending irqs

    return IRQ_HANDLED;
}


//----------------------------------------------------------------------------
//
//  universeII_procinfo()
//
//----------------------------------------------------------------------------
static int universeII_procinfo(char *buf, char **start, off_t fpos, int lenght,
                               int *eof, void *data)
{
    const char *const Axx[8] = { "A16", "A24", "A32", "Reserved", "Reserved",
                                 "CR/SCR", "User1", "User2" };
    const char *const Dxx[4] = { "D8", "D16", "D32", "D64" };

    char *p;
    int i, index;
    u32 ctl, bs, bd, to;

    p = buf;
    p += sprintf(p, "UniverseII driver version %s\n", Version);

    p += sprintf(p, "  baseaddr = %08X\n\n", (int) baseaddr);

    if (vrai_bs != 0)
        p += sprintf(p, "Access to universeII registers from VME at: "
                        "0x%08x\n\n", vrai_bs);

    p += sprintf(p, "  Status variables:          DMA: ");
    if (dma_in_use)
        p += sprintf(p, "in use\n\n");
    else
        p += sprintf(p, "free\n\n");

    p += sprintf(p, "    reads      = %li\n    writes     = %li\n"
                    "    ioctls     = %li\n    irqs       = %li\n"
                    "    DMA errors = %li\n    timeouts   = %li \n\n", 
                 statistics.reads, statistics.writes, statistics.ioctls, 
                 statistics.irqs, statistics.dmaErrors, statistics.timeouts);

    p += sprintf(p, "Allocated master images:\n");

    for (i = 0; i < 8; i++) {
        if (image[i].opened) {
            ctl = ioread32(baseaddr + aCTL[i]);
            bs = ioread32(baseaddr + aBS[i]);
            bd = ioread32(baseaddr + aBD[i]);
            to = ioread32(baseaddr + aTO[i]);

            p += sprintf(p, "  Image %i:\n", i);
            p += sprintf(p, "    Registers                VMEBus range\n");
            p += sprintf(p, "    LSI%i_CTL = %08x        %s/%s\n", i, ctl,
                         Axx[(ctl >> 16) & 0x7], Dxx[(ctl >> 22) & 0x3]);
            p += sprintf(p, "    LSI%i_BS  = %08x\n", i, bs);
            p += sprintf(p, "    LSI%i_BD  = %08x       %08x\n", i, bd,
                         bs + to);
            p += sprintf(p, "    LSI%i_TO  = %08x       %08x\n\n", i, to,
                         bd + to);
        }
    }

    p += sprintf(p, "Allocated slave images:\n");

    for (i = 10; i < 18; i++) {
        if (image[i].opened) {
            ctl = ioread32(baseaddr + aCTL[i]);
            bs = ioread32(baseaddr + aBS[i]);
            bd = ioread32(baseaddr + aBD[i]);
            to = ioread32(baseaddr + aTO[i]);

            p += sprintf(p, "  Image %i:\n", i);
            p += sprintf(p, "    Registers                VMEBus range\n");
            p += sprintf(p, "    VSI%i_CTL = %08x          %s\n", i, ctl,
                         Axx[(ctl >> 16) & 0x7]);
            p += sprintf(p, "    VSI%i_BS  = %08x\n", i, bs);
            p += sprintf(p, "    VSI%i_BD  = %08x       %08x\n", i, bd, bs);
            p += sprintf(p, "    VSI%i_TO  = %08x       %08x\n\n", i, to, bd);
        }
    }

    p += sprintf(p, "\nNumber of occured VMEBus errors: %li\n", statistics.berrs);

    if (statistics.berrs > 0) {
        p += sprintf(p, "Showing last 32 BERRs (maximum)\n"
                     " BERR address   AM code     MERR\n");
        for (i = 0; i < 32; i++) {
            index = (statistics.berrs - 31 + i) & 0x1F;
            if (vmeBerrList[index].valid)
                p += sprintf(p, "   %08x       %02x         %01x\n",
                             vmeBerrList[index].address, vmeBerrList[index].AM,
                             vmeBerrList[index].merr);
        }
    }

    *eof = 1;
    return p - buf;
}


//----------------------------------------------------------------------------
//
//  register_proc()
//
//----------------------------------------------------------------------------
static void register_proc(void)
{
    //create_proc_read_entry() is deprecated in kernel 3.10 
    //create_proc_read_entry("universeII", 0, NULL, universeII_procinfo, NULL);
    universeII_file = proc_create("universeII", 0, NULL, &universeII_proc_fops);
}

//-----------------------------------------------------------------------------
//
// proc_open
//
//-----------------------------------------------------------------------------
static int universeII_proc_show(struct seq_file *m, void *v) {
  seq_printf(m, "Hello, this is universeII under proc!\n");
  return 0;
}

static int universeII_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, universeII_proc_show, NULL);
}
//----------------------------------------------------------------------------
//
//  unregister_proc()
//
//----------------------------------------------------------------------------
static void unregister_proc(void)
{
    remove_proc_entry("universeII", NULL);
}


//----------------------------------------------------------------------------
//
//  testAndClearBERR()
//
//----------------------------------------------------------------------------
static int testAndClearBERR(void)
{
    u32 tmp = ioread32(baseaddr + PCI_CSR);            // Check for a bus error

    if (tmp & 0x08000000)                           // S_TA is Set
    {
        iowrite32(tmp, baseaddr + PCI_CSR);
        statistics.berrs++;
        return 1;
    }
    
    return 0;
}


//----------------------------------------------------------------------------
//
//  testAndClearDMAErrors()
//
//----------------------------------------------------------------------------
static int testAndClearDMAErrors(void)
{
    u32 tmp = ioread32(baseaddr + DGCS);

    if (!(tmp & 0x00000800))      // Check if DMA status is done
    {
        printk("UniverseII: DMA error, DGCS: %08x !\n", tmp);
        if (tmp & 0x00008000) {   // Check for timeout
            printk("UniverseII: DMA stopped with timeout. DGCS = %08x !\n",
                   tmp);
            iowrite32(0x40000000, baseaddr + DGCS);    // Stop DMA
        }

        if (tmp & 0x00006700)     // Check for errors
            printk("UniverseII: DMA write stopped with error. DGCS = %08x !\n",
                   tmp);

        iowrite32(0x00006F00, baseaddr + DGCS);    // Clear all errors and
        statistics.dmaErrors++;                 // Disable all DMA irqs
        return -1;
    }

    return 0;
}


//----------------------------------------------------------------------------
//
//  execDMA()
//
//----------------------------------------------------------------------------
static void execDMA(u32 chain)
{
    DEFINE_WAIT(wait);

    DMA_timer.expires = jiffies + DMA_ACTIVE_TIMEOUT;  // We need a timer to
    DMA_timer.function = DMA_timeout;                  // timeout DMA transfers
    add_timer(&DMA_timer);

    prepare_to_wait(&dmaWait, &wait, TASK_INTERRUPTIBLE);
    iowrite32(0x80006F0F | chain, baseaddr + DGCS);    // Start DMA, clear errors
                                                    // and enable all DMA irqs
    schedule();                                     // Wait for DMA to finish

    del_timer(&DMA_timer);
    finish_wait(&dmaWait, &wait);
}


//----------------------------------------------------------------------------
//
//  getPciBaseAddr()
//
//----------------------------------------------------------------------------
static u32 getPciBaseAddr(u32 size, const struct pci_bus *bus)
{
    //#define MEM_START 0x40000000
    //#define MEM_STOP  0xF0000000
    u32 MEM_START = 0xf0400000;
    u32 MEM_STOP = 0xf0500000;
    u32 pciAddr, stepping = 0x010000;
    u32 offset = 0;

    int i;
    struct resource *new_resource;
    unsigned long align = 0x10000;

    for(i = 0; i < PCI_BRIDGE_RESOURCE_NUM; i++) {
        struct resource *r = bus->resource[i];
        if (!r)
            continue;
        if (!(r->flags & IORESOURCE_MEM))
            continue;

        MEM_START = r->start;
        MEM_STOP = r->end;
    
        if(MEM_START & 0xFFFF)
            offset = (0x10000 - (MEM_START & 0xFFFF)) & 0xFFFF;

        MEM_START += offset;
        //int retval = allocate_resource(r, new_resource, size, 0x40000000, 0xF0000000, align, NULL, NULL);
        
        //if (!retval)
            //break;

    
    for (pciAddr = MEM_START; pciAddr < MEM_STOP; pciAddr += stepping) {
        if (check_mem_region(pciAddr, size) == 0)
            break;
    }

    /*
    if (pciAddr >= MEM_STOP) {
        stepping = 0x10000;
        for (pciAddr = MEM_START; pciAddr < MEM_STOP; pciAddr += stepping)
            if (check_mem_region(pciAddr, size) == 0)
                break;
    }
    */

    if (pciAddr >= MEM_STOP)
        return 0;
    
    }
    //pciAddr = new_resource->start;
    struct resource *pciResource =  request_mem_region(pciAddr, size, "universeII");
    if(!pciResource)
        pciAddr = 0;
    
    return pciAddr;
}


//----------------------------------------------------------------------------
//
//  universeII_read()
//
//----------------------------------------------------------------------------
static ssize_t universeII_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    int i = 0, okcount = 0, offset = 0, berr = 0;
    unsigned int dw, pci = 0;
    char *temp = buf;

    u8 vc;          // 8 bit transfers
    u16 vs;         // 16 bit transfers
    u32 vi;         // 32 bit transfers

    void __iomem *image_ptr;
    dma_param_t dmaParam;
    unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);

    statistics.reads++;
    switch (minor) {
        case CONTROL_MINOR:
            vi = ioread32(baseaddr + (*ppos & 0x0FFFFFFF));
            __copy_to_user(temp, &vi, 4);
            break;

        case DMA_MINOR:
            __copy_from_user(&dmaParam, buf, sizeof(dmaParam));
            if (dmaBufSize * dmaParam.bufNr + dmaParam.count > PCI_BUF_SIZE) {
            	printk("UniverseII: DMA operation exceeds DMA buffer size!");
            	return -1;
            }
            
            dma_dctl = dmaParam.dma_ctl | dmaParam.vas | dmaParam.vdw;
            pci = dmaHandle + dmaBufSize * dmaParam.bufNr;

            if ((pci < dmaHandle) || 
                (pci + dmaParam.count > dmaHandle + PCI_BUF_SIZE))
                return -2;

            // Check that DMA is idle
            if (ioread32(baseaddr + DGCS) & 0x00008000) {
                printk("UniverseII: DMA device is not idle!\n");
                return 0;
            }

            iowrite32(dma_dctl, baseaddr + DCTL);          // Setup Control Reg
            iowrite32(dmaParam.count, baseaddr + DTBC);    // Count 
            iowrite32(dmaParam.addr, baseaddr + DVA);      // VME Address

            // lower 3 bits of VME and PCI address must be identical,
            if ((pci & 0x7) == (dmaParam.addr & 0x7))
                iowrite32(pci, baseaddr + DLA);            // PCI address
            else {
                offset = (((dmaParam.addr & 0x7) + 0x8) - (pci & 0x7)) & 0x7;
                iowrite32(pci + offset, baseaddr + DLA);
            }

            execDMA(0);                          // Start and wait for DMA

            if (testAndClearDMAErrors())         // Check for DMA errors
                okcount = -1;
            else
                okcount = offset;

            break;

        default:
            if (image[minor].okToWrite) {
                if ((*ppos & 0x0FFFFFFF) + count > image[minor].size)
                    return -1;

                image_ptr = image[minor].vBase + (*ppos & 0x0FFFFFFF);

                dw = (*ppos >> 28) & 0xF;   // Data width 1, 2 or 4 byte(s)

                switch (dw) {
                    case 1:
                        for (i = 0; i < count; i++) {
                            spin_lock(&vme_lock);
                            vc = ioread8(image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount++;

                            __copy_to_user(temp, &vc, 1);
                            image_ptr++;
                            temp++;
                        }
                        break;

                    case 2:
                        count /= 2;                     // Calc number of words
                        for (i = 0; i < count; i++) {
                            spin_lock(&vme_lock);
                            vs = ioread16(image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            if(berr) {
                            printk("UniverseII: read %x ", *ppos&0x0FFFFFFF);
                            printk("UniverseII: read get bus error!");
                            }
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount += 2;

                            __copy_to_user(temp, &vs, 2);
                            image_ptr += 2;
                            temp += 2;
                        }
                        break;

                    case 4:
                        count /= 4;                     // Calc number of longs
                        for (i = 0; i < count; i++) {
                            spin_lock(&vme_lock);
                            vi = ioread32(image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount += 4;

                            __copy_to_user(temp, &vi, 4);
                            image_ptr += 4;
                            temp += 4;
                        }
                        break;
                }       // of switch(dw)
            }       // of if (okToWrite)
            break;
    }           // switch(minor)

    *ppos += count;
    return okcount;
}


//----------------------------------------------------------------------------
//
//  universeII_write()
//
//----------------------------------------------------------------------------
static ssize_t universeII_write(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos)
{
    int i = 0, okcount = 0, offset = 0, berr = 0;
    unsigned int dw, pci = 0;
    char *temp = (char *) buf;

    u8 vc;          // 8 bit transfers
    u16 vs;         // 16 bit transfers
    u32 vi;         // 32 bit transfers

    void __iomem *image_ptr;
    dma_param_t dmaParam;
    unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);

    statistics.writes++;
    switch (minor) {
        case CONTROL_MINOR:
            __copy_from_user(&vi, temp, 4);
            iowrite32(vi, baseaddr + (*ppos & 0x0FFFFFFF));
            break;

        case DMA_MINOR:
            __copy_from_user(&dmaParam, buf, sizeof(dmaParam));
            if (dmaBufSize * dmaParam.bufNr + dmaParam.count > PCI_BUF_SIZE) {
            	printk("UniverseII: DMA operation exceeds DMA buffer size!");
            	return -1;
            }
            
            dma_dctl = dmaParam.dma_ctl | dmaParam.vas | dmaParam.vdw;
            pci = dmaHandle + dmaBufSize * dmaParam.bufNr;

            if ((pci < dmaHandle) || 
                (pci + dmaParam.count > dmaHandle + PCI_BUF_SIZE))
                return -2;

            // Check that DMA is idle
            if (ioread32(baseaddr + DGCS) & 0x00008000) {
                printk("UniverseII: DMA device is not idle!\n");
                return 0;
            }

            iowrite32(0x80000000 | dma_dctl, baseaddr + DCTL);  // Setup Control Reg
            iowrite32(dmaParam.count, baseaddr + DTBC);         // Count
            iowrite32(dmaParam.addr, baseaddr + DVA);           // VME address

            // lower 3 bits of VME and PCI address must be identical,
            if ((pci & 0x7) == (dmaParam.addr & 0x7))
                iowrite32(pci, baseaddr + DLA);                 // PCI address
            else {
                offset = (((dmaParam.addr & 0x7) + 0x8) - (pci & 0x7)) & 0x7;
                iowrite32(pci + offset, baseaddr + DLA);
            }

            execDMA(0);                          // Start and wait for DMA

            if (testAndClearDMAErrors())         // Check for DMA errors
                okcount = -1;
            else
                okcount = offset;

            break;

        default:
            if (image[minor].okToWrite) {
                if ((*ppos & 0x0FFFFFFF) + count > image[minor].size)
                    return -1;

                image_ptr = image[minor].vBase + (*ppos & 0x0FFFFFFF);

                dw = (*ppos >> 28) & 0xF;        // Data width 1, 2 or 4 byte(s)

                switch (dw) {
                    case 1:
                        for (i = 0; i < count; i++) {
                            __copy_from_user(&vc, temp, 1);
                            spin_lock(&vme_lock);
                            iowrite8(vc, image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount++;

                            image_ptr++;
                            temp++;
                        }
                        break;

                    case 2:
                        count /= 2;                     // Calc number of words
                        for (i = 0; i < count; i++) {
                            __copy_from_user(&vs, temp, 2);
                            spin_lock(&vme_lock);
                            iowrite16(vs, image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount += 2;

                            image_ptr += 2;
                            temp += 2;
                        }
                        break;

                    case 4:
                        count /= 4;                     // Calc number of longs
                        for (i = 0; i < count; i++) {
                            __copy_from_user(&vi, temp, 4);
                            spin_lock(&vme_lock);
                            iowrite32(vi, image_ptr);
                            berr = testAndClearBERR();  // Check for a bus error
                            spin_unlock(&vme_lock);

                            if (berr)
                                return okcount;
                            else
                                okcount += 4;

                            image_ptr += 4;
                            temp += 4;
                        }
                        break;
                }       // of switch(dw)
            }       // of if (okToWrite)
            break;
    }           // switch(minor)

    *ppos += count;
    return okcount;
}


//----------------------------------------------------------------------------
//
//  universeII_mmap()
//
//----------------------------------------------------------------------------
static int universeII_mmap(struct file *file, struct vm_area_struct *vma)
{
    unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);
    image_desc_t *p;

    file->private_data = &image[minor];
    p = file->private_data;

    if (minor < MAX_IMAGE) {                     // master image
        if (vma->vm_end - vma->vm_start > p->size) {
            printk ("UniverseII mmap: INVALID, start at 0x%08lx end 0x%08lx, "
                    "pstart 0x%08x, pend 0x%08x\n", vma->vm_start,
                    vma->vm_end, p->phys_start, p->phys_end);
            return -EINVAL;
        }

        vma->vm_pgoff = p->phys_start >> PAGE_SHIFT;
    }

    if (minor == DMA_MINOR) {                    // DMA
       if (vma->vm_end - vma->vm_start > PCI_BUF_SIZE) {
            printk ("UniverseII mmap: INVALID, start at 0x%08lx end "
                    "0x%08lx\n", vma->vm_start, vma->vm_end);
            return -EINVAL;
        }

        vma->vm_pgoff = dmaHandle >> PAGE_SHIFT;
    }

    if ((minor > 9) && (minor <= MAX_MINOR)) {   // slave image 
        if (vma->vm_end - vma->vm_start > PCI_BUF_SIZE) {
            printk ("UniverseII mmap: INVALID, start at 0x%08lx end "
                    "0x%08lx\n", vma->vm_start, vma->vm_end);
            return -EINVAL;
        }

        vma->vm_pgoff = p->buffer >> PAGE_SHIFT;
    }

    if ((minor == CONTROL_MINOR) || (minor > MAX_MINOR))
        return -EBADF;
    
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
                        vma->vm_end - vma->vm_start, vma->vm_page_prot) != 0) {
        printk("UniverseII mmap: remap_pfn_range failed !\n");
        return (-EAGAIN);
    }

    vma->vm_file = file;

    return 0;
}


//----------------------------------------------------------------------------
//
//  universeII_open()
//
//----------------------------------------------------------------------------
static int universeII_open(struct inode *inode, struct file *file)
{
    unsigned int minor = MINOR(inode->i_rdev);

    if (minor > MAX_MINOR)
        return (-ENODEV);

    switch (minor) {
        case CONTROL_MINOR:
        case DMA_MINOR:
            image[minor].opened++;
            return 0;
    }

    if (image[minor].opened != 1) // this images wasn't allocated by IOCTL_GET_IMAGE
        return (-EBUSY);

    image[minor].opened = 2;
    //image[minor].buffer = 0;

    return 0;
}


//----------------------------------------------------------------------------
//
//  universeII_release()
//
//----------------------------------------------------------------------------
static int universeII_release(struct inode *inode, struct file *file)
{
    unsigned int minor = MINOR(inode->i_rdev);
    int i, j;

    if (image[minor].vBase != NULL) {
        iounmap(image[minor].vBase);
        image[minor].vBase = NULL;
    }

    if (minor < MAX_IMAGE)             // release pci mapping when master image
        release_mem_region(image[minor].phys_start, image[minor].size);

    image[minor].opened = 0;
    image[minor].okToWrite = 0;
    image[minor].phys_start = 0;
    image[minor].phys_end = 0;
    image[minor].size = 0;

    if ((minor > 9) && (minor < 18)) {    // Slave image
        image[minor].buffer = 0;
    }

    for (i = 0; i < 7; i++)               // make sure to free all VMEirq/Status
        for (j = 0; j < 256; j++)         // combinations of this image 
            if (irq_device[i][j].ok == minor + 1)
                irq_device[i][j].ok = 0;

    return 0;
}


//----------------------------------------------------------------------------
//
//  universeII_ioctl()
//
//----------------------------------------------------------------------------
static int universeII_ioctl(struct file *file, unsigned int cmd,
                            unsigned long arg)
{
    unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);
    unsigned int i = 0;
    u32 ctl = 0, to = 0, bs = 0, bd = 0, imageStart, imageEnd;

    //spin_lock(&ioctl_lock);
    statistics.ioctls++;
    switch (cmd) {
        case IOCTL_SET_CTL:
            iowrite32(arg, baseaddr + aCTL[minor]);
            break;

        case IOCTL_SET_OPT:
            if (arg & 0x10000000)
                iowrite32(ioread32(baseaddr + aCTL[minor]) & ~arg,
                       baseaddr + aCTL[minor]);
            else
                iowrite32(ioread32(baseaddr + aCTL[minor]) | arg,
                       baseaddr + aCTL[minor]);
            break;

        case IOCTL_SET_IMAGE: {
                unsigned int pciBase = 0;
                image_regs_t iRegs;

                copy_from_user(&iRegs, (char *) arg, sizeof(iRegs));

                if ((iRegs.ms < 0) || (iRegs.ms > 1))
                    return -1;

                spin_lock(&set_image_lock);

                if (image[minor].opened != 2) {
                    spin_unlock(&set_image_lock);
                    printk("UniverseII: Allocation of image %d conflicts with "
                           "existing image!\n", minor);
                    return -2;  // the requested image seems to be already configured
                }

                if (!iRegs.ms) {   // master image
                    struct pci_bus *bus = universeII_dev->bus;

                    pciBase = getPciBaseAddr(iRegs.size, bus);
                    if (pciBase == 0) {
                        spin_unlock(&set_image_lock);
                        printk("UniverseII: Not enough iomem found for "
                               "requested image size!\n");
                        return -3;
                    }
                }
                else {    // slave image
                    if (minor < 10) {
                        spin_unlock(&set_image_lock);
                        printk ("UniverseII: IOCTL_SET_IMAGE, Image %d is not "
                                "a slave image!\n", minor);
                        return -4;
                    }
                    
                    if (image[minor].buffer == 0) {   // check if high memory is
                                                      // available
                        spin_unlock(&set_image_lock);
                        printk ("UniverseII: IOCTL_SET_IMAGE, No memory for "
                                "slave image available!\n");
                        return -5;
                    }
                }

                // First we check if this image overlaps with existing ones
                if (img_ovl == 0) {
                    for (i = 0; i < MAX_IMAGE; i++) {
                        if ((image[i].opened == 2) && (i != minor)) {
                            imageStart = ioread32(baseaddr + aBS[i]) + ioread32(baseaddr + aTO[i]);
                            imageEnd = ioread32(baseaddr + aBD[i]) + ioread32(baseaddr + aTO[i]);
    
                            if (! ((iRegs.base + iRegs.size <= imageStart)
                                || (iRegs.base >= imageEnd))) {
                                spin_unlock(&set_image_lock);
                                if (!iRegs.ms)
                                    release_mem_region(pciBase, iRegs.size);
                                printk("UniverseII: Overlap of image %d and %d !\n",
                                       i, minor);
                                printk("imageStart1 = %x, imageEnd1 = %x, "
                                       "imageStart2 = %x, imageEnd2 = %x !\n",
                                       iRegs.base, iRegs.base + iRegs.size,
                                       imageStart, imageEnd);
                                return -6;  // overlap with existing image
                            }
                        }
                    }
                }

                if (!iRegs.ms) {   // master image
                    printk("UniverseII: %x, %x, %x", pciBase, iRegs.base, -pciBase + iRegs.base);

                    iowrite32(pciBase, baseaddr + aBS[minor]);
                    iowrite32(pciBase + iRegs.size, baseaddr + aBD[minor]);
                    iowrite32(-pciBase + iRegs.base, baseaddr + aTO[minor]);
                }
                else {             // slave image
                    iowrite32(iRegs.base, baseaddr + aBS[minor]);
                    iowrite32(iRegs.base + iRegs.size, baseaddr + aBD[minor]);
                    iowrite32(image[minor].buffer - iRegs.base, 
                           baseaddr + aTO[minor]);
                }
    
                image[minor].okToWrite = 1;
                image[minor].opened = 3;

                //spin_unlock(&set_image_lock);

                //if (!iRegs.ms) {   // master image  
                image[minor].phys_start = ioread32(baseaddr + aBS[minor]);
                image[minor].phys_end = ioread32(baseaddr + aBD[minor]);
                image[minor].size = image[minor].phys_end - image[minor].phys_start;
                //}
                //else {
                //    image[minor].phys_start = ioread32(baseaddr + aBS[minor]) + ioread32(baseaddr + aTO[minor]);
                //    image[minor].phys_end = ioread32(baseaddr + aBD[minor]) + ioread32(baseaddr + aTO[minor]);
                //    image[minor].size = image[minor].phys_end - image[minor].phys_start;
                //}

                if (image[minor].vBase != NULL)
                    iounmap(image[minor].vBase);

                //if(image[minor].phys_start == image[minor].buffer) {
                //    printk("UniverseII: IOCTL_SET_IMAGE, this is a slave image, write BS the vme address and read BS give a PCI address which equal to dma buffer in PCI!\n");
                //    return -2;
                //}
                //else {
                //    printk("UniverseII: IOCTL_SET_IMAGE, this is a slave image, write BS the vme address and read BS give a VME address!\n");
                //    return -3;
                //}
        
                if (!iRegs.ms)
                    image[minor].vBase = ioremap_nocache(image[minor].phys_start, iRegs.size);
                else
                    image[minor].vBase = ioremap_nocache(virt_to_phys(image[minor].slaveBuf), iRegs.size);

                spin_unlock(&set_image_lock); 

                if (!(image[minor].vBase)) {
                    image[minor].okToWrite = 0;
                    image[minor].opened = 2;
                    if (!iRegs.ms)
                        release_mem_region(image[minor].phys_start, iRegs.size);
                    printk("UniverseII: IOCTL_SET_IMAGE, Error in ioremap!\n");
                    return -7;
                }

                break;
            }

        case IOCTL_GET_IMAGE: {
                unsigned int offset = 0;

                if ((arg < 0) || (arg > 1))
                    return -1;

                if (arg)                       // slave image was requested
                    offset = 10;

                i = 0;                         // look for a free image
                spin_lock(&get_image_lock);    // lock to prevent allocation of
                                               // same image
                while ((image[i + offset].opened) && (i < MAX_IMAGE))
                    i++;

                if (i >= MAX_IMAGE) {
                    spin_unlock(&get_image_lock);
                    return -2;
                } else {
                    image[i + offset].opened = 1;
                    spin_unlock(&get_image_lock);
                    return (i + offset);
                }

                break;
            }

        case IOCTL_GEN_VME_IRQ: {
                int level;

                DEFINE_WAIT(wait);

                if (arg & 0x01FFFFF8)          // unused bit is set
                    return -1;

                iowrite32(arg & 0xFE000000, baseaddr + STATID);
                level = 0x1000000 << (arg & 0x7);
                iowrite32(~level & ioread32(baseaddr + VINT_EN), baseaddr + VINT_EN);

                prepare_to_wait(&vmeWait, &wait, TASK_INTERRUPTIBLE);

                iowrite32(level | ioread32(baseaddr + VINT_EN), baseaddr + VINT_EN);
                //spin_unlock(&ioctl_lock);
                schedule();
                //spin_lock(&ioctl_lock);
                finish_wait(&vmeWait, &wait);
                iowrite32(~level & ioread32(baseaddr + VINT_EN), baseaddr + VINT_EN);

                break;
            }

        case IOCTL_SET_IRQ: {
                u32 base, toffset;
                void __iomem *virtAddr;
                int virq, vstatid;
                irq_setup_t isetup;

                copy_from_user(&isetup, (char *) arg, sizeof(isetup));

                virq = isetup.vmeIrq - 1;
                vstatid = isetup.vmeStatus;

                if ((virq < 0) || (virq > 6) || (vstatid < 0) || (vstatid > 255)) {
                    printk("UniverseII: IOCTL_SET_IRQ: Parameter out of range!\n");
                    return -1;
                }

                if (irq_device[virq][vstatid].ok) {
                    printk ("UniverseII: irq/status combination is already in "
                            "use !\n");
                    return -2;
                }

                toffset = ioread32(baseaddr + aTO[minor]);
                base = ioread32(baseaddr + aBS[minor]);

                if (isetup.vmeAddrSt != 0) {
                    if ((isetup.vmeAddrSt - toffset < image[minor].phys_start) ||
                        (isetup.vmeAddrSt - toffset > image[minor].phys_end))
                        return -3;

                    virtAddr = image[minor].vBase +
                                           (isetup.vmeAddrSt - toffset - base);
                    irq_device[virq][vstatid].vmeAddrSt = virtAddr;
                    irq_device[virq][vstatid].vmeValSt = isetup.vmeValSt;
                }
                else
                    irq_device[virq][vstatid].vmeAddrSt = 0;

                if (isetup.vmeAddrCl != 0) {
                    if ((isetup.vmeAddrCl - toffset < image[minor].phys_start) ||
                        (isetup.vmeAddrCl - toffset > image[minor].phys_end))
                        return -3;

                    virtAddr = image[minor].vBase +
                                           (isetup.vmeAddrCl - toffset - base);

                    irq_device[virq][vstatid].vmeAddrCl = virtAddr;
                    irq_device[virq][vstatid].vmeValCl = isetup.vmeValCl;
                }
                else
                    irq_device[virq][vstatid].vmeAddrCl = 0;

                init_waitqueue_head(&irq_device[virq][vstatid].irqWait);
                init_timer(&irq_device[virq][vstatid].virqTimer);
                irq_device[virq][vstatid].ok = minor + 1;

                break;
            }

        case IOCTL_FREE_IRQ: {
                int virq, vstatid;
                irq_setup_t isetup;

                copy_from_user(&isetup, (char *) arg, sizeof(isetup));

                virq = isetup.vmeIrq - 1;
                vstatid = isetup.vmeStatus;

                if ((virq < 0) || (virq > 6) || (vstatid < 0) || (vstatid > 255)) {
                    printk("UniverseII: IOCTL_SET_IRQ: Parameter out of range!\n");
                    return -1;
                }

                if (irq_device[virq][vstatid].ok == 0) {
                    printk ("UniverseII: irq/status combination not found!\n");
                    return -2;
                }
                
                irq_device[virq][vstatid].ok = 0;
                
                break;
            }

        case IOCTL_WAIT_IRQ: {
                int vmeIrq, vmeStatus;
                unsigned long timeout=0;
                irq_wait_t irqData;
                struct timer_list *vTimer = NULL;

                DEFINE_WAIT(wait);

                copy_from_user(&irqData, (char *) arg, sizeof(irqData));

                vmeIrq = irqData.irqLevel - 1;
                vmeStatus = irqData.statusID;

                if ((vmeIrq < 0) || (vmeIrq > 6) || (vmeStatus < 0) || (vmeStatus > 255)) {
                    printk("UniverseII: IOCTL_SET_IRQ: Parameter out of range!\n");
                    return -1;
                }

                if (irqData.timeout > 0) {
                    timeout = (irqData.timeout * HZ) / 1000;
                    if (timeout == 0)
                        timeout = 1;

                    vTimer = &irq_device[vmeIrq][vmeStatus].virqTimer;
                    vTimer->expires = jiffies + timeout;
                    vTimer->function = VIRQ_timeout;
                    vTimer->data = (vmeIrq << 8) + vmeStatus;
                }
                
                printk("UniverseII: IOCTL_SET_IRQ: prepare_to_wait: %d, %d!\n", vmeIrq, vmeStatus);
                prepare_to_wait(&irq_device[vmeIrq][vmeStatus].irqWait, &wait,TASK_INTERRUPTIBLE);
                if (irqData.timeout > 0)
                    add_timer(vTimer);

                if (irq_device[vmeIrq][vmeStatus].vmeAddrSt != 0)
                    iowrite32(irq_device[vmeIrq][vmeStatus].vmeValSt,
                           irq_device[vmeIrq][vmeStatus].vmeAddrSt);
                //spin_unlock(&ioctl_lock);
                schedule();
                //spin_lock(&ioctl_lock);
                printk("UniverseII: IOCTL_SET_IRQ: wakeup: %d, %d!\n", vmeIrq, vmeStatus);
                finish_wait(&irq_device[vmeIrq][vmeStatus].irqWait, &wait);
                if (irqData.timeout > 0) {
                    del_timer(vTimer);
                    if (irq_device[vmeIrq][vmeStatus].virqTimer.data == 0xFFFF)
                        return -2;
                }

                break;
            }

        case IOCTL_SET_MBX: {
                u32 mbx_en;
                unsigned int mbxNr;

                mbxNr = 0x10000 << (arg & 0x3);

                spin_lock(&mbx_lock);               // lock access to mbx

                mbx_en = ioread32(baseaddr + LINT_EN);
                if (mbx_en & mbxNr) {    // mbx already in use
                    spin_unlock(&mbx_lock);
                    return -1;
                }

                iowrite32(mbx_en | mbxNr, baseaddr + LINT_EN);

                spin_unlock(&mbx_lock);

                break;
            }

        case IOCTL_WAIT_MBX: {
                u32 lintEn;
                unsigned int mbxNr;

                DEFINE_WAIT(wait);

                mbxNr = arg & 0x3;

                spin_lock(&mbx_lock);
                lintEn = ioread32(baseaddr + LINT_EN);   // disable mailbox
                iowrite32(lintEn & ~(0x10000 << mbxNr), baseaddr + LINT_EN);

                iowrite32(0, baseaddr + mbx[mbxNr]);     // set mbx to 0
                iowrite32(lintEn, baseaddr + LINT_EN);   // enable mailbox

                ioread32(baseaddr + LINT_EN);

                MBX_timer[mbxNr].expires = jiffies + (arg >> 16) * HZ;
                MBX_timer[mbxNr].function = MBX_timeout;
                MBX_timer[mbxNr].data = mbxNr;
                add_timer(&MBX_timer[mbxNr]);
                spin_unlock(&mbx_lock);

                prepare_to_wait(&mbxWait[mbxNr], &wait, TASK_INTERRUPTIBLE);
                if (ioread32(baseaddr + LINT_STAT) & ~(0x10000 << mbxNr)) {
                    finish_wait(&mbxWait[mbxNr], &wait);
                    printk ("UniverseII: previous mailbox interrupt detected!\n");
                } else {
                    //spin_unlock(&ioctl_lock);
                    schedule();                       // Wait for mbx interrupt
                    //spin_lock(&ioctl_lock);
                    finish_wait(&mbxWait[mbxNr], &wait);
                }

                del_timer(&MBX_timer[mbxNr]);

                if (MBX_timer[mbxNr].data == 0xFFFF)
                    return -1;

                return ioread32(baseaddr + mbx[mbxNr]);

                break;
            }

        case IOCTL_RELEASE_MBX: {
                u32 lintEn;
                unsigned int mbxNr;

                mbxNr = 0x10000 << (arg & 0x3);

                spin_lock(&mbx_lock);

                lintEn = ioread32(baseaddr + LINT_EN); // check if mbx is enabled
                if ((lintEn & mbxNr) == 0) {
                    spin_unlock(&mbx_lock);
                    return -1;
                }

                iowrite32(lintEn & ~mbxNr, baseaddr + LINT_EN);

                spin_unlock(&mbx_lock);
                break;
            }

        case IOCTL_NEW_DCP: {
                for (i = 0; i < 256; i++)   // find a free list
                    if (cpLists[i].free)
                        break;

                if (i > 255)                // can't create more lists
                    return -1;

                cpLists[i].free = 0;        // mark list as not free
                return i;

                break;
            }

        case IOCTL_ADD_DCP: {
                unsigned int dla, offset;
                list_packet_t lpacket;
                struct kcp *newP, *ptr;

                copy_from_user(&lpacket, (char *) arg, sizeof(lpacket));
                newP = kmalloc(sizeof(*newP), GFP_KERNEL | GFP_DMA);

                ptr = cpLists[lpacket.list].commandPacket;
                if (ptr == NULL) {
                    cpLists[lpacket.list].commandPacket = newP;
                    cpLists[lpacket.list].start = 
                               pci_map_single(universeII_dev, &(newP->dcp.dctl),
                                              sizeof(*newP), DMA_BIDIRECTIONAL);
                }
                else {
                    while (ptr->next != NULL)     // find end of list
                        ptr = ptr->next;
                    ptr->next = newP;              // append new command packet
                    ptr->dcp.dcpp =
                               pci_map_single(universeII_dev, &(newP->dcp.dctl),
                                              sizeof(*newP), DMA_BIDIRECTIONAL);

                    if (ptr->dcp.dcpp & 0x0000001F) {
                        printk ("UniverseII: last 5 bits of dcpp != 0. dcpp "
                                "is: %08x !\n", ptr->dcp.dcpp);
                        kfree(newP);
                        return -1;
                    }

                    ptr->dcp.dcpp &= 0xFFFFFFFE;   // clear end bit
                }

                // fill newP command packet
                newP->next = NULL;
                newP->dcp.dctl = lpacket.dctl;   // control register
                newP->dcp.dtbc = lpacket.dtbc;   // number of bytes to transfer
                newP->dcp.dva = lpacket.dva;     // VMEBus address
                newP->dcp.dcpp = 0x00000001;     // last packet in list

                // last three bits of PCI and VME address MUST be identical!

                if (ptr == NULL)                 // calculate offset
                    dla = dmaHandle;
                else
                    dla = ptr->pciStart + ptr->dcp.dtbc;

                offset = (((lpacket.dva & 0x7) + 0x8) - (dla & 0x7)) & 0x7;

                if (dla + offset + lpacket.dtbc > dmaHandle + PCI_BUF_SIZE) {
                    ptr->next = NULL;
                    pci_unmap_single(universeII_dev, ptr->dcp.dcpp,
                                     sizeof(*newP), DMA_BIDIRECTIONAL);
                    ptr->dcp.dcpp = 0x00000001;
                    kfree(newP);
                    printk("UniverseII: DMA linked list packet exceeds global DMA "
                           "buffer size!");
                    return -1;
                }

                newP->dcp.dla = dla + offset;    // PCI address
                newP->pciStart = dla + offset;

                return offset;
                break;
            }

        case IOCTL_EXEC_DCP: {
                int n = 0;
                u32 val;
                struct kcp *scan;

                // Check that DMA is idle
                val = ioread32(baseaddr + DGCS);
                if (val & 0x00008000) {
                    printk ("UniverseII: Can't execute list %ld! DMA status = "
                            "%08x!\n", arg, val);
                    return -1;
                }

                iowrite32(0, baseaddr + DTBC);              // clear DTBC register
                iowrite32(cpLists[arg].start, baseaddr + DCPP);

                execDMA(0x08000000);                     // Enable chained mode

                if (testAndClearDMAErrors())             // Check for DMA errors
                    return -2;

                // Check that all command packets have been processed properly

                scan = cpLists[arg].commandPacket;
                while (scan != NULL) {
                    n++;
                    if (!(scan->dcp.dcpp & 0x00000002)) {
                        printk ("UniverseII: Processed bit of packet number "
                                "%d is not set!\n", n);
                        return n;
                    }
                    scan = scan->next;
                }

                break;
            }

        case IOCTL_DEL_DCL: {
                struct kcp *del, *search;

                search = cpLists[arg].commandPacket;
                while (search != NULL) {
                    del = search;
                    search = search->next;
                    pci_unmap_single(universeII_dev, del->dcp.dcpp, 
                                     sizeof(*del), DMA_BIDIRECTIONAL);
                    kfree(del);
                }
                cpLists[arg].commandPacket = NULL;
                cpLists[arg].free = 1;

                break;
            }

        case IOCTL_TEST_ADDR: {
                void __iomem *virtAddr;
                int berr;
                there_data_t there;

                __copy_from_user(&there, (char *) arg, sizeof(there));

                for (i = 0; i < MAX_IMAGE; i++)       // Find image that covers
                                                      // address
                    if (image[i].opened) {
                        ctl = ioread32(baseaddr + aCTL[i]);
                        bs = ioread32(baseaddr + aBS[i]);
                        bd = ioread32(baseaddr + aBD[i]);
                        to = ioread32(baseaddr + aTO[i]);
                        if ((there.addr >= bs + to) && (there.addr < bd + to))
                            break;
                    }
                if (i == MAX_IMAGE)          // no image for this address found
                    return -1;

                virtAddr = image[i].vBase + there.addr - to - bs;

                spin_lock(&vme_lock);

                if (testAndClearBERR())
                    printk ("UniverseII: Resetting previous uncleared bus error!\n");

                if (there.mode != 1)
                    ctl = there.mode;

                switch (ctl & 0x00C00000) {
                    case 0:
                        ioread8(virtAddr);
                        break;
                    case 0x00400000:
                        ioread16(virtAddr);
                        break;
                    case 0x00800000:
                        ioread32(virtAddr);
                        break;
                    default:
                        spin_unlock(&vme_lock);
                        return -2; // D64 is only supported for block transfers
                }

                berr = testAndClearBERR();
                spin_unlock(&vme_lock);

                return !berr;
                break;
            }

        case IOCTL_TEST_BERR: {
                int berr;

                spin_lock(&vme_lock);
                berr = testAndClearBERR();
                spin_unlock(&vme_lock);

                return berr;
                break;
            }

        case IOCTL_REQUEST_DMA: {
                int code = 0;

                spin_lock(&dma_lock);   // set spinlock to protect "dma_in_use"
                if ((dma_in_use) || (!dmaBuf))
                    code = 0;
                else {
                    if (arg)
                        dmaBufSize = PCI_BUF_SIZE / arg; // Divide DMA buf in
                                                         // multiple blocks
                    else
                        dmaBufSize = 0;
                    dma_in_use = 1;
                    code = 1;
                }
                spin_unlock(&dma_lock);
                return code;
                break;
            }

        case IOCTL_RELEASE_DMA: {
                dma_in_use = 0;
                break;
            }

        case IOCTL_VMESYSRST: {
                iowrite32(ioread32(baseaddr + MISC_CTL) | 0x400000, baseaddr + MISC_CTL);
                printk("UniverseII: VME SYSRST initiated!\n");
                break;
            }

        case IOCTL_RESET_ALL: {
                int j, error = 0;
                u32 csr;
                struct kcp *del, *search;

                printk("UniverseII: General driver reset requested by user!");

                // clear all previous PCI errors

                csr = ioread32(baseaddr + PCI_CSR);
                iowrite32(0xF9000000 | csr, baseaddr + PCI_CSR);

                // clear and release DMA

                if (dma_in_use) {
                    iowrite32(0x40000000, baseaddr + DGCS);  // stop DMA
                    udelay(100);
                    if (ioread32(baseaddr + DGCS) & 0x8000)  // DMA still active?
                        error = -1;

                    iowrite32(0x00006F00, baseaddr + DGCS);  // clear all previous
                                                          // errors and
                                                          // disable DMA irqs
                    dma_in_use = 0;
                }

                // remove all existing command packet lists

                for (i = 0; i < 256; i++)
                    if (cpLists[i].free == 0) {
                        search = cpLists[arg].commandPacket;
                        cpLists[arg].commandPacket = NULL;
                        cpLists[arg].free = 1;
                        while (search != NULL) {
                            del = search;
                            search = search->next;
                            kfree(del);
                        }
                    }

                // remove all irq setups

                for (i = 0; i < 7; i++)
                    for (j = 0; j < 256; j++)
                        irq_device[i][j].ok = 0;

                // free all mailboxes by disabling MBX irq

                iowrite32(0x000005FE, baseaddr + LINT_EN);

                // free all images

                for (i = 0; i < MAX_IMAGE; i++) {
                    iowrite32(0x00800000, baseaddr + aCTL[i]);
                    iowrite32(0x00800000, baseaddr + aCTL[i + 10]);

                    if (image[i].vBase != NULL) {
                        iounmap(image[i].vBase);
                        image[i].vBase = NULL;
                    }

                    image[i].opened = 0;
                    image[i].okToWrite = 0;
                }

                // reset all counters

                statistics.reads = 0;
                statistics.writes = 0;
                statistics.ioctls = 0;
                statistics.irqs = 0;
                statistics.berrs = 0;
                statistics.dmaErrors = 0;
                statistics.timeouts = 0;

                return error;

                break;
            }

        default:
            return -ENOIOCTLCMD;
    }
    //spin_unlock(&ioctl_lock);
    return 0;
}


//----------------------------------------------------------------------------
//
//  cleanup_module()
//
//----------------------------------------------------------------------------

static void __exit universeII_exit(void)
{
    int i;
    int pcivector;
    void __iomem *virtAddr;
    struct page *page;

    iowrite32(0, baseaddr + LINT_EN);  // Turn off Ints
    pcivector = ioread32(baseaddr + PCI_MISC1) & 0x000000FF;
    free_irq(pcivector, universeII_dev);   // Free Vector

    for (i = 1; i < MAX_IMAGE + 1; i++)
        if (image[i].vBase != NULL)
            iounmap(image[i].vBase);

    if (baseaddr != 0)
        iounmap(baseaddr);

#ifdef VMIC
    if (VmicBaseAddr != NULL)
        iounmap(VmicBaseAddr);
#endif

    unregister_proc();
    unregister_chrdev(UNI_MAJOR, "universeII");

    for (i = 10; i < 18; i++) {
        if (image[i].buffer) {
            virtAddr = image[i].slaveBuf;
            for (page = virt_to_page(virtAddr);
                 page < virt_to_page(virtAddr + PCI_BUF_SIZE); ++page)
                ClearPageReserved(page);

            pci_free_consistent(universeII_dev, PCI_BUF_SIZE, virtAddr,
                                image[i].buffer);
        }
    }

    if (dmaHandle) {
        virtAddr = dmaBuf;
        for (page = virt_to_page(virtAddr);
             page < virt_to_page(virtAddr + PCI_BUF_SIZE); ++page)
            ClearPageReserved(page);

        pci_free_consistent(universeII_dev, PCI_BUF_SIZE, virtAddr, dmaHandle);
    }

    printk("UniverseII driver removed!\n");
}


//----------------------------------------------------------------------------
//
//  init_module()
//
//----------------------------------------------------------------------------

static int __init universeII_init(void)
{
    u32 ba, temp, status, misc_ctl, mast_ctl, vrai_ctl, pci_csr;
    int i, j, irq, result;
    void __iomem *virtAddr;
    struct page *page;

#ifdef VMIC
    struct pci_dev *vmicPci = NULL;
#endif

    printk("UniverseII driver version %s\n", Version);

    // since kernel 3.10 pci_find_device is replaced with pci_get_device
    //universeII_dev = pci_find_device(PCI_VENDOR_ID_TUNDRA, PCI_DEVICE_ID_TUNDRA_CA91C042, universeII_dev);
    universeII_dev = pci_get_device(PCI_VENDOR_ID_TUNDRA, PCI_DEVICE_ID_TUNDRA_CA91C042, universeII_dev);

    if (!universeII_dev) {
        printk("UniverseII device not found!\n");
        return -1;
    }

    if(pci_enable_device(universeII_dev))
        return EIO;

    int pci_misc;
    pci_read_config_dword(universeII_dev, PCI_MISC0, &pci_misc);
    mb();
    printk("UniverseII PCI_MISC: %x \n", pci_misc);
    pci_misc |= 0x00008000;
    //pci_write_config_dword(universeII_dev, PCI_MISC0, 0); // Turn latency off
    pci_write_config_dword(universeII_dev, PCI_MISC0, pci_misc); // Turn latency on 
    //mb();

    printk("UniverseII found at bus %x device %x\n",
           universeII_dev->bus->number, universeII_dev->devfn);

    pci_read_config_dword(universeII_dev, PCI_CSR, &status);
    printk(" Vendor = %04X Device = %04X Status = %08X",
           universeII_dev->vendor, universeII_dev->device, status);
    printk("  Class = %08X\n", universeII_dev->class);

    pci_read_config_dword(universeII_dev, PCI_MISC0, &temp);
    printk("  Misc0 = %08X\n", temp);

    // Setup Universe Config Space
    // This is a 4k wide memory area that needs to be mapped into the
    // kernel virtual memory space so we can access it.
   
    // ba2 for debug info, I try to test why the code needs to wirte CONFIG_REG_SPACE to PCI_BS address??? I might be the reason for painic while loading to kernel space
    int ba2;
    pci_read_config_dword(universeII_dev, PCI_BS, &ba2);
    //pci_write_config_dword(universeII_dev, PCI_BS, CONFIG_REG_SPACE); // should be for old kernel or PPC arch, CONFIG_REG_SPACE=0xA000000 is for system PCI start address. in the kernel has config the PCI_BS info automaticaly and store it in pci_dev->resource[n].start. 
    pci_read_config_dword(universeII_dev, PCI_BS, &ba);
    //baseaddr = (void __iomem *) ioremap_nocache(ba, 4096);
    unsigned int theSize = universeII_dev->resource[0].end - universeII_dev->resource[0].start + 1;
    baseaddr = (void __iomem *) ioremap_nocache(universeII_dev->resource[0].start, theSize);
    if (!baseaddr) {
	printk("UniverseII: Read PCI_BS: %04X\n", ba2);
	printk("UniverseII: Read PCI_BS: %04X\n", ba);
    //printk("PCI_Device Resource[0]: %04X\n", universeII_dev->resource[0].start);
        printk("UniverseII: Ioremap failed to map UniverseII to kernel "
               "space.\n");
        return -2;
    }

    // Check to see if the mapping worked out

    if (ioread32(baseaddr) != 0x000010E3) {
        printk("UniverseII chip failed to return PCI_ID in memory map.\n");
        return -3;
    }

    // Set universe II to be VMEbus system controller
    // (set module option sys_ctrl=0 to disable system controller)

    misc_ctl = ioread32(baseaddr + MISC_CTL);
    if (sys_ctrl) {
        misc_ctl |= 0x00020000;

        if ((vbto < 0) || (vbto > 7))
            printk("UniverseII: Invalid VMEBus Timeout-out value: %d, "
                   "ignoring!\n", vbto);
        else
            misc_ctl |= (vbto & 0x7) << 28;

        if ((varb < 0) || (varb > 1))
            printk("UniverseII: Invalid VMEBus Arbitration Mode: %d, "
                   "ignoring!\n", varb);
        else if (varb)
            misc_ctl |= 0x04000000;
        else
            misc_ctl &= 0xFBFFFFFF;

        if ((varbto < 0) || (varbto > 2))
            printk("UniverseII: Invalid VMEBus Arbitration Timeout-out "
                   "value: %d, ignoring!\n", varbto);
        else
            misc_ctl |= (varbto & 0x3) << 24;
    }
    else {
        misc_ctl &= 0xFFFDFFFF;
        printk("UniverseII: VMEBus system controller disabled !\n");
    }

    iowrite32(misc_ctl, baseaddr + MISC_CTL);
    printk("UniverseII: MISC_CTL is %08x\n", ioread32(baseaddr + MISC_CTL));

    mast_ctl = ioread32(baseaddr + MAST_CTL);
    if ((br_level < 0) || (br_level > 3))
        printk("UniverseII: Invalid VME BR level: %d, ignoring!\n", br_level);
    else if (br_level != 3) {
        mast_ctl &= 0xFF3FFFFF;
        mast_ctl |= br_level << 22;
    }

    if ((req_mode < 0) || (req_mode > 1))
        printk("UniverseII: Invalid VMEBus request mode: %d, ignoring!\n",
               req_mode);
    else if (req_mode)
        mast_ctl |= 0x00200000;
    else
        mast_ctl &= 0xFFDFFFFF;

    if ((rel_mode < 0) || (rel_mode > 1))
        printk("UniverseII: Invalid VMEBus release mode: %d, ignoring!\n",
               rel_mode);
    else if (rel_mode)
        mast_ctl |= 0x00100000;
    else
        mast_ctl &= 0xFFEFFFFF;

    iowrite32(mast_ctl, baseaddr + MAST_CTL);
    printk("UniverseII: MAST_CTL is %08x\n", ioread32(baseaddr + MAST_CTL));

    // Setup access to universeII registers via VME if desired by option

    if (vrai_bs != 0) {
        if (vrai_bs & 0x00000FFF)          // lower 12 bits must be zero
            printk("UniverseII: Ignoring invalid vrai_bs %08x!\n", vrai_bs);
        else {
            vrai_ctl = 0x80F00000;
            if (vrai_bs & 0xFF000000)
                vrai_ctl |= 0x00020000;
            else if (vrai_bs & 0x00FF0000)
                vrai_ctl |= 0x00010000;

            iowrite32(vrai_ctl, baseaddr + VRAI_CTL);
            iowrite32(vrai_bs, baseaddr + VRAI_BS);
            printk("UniverseII: Enabling VME access to regs from addr. "
                   "%08x\n", vrai_bs);
        }
    }

#ifdef VMIC
    // Enable byte-lane-swapping for master and slave images and VMEbus 
    // access which is disabled by default!!!

    if ((vmicPci = pci_find_device(VMIC_VEND_ID, VMIC_FPGA_DEVICE_ID1, NULL)) ||
        (vmicPci = pci_find_device(VMIC_VEND_ID, VMIC_FPGA_DEVICE_ID2, NULL)) ||
        (vmicPci = pci_find_device(VMIC_VEND_ID, VMIC_FPGA_DEVICE_ID3, NULL))) {

        printk("VMIC subsystem ID: %x\n", vmicPci->subsystem_device);

        pci_read_config_dword(vmicPci, VMIC_FPGA_BASE_ADDR_REG, &VmicBase);
        VmicBaseAddr = ioremap_nocache(VmicBase, PAGE_SIZE);

        if (VmicBaseAddr == NULL)
            printk("UniverseII mapping of VMIC registers failed!\n");
        else
            iowrite16(VME_EN | BTO_EN | BTO_64 | MEC_BE | SEC_BE,
                   VmicBaseAddr + FPGA_COMM_OFFSET);
    }
    else
        printk("UniverseII: Can't find VMIC FPGA device!\n");
#endif

    // To use VMEbus slave images, the master bit must be set

    pci_csr = ioread32(baseaddr + PCI_CSR);
    mb();
    printk("UniverseII: PCI_CSR %x\n", pci_csr);
    pci_csr |= 0xF9000000;
    iowrite32(pci_csr, baseaddr + PCI_CSR);
    udelay(100);
    mb();
    //pci_csr = ioread32(baseaddr + PCI_CSR);
    //printk("UniverseII: PCI_CSR %x\n", pci_csr);
    if (!(pci_csr & 0x00000004)) {
        pci_csr |= 0x00000004;
        printk("UniverseII: PCI_CSR %x\n", pci_csr);
        iowrite32(pci_csr, baseaddr + PCI_CSR);
        udelay(100);
    }
    //mb();
    pci_csr = ioread32(baseaddr + PCI_CSR);
    printk("UniverseII: PCI_CSR check %x\n", pci_csr);

    // Clear sysfail line which (on some boards) is active by default

    if (ioread32(baseaddr + VCSR_CLR) & 0x40000000) {
        iowrite32(0x40000000, baseaddr + VCSR_CLR);
        printk("UniverseII: Switching off active SYSFAIL line!\n");
    }
   
    // Everything is ok so lets turn off the windows and set VDW to A32

    for (i = 0; i < MAX_IMAGE; i++) {
        iowrite32(0x00800000, baseaddr + aCTL[i]);         // Master images
        iowrite32(0x00800000, baseaddr + aCTL[i + 10]);    // Slave images
    }

    // Lets turn off interrupts

    iowrite32(0x00000000, baseaddr + LINT_EN);             // Disable interrupts
    iowrite32(0x0000FFFF, baseaddr + LINT_STAT);           // Clear Any Pending irqs

    pci_read_config_dword(universeII_dev, PCI_INTERRUPT_LINE, &irq);
    printk("UniverseII: IRQ %x\n", irq);
    irq &= 0x000000FF;
    irq = universeII_dev->irq;
    printk("UniverseII: IRQ %x\n", irq);
    result = request_irq(irq, irq_handler, IRQF_SHARED,
                         "UniverseII", universeII_dev);
    if (result) {
        printk("UniverseII: Can't get assigned pci irq vector %02X\n", irq);
        iounmap(baseaddr);
        return -4;
    } else {
        printk("UniverseII: Using PCI irq %02d (shared)!\n", irq);

        iowrite32(0x000015FE, baseaddr + LINT_EN);      // enable DMA IRQ, BERR,
                                                     // VME IRQ#1..#7 and SW_IACK
        iowrite32(0, baseaddr + LINT_MAP0);                // Map all irqs to LINT#0
        iowrite32(0, baseaddr + LINT_MAP1);                // Map all irqs to LINT#0
        iowrite32(0, baseaddr + LINT_MAP2);                // Map all irqs to LINT#0
    }

    // Clear all image descriptors

    for (i = 0; i < MAX_MINOR + 1; i++) {
        image[i].phys_start = 0;
        image[i].phys_end = 0;
        image[i].size = 0;
        image[i].vBase = NULL;
        image[i].opened = 0;
        image[i].okToWrite = 0;
        image[i].slaveBuf = NULL;
        image[i].buffer = 0;
    }

    // Reserve 128kB wide memory area for DMA buffer

    virtAddr = pci_alloc_consistent(universeII_dev, PCI_BUF_SIZE, &dmaHandle);
    if (virtAddr == NULL) {
        printk("UniverseII: Unable to allocate memory for DMA buffer!\n");
        dmaBuf = 0;
        iounmap(baseaddr);
        return -5;
    } else {
        for (page = virt_to_page(virtAddr);
             page < virt_to_page(virtAddr + PCI_BUF_SIZE); ++page)
            SetPageReserved(page);

        dmaBuf = virtAddr;
    }

    // Reserve 8 memory areas (128kB wide) for slave images

    for (i = 10; i < 18; i++) {
        virtAddr = pci_alloc_consistent(universeII_dev, PCI_BUF_SIZE,
                                        &image[i].buffer);

        if (virtAddr == NULL) {
            printk("UniverseII: Unable to allocate memory for slave image!\n");
            image[i].buffer = 0;
        } else {
            for (page = virt_to_page(virtAddr);
                 page < virt_to_page(virtAddr + PCI_BUF_SIZE); ++page)
                SetPageReserved(page);

            image[i].slaveBuf = virtAddr;
        }
    }


    // Assign a major number to our driver

    if (register_chrdev(UNI_MAJOR, "universeII", &universeII_fops)) {
        printk("UniverseII: Error getting Major Number %d for driver!\n",
               UNI_MAJOR);
        iounmap(baseaddr);
        return -6;
    }
    else
        printk("UniverseII: Driver successfully loaded.\n");

    // Create entry "/proc/universeII"

    register_proc();
    if(!universeII_file)
        return -ENOMEM;

    dma_in_use = 0;

    // Setup a DMA and MBX timer to timeout 'infinite' transfers or hangups

    init_timer(&DMA_timer);

    for (i = 0; i < 4; i++)
        init_timer(&MBX_timer[i]);

    // Initialize list for DMA command packet structures

    for (i = 0; i < 256; i++) {
        cpLists[i].free = 1;
        cpLists[i].commandPacket = NULL;
    }

    // Initialize wait queues for DMA, VME irq and mailbox handling

    init_waitqueue_head(&dmaWait);
    init_waitqueue_head(&vmeWait);

    for (i = 0; i < 4; i++)
        init_waitqueue_head(&mbxWait[i]);

    // Reset all irq devices

    for (i = 0; i < 7; i++)
        for (j = 0; j < 256; j++)
            irq_device[i][j].ok = 0;

    // Initialize VMEBuserr list

    for (i = 0; i < 32; i++) {
        vmeBerrList[i].valid = 0;
        vmeBerrList[i].merr = 0;
    }

    // Reset all statistic counters

    statistics.reads = 0;
    statistics.writes = 0;
    statistics.ioctls = 0;
    statistics.irqs = 0;
    statistics.berrs = 0;
    statistics.dmaErrors = 0;
    statistics.timeouts = 0;

    //pci_write_config_dword(universeII_dev, PCI_MISC0, pci_misc); // Turn latency off
    //mb();
    pci_read_config_dword(universeII_dev, PCI_MISC0, &pci_misc);
    printk("UniverseII PCI_MISC: %x \n", pci_misc); 

    return 0;
}


module_init(universeII_init);
module_exit(universeII_exit);
