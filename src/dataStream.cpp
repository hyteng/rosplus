#include <dataStream.h>

#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/errno.h>

extern int errno;
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
}

void dataStream::init() {
    devRing = new ringBuffer();
    devRingSize = RINGSIZE;
    devCount = 0;
    totalDevSize = 0;

    if(devRing->isVaild())
        devRing->release();
    devRing->create(devRingSize);

    if((devKey=ftok("./ipc", 0)) == -1) {
        return;
    }
    if((devMsgQue=msgget(devKey, 0)) >= 0) {
        if((msgctl(devMsgQue, IPC_RMID, NULL)) < 0) {
            return;
        }
    }
    if((devMsgQue=msgget(devKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return;
    }

    netRing = new ringBuffer();
    netRingSize = RINGSIZE;
    netCount = 0;
    totalNetSize = 0;

    if(netRing->isVaild())
        netRing->release();
    netRing->create(netRingSize);

    if((netKey=ftok("./ipc", 1)) == -1) {
        return;
    }
    if((netMsgQue=msgget(netKey, 0)) >= 0) {
        if((msgctl(netMsgQue, IPC_RMID, NULL)) < 0) {
            return;
        }
    }
    if((netMsgQue=msgget(netKey, IPC_CREAT|IPC_EXCL|0666)) == -1) {
        return;
    }
}

dataStream::~dataStream() {
    devRing->release();
    netRing->release();
    delete devRing;
    delete netRing;
}

int dataStream::getDevMsgQue() {
    return devMsgQue;
}

int dataStream::getNetMsgQue() {
    return netMsgQue;
}

unsigned int dataStream::devWrite(const void* addr, const unsigned int& nBytes) {
    std::unique_lock<std::mutex> lock(devMutex);

    unsigned int sendSize = devRing->dmaWrite(addr, nBytes);
    //devCount++;
    //totalDevSize += sendSize;

    //if(devCount >= 5) {
        devMsg.signal = 1;
        devMsg.size = sendSize;//totalDevSize;
        devMsg.count = 1;//devCount;
        int res;
        if((res=msgsnd(devMsgQue, &devMsg, sizeof(devMsg)-sizeof(long), 0)) < 0) {
            std::cout << "error " << errno << ", " << EACCES << ", " << EAGAIN << ", " << EFAULT << ", " << EIDRM << ", " << EINTR << ", " << EINVAL << ", " << ENOMEM << std::endl;
            return 0;
        }
        //devCount = 0;
        //totalDevSize = 0;
    //}
    return sendSize;
}

int dataStream::devSetSnap() {
    std::unique_lock<std::mutex> lock(devMutex);
    return devRing->setSnapStatus();
}

int dataStream::devAddSnapRead() {
    return devRing->addSnapRead();
}

int dataStream::devRmSnapRead() {
    return devRing->rmSnapRead();
}

void* dataStream::devGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes) {
    //std::unique_lock<std::mutex> lock(devMutex);
    return devRing->getSnapPtr(nBias, nBytes);
}

unsigned int dataStream::devPopSnap(const unsigned int& nBytes) {
    return devRing->popSnapRead(nBytes);
}

unsigned int dataStream::netWrite(const void* addr, const unsigned int& nBytes) {
    std::unique_lock<std::mutex> lock(netMutex);

    unsigned int sendSize = netRing->dmaWrite(addr, nBytes);
    //netCount++;
    //totalNetSize += sendSize;

    //if(netCount >= 5) {
        netMsg.signal = 1;
        netMsg.size = sendSize;//totalNetSize;
        netMsg.count = 1;//netCount;
        if((msgsnd(netMsgQue, &netMsg, sizeof(netMsg)-sizeof(long), 0)) < 0) {
            return 0;
        }
        //netCount = 0;
        //totalNetSize = 0;
    //}
    return sendSize;
}

int dataStream::netSetSnap() {
    std::unique_lock<std::mutex> lock(netMutex);
    return netRing->setSnapStatus();
}

int dataStream::netAddSnapRead() {
    return netRing->addSnapRead();
}

int dataStream::netRmSnapRead() {
    return netRing->rmSnapRead();
}

void* dataStream::netGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes) {
    //std::unique_lock<std::mutex> lock(netMutex);
    return netRing->getSnapPtr(nBias, nBytes);
}

unsigned int dataStream::netPopSnap(const unsigned int& nBytes) {
    return netRing->popSnapRead(nBytes);
}
