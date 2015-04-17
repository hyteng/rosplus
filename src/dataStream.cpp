#include <dataStream.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

using std::string;

#define RINGSIZE 10000000

/*
struct streamMsg {
    long signal;
    unsigned int size;
    unsigned int count;
};
*/
dataStream::dataStream() {
    dmaRing = new ringBuffer();
    dmaRingSize = RINGSIZE;
    dmaCount = 0;
    totalDmaSize = 0;

    if(dmaRing->isVaild())
        dmaRing->release();
    dmaRing->create(dmaRingSize);

    key_t dmaKey;
    if((dmaKey=ftok(".", 'dma')) == -1) {
        return;
    }
    if((dmaMsgQue=msgget(dmaKey, 0)) >= 0) {
        if((msgctl(dmaMsgQue, IPC_RMID, NULL)) < 0){
            return;
        }
    }
    if((dmaMsgQue=msgget(dmaKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return;
    }

    netRing = new ringBuffer();
    netRingSize = RINGSIZE;
    netCount = 0;
    totalNetSize = 0;

    if(netRing->isVaild())
        netRing->release();
    netRing->create(netRingSize);

    key_t netKey;
    if((netKey=ftok(".", 'net')) == -1) {
        return;
    }
    if((netMsgQue=msgget(netKey, 0)) >= 0) {
        if((msgctl(netMsgQue, IPC_RMID, NULL)) < 0){
            return;
        }
    }
    if((netMsgQue=msgget(netKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return;
    }

}

dataStream::~dataStream() {
    dmaRing->release();
    netRing->release();
    delete dmaRing;
    delete netRing;
}

unsigned int dataStream::dmaSend(const void* addr, const unsigned int& nBytes) {
    std::unique_lock<std::mutex> lock(dmaMutex);

    unsigned int sendSize = dmaRing->dmaWrite(addr, nBytes);
    dmaCount++;
    totalDmaSize += sendSize;

    if(dmaCount >= 5) {
        dmaMsg.signal = 1;
        dmaMsg.size = totalDmaSize;
        dmaMsg.count = dmaCount;
        if((msgsnd(dmaMsgQue, &dmaMsg, sizeof(dmaMsg), 0)) < 0) {
            return 0;
        }
        totalDmaSize = 0;
        dmaCount = 0;
    }
    return sendSize;
}

int dataStream::dmaSetSnap() {
    std::unique_lock<std::mutex> lock(dmaMutex);
    return dmaRing->setSnapStatus();
}

int dataStream::dmaAddSnapRead() {
    return dmaRing->addSnapRead();
}

int dataStream::dmaRmSnapRead() {
    return dmaRing->rmSnapRead();
}

void* dataStream::dmaGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes) {
    //std::unique_lock<std::mutex> lock(dmaMutex);
    return dmaRing->getSnapPtr(nBias, nBytes);
}
