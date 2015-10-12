#ifndef ringBuffer_h
#define ringBuffer_h

#include <mutex>

class ringBuffer {

    public:
        ringBuffer();
        ~ringBuffer();

        bool isVaild();
        int create(unsigned int size);
        int release();
        int clear();
        bool isFull();
        bool isEmpty();
        unsigned int nBytesUsed();
        unsigned int dmaApply(const bool& rw, const unsigned int& nBytes, ringBuffer& other);
        unsigned int dmaWrite(const void* addr, const unsigned int& nBytes);
        unsigned int dmaRead(void* addr, const unsigned int& nBytes);
        int setSnapStatus();
        unsigned int getSnapSize();
        void* getSnapPtr(const unsigned int& nBias, unsigned int& nLength);
        unsigned int popSnap(const unsigned int& nBytes);
        int addSnapRead();
        int rmSnapRead();
        void show();
        
    private:
        void* checkWrite(const unsigned int& nBytes);
        void* checkRead(unsigned int& nBytes);
        void* checkSnapRead(unsigned int& nBytes);
        unsigned int afterWrite(const unsigned int& nBytes);
        unsigned int afterRead(const unsigned int& nBytes);

        bool vaild;
        char *bufBase;   /*appied address*/
        char *bufEnd;   /* pointer to end of buffer */
        char *bufStart; /* pointer to start of buffer ,the position %32*/
        char *pWrite;       /* where to write next */
        char *pRead;    /* where to read next */
        char *pLastWrite;   /* where the data was written before the write pointer was skipped to the bottom */
        unsigned int nBufSize;
        std::mutex semMutex;


        char *snapWrite;
        char *snapRead;
        char *snapLastWrite;
        std::mutex snapMutex;
        unsigned int snapSize;
        unsigned int snapSignal;
        unsigned int snapReadCount;
};

#endif
