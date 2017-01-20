#include "vme.h"
#include <thread>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>

using std::stringstream;
using std::string;
using std::thread;
using std::cout;
using std::endl;
using std::hex;
using std::dec;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2

#define DMASIZE 72

//uint32_t tmp[24] = {0xfa00400, 0xf8004010, 0xf8104050, 0xf8024110, 0xf8124550, 0xfc000001, 0xfa0000400, 0xf8004010, 0xf8104050, 0xf8024110, 0xf8124550, 0xfc000002, 0xfa0000400, 0xf8004010, 0xf8104050, 0xf8024110, 0xf8124550, 0xfc000003, 0xfa0000400, 0xf8004010, 0xf8104050, 0xf8024110, 0xf8124550, 0xfc000004};
//static uint32_t tmp[24] = {0x000400fa, 0x104000f8, 0x504010f8, 0x104102f8, 0x504512f8, 0x010000fc, 0x000400fa, 0x104000f8, 0x504010f8, 0x104102f8, 0x504512f8, 0x020000fc, 0x000400fa, 0x104000f8, 0x504010f8, 0x104102f8, 0x504512f8, 0x030000fc, 0x000400fa, 0x104000f8, 0x504010f8, 0x104102f8, 0x504512f8, 0x040000fc};

static uint32_t imgCtrlAddr[18] = {0x100, 0x114, 0x128, 0x13C, 0x1A0, 0x1B4, 0x1C8, 0x1DC, 0x400, 0x200, 0xF00, 0xF14, 0xF28, 0xF3C, 0xF90, 0xFA4, 0xFB8, 0xFCC};

static uint32_t awValue[3] = {A16, A24, A32}; // A64 is not supported by universeII
static uint32_t dwValue[4] = {D8, D16, D32, D64};

vme::vme(const string& n): smBase(n) {
}

vme::~vme() {
}

int vme::queryInterface(const string& funcName, void* para[], void* ret) {
    if(funcName == "getVME") {
        *(VMEBridge**)ret = getVME();
        return 1;
    }
    if(funcName == "getImgCtrl") {
        if(getImgCtrl(*(int*)para[0], *(uint32_t*)ret) == 1)
            return 1;
    }
    if(funcName == "getNameList") {
        *(std::vector<string>*)ret = getNameList();
        return 1;
    }
    return 0;
}

int vme::InitializedLOAD(string& ret, void* para[]) {
    //debugMsg << name << "# " << "InitializedLOAD";
    //stMsg->stateOut(debugMsg); 
    pvme = new VMEBridge;
    return smBase::InitializedLOAD(ret, para);
}

int vme::LoadedUNLD(string& ret, void* para[]) {
    //debugMsg << name << "# " << "LoadedUNLD";
    //stMsg->stateOut(debugMsg);
    delete pvme;
    pvme = NULL;
    return smBase::LoadedUNLD(ret, para);
}

int vme::LoadedCONF(string& ret, void* para[]) {
    //debugMsg << name << "# " << "LoadedCONF";
    //stMsg->stateOut(debugMsg);
    configVme();
    return smBase::LoadedCONF(ret, para);
}

int vme::ConfiguredUNCF(string& ret, void* para[]) {
    //debugMsg << name << "# " << "ConfiguredUNCF";
    //stMsg->stateOut(debugMsg);
    releaseVme();
    return smBase::ConfiguredUNCF(ret, para);
}

int vme::ConfiguredPREP(string& ret, void* para[]) {
    //debugMsg << name << "# " << "ConfiguredPREP";
    //stMsg->stateOut(debugMsg);
    if(!prepVme())
        return -1;
    return smBase::ConfiguredPREP(ret, para);
}

int vme::ReadySTOP(string& ret, void* para[]) {
    //debugMsg << name << "# " << "ReadySTOP";
    //stMsg->stateOut(debugMsg);
    finishVme();
    return smBase::ReadySTOP(ret, para);
}

