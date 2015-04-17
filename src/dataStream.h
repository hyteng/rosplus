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

        unsigned int dmaSend(const void* addr, const unsigned int& nBytes);
        int dmaSetSnap();
        int dmaAddSnapRead();
        int dmaRmSnapRead();
        void* dmaGetSnapPtr(const unsigned int& nBias, const unsigned int& nBytes);
    private:
        /* data */
        ringBuffer *dmaRing, *netRing;
        unsigned int dmaRingSize, netRingSize;
        unsigned int dmaCount, netCount;
        unsigned int totalDmaSize, totalNetSize;
        int dmaMsgQue, netMsgQue;
        streamMsg dmaMsg, netMsg;
        std::mutex dmaMutex, netMutex;

};

#endif
