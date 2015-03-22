#include "ringBuffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
    }

    return 1;
}

int ringBuffer::clear() {

    std::unique_lock<std::mutex> lock(semMutex);

    if(bufBase == NULL || bufStart == NULL)
        return 0;
    else {
        pWrite = bufStart;
        pRead = NULL;
        pLastWrite = bufEnd;
    }

    return 1;
}

bool ringBuffer::isFull() {

    if(pWrite == NULL)
        return true;
    else
        return false;
}

bool ringBuffer::isEmpty() {

    if(pRead == NULL)
        return true;
    else
        return false;
}

uint32_t ringBuffer::nBytesUsed() {

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

    if(!lock1.owns_lock())
        return 0;

    void *p0 = NULL;
    void *p1 = NULL;

    if(rw == 0) {
        p0 = checkRead(nBytes);
        p1 = other.checkWrite(nBytes);
    }
    else {
        p0 = other.checkRead(nBytes);
        p1 = checkWrite(nBytes);
    }

    if((p0 == NULL) || (p1 ==NULL))
        return 0;

    memcpy(p1, p0, nBytes);

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

    void *p = checkRead(nBytes);
    if(p == NULL)
        return 0;

    memcpy(addr, p, nBytes);

    unsigned int dmaSize = afterRead(nBytes);
    return dmaSize;
}

void* ringBuffer::checkWrite(const unsigned int& nBytes) {

    void *p = NULL;
    if(pWrite == NULL)
        return p;
    if((nBytes < 0) || (nBytes > nBufSize))
        return p;

    if(pRead == NULL) {
        p = (void*)pWrite;
        if((long)(bufEnd-pWrite) < (long)nBytes)
            p = bufStart;
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
    return writeSize;
}

void* ringBuffer::checkRead(const unsigned int& nBytes) {
    
    if(pRead == NULL)
        return 0;
    if((nBytes < 0) || (nBytes > nBufSize))
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
            /*
            else {
                if((long)(pLastWrite-pRead+pWrite-bufStart) >= (long)nBytes) {
                    int size1 = (int)(pLastWrite-pRead);
                    p = (void*)pRead;
                    memcpy(addr, p, size1);
                    pLastWrite = bufEnd;
                    pRead = bufStart;
                    p = (void*)pRead;
                    memcpy(addr+size1, p, nBytes-size1);
                    pRead += (nBytes-size1);
                    if(pRead == pWrite)
                        pRead = NULL;
                }
            }
            */
        }
    }
    return p;
}

unsigned int ringBuffer::afterRead(const unsigned int& nBytes) {
    
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
            /*
            else {
                if((long)(pLastWrite-pRead+pWrite-bufStart) >= (long)nBytes) {
                    int size1 = (int)(pLastWrite-pRead);
                    p = (void*)pRead;
                    memcpy(addr, p, size1);
                    pLastWrite = bufEnd;
                    pRead = bufStart;
                    p = (void*)pRead;
                    memcpy(addr+size1, p, nBytes-size1);
                    pRead += (nBytes-size1);
                    if(pRead == pWrite)
                        pRead = NULL;
                }
            }
            */
        }
    }
    return readSize;
}
