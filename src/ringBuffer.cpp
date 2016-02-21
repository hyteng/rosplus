#include "ringBuffer.h"
#include <stdlib.h>
#include <string.h> //for memcpy
#include <stdint.h>
#include <iostream>

using std::string;

ringBuffer::ringBuffer() {
    vaild = false;
    nBufSize = 0;
}

ringBuffer::~ringBuffer() {
    if(vaild == true)
        release();
}

bool ringBuffer::isVaild() {
    return vaild;
}

int ringBuffer::create(unsigned int size) {

    std::unique_lock<std::mutex> lock(semMutex);

    if(bufBase != NULL || size <= 0)
        return 0;
    else {
        bufBase = (char*)malloc(size+32);
        bufStart = (char*)(((uintptr_t)(bufBase)&(~(uintptr_t)(0x1f)))+32);
        nBufSize = size;
    }

    bufEnd = bufStart + size;
    pWrite = bufStart;
    pRead = NULL;
    pLastWrite = bufEnd;
    vaild = true;
    snapReadCount = 0;

    return 1;
}

int ringBuffer::release() {

    std::unique_lock<std::mutex> lock(semMutex);

    if(bufBase == NULL)
        return 0;
    else {
        free((void*)bufBase);
        bufBase = NULL;
        bufStart = NULL;
        nBufSize = 0;
        bufEnd = NULL;
        pWrite = NULL;
        pRead = NULL;
        pLastWrite = NULL;
        vaild = false;
        snapReadCount = 0;
    }

    return 1;
}

int ringBuffer::clear() {

    std::unique_lock<std::mutex> lock(semMutex);

    if(bufBase == NULL || bufStart == NULL || snapReadCount != 0)
        return 0;
    else {
        pWrite = bufStart;
        pRead = NULL;
        pLastWrite = bufEnd;
    }

    return 1;
}

bool ringBuffer::isFull() {

    std::unique_lock<std::mutex> lock0(semMutex);

    if(pWrite == NULL)
        return true;
    else
        return false;
}

bool ringBuffer::isEmpty() {

    std::unique_lock<std::mutex> lock0(semMutex);

    if(pRead == NULL)
        return true;
    else
        return false;
}

unsigned int ringBuffer::nBytesUsed() {

    //std::unique_lock<std::mutex> lock(semMutex);

    if(pWrite == NULL)
        return (bufEnd-bufStart);
    if(pRead == NULL)
        return 0;
    if(pWrite > pRead)
        return (pWrite-pRead);
    if(pWrite < pRead)
        return (pLastWrite-pRead+pWrite-bufStart);

    return 0;
}

unsigned int ringBuffer::dmaApply(const bool& rw, const unsigned int& nBytes, ringBuffer& other) {

    std::unique_lock<std::mutex> lock0(semMutex);
    std::unique_lock<std::mutex> lock1(other.semMutex, std::try_to_lock);
    //std::unique_lock<std::mutex> snapLock;
    if(!lock1.owns_lock())
        return 0;

    void *p0 = NULL;
    void *p1 = NULL;
    void *p2 = NULL;

    unsigned int readBytes = nBytes;
    unsigned theSnapSignal;
    if(rw == 0) {
        p0 = checkRead(readBytes);
        theSnapSignal = snapSignal;
        //snapLock(snapMutex, std::try_to_lock);
        p1 = other.checkWrite(nBytes);
    }
    else {
        p0 = other.checkRead(readBytes);
        theSnapSignal = other.snapSignal;
        //snapLock(other.snapMutex, std::try_to_lock);
        p1 = checkWrite(nBytes);
    }

    if((p0 == NULL) || (p1 ==NULL) || (theSnapSignal != 0))
        return 0;

    if(readBytes == nBytes)
        memcpy(p1, p0, nBytes);
    else {
        memcpy(p1, p0, readBytes);
        p1 = (void*)((char*)p1+readBytes);
        if(rw == 0)
            p2 = (void*)bufStart;
        else
            p2 = (void*)(other.bufStart);

        memcpy(p1, p2, nBytes-readBytes);
    }

    unsigned int dmaSize0, dmaSize1;
    if(rw == 0) {
        dmaSize0 = afterRead(nBytes);
        dmaSize1 = other.afterWrite(nBytes);
    }
    else {
        dmaSize0 = other.afterRead(nBytes);
        dmaSize1 = afterWrite(nBytes);
    }

    if((dmaSize0 != nBytes) || (dmaSize1 != nBytes))
        return 0;

    return nBytes;
}

unsigned int ringBuffer::dmaWrite(const void* addr, const unsigned int& nBytes) {

    std::unique_lock<std::mutex> lock(semMutex);

    void *p = checkWrite(nBytes);
    if(p == NULL)
        return 0;

    memcpy(p, addr, nBytes);

    unsigned int dmaSize = afterWrite(nBytes);
    return dmaSize;
}

