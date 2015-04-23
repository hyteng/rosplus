#ifndef dataStream_h
#define dataStream_h

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>
#include "ringBuffer.h"

struct streamMsg {
    long signal;
    unsigned int size;
    unsigned int count;
};

class dataStream {
    public:
        dataStream();
        ~dataStream();

        void init();
        int getDevMsgQue();
        int getNetMsgQue();

        unsigned int devWrite(const void* addr, const unsigned int& nBytes);
        int devSetSnap();
        int devAddSnapRead();
        int devRmSnapRead();
        void* devGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes);
        unsigned int devPopSnap(const unsigned int& nBytes);

        unsigned int netWrite(const void* addr, const unsigned int& nBytes);
        int netSetSnap();
        int netAddSnapRead();
        int netRmSnapRead();
        void* netGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes);
        unsigned int netPopSnap(const unsigned int& nBytes);

    private:
        /* data */
        ringBuffer *devRing, *netRing;
        unsigned int devRingSize, netRingSize;
        unsigned int devCount, netCount;
        unsigned int totalDevSize, totalNetSize;
        key_t devKey, netKey;
        int devMsgQue, netMsgQue;
        streamMsg devMsg, netMsg;
        std::mutex devMutex, netMutex;

};

#endif
