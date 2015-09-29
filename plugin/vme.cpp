#include "vme.h"
#include <thread>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>

using std::stringstream;
using std::string;
using std::thread;
using std::cout;
using std::endl;

#define TASK_START 1
#define TASK_STOP 0
#define TASK_RUN 1
#define TASK_EXIT 0
#define TASK_ERROR 2

#define DMASIZE 72

static uint32_t imgCtrlAddr[18] = {0x100, 0x114, 0x128, 0x13C, 0x1A0, 0x1B4, 0x1C8, 0x1DC, 0x400, 0x200, 0xF00, 0xF14, 0xF28, 0xF3C, 0xF90, 0xFA4, 0xFB8, 0xFCC};

static uint32_t awValue[3] = {A16, A24, A32}; // A64 is not supported by universeII
static uint32_t dwValue[4] = {D8, D16, D32, D64};

vme::vme(const string& n): smBase(n) {    
}

vme::~vme() {
}

int vme::queryInterface(const std::string& funcName, void* para[], void* ret) {
    if(funcName == "getVME") {
        *(VMEBridge**)ret = getVME();
        return 1;
    }
    if(funcName == "getImgCtrl") {
        if(getImgCtrl(*(int*)para[0], *(uint32_t*)ret) == 1)
            return 1;
    }
    if(funcName == "getDevList") {
        *(std::vector<std::string>*)ret = getDevList();
    }
    return 0;
}

int vme::InitializedLOAD(void* argv[]) {
    debugMsg << name << "# " << "InitializedLOAD";
    stMsg->stateOut(debugMsg); 
    //pvme = new VMEBridge;
    return 2;
}

int vme::LoadedUNLD(void* argv[]) {
    debugMsg << name << "# " << "LoadedUNLD";
    stMsg->stateOut(debugMsg);
    //delete pvme;
    return 1;
}

int vme::LoadedCONF(void* argv[]) {
    debugMsg << name << "# " << "LoadedCONF";
    stMsg->stateOut(debugMsg);
    //configVme();
    return 3;
}

int vme::ConfiguredUNCF(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredUNCF";
    stMsg->stateOut(debugMsg);
    //releaseVme();
    return 2;
}

int vme::ConfiguredPREP(void* argv[]) {
    debugMsg << name << "# " << "ConfiguredPREP";
    stMsg->stateOut(debugMsg);
    //if(!prepVme())
        //return -1;
    return 4;
}

int vme::ReadySTOP(void* argv[]) {
    debugMsg << name << "# " << "ReadySTOP";
    stMsg->stateOut(debugMsg);
    //finishVme();
    return 3;
}

int vme::ReadySATR(void* argv[]) {
    debugMsg << name << "# " << "ReadySATR";
    stMsg->stateOut(debugMsg);
    //startVme();
    return 5;
}

int vme::RunningSPTR(void* argv[]) {
    debugMsg << name << "# " << "RunningSPTR";
    stMsg->stateOut(debugMsg);
    //stopVme();
    return 4;
}

int vme::RunningPAUS(void* argv[]) {
    return 6;
}

int vme::PausedSPTR(void* argv[]) {
    debugMsg << name << "# " << "PausedSPTR";
    stMsg->stateOut(debugMsg);
    //stopVme();
    return 4;
}

int vme::PausedRESU(void* argv[]) {
    return 5;
}

int vme::OTFCONF(void* argv[]) {
    return stId;
}

int vme::configVme() {
    pvme->resetDriver();
    int res;
 
    uint32_t PCI_CSR, MISC_CTL, MAST_CTL, LSI0_CTL;
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name << "# " << std::hex << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);
    if(PCI_CSR&0xF9000000)
        pvme->writeUniReg(0x004, 0xF9000007);
    PCI_CSR = pvme->readUniReg(0x004);
    MISC_CTL = pvme->readUniReg(0x404);
    MAST_CTL = pvme->readUniReg(0x400);
    LSI0_CTL = pvme->readUniReg(0x100);
    debugMsg << name << "# " << "UniverseII Register after writing 0xF9000007 to register 0x004:" << endl;
    debugMsg << name << "# " << std::hex << "PCI_CSR: " << PCI_CSR << ", MISC_CTL: " << MISC_CTL << ", MAST_CTL: " << MAST_CTL << ", LSI0_CTL: " << LSI0_CTL;
    stMsg->stateOut(debugMsg);

    if((res=cfgInfo->infoGetUint("config."+name+".dmaNumber", dmaNumber)) != 1) {
        debugMsg << name << "# " << "config."+name+".dmaNumber not found.";
        stMsg->stateOut(debugMsg);
        dmaNumber = 1;
    }
    dmaBase = pvme->requestDMA(dmaNumber);
    pvme->setOption(DMA, BLT_ON);

    // dmaSize is used for adc1785 single board transfer and will be kick out soon
    if((res=cfgInfo->infoGetUint("config."+name+".dmaSize", dmaSize)) != 1) {
        debugMsg << name << "# " << "config."+name+".dmaSize not found.";
        stMsg->stateOut(debugMsg);
        dmaSize = DMASIZE;
    }
    return 1;
}

int vme::releaseVme() {
    pvme->releaseDMA();
    pvme->delCmdPktList(listNumber);
    return 1;
}