unsigned int ringBuffer::dmaRead(void* addr, const unsigned int& nBytes) {
    
    std::unique_lock<std::mutex> lock(semMutex);
    //std::unique_lock<std::mutex> lock1(snapMutex);

    if(snapSignal == 1)
        return 0;
    snapSize = 0;

    unsigned int readSize = nBytes;
    void *p = checkRead(readSize);
    if(p == NULL)
        return 0;

    if(readSize == nBytes)
        memcpy(addr, p, nBytes);
    else {
        memcpy(addr, p, readSize);
        addr = (void*)((char*)addr+readSize);
        memcpy(addr, (void*)bufStart, nBytes-readSize);
    }

    unsigned int dmaSize = afterRead(nBytes);
    return dmaSize;
}

int ringBuffer::setSnapStatus() {

    std::unique_lock<std::mutex> lock0(semMutex);
    std::unique_lock<std::mutex> lock1(snapMutex);

    if(snapSignal != 0)
        return 0;

    snapWrite = pWrite;
    snapLastWrite = pLastWrite;
    snapRead = pRead;
    snapSignal = 1;
    snapSize = nBytesUsed();
    return 1;
}

unsigned int ringBuffer::getSnapSize() {
    return snapSize;
}

int ringBuffer::addSnapRead() {

    std::unique_lock<std::mutex> lock(snapMutex);
    
    if(snapSignal != 1)
        return 0;

    snapReadCount++;

    return 1;
}

int ringBuffer::rmSnapRead() {

    std::unique_lock<std::mutex> lock(snapMutex);

    if(snapSignal != 1)
        return 0;

    snapReadCount--;

    return 1;
}

void* ringBuffer::getSnapPtr(const unsigned int& nBias, unsigned int& nLength) {

    if(snapSignal != 1)
        return NULL;

    void* p0 = NULL;
    void* p1 = NULL;
    unsigned int virtualSize0 = nBias;
    unsigned int virtualSize1 = nBias + nLength;
    p0 = checkSnapRead(virtualSize1);
    if(p0 == NULL) {
        //std::cout << "ringBuffer does not have enough snap capacity " << virtualSize1 << std::endl;
        return p0;
    }

    if(virtualSize1 == (nBias+nLength))
        return (void*)((char*)p0+nBias);
    else {
        p1 = checkSnapRead(virtualSize0);
        if(virtualSize0 < nBias)
            return (void*)(bufStart+nBias-(snapLastWrite-(char*)p1));
        else {
            if((snapLastWrite-((char*)p1+nBias)) == 0)
                return (void*)(bufStart);
            else {
                nLength = (snapLastWrite-(char*)p0)-nBias;
                return (void*)((char*)p0+nBias);
            }
        }
    }
}

void* ringBuffer::checkWrite(const unsigned int& nBytes) {

    void *p = NULL;
    if(pWrite == NULL)
        return p;
    if((nBytes == 0) || (nBytes > nBufSize))
        return p;

    if(pRead == NULL) {
        p = (void*)pWrite;
        if((long)(bufEnd-pWrite) < (long)nBytes)
            p = (void*)bufStart;
    }
    else {
        if(pRead > pWrite) {
            if((long)(pRead-pWrite) >= (long)nBytes) {
                p = (void*)pWrite;
            }
        }
        else {
            if((long)(bufEnd-pWrite) >= (long)nBytes) {
                p = (void*)pWrite;
            }
            else {
                if((long)(pRead-bufStart) >= (long)nBytes) {
                    p = (void*)bufStart;
                }
            }
        }
    }
    return p;
}

unsigned int ringBuffer::afterWrite(const unsigned int& nBytes) {

    if(nBytes == 0)
        return 0;

    unsigned int writeSize = 0;

    if(pRead == NULL) {
        if((long)(bufEnd-pWrite) < (long)nBytes)
            pWrite = bufStart;

        pRead = pWrite;
        pWrite += nBytes;
        writeSize = nBytes;
        if(pWrite == bufEnd)
            pWrite = bufStart;
        if(pWrite == pRead)
            pWrite = NULL;
    }
    else {
        if(pRead > pWrite) {
            if((long)(pRead-pWrite) >= (long)nBytes) {
                pWrite += nBytes;
                writeSize = nBytes;
                if(pWrite == pRead)
                    pWrite = NULL;
            }
        }
        else {
            if((long)(bufEnd-pWrite) >= (long)nBytes) {
                pWrite += nBytes;
                writeSize = nBytes;
                if(pWrite == bufEnd)
                    pWrite = bufStart;
                if(pWrite == pRead)
                    pWrite = NULL;
            }
            else {
                if((long)(pRead-bufStart) >= (long)nBytes) {
                    pLastWrite = pWrite;
                    pWrite = bufStart;
                    pWrite += nBytes;
                    writeSize = nBytes;
                    if(pWrite == pRead)
                        pWrite = NULL;
                }
            }
        }
    }

    //if(pWrite == NULL)
        //std::cout << "ringBuffer full." << std::endl;

    return writeSize;
}

