#include <iostream>
#include <iomanip>
#include "vmelib.h"

using namespace std;

#define VME_BASE 0xCC000000

int main() {
    int image, op, i, j;
    unsigned int dummy32, lsi0_ctl, adc_base, dma_base, fop, offset, *ptr;
    unsigned short int dummy16, adc_bitset1, adc_bitset2, adc_ctrl, adc_status ;
    VMEBridge vme;
    adc_base = VME_BASE;

    //vme.vmeSysReset();
    //cin >> dummy32;

    dma_base = vme.requestDMA();
    if (!dma_base) {
        cerr << "Can't allocate DMA !\n";
        return 0;
    }
    vme.setOption(DMA, BLT_ON);

    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

    if(vme.readUniReg(0x004)&0xF9000000) {
        vme.writeUniReg(0x004, 0xF9000007);
        //cin >> dummy32;
        cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;
    }

    image = vme.getImage(adc_base, 0x10000, A32, D16, MASTER);
    //image = vme.getImage(0xA4000000, 0x10000, A32, D32, SLAVE);
    if (image < 0) {
        cerr << "Can't allocate master image !\n";
        return 0;
    }

    vme.setupIrq(image, 7, 0, 0, 0, 0, 0);

    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

    //if((vmectl&0x00080000)>>19 == 1)
    //vme.writeUniReg(0x400, vmectl&0xFFFBFFFF);
    //cin >> dummy32;

    vme.rw(image, adc_base+0x1006, &adc_bitset1);
    vme.rw(image, adc_base+0x1032, &adc_bitset2);
    vme.rw(image, adc_base+0x1010, &adc_ctrl);
    vme.rw(image, adc_base+0x100E, &adc_status);
    cout << "ADC: \n" << hex << "BitSet1: 0x" << adc_bitset1 << ", BitSet2: 0x" << adc_bitset2 << ", Ctrl: 0x" << adc_ctrl << ", Status: 0x" << adc_status << dec << endl;

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

    // Acquisition Setup
    vme.ww(image, adc_base+0x1032, (short unsigned int)0x4000);
    vme.ww(image, adc_base+0x1034, (short unsigned int)0x4000);
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
    vme.ww(image, adc_base+0x1032, (short unsigned int)0x4000);


    lsi0_ctl = vme.readUniReg(0x100);
    lsi0_ctl &= 0xFF3FFFFF;
    lsi0_ctl |= D32;
    vme.writeUniReg(0x100, lsi0_ctl);

    /*
    cout << "offset for adc buff: ";
    cin >> hex >> offset;                                                                                                                                                                 
    cin.sync(); 
    vme.rl(image, adc_base+offset, &dummy32);
    cout << hex << "ADC: read 0x" << offset << ", D32, value 0x" << dummy32 << dec << "!\n";


    cout << "ADC: output buffer" << endl;
    for(offset = 0x0; offset <= 0x07FC; offset += 4) {
        if(offset%32 == 0)
            cout << hex << "Buffer " << setw(2) << setfill('0') << offset/32 << ":   ";
        vme.rl(image, adc_base+offset, &dummy32);
        cout << hex << "0x" << setw(8) << setfill('0') << dummy32 << " ";
        if(offset%32 == 28)
            cout << endl;
    }
    
    dummy16 = 0x4000;
    vme.ww(image, adc_base+0x1034, &dummy16);
    vme.rw(image, adc_base+0x1032, &adc_bitset2);
    cout << "ADC: \n" << hex << "BitSet1: 0x" << adc_bitset1 << ", BitSet2: 0x" << adc_bitset2 << ", Ctrl: 0x" << adc_ctrl << ", Status: 0x" << adc_status << dec << endl;
    */
    
    //j = 0;
    //while(j <= 2) {
    //j++;
    //cin >> dummy32;
    vme.waitIrq(7,0);

    //vme.generateVmeIrq(1, 0);
    //vme.generateVmeIrq(2, 0);
    //vme.generateVmeIrq(3, 0);
    //vme.generateVmeIrq(4, 0);
    //vme.generateVmeIrq(5, 0);
    //vme.generateVmeIrq(6, 0);
    //vme.generateVmeIrq(7, 0);
    offset = vme.DMAread(adc_base, 2048, A32, D32);
    if (offset < 0) {
        vme.releaseDMA();
        return 0;
    }
    ptr = (unsigned int *) (dma_base + offset);
    for(i = 0x0; i <= 2048/4; i++) {                                                                         
        if(i%8 == 0)                                                                                                     
            cout << hex << "Buffer " << setw(2) << setfill('0') << i/7 << dec << ":   ";
        cout << hex << "0x" << setw(8) << setfill('0') << *ptr++ << dec << " ";
        if(i%8 == 7)
            cout << endl;                                                                                                      
    }
    //}
 
    vme.freeIrq(image, 7, 0);   
    vme.releaseDMA();

    cout << "operation finish and release adc device" << endl;
    vme.releaseImage(image);
    //cin >> dummy32;
    cout << hex << "UniverseII Register: \n" << "PCI_CSR: 0x" << vme.readUniReg(0x004) << ", MISC_CTL: 0x" << vme.readUniReg(0x404) << ", MAST_CTL: 0x" << vme.readUniReg(0x400) << ", LSI0_CTL: 0x" << vme.readUniReg(0x100) << dec << endl;

}