int vme::ReadySATR(string& ret, void* para[]) {
    //debugMsg << name << "# " << "ReadySATR";
    //stMsg->stateOut(debugMsg);
    startVme();
    return smBase::ReadySATR(ret, para);
}

int vme::RunningSPTR(string& ret, void* para[]) {
    //debugMsg << name << "# " << "RunningSPTR";
    //stMsg->stateOut(debugMsg);
    stopVme();
    return smBase::RunningSPTR(ret, para);
}

int vme::PausedSPTR(string& ret, void* para[]) {
    //debugMsg << name << "# " << "PausedSPTR";
    //stMsg->stateOut(debugMsg);
    stopVme();
    return smBase::PausedSPTR(ret, para);
}

int vme::configVme() {
    debugMsg << hex;

    pvme->resetDriver();
    int res;
    uint32_t PCI_CSR, MISC_CTL, MAST_CTL, LSI0_CTL;
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    //debugMsg << name << "# " << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    //stMsg->stateOut(debugMsg);
    if(PCI_CSR&0xF9000000)
        pvme->writeUniReg(0x004, 0xF9000007);
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    //debugMsg << name << "# " << "UniverseII Register after writing 0xF9000007 to register 0x004:" << endl;
    //debugMsg << name << "# " << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    //stMsg->stateOut(debugMsg);

    if((res=cfgInfo->infoGetUint("config."+name+".dmaNumber", dmaNumber)) != 1) {
        debugMsg << name << "# " << "config."+name+".dmaNumber not found.";
        stMsg->stateOut(debugMsg);
        dmaNumber = 1;
    }
    dmaBase = pvme->requestDMA(dmaNumber);
    pvme->setOption(DMA, BLT_ON);
    /*
    // dmaSize is used for single board transfer and will be obsolete soon
    if((res=cfgInfo->infoGetUint("config."+name+".dmaSize", dmaSize)) != 1) {
    debugMsg << name << "# " << "config."+name+".dmaSize not found.";
    stMsg->stateOut(debugMsg);
    dmaSize = DMASIZE;
    }
    */

    vmeStatus = TASK_EXIT;
    joinable = false;

    debugMsg << dec;
    return 1;
}

int vme::releaseVme() {
    pvme->releaseDMA();
    return 1;
}

int vme::prepVme() {
    debugMsg << hex;

    string triggerDevice, linkList;
    int res;
    std::vector< std::pair<string, smBase*> >::const_iterator iter;

    triDev = NULL;
    if((res=cfgInfo->infoGetString("config."+name+".triggerDevice", triggerDevice)) == 1) {
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == triggerDevice) {
                triDev = iter->second;
                if(!triDev->queryInterface("getEventTh", NULL, (void*)&eventTh))
                    return 0;
                //debugMsg << name << "# " << "helper get " << triggerDevice << " and get " << eventTh << " eventTh";
                //stMsg->stateOut(debugMsg);
            }
        }
    }

    if((res=cfgInfo->infoGetString("config."+name+".linkList", linkList)) == 1) {
        devStringSplit(linkList);
        buffList.clear();
        devList.clear();
        buffList.resize(listSize);
        devList.reserve(listSize);
        for(unsigned int i=0; i<listSize; i++) {
            for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
                if(iter->first == nameList[i]) {
                    smBase* pDev = iter->second;
                    devList.push_back(pDev);
                    if(!pDev->queryInterface("getBuffAddr", NULL, (void*)&buffList[i]))
                        return 0;
                    if(!pDev->queryInterface("getTranSize", NULL, (void*)&sizeList[i])) // if the dev change transfer size, sizeList will be updated
                        return 0;
                }
            }
        }
    }

    if(triDev == NULL || listSize == 0) {
        debugMsg << name << "# " << "helper or link List not found.";
        stMsg->stateOut(debugMsg);
        return 0;
    }

    listNumber = pvme->newCmdPktList();
    //debugMsg << name << "# " << "add cmd packet list number " << listNumber;
    //stMsg->stateOut(debugMsg);
    if(listNumber < 0 || listNumber > 255)
        return 0;
    else {
        offsetList.clear();
        offsetList.resize(listSize, 0);
        for(unsigned int i=0; i<listSize;i++) {
            offsetList[i] = pvme->addCmdPkt(listNumber, 0, buffList[i], sizeList[i]*cbltList[i], awList[i], dwList[i]);
            //debugMsg << name << "# " << "add cmd packet list, idx " << i << ", buff addr " << buffList[i] << ", tranSize " << sizeList[i]*cbltList[i] << ", aw " << awList[i] << ", dw " << dwList[i] << ", offset " << offsetList[i];
            //stMsg->stateOut(debugMsg);
        }
    }

    debugMsg << dec;

    devMsgQue = dataPool->getDevMsgQue();
    dataPool->devClear();

    return 1;
}

