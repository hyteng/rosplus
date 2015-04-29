#include <iostream>
#include <iomanip>
#include "vmelib.h"

using namespace std;

// ADC板卡上的vme基地址，通过跳线设置为0xCC000000
#define VME_BASE 0xCC000000

int main() {
    int image, op, i, j;
    unsigned int dummy32, lsi0_ctl, adc_base, dma_base, fop, offset, *ptr;
    unsigned short int dummy16, adc_bitset1, adc_bitset2, adc_ctrl, adc_status ;
    VMEBridge vme;
    adc_base = VME_BASE;

    // 在VME系统上申请DMA，驱动自动分配128K长度的DMA缓冲区，默认均匀划分为1段子缓冲区。缓冲区被整体映射到用户空间，返回数值为用户空间的buff地址，这里保存于dma_base。
    dma_base = vme.requestDMA();
    if (!dma_base) {
        cerr << "Can't allocate DMA !\n";
        return 0;
    }
    // 给VME系统的DMA通道设置一些参数，主要是设置数据传输模式，DMA操作采用块传输（block transfer)
    vme.setOption(DMA, BLT_ON);

    // 检查universeII的寄存器状态，比如PCI是否允许latency等，具体数值暂时可以不管
    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;
    if(vme.readUniReg(0x004)&0xF9000000) {
        vme.writeUniReg(0x004, 0xF9000007);
        //cin >> dummy32;
        cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;
    }

    // 为ADC板卡分配镜像，universeII芯片支持8个master类型的镜像和8个slave类型的镜像。master镜像将vme的一段地址映射到pci总线上，slave镜像则相反将现有的PCI地址段映射到VME总线上。在分配镜像之后所有的设备操作均统一使用vme绝对地址。ADC板卡在vme总线上基地址为adc_base（0xCC000000），长度为64K（0x10000）, vme的地址模式为32位地址模式（A32），采用16位数据宽度（D16）
    image = vme.getImage(adc_base, 0x10000, A32, D16, MASTER);
    //image = vme.getImage(0xA4000000, 0x10000, A32, D32, SLAVE);
    if (image < 0) {
        cerr << "Can't allocate master image !\n";
        return 0;
    }

    // 为ADC板卡分配Irq，中断级别IrqLevel为7, 中断状态字StatusID为0。这个设置需要和ADC板卡上的中断设置一致，即稍后在对ADC板卡进行配置时，要将同样的IrqLevel和StatusID设置写到板卡对应的中断设置寄存器里。当板卡在工作中达到预期的条件后，会在VME中断总线上发出中断申请，使用的申请信息既为板卡中断寄存器里设置的IrqLevel和StatusID。而只有VME总线上分配和注册了对应的中断申请，板卡发出的中断才能被vme总线处理（传递到PCI侧并由驱动响应）。暂时ADC板卡没有中断使能设置，并且该板卡能在中断获得响应后自动清除中断请求，因此后面四个中断寄存器参数不用设置。
    vme.setupIrq(image, 7, 0, 0, 0, 0, 0);

    // 检查vme系统的寄存器状态
    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

    // 读取ADC板卡的状态信息，参考ADC手册中各个寄存器定义
    vme.rw(image, adc_base+0x1006, &adc_bitset1);
    vme.rw(image, adc_base+0x1032, &adc_bitset2);
    vme.rw(image, adc_base+0x1010, &adc_ctrl);
    vme.rw(image, adc_base+0x100E, &adc_status);
    cout << "ADC: \n" << hex << "BitSet1: 0x" << adc_bitset1 << ", BitSet2: 0x" << adc_bitset2 << ", Ctrl: 0x" << adc_ctrl << ", Status: 0x" << adc_status << dec << endl;

    // 手工设置ADC的工作参数，主要设置一下中断，参考ADC手册。后面的随机测试模式有额外的代码来自动设置。
    dummy16 = 0;
    cout << "manual operation" << endl;
    cout << "offset for adc register: ";
    cin >> hex >> offset;
    cin.sync();
    cout << "0 for quit; positive value for write; nagative value for read; what is your choice: ";
    cin >> hex >> op;
    cin.sync();
    do {
        if(op < 0 && (offset <= 0x10BC || (offset >= 0x8000 && offset <= 0xFFFE)) && offset >= 0x0) {
            dummy16 = 0x0;
            vme.rw(image, adc_base+offset, &dummy16);
            cout << hex << "ADC: read 0x" << offset << ", value 0x" << dummy16 << dec << "!\n";
            op = 0;
        }
        if(op > 0 && offset <= 0x10BC && offset >= 0x0) {
            cout << hex << "word for adc register 0x" << offset << " : " << dec;
            cin >> hex >> dummy16;
            cin.sync();
            cout <<hex << "write 0x" << dummy16 << endl;
            vme.ww(image, adc_base+offset, &dummy16);
            op = 0;
        }
        cout << "manual operation" << endl;
        cout << "offset for adc register: ";
        cin >> hex >> offset;
        cin.sync();
        cout << "0 for quit; positive value for write; nagative value for read; what is your choice: ";
        cin >> hex >> op;
        cin.sync();
    } while(op != 0);

    // 设置ADC进入随机测试模式，参考ADC手册的5.7.2章节部分。
    // Acquisition Setup
    vme.ww(image, adc_base+0x1032, (short unsigned int)0x4000);
    vme.ww(image, adc_base+0x1034, (short unsigned int)0x4000); // 注意清除Bit Set 2 Register（adc_base+0x1032）的各个Bit位是通过对Bit Clear 2 Register（adc_base+0x1034）的对应位置1来操作的。
    // 将16个字的数据写入内部缓存，然后下面会等待中断。测试中给ADC发送一个gate input信号，ADC就会以FIFO形式将数据写到output buffer里，buffer中的event数目一旦达到了预设的阈值，ADC板卡将发出VME中断信号，既IrqLevel=7, StatusID=0的中断请求。
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0000); 
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x4000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0400);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x1000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x5000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0100);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0500);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x2000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x6000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0200);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0600);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x3000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x7000);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0300);
    vme.ww(image, adc_base+0x103E, (short unsigned int)0x0700);
    // 写入内部缓存完毕，已经可以恢复工作状态
    vme.ww(image, adc_base+0x1032, (short unsigned int)0x4000);

    // 设置板卡中使用了16位数据传输，切换到数据读取时恢复32位数据传输。这里直接写了universeII的对应寄存器，使用相对地址偏移。
    lsi0_ctl = vme.readUniReg(0x100);
    lsi0_ctl &= 0xFF3FFFFF;
    lsi0_ctl |= D32;
    vme.writeUniReg(0x100, lsi0_ctl);

    // 等待ADC产生中断。这里需要手工触发信号发生器，给ADC板卡的commom input gate输入一个NIM标准的单触发信号（信号1对应>=5V还是10V的数值）。ADC接收到gate信号就会以FIFO的顺序把内部缓存里写入的数据放到output buffer里，然后当output buffer里的event数目达到阈值（=1）后，ADC板卡发出vme中断信号，waitIrq被唤醒返回。
    vme.waitIrq(7,0);

    //vme.generateVmeIrq(1, 0);
    //vme.generateVmeIrq(2, 0);
    //vme.generateVmeIrq(3, 0);
    //vme.generateVmeIrq(4, 0);
    //vme.generateVmeIrq(5, 0);
    //vme.generateVmeIrq(6, 0);
    //vme.generateVmeIrq(7, 0);
    
    // waitIrq被唤醒，说明ADC的output buffer里已经有了足够的event数据，于是启动DMA传输，将output buffer里的数据全部读出到DMA缓存里。这里一次性读了2KB的数据，等于将环形的output buffer全部读取重置一次。返回的是DMA缓存中的数据相对偏移地址。
    offset = vme.DMAread(adc_base, 2048, A32, D32);
    if (offset < 0) {
        vme.releaseDMA();
        return 0;
    }
    // 获得数据，打印信息检查数据是否正确（这里会发现有大小端差异）。由于dma缓存已经被映射到了用户空间，数据地址也为用户空间下的数值，大小为dma的缓存映射基地址+数据在dma缓存中的相对偏移地址
    ptr = (unsigned int *) (dma_base + offset);
    for(i = 0x0; i <= 2048/4; i++) {                                                                         
        if(i%8 == 0)                                                                                                     
            cout << hex << "Buffer " << setw(2) << setfill('0') << i/7 << dec << ":   ";
        cout << hex << "0x" << setw(8) << setfill('0') << *ptr++ << dec << " ";
        if(i%8 == 7)
            cout << endl;                                                                                                      
    }

    // 测试完毕，释放中断和DMA
    vme.freeIrq(image, 7, 0);   
    vme.releaseDMA();

    // 释放ADC板卡的vme镜像
    cout << "operation finish and release adc device" << endl;
    vme.releaseImage(image);
    
    // 检查universeII的寄存器，测试用可不管
    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;
}