void* ringBuffer::checkRead(unsigned int& nBytes) {
    
    if(pRead == NULL)
        return 0;
    if((nBytes == 0) || (nBytes > nBufSize))
        return 0;

    void* p = NULL;
    if(pWrite == NULL) {
        if((long)(pLastWrite-pRead) >= (long)nBytes) {
            p = (void*)pRead;
        }
    }
    else {
        if(pWrite > pRead) {
            if((long)(pWrite-pRead) >= (long)nBytes) {
                p = (void*)pRead;
            }
        }
        else {
            if((long)(pLastWrite-pRead) >= (long)nBytes) {
                p = (void*)pRead;
            }
            else {
                if((long)(pLastWrite-pRead+pWrite-bufStart) >= (long)nBytes) {
                    int size1 = (int)(pLastWrite-pRead);
                    p = (void*)pRead;
                    nBytes = size1;
                    //memcpy(addr, p, size1);
                    //pLastWrite = bufEnd;
                    //pRead = bufStart;
                    //p = (void*)pRead;
                    //memcpy(addr+size1, p, nBytes-size1);
                    //pRead += (nBytes-size1);
                    //if(pRead == pWrite)
                        //pRead = NULL;
                }
            }
        }
    }
    return p;
}

unsigned int ringBuffer::afterRead(const unsigned int& nBytes) {

    if(nBytes == 0)
        return 0;
    unsigned int readSize = 0;

    if(pWrite == NULL) {
        if((long)(pLastWrite-pRead) >= (long)nBytes) {
            pWrite = pRead;
            pRead += nBytes;
            readSize = nBytes;
            if(pRead == pLastWrite) {
                pLastWrite = bufEnd;
                pRead = bufStart;
                if(pRead == pWrite)
                    pRead = NULL;
            }
        }
    }
    else {
        if(pWrite > pRead) {
            if((long)(pWrite-pRead) >= (long)nBytes) {
                pRead += nBytes;
                readSize = nBytes;
                if(pRead == pWrite)
                    pRead = NULL;
            }
        }
        else {
            if((long)(pLastWrite-pRead) >= (long)nBytes) {
                pRead += nBytes;
                readSize = nBytes;
                if(pRead == pLastWrite) {
                    pLastWrite = bufEnd;
                    pRead = bufStart;
                    if(pRead == pWrite)
                        pRead = NULL;
                }
            }
            else {
                if((long)(pLastWrite-pRead+pWrite-bufStart) >= (long)nBytes) {
                    unsigned int size1 = pLastWrite-pRead;
                    pLastWrite = bufEnd;
                    pRead = bufStart;
                    pRead += (nBytes-size1);
                    readSize = nBytes;
                    if(pRead == pWrite)
                        pRead = NULL;
                }
            }
        }
    }
    return readSize;
}

void* ringBuffer::checkSnapRead(unsigned int& nBytes) {
    
    if(snapSignal != 1)
        return NULL;

    if(snapRead == NULL)
        return NULL;
    if((nBytes == 0) || (nBytes > nBufSize))
        return NULL;

    void* p = NULL;
    if(snapWrite == NULL) {
        if((long)(snapLastWrite-snapRead) >= (long)nBytes) {
            p = (void*)snapRead;
        }
    }
    else {
        if(snapWrite > snapRead) {
            if((long)(snapWrite-snapRead) >= (long)nBytes) {
                p = (void*)snapRead;
            }
        }
        else {
            if((long)(snapLastWrite-snapRead) >= (long)nBytes) {
                p = (void*)snapRead;
            }
            else {
                if((long)(snapLastWrite-snapRead+snapWrite-bufStart) >= (long)nBytes) {
                    int size1 = (int)(snapLastWrite-snapRead);
                    p = (void*)snapRead;
                    nBytes = size1;
                }
            }
        }
    }
    return p;
}

unsigned int ringBuffer::popSnap(const unsigned int& nBytes) {

    std::unique_lock<std::mutex> lock0(semMutex);
    std::unique_lock<std::mutex> lock1(snapMutex);

    if(snapSignal != 1 || snapReadCount != 0)
        return 0;

    snapSignal = 0;
    unsigned int readSize = afterRead(nBytes);
    snapSize -= readSize;
    return readSize;
}
