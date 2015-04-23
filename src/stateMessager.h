/*=============================================================================
FileName: stateMessager.h
Desc: message handler for states
Author: Haiyun.teng
Email: Haiyun.teng@gmail.com
Version: 0.0.1
=============================================================================*/ 

#ifndef stateMessager_h
#define stateMessager_h
    
#include <iostream>
#include <string>
#include <thread>
#include <netinet/in.h>
#include <mutex>
#include <sstream>

class stateMessager {

    public:
        stateMessager();
        ~stateMessager();
        int init();
        int finish();
        int stateOut(const int& stKey, const std::string& stMsg);
        int stateOut(std::stringstream& msg);
        int setMsgSocket();
        int setDataSocket();
        int sendMsg(const std::string& msg);
        int sendData(void *p0, int nBytes);
    private:
        int status;
        int msgSocket, dataSocket; //socket0: message, socket1: data
        int clientMsg, clientData;
        struct sockaddr_in hostAddr[2];
        struct sockaddr_in clientAddr[2];
        std::thread *t0, *t1;
        std::mutex dataMutex, msgMutex;
};


#endif