int vme::finishVme() {
    pvme->delCmdPktList(listNumber);
    return 1;
}

int vme::startVme() {
    runVmeCtrl = TASK_START;
    //debugMsg << name << "# vmeStatus: " << vmeStatus;
    //stMsg->stateOut(debugMsg);
    if(vmeStatus!=TASK_RUN) {
        //debugMsg << name << "# new runVme thread.";
        //stMsg->stateOut(debugMsg);
        t0 = new thread(&vme::runVme, this);
        joinable=true;
    }
    return 1;
}

int vme::stopVme() {
    if(joinable==true) {
        t0->detach();
        joinable=false;
    }
    //sleep(1);
    runVmeCtrl = TASK_STOP;
    usleep(100000);
    //t0->join();
    // first stop the reading, then flush data?
    int res;
    triDev->queryInterface("flushData", NULL, &res);
    // handle zombie adc and suspended thread
    //debugMsg << name << "# vmeStatus: " << vmeStatus;
    //stMsg->stateOut(debugMsg);
    if(vmeStatus==TASK_RUN) {
        vmeMsg.signal = 3;
        vmeMsg.size = 0;
        msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg)-sizeof(long), 0);
    }

    return 1;
}

void vme::runVme() {
    vmeStatus = TASK_RUN;

    unsigned long genSize = 0;
    unsigned long sndSize = 0;
    while(runVmeCtrl == TASK_START) {
        int res;
        // wait for trigger device
        triDev->queryInterface("waitTrigger", NULL, &res);
        /*
        // for single board transfer, will be obsolete soon
        dmaSize = sizeList[0];
        uintptr_t offset = pvme->DMAread(buffList[0], dmaSize, A32, D32);
        if(offset < 0) {
        vmeStatus = TASK_ERROR;
        break;
        }
        tranSize = dataPool->devWrite((void*)(dmaBase+offset), dmaSize);
        */
        // test 
        //usleep(1000);
        //dmaSize = 24*sizeof(uint32_t);
        //tranSize = dataPool->devWrite((void*)&tmp[0], dmaSize, 1);

        res = pvme->execCmdPktList(listNumber);
        if(res) {
            vmeStatus = TASK_ERROR;
            break;
        }

        tranSize = 0;
        dmaSize = 0;
        unsigned int fillSize;
        for(unsigned int i=0; i<listSize; i++) {
            //cout << "vme data: " << endl;
            //uint32_t* ptr = (uint32_t*)(dmaBase+offsetList[i]);
            //for(unsigned int j=0; j<sizeList[i]*cbltList[i]/4;j++)
            //    cout << hex << "0x" << setw(8) << setfill('0') << *ptr++ << ", ";
            //cout << endl;

            for(unsigned int j=0; j<cbltList[i];j++) {
                fillSize = dataPool->devWrite((void*)(dmaBase+offsetList[i]+j*sizeList[i]), sizeList[i], 1);
                if(fillSize != sizeList[i]) {
                    vmeStatus = TASK_ERROR;
                }
                tranSize += fillSize;
                dmaSize += sizeList[i];
            }
            devList[i]->queryInterface("afterTransfer", NULL, &res);
        }

        if(runVmeCtrl == TASK_START)
            triDev->queryInterface("ackTrigger", NULL, &res);

        genSize += dmaSize;
        sndSize += tranSize;
        //debugMsg << name << "# " << "vme generate " << dmaSize << " bytes data, pool write " << tranSize << " bytes. total gen " << genSize << ", total send " << sndSize << " bytes";
        //stMsg->stateOut(debugMsg);

        vmeMsg.signal = 2;
        vmeMsg.size = eventTh;
        int eventSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg)-sizeof(long), 0);
        if(eventSend < 0 || vmeStatus == TASK_ERROR) {
            vmeStatus = TASK_ERROR;
            break;
        }
    }

    vmeMsg.signal = 3;
    vmeMsg.size = 0;
    int stopSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg)-sizeof(long), 0);
    if(stopSend < 0)
        vmeStatus = TASK_ERROR;

    if(vmeStatus == TASK_RUN)
        vmeStatus = TASK_EXIT;

    //debugMsg << name << "# " << "vme stop thread " << vmeStatus << endl;
    //stMsg->stateOut(debugMsg);

    return;
}

