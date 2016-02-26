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
    totalDevSize = 0;

    if(devRing->isVaild())
        devRing->release();
    devRing->create(devRingSize);

    if((devKey=ftok(".", 0)) == -1) {
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
    totalNetSize = 0;

    if(netRing->isVaild())
        netRing->release();
    netRing->create(netRingSize);

    if((netKey=ftok(".", 1)) == -1) {
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

int dataStream::devClear() {
    std::unique_lock<std::mutex> lock(devMutex);
    return devRing->clear();
}

unsigned int dataStream::devWrite(const void* addr, const unsigned int& nBytes, int sendMsg) {
    std::unique_lock<std::mutex> lock(devMutex);

    unsigned int sendSize = devRing->dmaWrite(addr, nBytes);
    if(sendSize == 0)
        return 0;

    totalDevSize += sendSize;
    if(sendMsg == 1) {
        devMsg.signal = 1;
        devMsg.size = sendSize;
        devMsg.count = 1;
        if(msgsnd(devMsgQue, &devMsg, sizeof(devMsg)-sizeof(long), 0) < 0) {
            //std::cout << "error " << errno << ", " << EACCES << ", " << EAGAIN << ", " << EFAULT << ", " << EIDRM << ", " << EINTR << ", " << EINVAL << ", " << ENOMEM << std::endl;
            return 0;
        }
    }
    return sendSize;
}

int dataStream::devSetSnap() {
    //std::unique_lock<std::mutex> lock(devMutex);
    return devRing->setSnapStatus();
}

unsigned int dataStream::devGetSnapSize() {
    return devRing->getSnapSize();
}

int dataStream::devAddSnapRead() {
    return devRing->addSnapRead();
}

int dataStream::devRmSnapRead() {
    return devRing->rmSnapRead();
}

void* dataStream::devGetSnapPtr(const unsigned int& nBias, unsigned int& nBytes) {
    return devRing->getSnapPtr(nBias, nBytes);
}

unsigned int dataStream::devPopSnap(const unsigned int& nBytes) {
    unsigned int readSize = devRing->popSnap(nBytes);
    totalDevSize -= readSize;
    return readSize;
}

int dataStream::netClear() {
    std::unique_lock<std::mutex> lock(netMutex);
    return netRing->clear();
}

unsigned int dataStream::netWrite(const void* addr, const unsigned int& nBytes, int sendMsg) {
    std::unique_lock<std::mutex> lock(netMutex);

    unsigned int sendSize = netRing->dmaWrite(addr, nBytes);
    if(sendSize == 0)
        return 0;

    totalNetSize += sendSize;
    if(sendMsg == 1) {
        netMsg.signal = 1;
        netMsg.size = sendSize;
        netMsg.count = 1;
        if((msgsnd(netMsgQue, &netMsg, sizeof(netMsg)-sizeof(long), 0)) < 0) {
            //std::cout << "error " << errno << ", " << EACCES << ", " << EAGAIN << ", " << EFAULT << ", " << EIDRM << ", " << EINTR << ", " << EINVAL << ", " << ENOMEM << std::endl;
            return 0;
        }
        //totalNetSize = 0;
    }
    return sendSize;
}

int dataStream::netSetSnap() {
    //std::unique_lock<std::mutex> lock(netMutex);
    return netRing->setSnapStatus();
}

unsigned int dataStream::netGetSnapSize() {
    return netRing->getSnapSize();
}

int dataStream::netAddSnapRead() {
    return netRing->addSnapRead();
}

int dataStream::netRmSnapRead() {
    return netRing->rmSnapRead();
}

void* dataStream::netGetSnapPtr(const unsigned int& nBias, unsigned int& nBytes) {
    return netRing->getSnapPtr(nBias, nBytes);
}

unsigned int dataStream::netPopSnap(const unsigned int& nBytes) {
    unsigned int readSize = netRing->popSnap(nBytes);
    totalNetSize -= readSize;
    return readSize;
}