int vme::prepVme() {

    string triggerDevice, linkList;
    int res;
    std::vector< std::pair<std::string, smBase*> >::const_iterator iter;

    triDev = NULL;
    if((res=cfgInfo->infoGetString("config."+name+".triggerDevice", triggerDevice)) == 1) {
        for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
            if(iter->first == triggerDevice) {
                triDev = iter->second;
                if(!triDev->queryInterface("getEventTh", NULL, (void*)&eventTh))
                    return 0;
            }
        }
    }

    if((res=cfgInfo->infoGetString("config."+name+".linkList", linkList)) == 1) {
        devStringSplit(linkList);
        buffList.clear();
        buffList.resize(listSize);
        for(int i=0; i<listSize; i++) {
            for(iter=helpList->begin(); iter!=helpList->end(); iter++) {
                if(iter->first == devList[i]) {
                    smBase* pDev = iter->second;
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
    if(listNumber < 0 || listNumber > 255)
        return 0;
    else {
        offsetList.clear();
        offsetList.resize(listSize, 0);
        for(int i=0; i<listSize;i++) {
            offsetList[i] = pvme->addCmdPkt(listNumber, 0, buffList[i], sizeList[i], awList[i], dwList[i]);
        }
    }
    
    devMsgQue = dataPool->getDevMsgQue();
    return 1;
}

int vme::finishVme() {
    return 1;
}

int vme::startVme() {
    runVmeCtrl = TASK_START;
    t0 = new thread(&vme::runVme, this);
    return 1;
}

int vme::stopVme() {
    runVmeCtrl = TASK_STOP;
    //t0->join();
    t0->detach();
    return 1;
}

void vme::runVme() {
    vmeStatus = TASK_RUN;

    //vmeCount = 0;
    //totalVmeSize = 0;
    unsigned int genSize = 0;
    unsigned int sndSize = 0;
    char *tmp = "ABCDEFGH";
    while(1) {

        if(runVmeCtrl == TASK_STOP) {
            break;
        }
        // wait for trigger device
        triDev->queryInterface("run", NULL, NULL);
        
        // test adc1785 single board
        //uintptr_t offset = pvme->DMAread(buff, dmaSize, A32, D32);
        //if(offset < 0) {
            //vmeStatus = TASK_ERROR;
            //break;
        //}
        //unsigned int tranSize = dataPool->devWrite((void*)(dmaBase+offset), dmaSize);
        
        // test 2
        sleep(1);
        unsigned int tranSize = dataPool->devWrite(tmp, 8);
        /*
        pvme->execCmdPktList(listNumber);
        unsigned int tranSize = 0;
        dmaSize = 0;
        for(int i=0; i< listSize; i++) {
            tranSize += dataPool->devWrite((void*)(dmaBase+offsetList[i]), sizeList[i]);
            dmaSize += sizeList[i];
        }
        */
        vmeMsg.signal = 1;
        vmeMsg.size = eventTh;
        int eventSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg), 0);
        if(eventSend < 0) {
            vmeStatus = TASK_ERROR;
            break;
        }

        genSize += dmaSize;
        sndSize += tranSize;

        //vmeCount++;
        //totalVmeSize += dmaSize;
        debugMsg << name << "# " << "vme generate " << dmaSize << " bytes data, pool write " << tranSize << " bytes. total gen " << genSize << ", total send " << sndSize << " bytes";
        stMsg->stateOut(debugMsg);
    }

    if(runVmeCtrl == TASK_STOP || vmeStatus == TASK_ERROR) {
        vmeMsg.signal = 2;
        vmeMsg.size = 0;
        int stopSend = msgsnd(devMsgQue, &vmeMsg, sizeof(vmeMsg), 0);
        if(stopSend < 0) {
            vmeStatus = TASK_ERROR;
        }

        debugMsg << name << "# " << "vme send stop to devMsg and return " << stopSend << endl;
        stMsg->stateOut(debugMsg);

        if(vmeStatus == TASK_RUN)
            vmeStatus = TASK_EXIT;
    }

    debugMsg << name << "# " << "vme stop thread" << vmeStatus << endl;
    stMsg->stateOut(debugMsg);
}

int vme::getImgCtrl(int i, uint32_t& addr) {
    if(i < 0 || i >= 18)
        return 0;
    else {
        addr = imgCtrlAddr[i];
    }
    return 1;
}

std::vector<std::string>& vme::getDevList() {
    return devList;
}

int vme::devStringSplit(const string& dList) {
    stringstream sList(dList);
    string idx, dev, addr, size, aw, dw;
    uint32_t tSize, tAW, tDW;
    debugMsg << name << "# " << "vme take device list" << dList << endl;
    stMsg->stateOut(debugMsg);
    devList.clear();
    sizeList.clear();
    awList.clear();
    dwList.clear();
    listSize = 0;
    while(getline(sList, idx, ';')) {
        listSize++;
        debugMsg << name << "# " << "dev link " << listSize << ", " << idx << endl;
        stMsg->stateOut(debugMsg);
        stringstream sIdx(idx);
        getline(sIdx, dev, ',');
        getline(sIdx, size, ',');
        getline(sIdx, aw, ',');
        getline(sIdx, dw, ',');

        debugMsg << name << "# " << "dev link: " << dev << ", " << size << ", " << aw << ", " << dw <<endl;
        stMsg->stateOut(debugMsg);

        stringstream sSize(size);
        sSize >> tSize;
        
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

        debugMsg << name << "# " << "dev link: " << dev << ", " << tSize << ", " << tAW << ", " << tDW <<endl;
        stMsg->stateOut(debugMsg);
        
        devList.push_back(dev);
        sizeList.push_back(tSize);
        awList.push_back(tAW);
        awList.push_back(tAW);
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