int vme::getImgCtrl(int i, uint32_t& addr) {
    if(i < 0 || i >= 18)
        return 0;
    else
        addr = imgCtrlAddr[i];
    return 1;
}

std::vector<string>& vme::getNameList() {
    return nameList;
}

unsigned int vme::devStringSplit(const string& dList) {
    stringstream sList(dList);
    string idx, dev, size, cblt, aw, dw;
    uint32_t tSize, tCBLT, tAW, tDW;
    //debugMsg << name << "# " << "vme take device list" << dList << endl;
    //stMsg->stateOut(debugMsg);
    nameList.clear();
    sizeList.clear();
    cbltList.clear();
    awList.clear();
    dwList.clear();
    listSize = 0;
    while(getline(sList, idx, ';')) {
        listSize++;
        //debugMsg << name << "# " << "dev link " << listSize << ", " << idx << endl;
        //stMsg->stateOut(debugMsg);
        stringstream sIdx(idx);
        getline(sIdx, dev, ',');
        getline(sIdx, size, ',');
        getline(sIdx, cblt, ',');
        getline(sIdx, aw, ',');
        getline(sIdx, dw, ',');

        //debugMsg << name << "# " << "dev link: " << dev << ", " << cblt << ", " << aw << ", " << dw <<endl;
        //stMsg->stateOut(debugMsg);

        //tSize = 0; // tSize is set to 0 and will be change by getTransferSize() calling
        stringstream sSize(size);
        sSize >> tSize;

        stringstream sCBLT(cblt);
        sCBLT >> tCBLT;

        if(aw == "A16")
            tAW = awValue[0];
        else if(aw == "A24")
            tAW = awValue[1];
        else if(aw == "A32")
            tAW = awValue[2];
        else
            tAW = awValue[2]; // default

        if(dw == "D8")
            tDW = dwValue[0];
        else if(dw == "D16")
            tDW = dwValue[1];
        else if(dw == "D32")
            tDW = dwValue[2];
        else if(dw == "D64")
            tDW = dwValue[3];
        else
            tDW = dwValue[2]; // default

        //debugMsg << name << "# " << "dev link: " << dev << ", " << tSize << ", " << tCBLT << ", " << tAW << ", " << tDW <<endl;
        //stMsg->stateOut(debugMsg);

        nameList.push_back(dev);
        sizeList.push_back(tSize);
        cbltList.push_back(tCBLT);
        awList.push_back(tAW);
        dwList.push_back(tDW);
    }
    return listSize;
}

extern "C" smBase* create(const string& n) {
    smBase* pModule = new vme(n);
    return pModule;
}

extern "C" void destroy(smBase* pModule) {
    delete pModule;
}
